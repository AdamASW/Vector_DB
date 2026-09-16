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
from _vector_db import VectorDB
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