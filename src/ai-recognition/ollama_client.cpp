#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../core/open_terminal.h"
#include "../core/prompts.h"
#include "../include/base64.h"
#include "../include/json.hpp"
#include "ollama_client.h"

using json = nlohmann::json;

OllamaClient::OllamaClient(Settings &settings){
    this->ip = settings.getOllamaIP();
    this->port = settings.getOllamaPort();
    this->model = settings.getOllamaModel();
    this->keep_alive = settings.getOllamaKeepAlive();
}

std::string OllamaClient::buildRequestBody(const std::string &base64_image){
    json request;
    request["model"] = this->model;
    request["stream"] = false;
    request["keep_alive"] = this->keep_alive;

    json message;
    message["role"] = "user";
    message["images"] = { base64_image };
    message["content"] = Prompts::default_simple_prompt; //TODO: pass a value for this.

    request["messages"] = { message };

    return request.dump();
}

std::string OllamaClient::sendRequest(const std::string &body){
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
        std::string url = "http://" + this->ip + ":" + std::to_string(port) + "/api/chat";
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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

std::string OllamaClient::parseResponse(const std::string &response){
    auto json_response = json::parse(response);
    if(json_response.contains("error")){
        throw std::runtime_error("Ollama API Error: " + json_response["error"].get<std::string>());
    }
    else if(json_response.contains("message") && !json_response["message"].empty()){
        return json_response["message"]["content"].get<std::string>();
    }
    else{
        throw std::runtime_error("Unexpected Ollama API response format.");
    }
}

std::string OllamaClient::textract(const std::string &file_path){
    try{
        std::string base64_image = getBase64Image(file_path);
        std::string request_body = buildRequestBody(base64_image);
        std::string response = sendRequest(request_body);
        return parseResponse(response);
    }
    catch(const std::exception& e){
        return "Could not perform text extraction with Ollama. The error was: " + std::string(e.what());
    }
}

std::vector<std::string> OllamaClient::getModels(){
    CURL *curl;
    CURLcode res;
    std::string response_data;

    curl = curl_easy_init();
    if(curl){
        std::string url = "http://" + this->ip + ":" + std::to_string(port) + "/api/tags";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);
    } else {
        throw std::runtime_error("Could not initialize CURL");
    }

    auto json_response = json::parse(response_data);
    std::vector<std::string> models;
    if(json_response.contains("models")){
        for(const auto& model : json_response["models"]){
            if (model.contains("name")) {
                models.push_back(model["name"].get<std::string>());
            } else {
                throw std::runtime_error("Unexpected model format in Ollama API response.");
            }
        }
    }
    return models;
}

std::string OllamaClient::getModelsAsString(){
    std::vector<std::string> models = getModels();
    std::string models_str;
    for(const auto& model : models){
        models_str += model + "\n";
    }
    return models_str;
}

bool OllamaClient::isOnline(){
    try {
        std::vector<std::string> models = getModels();
        return true;
    } catch (...) {
        return false;
    }
}

void OllamaClient::installServer(){
    execCommandInTerminal("curl -fsSL https://ollama.com/install.sh | sh");
}

void OllamaClient::installModel(const std::string &model_name){
    execCommandInTerminal("ollama pull " + model_name);
}