#pragma once
#include <string>

class AiClient{
protected:
    std::string getBase64Image(const std::string &file_path);

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    std::string findImageMimeType(const std::string &file_path);
};
