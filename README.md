# Vector_DB
Implementation of a Vector Database and search engine as a launch project in learning C++ from a solid experience with Python, Java, and C.

## Description
This project is a high-performance vector search engine written in C++ with Python bindings. It is designed to store high-dimensional vectors, such as text embeddings, and retrieve the vectors most similar to a given query. The engine begins with an exact brute-force search implementation using cosine similarity before introducing persistent storage and a graph-based approximate nearest-neighbor index to improve search efficiency. C++ handles the core storage, similarity calculations, indexing, and search operations, while Python is used for generating embeddings and providing an accessible interface to the engine. The project includes benchmarking tools to evaluate the performance and accuracy tradeoffs between exact and approximate search approaches. Expansions include RAG retrieval agent, training or fine-tuning an embedding model, or Graph ML, all of which supported by the underlying Vector DB implementation.

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

## After HSNW Algorithm and Persistent Storage implementations:

The following expansions will extend the project from a vector-search engine into a broader AI/ML retrieval platform.

### A. Retrieval-Augmented Generation and Agentic Search

Build an application that uses the vector database as the retrieval layer for an LLM-powered system.

Potential projects include:

- ITSM troubleshooting assistant using incident, change, and knowledge-base data.
- Research-paper assistant with semantic search and citation-aware responses.
- Codebase documentation assistant.
- Personal knowledge-base assistant.

Potential features:

- Document ingestion and chunking.
- Metadata filtering.
- Hybrid keyword and embedding search.
- Query rewriting.
- Result reranking.
- Retrieval evaluation using Recall@k, MRR, and nDCG.
- LLM tool calling.
- Conversation memory.
- Multi-step agentic retrieval workflows.

The goal is to evaluate whether improved retrieval quality produces better downstream LLM responses.

### B. Fine-Tuned Embedding Models

Investigate whether a domain-specific embedding model improves retrieval performance compared with general-purpose embedding models.

Potential work:

- Compare multiple pretrained embedding models.
- Create labeled query-document relevance pairs.
- Build a contrastive-learning dataset.
- Fine-tune a bi-encoder embedding model.
- Compare pretrained and fine-tuned embeddings.
- Index both representations in the vector database.
- Evaluate Recall@k, MRR, nDCG, latency, and storage requirements.

This would connect the C++ retrieval infrastructure with practical machine-learning model development.

### C. Graph Machine Learning and Graph-Enhanced Retrieval

Extend the vector database with graph-based representations of entities and relationships.

Potential projects include:

- Incident-to-incident similarity graphs.
- Knowledge graphs for ITSM troubleshooting.
- Research-paper citation or concept graphs.
- Graph-based recommendation systems.
- Entity relationship discovery.

Potential techniques:

- Node2Vec or other graph-embedding methods.
- Graph neural networks.
- Node classification.
- Link prediction.
- Community detection.
- Graph-enhanced retrieval.
- Combining graph traversal with vector similarity search.

The goal is to investigate whether structural relationships can improve retrieval beyond embedding similarity alone.

### D. Learned Retrieval and Reranking

Build a multi-stage retrieval pipeline in which the vector database retrieves candidates and a learned model determines their final ranking.

Example pipeline:

Query  
→ Embedding model  
→ VectorDB candidate retrieval  
→ Candidate filtering  
→ Learned reranker  
→ Final ranked results

Potential work:

- Generate query-document relevance labels.
- Retrieve an initial candidate set using HNSW.
- Train a cross-encoder or other relevance model.
- Compare heuristic ranking against learned reranking.
- Study the tradeoff between retrieval recall, ranking quality, and latency.
- Experiment with query expansion and metadata-aware ranking.
- Evaluate the complete pipeline using Recall@k, MRR, nDCG, and end-to-end response quality.

This would move the project from approximate nearest-neighbor search toward a complete, evaluated information-retrieval system.