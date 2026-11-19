cmake_minimum_required(VERSION 3.16)

project(Translator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

find_path(HTTPLIB_INCLUDE_DIR httplib.h REQUIRED)
find_library(HTTPLIB_LIBRARY NAMES cpp-httplib httplib REQUIRED)

add_executable(Translator main.cpp)

target_include_directories(Translator PRIVATE
    ${HTTPLIB_INCLUDE_DIR}
)

target_link_libraries(Translator PRIVATE
    CURL::libcurl
    nlohmann_json::nlohmann_json
    ${HTTPLIB_LIBRARY}
)