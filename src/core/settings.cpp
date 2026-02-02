#include"settings.h"
#include<iostream>
#include<filesystem>
#include<fstream>
#include<../include/json.hpp>

using json = nlohmann::json;

// load_defaults is true by default. 
// Set to false when adjusting settings to make sure defaults are not written to config.json.
Settings::Settings(const bool load_defaults){
    if (hasSettings()){
        std::ifstream config_file(config_file_path);
        config_file >> config;
        this->setDefaults();
        //TODO: test broken json
    } else if (load_defaults) {
        //Set default settings for app to use when no value is set yet.
        std::cout << "No existing settings found. Setting default configuration values.\n";
        this->setDefaults();
    } else {
        //Empty config when load_defaults is false and no settings file exists.
        std::cout << "No existing settings found. Starting with an empty configuration.\n";
        config = json::object();
    }
}

//General Methods:

void Settings::setDefaults() {
    if (!config.contains("ocr_language")) config["ocr_language"] = this->ocr_default_language;
    if (!config.contains("ollama_ip")) config["ollama_ip"] = this->ollama_default_ip;
    if (!config.contains("ollama_port")) config["ollama_port"] = this->ollama_default_port;
    if (!config.contains("ollama_model")) config["ollama_model"] = this->ollama_default_model;
    if (!config.contains("ollama_keep_alive")) config["ollama_keep_alive"] = this->ollama_default_keep_alive;
    if (!config.contains("gemini_api_url")) config["gemini_api_url"] = this->gemini_default_api_url;
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

// Updates the in-memory config with the contents of the config file.
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

// OLLAMA Settings: (ip, port, model, keep_alive)

void Settings::setOllamaIP(const std::string &ip){
    config["ollama_ip"] = ip;
}

std::string Settings::getOllamaIP(){
    return config["ollama_ip"].get<std::string>();
}

void Settings::setOllamaPort(int port){
    config["ollama_port"] = port;
}

int Settings::getOllamaPort(){
    return config["ollama_port"].get<int>();
}

void Settings::setOllamaModel(const std::string &model){
    config["ollama_model"] = model;
}

std::string Settings::getOllamaModel(){
    return config["ollama_model"].get<std::string>();
}

void Settings::setOllamaKeepAlive(int keep_alive){
    config["ollama_keep_alive"] = keep_alive;
}

int Settings::getOllamaKeepAlive(){
    return config["ollama_keep_alive"].get<int>();
}

// Gemini Settings: (api_key, api_url)

void Settings::setGeminiApiUrl(const std::string &api_url){
    config["gemini_api_url"] = api_url;
}

std::string Settings::getGeminiApiUrl(){
    return config["gemini_api_url"].get<std::string>();
}

void Settings::setGeminiApiKey(const std::string &api_key){
    config["gemini_api_key"] = api_key;
}

std::string Settings::getGeminiApiKey(){
    return config["gemini_api_key"].get<std::string>();
}
