#include"settings.h"
#include<iostream>
#include<filesystem>
#include<fstream>
#include "../include/json.hpp"

using json = nlohmann::json;

// load_defaults is true by default. 
// Set to false when adjusting settings to make sure defaults are not written to config.json.
Settings::Settings(const bool load_defaults){
    if (hasSettings()){
        std::ifstream config_file(config_file_path);
        config_file >> config;
        //TODO: test broken json
    } else {
        this->setDefaults();
        this->saveSettings();
    }
}

//General Methods:

void Settings::setDefaults() {
    if (!config.contains("ocr_language")) config["ocr_language"] = this->ocr_default_language;
    if (!config.contains("gemini_configured")) config["gemini_configured"] = this->gemini_configured;
    if (!config.contains("ollama_configured")) config["ollama_configured"] = this->ollama_configured;
    if (!config.contains("default_ai_provider")) config["default_ai_provider"] = this->default_ai_provider;
}

bool Settings::hasSettings(){
    return std::filesystem::exists(config_file_path);
}

std::string Settings::getSettingsAsString(){
    return config.dump(4);
}

void Settings::removeAllSettings(){
    if (hasSettings()){
        std::filesystem::remove_all(config_folder_path);
    }
}

void Settings::removeGivenSetting(const std::string &key) {
    if (config.contains(key)){
        config.erase(key);
    }
}

/*
Updates the in-memory config with the contents of the config file.
*/
void Settings::updateSettings(){
    std::ifstream config_file(config_file_path);
    config_file >> config;
}

void Settings::saveSettings(){
    if (hasSettings()){
        std::ofstream config_file(config_file_path);
        config_file << config.dump(4);
    } else {
        std::filesystem::create_directories(home + "/.config/textractor");
        std::ofstream config_file(config_file_path);
        config_file << config.dump(4);
    }
}

// Settings always relevant:
void Settings::setOcrLanguage(const std::string &lang){
    config["ocr_language"] = lang;
}

std::string Settings::getOCRLanguage(){
    return config["ocr_language"].get<std::string>();
}

void Settings::setDefaultAiProvider(const std::string &provider){
    config["default_ai_provider"] = provider;
}

std::string Settings::getDefaultAiProvider(){
    // Determine the preferred provider from settings (or fall back to the compiled default)
    std::string provider = default_ai_provider;
    if (config.contains("default_ai_provider")){
        provider = config["default_ai_provider"].get<std::string>();
    } else {
        provider = default_ai_provider;
        std::cerr << "Warning: Default AI provider is not set, using default: " << default_ai_provider << std::endl;
    }

    // Which providers are actually configured?
    bool ollama_cfg = config.contains("ollama_configured") && config["ollama_configured"].get<bool>();
    bool gemini_cfg = config.contains("gemini_configured") && config["gemini_configured"].get<bool>();

    // Priority rules:
    // 1) If both providers are configured, return the preferred provider (from settings or default).
    // 2) If only one provider is configured, return that one.
    // 3) If none are configured, return the preferred provider (even if unconfigured).

    if (ollama_cfg && gemini_cfg) {
        return provider;
    }

    // If only one provider is configured, return it regardless of the preferred provider setting
    //! Magic hardcoded strings
    if (ollama_cfg) return std::string("ollama");
    if (gemini_cfg) return std::string("gemini");

    // None configured: return whatever the settings/default say (validate known values)
    return provider;
}

bool Settings::getAnyProviderConfigured() {
    return getOllamaConfigured() || getGeminiConfigured();
}

// OLLAMA Settings: (ip, port, model, keep_alive)

void Settings::setOllamaConfigured(bool configured){
    config["ollama_configured"] = configured;
}

bool Settings::getOllamaConfigured(){
    if (!config.contains("ollama_configured")){
        return false;
    }

    return config["ollama_configured"].get<bool>();
}

void Settings::setOllamaIP(const std::string &ip){
    config["ollama_ip"] = ip;
}

std::string Settings::getOllamaIP(){
    if (config.contains("ollama_ip")){
        return config["ollama_ip"].get<std::string>();
    } else {
        std::cerr << "Warning: Ollama IP is not set, using default: " << ollama_default_ip << std::endl;
        return ollama_default_ip;
    }
}

void Settings::setOllamaPort(int port){
    config["ollama_port"] = port;
}

int Settings::getOllamaPort(){
    if (config.contains("ollama_port")){
        return config["ollama_port"].get<int>();
    } else {
        std::cerr << "Warning: Ollama Port is not set, using default: " << ollama_default_port << std::endl;
        return ollama_default_port;
    }
}

void Settings::setOllamaModel(const std::string &model){
    config["ollama_model"] = model;
}

std::string Settings::getOllamaModel(){
    if (config.contains("ollama_model")){
        return config["ollama_model"].get<std::string>();
    } else {
        std::cerr << "Warning: Ollama Model is not set, using default: " << ollama_default_model << std::endl;
        return ollama_default_model;
    }
}

void Settings::setOllamaKeepAlive(int keep_alive){
    config["ollama_keep_alive"] = keep_alive;
}

int Settings::getOllamaKeepAlive(){
    if (config.contains("ollama_keep_alive")){
        return config["ollama_keep_alive"].get<int>();
    } else {
        std::cerr << "Warning: Ollama Keep Alive is not set, using default: " << ollama_default_keep_alive << std::endl;
        return ollama_default_keep_alive;
    }
}

void Settings::resetAllOllamaSettings(){
    removeGivenSetting("ollama_ip");
    removeGivenSetting("ollama_port");
    removeGivenSetting("ollama_model");
    removeGivenSetting("ollama_keep_alive");
    removeGivenSetting("ollama_configured");
    saveSettings();
}

// Gemini Settings: (api_key, api_url)

void Settings::setGeminiConfigured(bool configured){
    config["gemini_configured"] = configured;
}

bool Settings::getGeminiConfigured(){
    if (!config.contains("gemini_configured")){
        return false;
    }

    return config["gemini_configured"].get<bool>();
}

void Settings::setGeminiApiUrl(const std::string &api_url){
    config["gemini_api_url"] = api_url;
}

std::string Settings::getGeminiApiUrl(){
    if (config.contains("gemini_api_url")){
        return config["gemini_api_url"].get<std::string>();
    } else {
        std::cerr << "Warning: Gemini API URL is not set, using default: " << gemini_default_api_url << std::endl;
        return gemini_default_api_url;
    }
}

void Settings::setGeminiApiKey(const std::string &api_key){
    config["gemini_api_key"] = api_key;
}

std::string Settings::getGeminiApiKey(){
    if (config.contains("gemini_api_key")){
        return config["gemini_api_key"].get<std::string>();
    } else {
        std::cerr << "Warning: Gemini API key is not set" << std::endl;
        return "none_set";
    }
}

void Settings::resetAllGeminiSettings(){
    removeGivenSetting("gemini_api_key");
    removeGivenSetting("gemini_api_url");
    removeGivenSetting("gemini_configured");
    saveSettings();
}
