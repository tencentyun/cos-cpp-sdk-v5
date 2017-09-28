# demo directory CMakeLists.txt
ADD_EXECUTABLE(cos_demo cos_demo.cpp)
TARGET_LINK_LIBRARIES(cos_demo cossdk ssl crypto rt jsoncpp stdc++ pthread boost_system boost_thread)
