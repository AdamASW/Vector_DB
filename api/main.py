from fastapi import FastAPI, HTTPException

from api.schemas import (
    InsertVectorsRequest,
    SearchRequest,
    SearchResponse,
    SearchResultResponse,
)
from python.vector_db import VectorSearchDB


DIMENSION = 384

def create_app() -> FastAPI:
    app = FastAPI(
        title="Vector Search Engine API",
        version="0.1.0",
        description="FastAPI service backed by a C++ vector-search engine.",
    )
    db = VectorSearchDB(dimension=DIMENSION)

    @app.get("/health")
    def health():
        return {
            "status": "ok",
            "dimension": db.dimension,
            "vectors": db.size(),
        }

    @app.get("/stats")
    def stats():
        return {
            "dimension": db.dimension,
            "vectors": db.size(),
        }

    @app.post("/vectors", status_code=201)
    def insert_vectors(request: InsertVectorsRequest):
        inserted = 0

        for vector in request.vectors:
            if len(vector.values) != db.dimension:
                raise HTTPException(
                    status_code=400,
                    detail=(
                        f"Vector {vector.id} has dimension "
                        f"{len(vector.values)}; expected {db.dimension}."
                    ),
                )

            try:
                db.insert(vector.id, vector.values)
                inserted += 1
            except Exception as exc:
                raise HTTPException(
                    status_code=400,
                    detail=str(exc),
                ) from exc

        return {
            "inserted": inserted,
            "total_vectors": db.size(),
        }

    @app.post("/search", response_model=SearchResponse)
    def search(request: SearchRequest):
        if len(request.query) != db.dimension:
            raise HTTPException(
                status_code=400,
                detail=(
                    f"Query has dimension {len(request.query)}; "
                    f"expected {db.dimension}."
                ),
            )

        try:
            results = db.search(request.query, request.top_k)
        except Exception as exc:
            raise HTTPException(
                status_code=400,
                detail=str(exc),
            ) from exc

        return SearchResponse(
            results=[
                SearchResultResponse(
                    id=result["id"],
                    score=result["score"],
                )
                for result in results
            ]
        )

    return app


app = create_app()