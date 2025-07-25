project(cos-cpp-sdk-ut)

file(GLOB common_src src/test_utils.cpp)
file(GLOB object_request_test_src src/object_request_test.cpp)
file(GLOB object_response_test_src src/object_response_test.cpp)
file(GLOB bucket_response_test_src src/bucket_response_test.cpp)
file(GLOB util_test_src src/util_test.cpp)
file(GLOB object_op_test_src src/object_op_test.cpp)
file(GLOB bucket_op_test_src src/bucket_op_test.cpp)
file(GLOB live_channel_test_src src/live_channel_test.cpp)
file(GLOB async_op_test_src src/async_op_test.cpp)
file(GLOB auditing_req_test_src src/auditing_req_test.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
link_directories(${POCO_LINK_DIR} ${GTEST_LINK_DIR}) #这一行要放到add_executable前面
include_directories(${GTEST_INCLUDE_DIS} ${CMAKE_SOURCE_DIR}/include/ ${POCO_INCLUDE_DIR} ${UT_INCLUDE_DIR})
add_executable(object-request-test ${object_request_test_src} ${common_src})
target_link_libraries(object-request-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(object-response-test ${object_response_test_src} ${common_src})
target_link_libraries(object-response-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(bucket-response-test ${bucket_response_test_src} ${common_src})
target_link_libraries(bucket-response-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(util-test ${util_test_src} ${common_src})
target_link_libraries(util-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(object-op-test ${object_op_test_src} ${common_src})
target_link_libraries(object-op-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(bucket-op-test ${bucket_op_test_src} ${common_src})
target_link_libraries(bucket-op-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(live-channel-test ${live_channel_test_src} ${common_src})
target_link_libraries(live-channel-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(async-op-test ${async_op_test_src} ${common_src})
target_link_libraries(async-op-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

add_executable(auditing-req-test ${auditing_req_test_src} ${common_src})
target_link_libraries(auditing-req-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})
# coverage option
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")

add_executable(all-test
        ${object_request_test_src}
        ${object_response_test_src}
        ${bucket_response_test_src}
        ${util_test_src}
        ${object_op_test_src}
        ${bucket_op_test_src}
        ${live_channel_test_src}
        ${async_op_test_src}
        ${auditing_req_test_src}
        ${common_src})
target_link_libraries(all-test cossdk ${POCO_LIBS} ${OPENSSL_LIBS} ${SYSTEM_LIBS} ${GTEST_LIBS})

file(COPY conf/config.json DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)