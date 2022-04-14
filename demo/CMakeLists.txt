project(cos-demo)

if (NOT ${OS_TYPE} STREQUAL "WINDOWS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
endif()
file(GLOB demo_src "${CMAKE_SOURCE_DIR}/demo/cos_demo.cpp")
file(GLOB stable_test_src "${CMAKE_SOURCE_DIR}/demo/stable_test.cpp")

link_directories(${POCO_LINK_DIR}) #这一行要放到add_executable前面
add_executable(${PROJECT_NAME} ${demo_src})
add_executable(stable_test ${stable_test_src})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(${PROJECT_NAME} cossdk ${POCO_LIBS} ${SYSTEM_LIBS})
target_link_libraries(stable_test cossdk ${POCO_LIBS} ${SYSTEM_LIBS})
include_directories(${CMAKE_SOURCE_DIR}/include/ ${POCO_INCLUDE_DIR})

if(${OS_TYPE} STREQUAL "WINDOWS")
    file(GLOB poco_libs "${CMAKE_SOURCE_DIR}/third_party/lib/${BUILD_TARGET}/poco/*")
    file(GLOB ssl_libs "${CMAKE_SOURCE_DIR}/third_party/lib/${BUILD_TARGET}/openssl/*")
    file(COPY ${poco_libs}
            DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    file(COPY ${ssl_libs}
            DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()