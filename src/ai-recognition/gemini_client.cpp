#include <iostream>
#include <fstream>
#include <filesystem>
#include <curl/curl.h>
#include "gemini_client.h"
#include <../include/json.hpp>
#include "../core/prompts.h"
#include "../include/base64.h"

using json = nlohmann::json;

GeminiClient::GeminiClient(Settings &settings){
    this->api_key = settings.getGeminiApiKey();
    this->api_url = settings.getGeminiApiUrl();
}

std::string GeminiClient::buildRequestBody(const std::string &file_path){
    std::string base64_image = getBase64Image(file_path);

    json inline_data;
    inline_data["mime_type"] = findImageMimeType(file_path);
    inline_data["data"] = base64_image;

    json part_image;
    part_image["inline_data"] = inline_data;

    json part_text;
    part_text["text"] = Prompts::gemeini_prompt; //TODO: pass a value for this.

    json contents;
    contents["parts"] = { part_image, part_text };

    json request;
    request["contents"] = { contents };

    return request.dump();
}

std::string GeminiClient::sendRequest(const std::string &body){
    CURL *curl;
    CURLcode res;
    std::string response_data;

    /*
    res = curl_global_init(CURL_GLOBAL_ALL);
    if(res){
        return (int)res;
    }
    */

    curl = curl_easy_init();
    if(curl){
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string api_key_header = "x-goog-api-key: " + this->api_key;
        headers = curl_slist_append(headers, api_key_header.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, this->api_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        res = curl_easy_perform(curl);

        if (headers) {
            curl_slist_free_all(headers);
        }

        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);
    } else {
        throw std::runtime_error("Could not initialize CURL");
    }

    return response_data;
}

std::string GeminiClient::textract(const std::string &file_path){
    try{
        std::string request_body = buildRequestBody(file_path);
        return sendRequest(request_body);
    }
    catch(const std::exception& e){
        return "Could not perform text extraction with Gemini. The error was: " + std::string(e.what());
    }
}