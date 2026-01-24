#include"ollama_client.h"
#include"../core/prompts.h"
#include"../include/base64.h"
#include<iostream>
#include<fstream>
#include<filesystem>
#include<curl/curl.h>
#include<../include/json.hpp>

using json = nlohmann::json;

OllamaClient::OllamaClient(Settings &settings){
    this->ip = settings.getOllamaIP();
    this->port = settings.getOllamaPort();
    this->model = settings.getOllamaModel();
    this->keep_alive = settings.getOllamaKeepAlive();
}

size_t OllamaClient::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string OllamaClient::getBase64Image(const std::string &file_path){
    std::ifstream imageFile(file_path, std::ios::binary);
    if (!imageFile){
        throw std::runtime_error("Could not open image file: " + file_path);
    }

    // Get file size
    imageFile.seekg(0, std::ios::end);
    std::streamsize fileSize = imageFile.tellg();
    imageFile.seekg(0, std::ios::beg);

    // Resize buffer and read
    std::string imageBuffer;
    imageBuffer.resize(fileSize);
    imageFile.read(&imageBuffer[0], fileSize);

    // Encode to Base64
    std::string base64String = base64_encode(imageBuffer);

    return base64String;
}

std::string OllamaClient::buildRequestBody(const std::string &base64_image){
    json request;
    request["model"] = this->model;
    request["stream"] = false;
    request["keep_alive"] = this->keep_alive;

    json message;
    message["role"] = "user";
    message["content"] = "hey";
    message["content"] = Prompts::default_prompt; //TODO: pass a value for this.
    message["images"] = { base64_image };

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

std::string OllamaClient::textract(const std::string &file_path){
    try{
        std::string base64_image = getBase64Image(file_path);
        std::string request_body = buildRequestBody(base64_image);
        
        std::ofstream debug_file("debug_base64.txt");
        debug_file << request_body;
        debug_file.close();

        return sendRequest(request_body);
    }
    catch(const std::exception& e){
        return "Could not perform text extraction with Ollama. The error was: " + std::string(e.what());
    }
}