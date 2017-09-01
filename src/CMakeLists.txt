cmake_minimum_required(VERSION 2.8)
CMAKE_policy(SET CMP0015 NEW)

# CMakeLists for src directory
PROJECT(COS_CPP_SDK)

if (OPENSSL_VERSION VERSION_LESS 1.1.0)
    message("old openssl version less than 1.1.0")
    set(COSSDK_SOURCE_FILES cos_api.cpp cos_config.cpp cos_sys_config.cpp
        request/base_req.cpp request/bucket_req.cpp request/object_req.cpp response/base_resp.cpp
        response/object_resp.cpp response/bucket_resp.cpp op/file_download_task.cpp
        op/file_upload_task.cpp op/base_op.cpp op/object_op.cpp op/bucket_op.cpp op/cos_result.cpp
        util/auth_tool.cpp util/codec_util.cpp util/file_util.cpp util/http_sender.cpp
        util/sha1.cpp util/string_util.cpp)
ELSE()
    message("new version upper than 1.1.0")
    set(COSSDK_SOURCE_FILES cos_api.cpp cos_config.cpp cos_sys_config.cpp
        request/base_req.cpp request/bucket_req.cpp request/object_req.cpp response/base_resp.cpp
        response/object_resp.cpp response/bucket_resp.cpp op/file_download_task.cpp
        op/file_upload_task.cpp op/base_op.cpp op/object_op.cpp op/bucket_op.cpp op/cos_result.cpp
        util/auth_tool.cpp util/codec_util_high_openssl.cpp util/file_util.cpp util/http_sender.cpp
        util/sha1.cpp util/string_util.cpp)
ENDIF()

add_library(cossdk STATIC ${COSSDK_SOURCE_FILES})
target_link_libraries(cossdk PocoNet PocoUtil PocoFoundation PocoJSON PocoXML ssl crypto rt jsoncpp stdc++ pthread z boost_thread)
set_target_properties(cossdk PROPERTIES OUTPUT_NAME "cossdk")
