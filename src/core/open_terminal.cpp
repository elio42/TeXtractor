#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include "open_terminal.h"

// Check if a program exists in PATH
static bool commandExists(const std::string& cmd) {
    std::string check = "command -v " + cmd + " 1>/dev/null 2>&1";
    return std::system(check.c_str()) == 0;
}

void execCommandInTerminal(const std::string &command) {
    // We use 'bash -lc' to ensure environment variables are loaded
    // We append '; exec bash' or read line to keep the window open after execution
    // Using simple concatenation as requested for simplicity
    std::string wrappedCommand = " bash -lc '" + command + "; read -p \"\nPress enter to close...\"'";

    struct Terminal {
        std::string name;
        std::string execFlag;
    };

    std::vector<Terminal> terminals = {
        {"xdg-terminal-exec", ""},     // The future standard (if installed)
        {"x-terminal-emulator", "-e"}, // Debian/Ubuntu standard wrapper
        {"konsole", "-e"},             // KDE Default
        {"gnome-terminal", "--"},      // GNOME Default (note the double dash)
        {"xfce4-terminal", "-x"},      // XFCE
        {"alacritty", "-e"},           // Popular fast terminal
        {"xterm", "-e"}                // Fallback
    };

    for (const Terminal& term : terminals) {
        if (commandExists(term.name)) {
            std::string fullCmd;
            // Construct command: nohup terminal_name [flag] "bash -lc ..." >/dev/null 2>&1 &
            // nohup prevents the terminal from closing if the parent process closes.
            if (term.execFlag.empty()) {
                 // xdg-terminal-exec handles flags internally
                fullCmd = term.name + wrappedCommand + " 1> /dev/null 2>&1 &";
            } else {
                fullCmd = term.name + " " + term.execFlag + wrappedCommand + " 1> /dev/null 2>&1 &";
            }
            
            std::cout << "Launching with: " << term.name << std::endl;
            std::system(fullCmd.c_str());
            return;
        }
    }

    std::cerr << "Error: No supported terminal emulator found." << std::endl;
}