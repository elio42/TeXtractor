#include <string>

#include "settings.h"
#include "open_terminal.h"

#include "../include/json.hpp"
using json = nlohmann::json;

class Wizard{
private:
    const std::string command = "curl -fsSL https://ollama.com/install.sh | sh";

public:
    void setup_gemini(const std::string &api_key, const std::string &api_url);
    void undo_gemini_setup();
    
    void install_ollama();
    void setup_ollama(const std::string &ip, int port, const std::string &model, int keep_alive);
    void download_ollama_model(const std::string &model);
    void undo_ollama_setup();

};
