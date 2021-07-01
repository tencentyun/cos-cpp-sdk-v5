package main

import (
	"context"
	"encoding/json"
	"fmt"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/tencentyun/cos-go-sdk-v5"
	"github.com/tencentyun/cos-go-sdk-v5/debug"
)

type Config struct {
	SecretID  string
	SecretKey string
	Region    string
	Bucket    string
}

func log_status(err error) {
	if err == nil {
		return
	}
	if cos.IsNotFoundError(err) {
		// WARN
		fmt.Println("WARN: Resource is not existed")
	} else if e, ok := cos.IsCOSError(err); ok {
		fmt.Printf("ERROR: Code: %v\n", e.Code)
		fmt.Printf("ERROR: Message: %v\n", e.Message)
		fmt.Printf("ERROR: Resource: %v\n", e.Resource)
		fmt.Printf("ERROR: RequestId: %v\n", e.RequestID)
		// ERROR
	} else {
		fmt.Printf("ERROR: %v\n", err)
		// ERROR
	}
}

func main() {
	// 解析配置
	fileptr, err := os.Open("config.json")
	if err != nil {
		fmt.Printf("Failed to open config file:%s", err.Error())
		return
	}

	defer fileptr.Close()

	var config Config

	decoder := json.NewDecoder(fileptr)
	err = decoder.Decode(&config)
	if err != nil {
		fmt.Println("Decoder json failed", err.Error())
		return
	} else {
		fmt.Printf("Region:%s, Bucket:%s\n", config.Region, config.Bucket)
	}

	var cosurl string
	cosurl = "http://" + config.Bucket + ".cos." + config.Region + ".myqcloud.com"
	u, _ := url.Parse(cosurl)
	b := &cos.BaseURL{BucketURL: u}
	c := cos.NewClient(b, &http.Client{
		Transport: &cos.AuthorizationTransport{
			SecretID:  config.SecretID,
			SecretKey: config.SecretKey,
			Transport: &debug.DebugRequestTransport{
				RequestHeader: false,
				// Notice when put a large file and set need the request body, might happend out of memory error.
				RequestBody:    false,
				ResponseHeader: false,
				ResponseBody:   false,
			},
		},
	})

	testSizeArray := [...]int{1024 * 1024, 4 * 1024 * 1024}

	testTimes := 10
	var totalComsumeTime int
	var speed, avgSpeed float32

	for _, size := range testSizeArray {
		sizeInMb := size / (1024 * 1024)
		fmt.Printf("============test upload %dMB file===============\n", sizeInMb)
		testStr := strings.Repeat("t", size)
		testFilenamePrefix := "testfile" + strconv.Itoa(sizeInMb) + "MB-"
		i := 0
		for i < testTimes {
			startTime := time.Now().UnixNano() / 1e6
			testKey := testFilenamePrefix + strconv.Itoa(i)
			testReader := strings.NewReader(testStr)
			resp, err := c.Object.Put(context.Background(), testKey, testReader, nil)
			if err != nil {
				fmt.Printf("upload failed")
				log_status(err)
			} else {
				endTime := time.Now().UnixNano() / 1e6
				consumeTimeMs := endTime - startTime
				totalComsumeTime += int(consumeTimeMs)
				speed = (float32)(sizeInMb) / ((float32)(consumeTimeMs) / 1000)
				fmt.Printf("succeed to upload %s, requestId: %s, httpCode: %s, consumeTime: %dms, speed: %0.2f MB/s\n",
					testKey, resp.Header.Get("x-cos-request-id"), resp.Status, consumeTimeMs, speed)

				// 删除对象
				_, err := c.Object.Delete(context.Background(), testKey)
				if err != nil {
					fmt.Printf("failed to delete %s", testKey)
				}
				i++
			}
		}
		avgSpeed = (float32)(sizeInMb*testTimes) / ((float32)(totalComsumeTime) / 1000)
		totalComsumeTime = 0
		fmt.Printf("\naverage speed %0.2f MB/s\n", avgSpeed)
	}

	fmt.Scanln()
}
