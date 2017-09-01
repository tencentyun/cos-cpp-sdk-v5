1、开发环境

依赖静态库: jsoncpp boost_system boost_thread Poco (在lib文件夹下)

依赖动态库: ssl crypto rt z  (需要安装)

(1)安装openssl的库和头文件 http://www.openssl.org/source/

(2)安装Poco的库和头文件 https://pocoproject.org/download/index.html


(3)安装jsoncpp的库和头文件 https://github.com/open-source-parsers/jsoncpp

(4)安装boost的库和头文件 http://www.boost.org/

(5)安装cmake工具 http://www.cmake.org/download/

sdk中提供了Poco、jsoncpp的库以及头文件，以上库编译好后替换掉sdk中相应的库和头文件即可，如果以上库已经安装到系统里，也可删除sdk中相应的库和头文件。

2、本地编译说明：
修改CMakeList.txt文件中，指定本地boost头文件路径，修改如下语句：
SET(BOOST_HEADER_DIR "/root/boost_1_61_0")

3、配置文件说明
```
"AppID":********,
"AccessKey":"*********************************",
"SecretKey":"********************************",
"Region":"cn-north",                // COS区域, 一定要保证正确
"SignExpiredTime":360,              // 签名超时时间, 单位s
"ConnectTimeoutInms":6000,          // connect超时时间, 单位ms
"HttpTimeoutInms":60000,            // http超时时间, 单位ms
"UploadPartSize":1048576,           // 上传文件分块大小，1M~5G, 默认为1M
"UploadThreadPoolSize":5,           // 单文件分块上传线程池大小
"DownloadSliceSize":4194304,        // 下载文件分片大小
"DownloadThreadPoolSize":5,         // 单文件下载线程池大小
"AsynThreadPoolSize":2,             // 异步上传下载线程池大小
"LogoutType":1,                     // 日志输出类型,0:不输出,1:输出到屏幕,2输出到syslog
"LogLevel":3                        // 日志级别:1: ERR, 2: WARN, 3:INFO, 4:DBG
```

