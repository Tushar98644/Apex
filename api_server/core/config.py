from typing import List
from pydantic_settings import BaseSettings
from pydantic import field_validator

class Settings(BaseSettings):
  API_PREFIX: str = "/api/v1"
  DEBUG: bool = False
  ALLOWED_ORIGINS: str = ""
  
  @field_validator("ALLOWED_ORIGINS")
  def parse_allowed_origins(cls, v: str) -> List[str]:
      return v.split(",") if v else []
      
  class Config:
    env = ".env"
    env_file_encoding = "utf-8"
    class_sensitive = True
    
settings = Settings()
  