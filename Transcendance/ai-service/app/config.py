from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    database_url: str = ""

    llm_api_key: str = ""
    mistral_api_base: str = "https://api.mistral.ai/v1"
    llm_model: str = "mistral-small-latest"
    embedding_model: str = "mistral-embed"
    moderation_model: str = "mistral-moderation-latest"

    google_api_key: str = ""

    model_config = SettingsConfigDict(extra="ignore")


settings = Settings()
