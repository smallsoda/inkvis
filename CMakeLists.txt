cmake_minimum_required(VERSION 3.14)
project(image-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG fede005abaf93d9d7f3a679d1999b2db341b360f
)

FetchContent_MakeAvailable(stb)

add_executable(${PROJECT_NAME} image-test.cpp image.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
