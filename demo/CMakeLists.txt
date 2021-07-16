project(cos-demo)

file(GLOB demo_src "${CMAKE_SOURCE_DIR}/demo/*.cpp")

link_directories(${POCO_LINK_DIR}) #这一行要放到add_executable前面
add_executable(${PROJECT_NAME} ${demo_src})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(${PROJECT_NAME} cossdk ${POCO_LIBS} ${SYSTEM_LIBS})
include_directories(${CMAKE_SOURCE_DIR}/include/ ${POCO_INCLUDE_DIR})

if(${OS_TYPE} STREQUAL "WINDOWS")
    file(GLOB poco_libs "${CMAKE_SOURCE_DIR}/third_party/lib/${BUILD_TARGET}/poco/*")
    file(GLOB ssl_libs "${CMAKE_SOURCE_DIR}/third_party/lib/${BUILD_TARGET}/openssl/*")
    file(COPY ${poco_libs}
            DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    file(COPY ${ssl_libs}
            DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()