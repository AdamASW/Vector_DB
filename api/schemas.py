from pydantic import BaseModel, Field

class VectorRecord(BaseModel):
    id: int
    values: list[float]

class InsertVectorsRequest(BaseModel):
    vectors: list[VectorRecord] = Field(min_length=1)

class SearchRequest(BaseModel):
    query: list[float]
    top_k: int = Field(default=5, ge=1)

class SearchResultResponse(BaseModel):
    id: int
    score: float

class SearchResponse(BaseModel):
    results: list[SearchResultResponse]