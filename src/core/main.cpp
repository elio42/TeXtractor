#include <iostream>
#include <filesystem>
#include "../include/argparse.hpp"
#include "../ocr/ocr.h"
#include "../ai-recognition/gemini_client.h"
#include "../ai-recognition/ollama_client.h"
#include "settings.h"
#include "open_terminal.h"

int main(int argc, char *argv[]){
    argparse::ArgumentParser program("TeXtractor OCR Module", "0.0.2");
    
    program.add_argument("image_path")
        .help("Path to the image file to be processed")
        .default_value(std::string("")) //Needed to make sure there is no runtime error.
        .nargs(argparse::nargs_pattern::optional);

    program.add_argument("--set-lang")
        .help("Set the OCR language (e.g., 'eng' for English)")
        .nargs(1);

    program.add_argument("--settings")
        .help("Show all the currently set settings")
        .flag();

    program.add_argument("--remove-given-setting")
        .help("Remove a given setting by its key")
        .nargs(1);

    program.add_argument("--remove-all-settings")
        .help("Remove all saved settings from disk")
        .flag();

    program.add_argument("--set-ollama-model")
        .help("Set the Ollama model to use")
        .nargs(1);

    program.add_argument("--get-ollama-models")
        .help("Get the list of available Ollama models from the server")
        .flag();

    program.add_argument("--install-ollama-server")
        .help("Install the Ollama server on this machine")
        .flag();

    program.add_argument("--install-ollama-model")
        .help("Install the specified Ollama model on this machine")
        .nargs(1);

    //! Must change the API key storage to use the system keychain in the future.
    program.add_argument("--set-gemini-api-key")
        .help("Set the Gemini API key to use")
        .nargs(1);
    
    program.add_argument("--set-gemini-api-url")
        .help("Set the Gemini API URL to use")
        .nargs(1);

    //! Temporary
    program.add_argument("--use-ollama")
        .help("Use Ollama for text processing")
        .flag();

    program.add_argument("--use-gemini")
        .help("Use Gemini for text processing")
        .flag();
    
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << program.help().str() << std::endl;
        return 1;
    }

    {
        Settings settings(false);
        if (program.get<bool>("--remove-all-settings")){
            settings.removeAllSettings();
            std::cout << "All saved settings have been removed from disk.\n";
        }
        if (program.is_used("--remove-given-setting")){
            settings.removeGivenSetting(program.get<std::string>("--remove-given-setting"));
            settings.saveSettings();
            std::cout << "The given setting has been removed successfully.\n";
        }
        if (program.is_used("--set-lang")){
            settings.setOcrLanguage(program.get<std::string>("--set-lang"));
            settings.saveSettings();
            std::cout << "OCR language setting has been updated successfully.\n";
        }
        if (program.is_used("--set-ollama-model")){
            settings.setOllamaModel(program.get<std::string>("--set-ollama-model"));
            settings.saveSettings();
            std::cout << "Ollama model setting has been updated successfully.\n";
        }
        if (program.is_used("--set-gemini-api-key")){
            settings.setGeminiApiKey(program.get<std::string>("--set-gemini-api-key"));
            settings.saveSettings();
            std::cout << "Gemini API key setting has been updated successfully.\n";
        }
        if (program.is_used("--set-gemini-api-url")){
            settings.setGeminiApiUrl(program.get<std::string>("--set-gemini-api-url"));
            settings.saveSettings();
            std::cout << "Gemini API URL setting has been updated successfully.\n";
        }
        if (program.get<bool>("--settings")){
            if (!settings.hasSettings()){
                std::cout << "No settings have beens saved yet, default settings are in use.\n";
            } else {
                std::cout << settings.getSettingsAsString() << "\n";
            }
        }
        if (program.get<bool>("--install-ollama-server")){
            OllamaClient ollama(settings);
            ollama.installServer();
        }
        if (program.is_used("--install-ollama-model")){
            OllamaClient ollama(settings);
            ollama.installModel(program.get<std::string>("--install-ollama-model"));
        }
        if (program.get<bool>("--get-ollama-models")){
            OllamaClient ollama(settings);
            std::cout << "Retrieving Ollama models from server...\n";
            try {
                std::string models = ollama.getModelsAsString();
                std::cout << "Available Ollama models:\n" << models << "\n";
            } catch (const std::exception &e){
                std::cout << "Could not retrieve Ollama models. The error was: " << e.what() << "\n";
            }
        }
    }

    std::string image_path = program.get<std::string>("image_path");
    if (image_path.empty() || !std::filesystem::exists(image_path)){
        std::cout << "The program did not detect a valid image path to process. Exiting now.\n";
        return 0;
    }

    std::cout << "Processing image: " << image_path << "\n";

    std::string outText;
    outText = getOCR(image_path);    
    std::cout << outText << "\n";

    if (program.get<bool>("--use-ollama")){
        Settings settings;
        OllamaClient ollama(settings);
        outText = ollama.textract(image_path);
        std::cout << "\nOllama response:\n\n" << outText << "\n";
    }

    if (program.get<bool>("--use-gemini")){
        Settings settings;
        GeminiClient gemini(settings);
        outText = gemini.textract(image_path);
        std::cout << "\nGemini response:\n\n" << outText << "\n";
    }
    

    return 0;
}