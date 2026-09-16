cmake_minimum_required(VERSION 3.18)

project(VectorSearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE pybind11_cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE pybind11_cmake_result
)

if(pybind11_cmake_result EQUAL 0 AND EXISTS "${pybind11_cmake_dir}")
    list(APPEND CMAKE_PREFIX_PATH "${pybind11_cmake_dir}")
endif()

find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(
    _vector_db
    bindings/bindings.cpp
    src/VectorDB.cpp
)

target_include_directories(
    _vector_db
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)