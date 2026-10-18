import sys
from pathlib import Path

import pytest


# Allow the test suite to find the compiled pybind11 module.
PROJECT_ROOT = Path(__file__).resolve().parents[2]
BUILD_DIR = PROJECT_ROOT / "build" / "Debug"
PYTHON_DIR = PROJECT_ROOT / "python"

sys.path.insert(0, str(BUILD_DIR))
sys.path.insert(0, str(PYTHON_DIR))

# Import pybind11 module directly from C++ implementation.
from _vector_db import VectorDB, VectorDBVariant
# Import python wrapper class over C++ implementation.
from vector_db import VectorSearchDB


def test_import_and_basic_search():
    db = VectorDB(3)

    db.insert(1, [1.0, 0.0, 0.0])
    db.insert(2, [0.0, 1.0, 0.0])
    db.insert(3, [0.9, 0.1, 0.0])

    results = db.search([1.0, 0.0, 0.0], 2)

    assert len(results) == 2
    assert results[0].id == 1
    assert results[0].score == pytest.approx(1.0)

    assert results[1].id == 3
    assert results[1].score == pytest.approx(0.9 / (0.9**2 + 0.1**2) ** 0.5)

def test_wrapped_import_and_basic_search():
    db = VectorSearchDB(dimension=3)

    assert db.size() == 0

    db.insert(1, [1.0, 0.0, 0.0])
    db.insert(2, [0.0, 1.0, 0.0])
    db.insert(3, [0.9, 0.1, 0.0])

    assert db.size() == 3

    results = db.search([1.0, 0.0, 0.0], 2)

    assert results[0]["id"] == 1
    assert results[0]["score"] == pytest.approx(1.0)

    assert results[1]["id"] == 3
    assert results[1]["score"] == pytest.approx(0.9 / (0.9**2 + 0.1**2) ** 0.5)


def test_hnsw_native_returns_ranked_cosine_results():
    db = VectorDB(3, VectorDBVariant.HNSW)
    db.insert(10, [0.9, 0.1, 0.0])
    db.insert(20, [0.0, 1.0, 0.0])
    db.insert(30, [1.0, 0.0, 0.0])

    results = db.search([1.0, 0.0, 0.0], 3)

    assert len(results) == 3
    assert [result.id for result in results] == [30, 10, 20]
    assert [result.score for result in results] == pytest.approx(
        [1.0, 0.9 / (0.9**2 + 0.1**2) ** 0.5, 0.0]
    )


def test_hnsw_empty_singleton_and_result_limits():
    db = VectorDB(2, VectorDBVariant.HNSW)
    assert db.search([1.0, 0.0], 3) == []

    db.insert(7, [1.0, 0.0])
    assert db.search([1.0, 0.0], 5)[0].id == 7
    with pytest.raises(ValueError, match="top_k"):
        db.search([1.0, 0.0], 0)


def test_hnsw_rejects_invalid_dimensions_and_duplicate_id_updates():
    db = VectorDB(2, VectorDBVariant.HNSW)
    with pytest.raises(ValueError, match="same dimension"):
        db.insert(1, [1.0])

    db.insert(1, [0.0, 1.0])
    db.insert(1, [1.0, 0.0])
    assert db.size() == 1
    assert db.search([1.0, 0.0], 1)[0].score == pytest.approx(1.0)

    with pytest.raises(ValueError, match="same dimension"):
        db.search([1.0], 1)


def test_hnsw_python_wrapper_uses_selected_variant():
    db = VectorSearchDB(dimension=3, variant=VectorDBVariant.HNSW)
    db.insert(1, [0.0, 1.0, 0.0])
    db.insert(2, [1.0, 0.0, 0.0])

    assert db.size() == 2
    assert db.search([1.0, 0.0, 0.0], 1) == [
        {"id": 2, "score": pytest.approx(1.0)}
    ]