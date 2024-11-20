project(TestProject VERSION 1.0)

find_package(GTest CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)

file(GLOB_RECURSE TESTS "src/*.cpp")
file(GLOB_RECURSE SOURCES "../src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "Main.cpp")

add_executable(TestProject ${TESTS} ${SOURCES})

target_link_libraries(
    TestProject
    GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

gtest_discover_tests(TestProject)
