#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 管理直播通道：
 *   - CreateLiveChannel             : 创建直播通道
 *   - GetLiveChannel                : 获取直播通道配置
 *   - GetRtmpSignedPublishUrl       : 获取通道推流 URL
 *   - PutLiveChannelSwitch          : 修改通道开关
 *   - GetLiveChannelHistory         : 获取通道推流历史
 *   - GetLiveChannelStatus          : 获取通道推流状态
 *   - GetLiveChannelVodPlaylist     : 下载点播播放列表
 *   - PostLiveChannelVodPlaylist    : 产生点播播放列表
 *   - DeleteLiveChannel             : 删除直播通道
 *   - ListLiveChannel               : 列举直播通道
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";

// 示例通道名
std::string channel_name = "example-channel";

void PrintResult(const qcloud_cos::CosResult& result, const qcloud_cos::BaseResp& resp) {
    if (result.IsSucc()) {
        std::cout << "Request Succ." << std::endl;
        std::cout << resp.DebugString() << std::endl;
    } else {
        std::cout << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
        std::cout << "HttpStatus=" << result.GetHttpStatus() << std::endl;
        std::cout << "ErrorCode=" << result.GetErrorCode() << std::endl;
        std::cout << "ResourceAddr=" << result.GetResourceAddr() << std::endl;
        std::cout << "XCosRequestId=" << result.GetXCosRequestId() << std::endl;
        std::cout << "XCosTraceId=" << result.GetXCosTraceId() << std::endl;
    }
}

qcloud_cos::CosAPI InitCosAPI() {
    qcloud_cos::CosConfig config(appid, tmp_secret_id, tmp_secret_key, region);
    config.SetTmpToken(tmp_token);
    qcloud_cos::CosAPI cos_tmp(config);
    return cos_tmp;
}

// 创建直播通道
void CreateLiveChannelDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutLiveChannelReq req(bucket_name, channel_name);
    qcloud_cos::PutLiveChannelResp resp;
    std::map<std::string, std::string> url_params;
    url_params.insert(std::make_pair("a", "b"));
    LiveChannelConfiguration config("test", "Enabled", "HLS", 5, 10, "playlist.m3u8");
    req.SetLiveChannelConfig(config);
    req.SetUrlParams(url_params);
    req.SetExpire(1000);
    qcloud_cos::CosResult result = cos.PutLiveChannel(req, &resp);
    std::cout << "===================CreateLiveChannel====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        std::cout << "PlayUrl: " << resp.GetPlayUrl() << std::endl;
        std::cout << "PublishUrl: " << resp.GetPublishUrl() << std::endl;
    }
    std::cout << "========================================================" << std::endl;
}

// 获取直播通道配置
void GetLiveChannelDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetLiveChannelReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelResp resp;
    qcloud_cos::CosResult result = cos.GetLiveChannel(req, &resp);
    std::cout << "===================GetLiveChannel=======================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        LiveChannelConfiguration chan_conf = resp.GetLiveChannelConf();
        std::stringstream oss;
        oss << "Description: " << chan_conf.GetDescription() << ", ";
        oss << "Switch: " << chan_conf.GetSwitch() << ", ";
        oss << "Type: " << chan_conf.GetType() << ", ";
        oss << "FragDuration: " << chan_conf.GetFragDuration() << ", ";
        oss << "FragCount: " << chan_conf.GetFragCount() << ", ";
        oss << "PlaylistName: " << chan_conf.GetPlaylistName() << ", ";
        oss << "PublishUrls: " << chan_conf.GetPublishUrl() << ", ";
        oss << "PlayUrls: " << chan_conf.GetPlayUrl();
        std::cout << "LiveChannelConfiguration: " << oss.str() << std::endl;
    }
    std::cout << "========================================================" << std::endl;
}

// 获取通道推流 URL
void GetRtmpSignedPublishUrlDemo(qcloud_cos::CosAPI& cos) {
    std::cout << "===================GetRtmpSignedPublishUrl==============" << std::endl;
    std::cout << "PublishUrl: "
              << cos.GetRtmpSignedPublishUrl(bucket_name, channel_name, 3600,
                                             std::map<std::string, std::string>())
              << std::endl;
    std::cout << "========================================================" << std::endl;
}

// 修改直播通道开关
void PutLiveChannelSwitchDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutLiveChannelSwitchReq req(bucket_name, channel_name);
    qcloud_cos::PutLiveChannelSwitchResp resp;
    qcloud_cos::CosResult result;

    std::cout << "===================PutLiveChannelSwitch disabled========" << std::endl;
    req.SetDisabled();
    result = cos.PutLiveChannelSwitch(req, &resp);
    PrintResult(result, resp);

    std::cout << "===================PutLiveChannelSwitch enabled=========" << std::endl;
    req.SetEnabled();
    result = cos.PutLiveChannelSwitch(req, &resp);
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 获取直播通道推流历史
void GetLiveChannelHistoryDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetLiveChannelHistoryReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelHistoryResp resp;
    qcloud_cos::CosResult result = cos.GetLiveChannelHistory(req, &resp);
    std::cout << "===================GetLiveChannelHistory================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        const std::vector<LiveRecord>& chan_history = resp.GetChanHistory();
        for (std::vector<LiveRecord>::const_iterator it = chan_history.begin();
             it != chan_history.end(); ++it) {
            std::stringstream oss;
            oss << "StartTime: " << it->m_start_time << ", ";
            oss << "EndTime: " << it->m_end_time << ", ";
            oss << "RemoteAddr: " << it->m_remote_addr << ", ";
            oss << "RequestId: " << it->m_request_id;
            std::cout << oss.str() << std::endl;
        }
    }
    std::cout << "========================================================" << std::endl;
}

