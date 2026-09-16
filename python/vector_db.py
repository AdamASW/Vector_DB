import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = PROJECT_ROOT / "build" / "Release"

sys.path.insert(0, str(BUILD_DIR))

from _vector_db import VectorDB as _CppVectorDB

class VectorSearchDB:
    def __init__(self, dimension: int):
        self._db = _CppVectorDB(dimension)
        self._dimension = dimension

    @property
    def dimension(self):
        return self._dimension

    def insert(self, vector_id: int, vector):
        vector = [float(value) for value in vector]
        self._db.insert(vector_id, vector)

    def search(self, query_vector, top_k: int = 5):
        query_vector = [float(value) for value in query_vector]

        results = self._db.search(query_vector, top_k)

        return [
            {
                "id": result.id,
                "score": result.score,
            }
            for result in results
        ]

    def size(self):
        return self._db.size() # Need to implement.