#include "wizard.h"
#include <iostream>

void Wizard::setup_gemini(const std::string &api_key, const std::string &api_url) {
    Settings settings(false);
    settings.setGeminiApiKey(api_key);
    settings.setGeminiApiUrl(api_url);

    settings.saveSettings();
}

void Wizard::undo_gemini_setup() {
    Settings settings(false);
    settings.removeGivenSetting("gemini_api_key");
    settings.removeGivenSetting("gemini_api_url");
    settings.removeGivenSetting("gemini_configured");
    settings.saveSettings();
}

void Wizard::install_ollama() {
    execCommandInTerminal(this->command);
}

void Wizard::download_ollama_model(const std::string &model) {
    std::string command = "ollama pull " + model;
    execCommandInTerminal(command);
    std::cout << "Downloading Ollama model: " << model << std::endl;
}

void Wizard::setup_ollama(const std::string &ip, int port, const std::string &model, int keep_alive) {
    Settings settings;
    settings.setOllamaIP(ip);
    settings.setOllamaPort(port);
    settings.setOllamaModel(model);
    settings.setOllamaKeepAlive(keep_alive);
    settings.setOllamaConfigured(true);
    
    settings.saveSettings();
}

void Wizard::undo_ollama_setup() {
    Settings settings(false);
    settings.removeGivenSetting("ollama_ip");
    settings.removeGivenSetting("ollama_port");
    settings.removeGivenSetting("ollama_model");
    settings.removeGivenSetting("ollama_keep_alive");
    settings.removeGivenSetting("ollama_configured");
    settings.saveSettings();
}
