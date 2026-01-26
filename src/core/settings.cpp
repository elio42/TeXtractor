#include"settings.h"
#include<iostream>
#include<filesystem>
#include<fstream>
#include<../include/json.hpp>

using json = nlohmann::json;

Settings::Settings(){
    if (hasSettings()){
        std::ifstream config_file(config_file_path);
        config_file >> config;
        //ToDo test broken json
    } else {
        // Create default settings
        //TODO: In the future, the defaults could be loaded such that no setting is mandatory even when setting ollama defaults.
        config["ocr_language"] = "eng";
    }
}

//General Methods:
bool Settings::hasSettings(){
    return std::filesystem::exists(config_file_path);
}

std::string Settings::getSettingsAsString(){
    return config.dump(4);
}

void Settings::removeSettings(){
    if (hasSettings()){
        std::filesystem::remove_all(config_folder_path);
    }
}

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

//Settings always relevant:
void Settings::setOcrLanguage(const std::string &lang){
    config["ocr_language"] = lang;
}

std::string Settings::getOCRLanguage(){
    return config["ocr_language"].get<std::string>();
}

//Settings relevant when using Ollama:
void Settings::setOllamaDefaults(){
    config["ip"] = "127.0.0.1";
    config["port"] = 11434;
    config["model"] = "gemma3:12b";
    config["keep_alive"] = 0;
}

void Settings::removeOllamaSettings(){
    config.erase("ip");
    config.erase("port");
    config.erase("model");
    config.erase("keep_alive");
}

void Settings::setOllamaIP(const std::string &ip){
    config["ip"] = ip;
}

std::string Settings::getOllamaIP(){
    return config["ip"].get<std::string>();
}

void Settings::setOllamaPort(int port){
    config["port"] = port;
}

int Settings::getOllamaPort(){
    return config["port"].get<int>();
}

void Settings::setOllamaModel(const std::string &model){
    config["model"] = model;
}

std::string Settings::getOllamaModel(){
    return config["model"].get<std::string>();
}

void Settings::setOllamaKeepAlive(int keep_alive){
    config["keep_alive"] = keep_alive;
}

int Settings::getOllamaKeepAlive(){
    return config["keep_alive"].get<int>();
}

//Settings relevant when using Gemini:
void Settings::setGeminiApiKey(const std::string &api_key){
    config["gemini_api_key"] = api_key;
}

std::string Settings::getGeminiApiKey(){
    return config["gemini_api_key"].get<std::string>();
}

void Settings::setGeminiApiUrl(const std::string &api_url){
    config["gemini_api_url"] = api_url;
}

std::string Settings::getGeminiApiUrl(){
    return config["gemini_api_url"].get<std::string>();
}