// 获取直播通道推流状态
void GetLiveChannelStatusDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetLiveChannelStatusReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelStatusResp resp;
    qcloud_cos::CosResult result = cos.GetLiveChannelStatus(req, &resp);
    std::cout << "===================GetLiveChannelStatus=================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        const LiveChannelStatus& chan_status = resp.GetLiveChannelStatus();
        if (chan_status.m_status == "Idle") {
            std::cout << "Status: " << chan_status.m_status << std::endl;
        } else {
            std::stringstream oss;
            oss << "Status: " << chan_status.m_status << ", ";
            oss << "ConnectedTime: " << chan_status.m_connected_time << ", ";
            oss << "RemoteAddr: " << chan_status.m_remote_addr << ", ";
            oss << "RequestId: " << chan_status.m_request_id;
            if (chan_status.m_has_video) {
                oss << ", Width: " << chan_status.m_video.m_width;
                oss << ", Height: " << chan_status.m_video.m_heigh;
                oss << ", FrameRate: " << chan_status.m_video.m_framerate;
                oss << ", Bandwidth: " << chan_status.m_video.m_bandwidth;
                oss << ", Codec: " << chan_status.m_video.m_codec;
            }
            if (chan_status.m_has_audio) {
                oss << ", AudioBandwidth: " << chan_status.m_audio.m_bandwidth;
                oss << ", SampleRate: " << chan_status.m_audio.m_samplerate;
                oss << ", AudioCodec: " << chan_status.m_audio.m_codec;
            }
            std::cout << oss.str() << std::endl;
        }
    }
    std::cout << "========================================================" << std::endl;
}

// 下载直播通道点播播放列表
void GetLiveChannelVodPlaylistDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetLiveChannelVodPlaylistReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelVodPlaylistResp resp;
    req.SetTime(time(NULL) - 10000, time(NULL));
    qcloud_cos::CosResult result = cos.GetLiveChannelVodPlaylist(req, &resp);
    std::cout << "===================GetLiveChannelVodPlaylist============" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        resp.WriteResultToFile("./playlist.m3u8");
    }
    std::cout << "========================================================" << std::endl;
}

// 为直播通道产生点播播放列表
void PostLiveChannelVodPlaylistDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PostLiveChannelVodPlaylistReq req(bucket_name, channel_name);
    qcloud_cos::PostLiveChannelVodPlaylistResp resp;
    req.SetTime(time(NULL) - 10000, time(NULL));
    req.SetPlaylistName("newplaylist.m3u8");
    qcloud_cos::CosResult result = cos.PostLiveChannelVodPlaylist(req, &resp);
    std::cout << "===================PostLiveChannelVodPlaylist===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 删除直播通道
void DeleteLiveChannelDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteLiveChannelReq req(bucket_name, channel_name);
    qcloud_cos::DeleteLiveChannelResp resp;
    qcloud_cos::CosResult result = cos.DeleteLiveChannel(req, &resp);
    std::cout << "===================DeleteLiveChannel====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 获取直播通道列表
void ListLiveChannelDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::ListLiveChannelReq req(bucket_name);
    qcloud_cos::ListLiveChannelResp resp;
    ListLiveChannelResult list_result;
    qcloud_cos::CosResult result;
    std::cout << "===================ListLiveChannel======================" << std::endl;
    do {
        req.SetMaxKeys(10);
        req.SetMarker(list_result.m_next_marker);
        result = cos.ListLiveChannel(req, &resp);
        PrintResult(result, resp);
        if (result.IsSucc()) {
            list_result = resp.GetListResult();
            std::stringstream oss;
            oss << "MaxKeys: " << list_result.m_max_keys << ", ";
            oss << "Marker: " << list_result.m_marker << ", ";
            oss << "Prefix: " << list_result.m_prefix << ", ";
            oss << "IsTruncated: " << list_result.m_is_truncated << ", ";
            oss << "NextMarker: " << list_result.m_next_marker;
            for (std::vector<LiveChannel>::const_iterator it = list_result.m_channels.begin();
                 it != list_result.m_channels.end(); ++it) {
                oss << "\nName: " << it->m_name << ", LastModified: " << it->m_last_modified;
            }
            std::cout << oss.str() << std::endl;
            resp.ClearResult();
        }
    } while (list_result.m_is_truncated == "true" && result.IsSucc());
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        CreateLiveChannelDemo(cos);
        GetLiveChannelDemo(cos);
        GetRtmpSignedPublishUrlDemo(cos);
        PutLiveChannelSwitchDemo(cos);
        GetLiveChannelHistoryDemo(cos);
        GetLiveChannelStatusDemo(cos);
        GetLiveChannelVodPlaylistDemo(cos);
        PostLiveChannelVodPlaylistDemo(cos);
        ListLiveChannelDemo(cos);
        DeleteLiveChannelDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
