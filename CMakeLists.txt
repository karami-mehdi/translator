
cmake_minimum_required(VERSION 3.16)

project(Translator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(NLOHMANN_JSON_INCLUDE_DIR
    NAMES nlohmann/json.hpp
    PATHS /opt/homebrew/opt/nlohmann-json/include
    NO_CMAKE_FIND_ROOT_PATH
    REQUIRED
)

add_executable(translator main.cpp)

target_include_directories(translator PRIVATE
    ${NLOHMANN_JSON_INCLUDE_DIR}
)

target_link_options(translator PRIVATE
    "-sFETCH=1"
    "-sEXPORTED_FUNCTIONS=['_translate']"
    "-sEXPORTED_RUNTIME_METHODS=['ccall']"
    "-sALLOW_MEMORY_GROWTH=1"
)

set_target_properties(translator PROPERTIES
    SUFFIX ".js"
)