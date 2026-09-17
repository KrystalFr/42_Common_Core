FROM docker.io/library/python:3.12-slim
WORKDIR /app

RUN python -m pip install --no-cache-dir --upgrade pip==26.2.1

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

RUN useradd --create-home appuser && chown -R appuser:appuser /app
USER appuser

EXPOSE 8000
CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]
