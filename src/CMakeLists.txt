project(cossdk)

file(GLOB sdk_common_header "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB sdk_op_header "${CMAKE_SOURCE_DIR}/include/op/*.h")
file(GLOB sdk_request_header "${CMAKE_SOURCE_DIR}/include/request/*.h")
file(GLOB sdk_response_header "${CMAKE_SOURCE_DIR}/include/response/*.h")
file(GLOB sdk_threadpool_header "${CMAKE_SOURCE_DIR}/include/threadpool/*.h")
file(GLOB sdk_trsf_header "${CMAKE_SOURCE_DIR}/include/trsf/*.h")
file(GLOB sdk_util_header "${CMAKE_SOURCE_DIR}/include/util/*.h")
file(GLOB sdk_header
        ${sdk_common_header}
        ${sdk_op_header}
        ${sdk_request_header}
        ${sdk_response_header}
        ${sdk_threadpool_header}
        ${sdk_trsf_header}
        ${sdk_util_header})

file(GLOB sdk_common_src "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB sdk_op_src "${CMAKE_SOURCE_DIR}/src/op/*.cpp")
file(GLOB sdk_request_src "${CMAKE_SOURCE_DIR}/src/request/*.cpp")
file(GLOB sdk_response_src "${CMAKE_SOURCE_DIR}/src/response/*.cpp")
file(GLOB sdk_trsf_src "${CMAKE_SOURCE_DIR}/src/trsf/*.cpp")
file(GLOB sdk_util_src "${CMAKE_SOURCE_DIR}/src/util/*.cpp")
file(GLOB sdk_src
        ${sdk_common_src}
        ${sdk_op_src}
        ${sdk_request_src}
        ${sdk_response_src}
        ${sdk_trsf_src}
        ${sdk_util_src})

message("Sdk header: ${sdk_header}")
message("Sdk src: ${sdk_src}")

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include/ ${POCO_INCLUDE_DIR})
add_library(${PROJECT_NAME} STATIC ${sdk_src} ${sdk_header})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "cossdk")

if(BUILD_SHARED_LIB)
    message(STATUS "Build shared lib")
    link_directories(${POCO_LINK_DIR}) #这一行要放到add_library前面
    add_library(${PROJECT_NAME}-shared SHARED ${sdk_src})
    include_directories(${CMAKE_SOURCE_DIR}/include/ ${POCO_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME}-shared ${POCO_LIBS} ${SYSTEM_LIBS})
endif()