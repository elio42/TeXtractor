#include<iostream>
#include<filesystem>
#include<../include/argparse.hpp>
#include<../ocr/ocr.h>
#include<../ai-recognition/gemini_client.h>
#include<../ai-recognition/ollama_client.h>
#include "settings.h"

int main(int argc, char *argv[]){
    argparse::ArgumentParser program("TeXtractor OCR Module", "0.0.1");
    
    program.add_argument("image_path")
        .help("Path to the image file to be processed")
        .default_value(std::string("")) //Needed to make sure there is no runtime error.
        .nargs(argparse::nargs_pattern::optional);

    program.add_argument("--set-lang")
        .help("Set the OCR language (e.g., 'eng' for English)")
        .nargs(1);

    program.add_argument("--set-ollama-defaults")
        .help("Set Ollama connection settings to default values")
        .flag();

    program.add_argument("--settings")
        .help("Show all the currently set settings")
        .flag();

    program.add_argument("--remove-settings")
        .help("Remove all saved settings from disk")
        .flag();

    program.add_argument("--set-ollama-model")
        .help("Set the Ollama model to use")
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
        Settings settings;
        if (program.get<bool>("--remove-settings")){
            settings.removeSettings();
            std::cout << "All saved settings have been removed from disk.\n";
        }
        if (program.is_used("--set-lang")){
            settings.setOcrLanguage(program.get<std::string>("--set-lang"));
            settings.saveSettings();
        }
        if (program.get<bool>("--set-ollama-defaults")){
            settings.setOllamaDefaults();
            settings.saveSettings();
        }
        if (program.is_used("--set-ollama-model")){
            settings.setOllamaModel(program.get<std::string>("--set-ollama-model"));
            settings.saveSettings();
        }
        if (program.is_used("--set-gemini-api-key")){
            settings.setGeminiApiKey(program.get<std::string>("--set-gemini-api-key"));
            settings.saveSettings();
        }
        if (program.is_used("--set-gemini-api-url")){
            settings.setGeminiApiUrl(program.get<std::string>("--set-gemini-api-url"));
            settings.saveSettings();
        }
        if (program.get<bool>("--settings")){
            if (!settings.hasSettings()){
                std::cout << "No settings have beens saved yet, default settings are in use.\n";
            } else {
                std::cout << settings.getSettingsAsString() << "\n";
            }
        }
        
        std::cout << "\nIf you changed any settings, they have been saved successfully.\n\n";
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