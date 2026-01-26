#pragma once
#include<../include/json.hpp>

using json = nlohmann::json;

class Settings{
private:
    json config;
    const std::string home = std::getenv("HOME");
    const std::string config_folder_path = home + "/.config/textractor";
    const std::string config_file_path = config_folder_path + "/config.json";

public:
    Settings();

    //General Methods:
    bool hasSettings();
    std::string getSettingsAsString();
    void removeSettings();
    void updateSettings();
    void saveSettings();

    //Settings always relevant:
    void setOcrLanguage(const std::string &lang);
    std::string getOCRLanguage();

    //Settings relevant when using Ollama:
    void setOllamaDefaults();
    void removeOllamaSettings();
    void setOllamaIP(const std::string &ip);
    std::string getOllamaIP();
    void setOllamaPort(int port);
    int getOllamaPort();
    void setOllamaModel(const std::string &model);
    std::string getOllamaModel();
    void setOllamaKeepAlive(int keep_alive);
    int getOllamaKeepAlive();

    //Settings relevant when using Gemini:
    void setGeminiApiKey(const std::string &api_key);
    std::string getGeminiApiKey();
    void setGeminiApiUrl(const std::string &api_url);
    std::string getGeminiApiUrl(); //TODO: Make sure there is a default
};
