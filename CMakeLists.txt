cmake_minimum_required(VERSION 3.18)

project(VectorSearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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