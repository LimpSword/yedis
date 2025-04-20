cmake_minimum_required(VERSION 3.31)
project(yedis)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(common_lib INTERFACE)
target_include_directories(common_lib INTERFACE include)

add_executable(yedis server.cpp)
target_link_libraries(yedis PRIVATE common_lib)

add_executable(client client.cpp)
target_link_libraries(client PRIVATE common_lib)

add_executable(common_test tests/buffer_test.cpp)
target_link_libraries(common_test PRIVATE common_lib GTest::gtest_main GTest::gtest GTest::gmock GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(common_test)