from _vector_db import VectorDB as _CppVectorDB

class VectorSearchDB:
    def __init__(self, dimension: int):
        self._db = _CppVectorDB(dimension)

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