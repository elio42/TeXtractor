#pragma once
#include <string>
#include <../core/settings.h>

class OllamaClient{
private:
    std::string ip;
    int port;
    std::string model;
    int keep_alive;

    std::string buildRequestBody(const std::string &base64_image);

    std::string sendRequest(const std::string &body);

    std::string getBase64Image(const std::string &file_path);

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

public:
    OllamaClient(Settings &settings);

    std::string textract(const std::string &file_path);

    bool isOnline();

    void installServer();

    //TODO: Think about maybe using the settings model as default
    void installModel(const std::string &model_name = "qwen3-vl");
};
