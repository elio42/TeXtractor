#pragma once

#include <string>
#include <vector>

#include "../core/settings.h"
#include "ai_client.h"

class OllamaClient : public AiClient {
private:
    std::string ip;
    int port;
    std::string model;
    int keep_alive;

    std::string buildRequestBody(const std::string &base64_image, const std::string &prompt);

    std::string sendRequest(const std::string &body);

    std::string parseResponse(const std::string &response);

public:
    OllamaClient(Settings &settings);

    std::string textract(const std::string &file_path, const std::string &prompt = "");

    bool isOnline();

    void installServer();

    //TODO: Think about maybe using the settings model as default
    void installModel(const std::string &model_name = "qwen3-vl");

    std::vector<std::string> getModels();

    std::string getModelsAsString();
};
