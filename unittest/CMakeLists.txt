# CMakeLists.txt for directory unittest

IF(ENABLE_UNITTEST)
    ADD_EXECUTABLE(object_request_test object_request_test.cpp)
    TARGET_LINK_LIBRARIES(object_request_test cossdk gtest gmock gtest_main gmock_main)

#ADD_EXECUTABLE(auth_tool_test auth_tool_test.cpp)
#TARGET_LINK_LIBRARIES(auth_tool_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gmock gtest_main gmock_main )

    ADD_EXECUTABLE(object_op_test object_op_test.cpp)
    TARGET_LINK_LIBRARIES(object_op_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gmock gtest_main gmock_main PocoXML PocoFoundation)

    ADD_EXECUTABLE(bucket_op_test bucket_op_test.cpp)
    TARGET_LINK_LIBRARIES(bucket_op_test cossdk ssl crypto rt stdc++ pthread z boost_system boost_thread gtest gmock gtest_main gmock_main PocoXML PocoFoundation)

#ADD_EXECUTABLE(cos_api_test cos_api_test.cpp)
#TARGET_LINK_LIBRARIES(cos_api_test cossdk gtest gmock gtest_main gmock_main)
ENDIF()
