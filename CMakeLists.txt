cmake_minimum_required(VERSION 3.27.0)

project(FlashKVDev VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/FlashKV)

add_executable(FlashKVDev)

target_sources(FlashKVDev PRIVATE
    "src/FlashKVDev.cpp"
)

target_link_libraries(FlashKVDev PRIVATE
    FlashKV
)