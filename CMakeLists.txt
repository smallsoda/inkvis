cmake_minimum_required(VERSION 3.14)
project(inkvis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG fede005abaf93d9d7f3a679d1999b2db341b360f
)

FetchContent_MakeAvailable(stb)

# image-test
add_executable(image-test image-test.cpp image.cpp)

target_include_directories(image-test PRIVATE ${stb_SOURCE_DIR})

# inkvis
set(SOURCES
    "main.cpp"
    "converter.cpp"
    "display.cpp"
    "image.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SYSROOT}/usr/include)

find_library(GPIODCXX_LIB NAMES gpiodcxx)
target_link_libraries(${PROJECT_NAME} ${GPIODCXX_LIB})
