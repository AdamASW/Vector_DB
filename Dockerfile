FROM python:3.12-slim AS builder

ENV PIP_NO_CACHE_DIR=1 \
    PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

WORKDIR /app

RUN apt-get update \
    && apt-get install --no-install-recommends -y build-essential cmake \
    && rm -rf /var/lib/apt/lists/*

COPY requirements-build.txt .
RUN python -m pip install -r requirements-build.txt

COPY CMakeLists.txt .
COPY bindings ./bindings
COPY include ./include
COPY src ./src

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPython_EXECUTABLE="$(command -v python)" \
    && cmake --build build --config Release --parallel \
    && mkdir -p build/Release \
    && cp build/_vector_db*.so build/Release/


FROM python:3.12-slim AS runtime

ENV PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1 \
    PYTHONPATH=/app

WORKDIR /app

COPY requirements.txt .
RUN python -m pip install -r requirements.txt \
    && useradd --create-home --uid 10001 appuser

COPY --from=builder /app/build/Release ./build/Release
COPY api ./api
COPY python ./python

RUN chown -R appuser:appuser /app
USER appuser

EXPOSE 8000

HEALTHCHECK --interval=30s --timeout=5s --start-period=10s --retries=3 \
    CMD python -c "import urllib.request; urllib.request.urlopen('http://127.0.0.1:8000/health')"

CMD ["uvicorn", "api.main:app", "--host", "0.0.0.0", "--port", "8000"]


FROM runtime AS test

COPY requirements-dev.txt .
RUN python -m pip install -r requirements-dev.txt
COPY tests ./tests

CMD ["python", "-m", "pytest", "tests", "-q"]
