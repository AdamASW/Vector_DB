# Vector_DB
Implementation of a Vector Database and search engine as a launch project in learning C++ from a solid experience with Python, Java, and C.

## Description
This project is a high-performance vector search engine written in C++ with Python bindings. It is designed to store high-dimensional vectors, such as text embeddings, and retrieve the vectors most similar to a given query. The engine begins with an exact brute-force search implementation using cosine similarity before introducing persistent storage and a graph-based approximate nearest-neighbor index to improve search efficiency. C++ handles the core storage, similarity calculations, indexing, and search operations, while Python is used for generating embeddings and providing an accessible interface to the engine. The project includes benchmarking tools to evaluate the performance and accuracy tradeoffs between exact and approximate search approaches.

## Project Roadmap

### Phase 1: Core Vector Database
- Implement an initial `VectorDB` header and class in C++ with vector insertion, cosine similarity, and exact brute-force top-*k* search.

### Phase 2: Python Integration and Deployment
1. **Vectorization and Insertion:** Write a Python script that generates embeddings from text and inserts them into the C++ vector database.
2. **Python–C++ Integration:** Use `pybind11` to expose the `VectorDB` class and its methods to Python.
3. **Semantic Search:** Extend the Python application to vectorize queries and retrieve the most similar vectors from the database.
4. **Testing:** Build a C++ testing suite covering insertion, search correctness, result ordering, invalid dimensions, edge cases, and error handling.
5. **Containerization and Deployment:** Containerize the application with Docker and optionally expose the vector database through a FastAPI service.

### Phase 3: Approximate Search
- Implement a graph-based approximate nearest-neighbor index and progress toward a full HNSW implementation.
- Benchmark the tradeoff between search latency and retrieval accuracy.

### Phase 4: Optimization
Explore performance improvements including:
- SIMD similarity calculations
- Multithreaded search
- Optimized memory layouts
- Vector quantization
- Memory-mapped storage

## Potential Future Extensions

### Containerized API Deployment: 
Wrap the vector search engine in a FastAPI service and deploy it using Docker, exposing the engine through REST API endpoints.

### Running the API-backed AG News notebook
Build the Linux image and start the Dockerized API service:

```powershell
docker build -t vector-db .
docker run --rm --name vector-db-api -p 8000:8000 vector-db
```

With the service running, open `python/semantic_search.ipynb` and run the cells in order. The notebook keeps the local C++/Python mode separate from the HTTP API mode. The API section starts with a 20-row AG News validation subset, inserts embeddings through batched `POST /vectors` requests, and queries them through `POST /search`. Set `API_DATASET_LIMIT = None` only after the subset validation succeeds to use all loaded rows.

### RAG System with LangChain: 
Integrate the engine with LangChain and use it as the retrieval backend for a Retrieval-Augmented Generation pipeline.

### Hybrid Search: 
Combine vector similarity search with traditional keyword search, such as BM25, to improve retrieval for queries where exact terms are important.

### Similarity Algorithm Optimization:
Optimize similarity calculations by exploring improved memory layouts and SIMD instructions such as AVX and AVX2.

### Parallel Search:
Parallelize similarity calculations across multiple CPU cores and benchmark how multithreading affects search latency at different dataset sizes.

### Full HNSW Implementation: 
Expand the simplified graph-based approximate nearest-neighbor index into a complete Hierarchical Navigable Small World implementation with configurable search and construction parameters.

### Advanced Metadata Filtering: 
Implement structured metadata filtering and secondary indexes to efficiently combine traditional database queries with vector similarity search.

### Vector Quantization: 
Explore lower-precision vector representations, such as float16 and int8, to reduce memory usage while measuring the resulting impact on retrieval accuracy.

### Memory-Mapped Storage: 
Use memory-mapped files to support vector datasets larger than available RAM and explore the performance tradeoffs of disk-backed storage.