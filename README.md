# Vector_DB
Implementation of a Vector Database and search engine as a launch project in learning C++ from a solid experience with Python, Java, and C.

## Description

This project is a high-performance vector search engine written in C++ with Python bindings. It is designed to store high-dimensional vectors, such as text embeddings, and retrieve the vectors most similar to a given query. The engine begins with an exact brute-force search implementation using cosine similarity before introducing persistent storage and a graph-based approximate nearest-neighbor index to improve search efficiency. C++ handles the core storage, similarity calculations, indexing, and search operations, while Python is used for generating embeddings and providing an accessible interface to the engine. The project includes benchmarking tools to evaluate the performance and accuracy tradeoffs between exact and approximate search approaches.

## Future Extensions

### Containerized API Deployment: 
Wrap the vector search engine in a FastAPI service and deploy it using Docker, exposing the engine through REST API endpoints.

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