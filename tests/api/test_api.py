import pytest
from fastapi.testclient import TestClient

from api.main import create_app


@pytest.fixture
def client():
    with TestClient(create_app()) as test_client:
        yield test_client


def test_health(client):
    response = client.get("/health")

    assert response.status_code == 200
    body = response.json()

    assert body["status"] == "ok"
    assert "dimension" in body
    assert "vectors" in body


def test_stats(client):
    response = client.get("/stats")

    assert response.status_code == 200
    body = response.json()

    assert body == {"dimension": 384, "vectors": 0}


def test_rejects_wrong_dimension(client):
    response = client.post(
        "/vectors",
        json={
            "vectors": [
                {
                    "id": 1,
                    "values": [1.0, 2.0],
                }
            ]
        },
    )

    assert response.status_code == 400


def test_insert_and_search(client):
    dimension = client.get("/stats").json()["dimension"]

    vector_a = [1.0] + [0.0] * (dimension - 1)
    vector_b = [0.0, 1.0] + [0.0] * (dimension - 2)

    insert_response = client.post(
        "/vectors",
        json={
            "vectors": [
                {"id": 101, "values": vector_a},
                {"id": 102, "values": vector_b},
            ]
        },
    )

    assert insert_response.status_code == 201
    assert insert_response.json()["inserted"] == 2

    search_response = client.post(
        "/search",
        json={
            "query": vector_a,
            "top_k": 1,
        },
    )

    assert search_response.status_code == 200

    results = search_response.json()["results"]

    assert len(results) == 1
    assert results[0]["id"] == 101
    assert results[0]["score"] > 0.99


def test_each_client_starts_with_an_empty_database(client):
    assert client.get("/stats").json()["vectors"] == 0