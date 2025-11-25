#ifndef PARSER_HPP
# define PARSER_HP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

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
};

IrcCommand parseMessage(const std::string& raw_message);

#endif