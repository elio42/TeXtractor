#pragma once
#include <string>
#include <../core/settings.h>
#include "ai_client.h"
#include "../core/settings.h"

class GeminiClient : public AiClient {
private:
    std::string api_key;
    std::string api_url;

    std::string buildRequestBody(const std::string &base64_image);

    std::string sendRequest(const std::string &body);

    std::string parseResponse(const std::string &response);

public:
    GeminiClient(Settings &settings);

    std::string textract(const std::string &file_path);
};
