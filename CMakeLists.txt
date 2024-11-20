cmake_minimum_required(VERSION 3.19)

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    cmake_path(SET VCPKG_PATH NORMALIZE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    set(CMAKE_TOOLCHAIN_FILE ${VCPKG_PATH} CACHE STRING "")
endif()

project(SdlWithCMake VERSION 1.0)

if (NOT EMSCRIPTEN)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(PNG REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(
    SdlWithCMake
    ${SOURCES}
)

if (EMSCRIPTEN)

    set(USE_FLAGS "-s USE_SDL=2 -s USE_LIBPNG=1 --preload-file resources/")
    set(CMAKE_CXX_FLAGS ${USE_FLAGS})
    set_target_properties(SdlWithCMake PROPERTIES SUFFIX ".html")

    file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

else()

    target_link_libraries(
        SdlWithCMake PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        GLEW::GLEW
        PNG::PNG
    )

    add_custom_command(
        TARGET SdlWithCMake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:SdlWithCMake>/resources
    )

    enable_testing()

    add_subdirectory(test)

endif()
