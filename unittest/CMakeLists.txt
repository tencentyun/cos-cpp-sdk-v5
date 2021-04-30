# CMakeLists.txt for directory unittest

IF(ENABLE_UNITTEST)
    file(GLOB comm_src test_utils.cpp)

    ADD_EXECUTABLE(object_request_test object_request_test.cpp ${comm_src})
    TARGET_LINK_LIBRARIES(object_request_test cossdk gtest gtest_main)

    ADD_EXECUTABLE(auth_tool_test auth_tool_test.cpp ${comm_src})
    TARGET_LINK_LIBRARIES(auth_tool_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gtest_main )

    ADD_EXECUTABLE(object_op_test object_op_test.cpp ${comm_src})
    TARGET_LINK_LIBRARIES(object_op_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gtest_main PocoXML PocoFoundation)

    ADD_EXECUTABLE(bucket_op_test bucket_op_test.cpp ${comm_src})
    TARGET_LINK_LIBRARIES(bucket_op_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gtest_main PocoXML PocoFoundation)

    ADD_EXECUTABLE(live_channel_test live_channel_test.cpp ${comm_src})
    TARGET_LINK_LIBRARIES(live_channel_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gtest_main PocoXML PocoFoundation)
ENDIF()
