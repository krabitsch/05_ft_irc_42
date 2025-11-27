#include "../includes/Parser.hpp"

/*
//Data Structure to hold the final parsed command
struct IrcCommand {
    std::string command;
    std::vector<std::string> parameters;
    std::string prefix;

    IrcCommand() {}
    void print() const {
        std::cout << "--- Parsed Command ---" << std::endl;
        std::cout << "Command: " << command << std::endl;
        std::cout << "Prefix: " << (prefix.empty() ? "(none)" : prefix) << std::endl;
        std::cout << "Parameters (" << parameters.size() << "):" << std::endl;
        for (size_t i = 0; i < parameters.size(); ++i) {
            std::cout << "  [" << i << "]: '" << parameters[i] << "'" << std::endl;
        }
        std::cout << "----------------------" << std::endl;
    }
};*/

//Utility function for trimming whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Parsing Function
IrcCommand parseMessage(const std::string& raw_message) {
    IrcCommand cmd;
    std::string message = raw_message;

    if (message.empty()) {
        return cmd;
    }
    std::string trailing_param;

    //Find and extract the optional trailing parameter
    // We search for " :" to correctly distinguish it from the optional prefix ":".
    size_t trailing_pos = message.find(" :");
    std::string message_content;

    if (trailing_pos != std::string::npos) {
        trailing_param = message.substr(trailing_pos + 2);
        message_content = message.substr(0, trailing_pos);
    } else {
        message_content = message;
    }

    std::stringstream ss(message_content);
    std::string token;
    bool is_command_or_prefix = true;

    while (ss >> token) {
        if (is_command_or_prefix) {
            if (token[0] == ':') {
                cmd.prefix = token.substr(1);
                if (!(ss >> token)) {
                    cmd.command.clear();
                    return cmd;
                }
            }
            std::transform(token.begin(), token.end(), token.begin(), ::toupper);
            cmd.command = token;
            is_command_or_prefix = false;
        } else {
            cmd.parameters.push_back(token);
        }
    }
    if (!trailing_param.empty()) {
        cmd.parameters.push_back(trailing_param);
    }
    
    return cmd;
}

/*int main() {
    
    std::vector<std::string> tests;
    tests.push_back("NICK new_user");
    tests.push_back("JOIN #general");
    tests.push_back("PRIVMSG #ft_irc :This is a test message.");
    tests.push_back("MODE #channel +o user1 +l 10");
	tests.push_back("");

    for (std::vector<std::string>::const_iterator it = tests.begin(); it != tests.end(); ++it) {
        const std::string& raw = *it; 
        
        std::cout << "\n>>> Parsing: '" << raw << "'" << std::endl;
        IrcCommand command = parseMessage(trim(raw));
        if (!command.command.empty()) {
            command.print();
        } else {
            std::cout << "Invalid or empty command string." << std::endl;
        }
    }

    return 0;
}*/