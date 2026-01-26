#include <string>
#include <fstream>
#include <bits/stdc++.h>
#include "../include/base64.h"
#include "ai_client.h"

size_t AiClient::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string AiClient::getBase64Image(const std::string &file_path){
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

std::string AiClient::findImageMimeType(const std::string &file_path){
    std::string extension = std::filesystem::path(file_path).extension().string();
    if (transform(extension.begin(), extension.end(), extension.begin(), ::tolower), extension == ".png") {
        return "image/png";
    } else if (transform(extension.begin(), extension.end(), extension.begin(), ::tolower), extension == ".jpeg") {
        return "image/jpeg";
    } else if (transform(extension.begin(), extension.end(), extension.begin(), ::tolower), extension == ".jpg") {
        return "image/jpg";
    } else {
        throw std::runtime_error("Unsupported image format: " + extension);
    }
    return "";
}
