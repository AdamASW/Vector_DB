import sys
from pathlib import Path

import pytest


# Allow the test suite to find the compiled pybind11 module.
PROJECT_ROOT = Path(__file__).resolve().parents[2]
BUILD_DIR = PROJECT_ROOT / "build" / "Debug"

sys.path.insert(0, str(BUILD_DIR))

from _vector_db import VectorDB


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