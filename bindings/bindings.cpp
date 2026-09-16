#include <cstddef>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "VectorDB.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_vector_db, m) {
    m.doc() = "Python bindings for the C++ VectorDB";

    // Expose SearchResult to Python.
    py::class_<SearchResult>(m, "SearchResult")
        .def_readonly("id", &SearchResult::id)
        .def_readonly("score", &SearchResult::score);

    // Expose VectorDB to Python.
    py::class_<VectorDB>(m, "VectorDB")
        .def(
            py::init<std::size_t>(),
            py::arg("dimension")
        )
        .def(
            "insert",
            &VectorDB::insert,
            py::arg("id"),
            py::arg("vector")
        )
        .def(
            "search",
            &VectorDB::search,
            py::arg("query_vector"),
            py::arg("top_k")
        )
        .def(
            "size",
            &VectorDB::size
        );
}