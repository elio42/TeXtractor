#pragma once
#include <cstdlib>
#include <string>

#include "../include/json.hpp"

using json = nlohmann::json;

class Settings{
private:
    json config;
    const std::string home = std::getenv("HOME");
    const std::string config_folder_path = home + "/.config/textractor";
    const std::string config_file_path = config_folder_path + "/config.json";
    
    //Default settings values:
    const std::string ocr_default_language = "eng";
    const std::string ollama_default_ip = "127.0.0.1";
    const int ollama_default_port = 11434;
    const std::string ollama_default_model = "gemma4:e4b";
    const int ollama_default_keep_alive = 0;
    const std::string gemini_default_api_url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3-flash-preview:generateContent";
    
    const std::string default_ai_provider = "gemini";
    const bool gemini_configured = false;
    const bool ollama_configured = false;

    //Internal Methods:
    void setDefaults();

public:
    Settings(const bool load_defaults = true);

    //General Methods:
    bool hasSettings();
    std::string getSettingsAsString();
    void removeAllSettings();
    void removeGivenSetting(const std::string &key);
    void updateSettings();
    void saveSettings();

    //Settings always relevant:
    void setOcrLanguage(const std::string &lang);
    std::string getOCRLanguage();
    void setDefaultAiProvider(const std::string &provider);
    std::string getDefaultAiProvider();
    bool getAnyProviderConfigured();

    //Settings relevant when using Ollama:
    void setOllamaConfigured(bool configured);
    bool getOllamaConfigured();
    void setOllamaIP(const std::string &ip);
    std::string getOllamaIP();
    void setOllamaPort(int port);
    int getOllamaPort();
    void setOllamaModel(const std::string &model);
    std::string getOllamaModel();
    void setOllamaKeepAlive(int keep_alive);
    int getOllamaKeepAlive();
    void resetAllOllamaSettings();

    //Settings relevant when using Gemini:
    void setGeminiConfigured(bool configured);
    bool getGeminiConfigured();
    void setGeminiApiKey(const std::string &api_key);
    std::string getGeminiApiKey();
    void setGeminiApiUrl(const std::string &api_url);
    std::string getGeminiApiUrl();
    void resetAllGeminiSettings();
};
