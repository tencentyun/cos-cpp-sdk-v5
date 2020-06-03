cmake_minimum_required(VERSION 2.8)
CMAKE_policy(SET CMP0015 NEW)

project(cos-cpp-sdk)
#SET(BOOST_HEADER_DIR "/root/boost_1_68_0/")
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_VERBOSE_MAKEFILE  ON)
# find_package自动查找，或者通过SET(OPENSSL_VERSION 1.0.0)人工设置
find_package(OpenSSL REQUIRED)
#SET(OPENSSL_VERSION 1.0.1e)
message(${OPENSSL_VERSION})

# 单元测试与覆盖率统计
OPTION (ENABLE_UNITTEST "Unittest" OFF)

# coverage option
IF(ENABLE_UNITTEST)
    find_package(GTest REQUIRED)
    if (NOT GTest_FOUND)
        message(FATAL_ERROR "GTest Not Found!")
    endif (NOT GTest_FOUND)
    MESSAGE(STATUS ENABLE_UNITTEST=${ENABLE_UNITTEST})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
ENDIF()

add_definitions(-D__COS_DEBUG__)

# include directories
INCLUDE_DIRECTORIES(./include)
INCLUDE_DIRECTORIES(/usr/local/ssl/include)
# include_directories(${BOOST_HEADER_DIR})

# lib directories
link_directories(/usr/local/lib)
link_directories(./lib)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

IF(ENABLE_UNITTEST)
    FILE(COPY unittest/testdata/config.json DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
    FILE(COPY unittest/testdata/config_bucket_test.json DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
    FILE(COPY unittest/testdata/sevenyou.txt DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
    FILE(COPY unittest/merge_xml_results.py DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
    INCLUDE_DIRECTORIES(./include/googletest/include/)
    ADD_SUBDIRECTORY(unittest)
ENDIF()

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(demo)
