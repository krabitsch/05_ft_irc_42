
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvass <pvass@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 14:38:51 by pvass             #+#    #+#             */
/*   Updated: 2025/11/27 14:39:15 by pvass            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
*/

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (std::string::npos == first) {
		return str;
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

IrcCommand parseMessage(const std::string& raw_message) {
    IrcCommand cmd;
    std::string message = raw_message;

    // Strip trailing CR LF
    while (!message.empty() && (message[message.length() - 1] == '\r' || message[message.length() - 1] == '\n')) {
        message.erase(message.length() - 1);
    }

    if (message.empty()) {
        return cmd;
    }

    size_t pos = 0;

    // Parse optional prefix: [':' <prefix> <SPACE> ]
    if (message[pos] == ':') {
        pos++; // skip ':'
        size_t space_pos = message.find(' ', pos);
        if (space_pos == std::string::npos) {
            // malformed: prefix without command
            return cmd;
        }
        cmd.prefix = message.substr(pos, space_pos - pos);
        pos = space_pos;
    }

    // Skip SPACE (one or more spaces)
    while (pos < message.length() && message[pos] == ' ') {
        pos++;
    }
    if (pos >= message.length()) {
        // no command
        return cmd;
    }

    // Parse <command>: letters or 3 digits
    size_t cmd_start = pos;
    if (std::isdigit(message[pos])) {
        // must be exactly 3 digits
        if (pos + 3 > message.length() || 
            !std::isdigit(message[pos]) ||
            !std::isdigit(message[pos + 1]) ||
            !std::isdigit(message[pos + 2])) {
            return cmd;
        }
        cmd.command = message.substr(pos, 3);
        pos += 3;
    } else {
        // one or more letters
        while (pos < message.length() && std::isalpha(message[pos])) {
            pos++;
        }
        if (pos == cmd_start) {
            return cmd; // no command
        }
        cmd.command = message.substr(cmd_start, pos - cmd_start);
        std::transform(cmd.command.begin(), cmd.command.end(), 
                       cmd.command.begin(), ::toupper);
    }

    // Parse <params>: <SPACE> [ ':' <trailing> | <middle> <params> ]
    while (pos < message.length()) {
        // Skip SPACE
        if (message[pos] != ' ') {
            break; // end of params
        }
        while (pos < message.length() && message[pos] == ' ') {
            pos++;
        }
        if (pos >= message.length()) {
            break; // end of message
        }

        // Check for trailing (starts with ':')
        if (message[pos] == ':') {
            pos++; // skip ':'
            cmd.parameters.push_back(message.substr(pos));
            break; // trailing is always last
        }

        // Parse <middle>: non-empty, no SPACE/NUL/CR/LF, first char not ':'
        size_t middle_start = pos;
        while (pos < message.length() && 
               message[pos] != ' ' && 
               message[pos] != '\0' && 
               message[pos] != '\r' && 
               message[pos] != '\n') {
            pos++;
        }
        if (pos > middle_start) {
            cmd.parameters.push_back(message.substr(middle_start, pos - middle_start));
        }
    }

    return cmd;
}

/* Main Verison 	
  IrcCommand cmd;
	std::string message = trim(raw_message);

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
			//std::cout << "Token: [" << token << "]\n";
			if (!token.empty() && token[0] == ':') {
				cmd.prefix = token.substr(1);
				//std::cout << "Detected prefix: [" << cmd.prefix << "]\n";
				if (!(ss >> token)) {
					cmd.command.clear();
					return cmd;
				}
			}
			DBG({std::cout << "Token: [" << token << "]\n"; });
			std::transform(token.begin(), token.end(), token.begin(), ::toupper);
			cmd.command = token;
			is_command_or_prefix = false;
		} else {
			cmd.parameters.push_back(token);
		}
	}
	if (trailing_pos != std::string::npos) {
		cmd.parameters.push_back(trailing_param);
	}
	
	return cmd;
}*/
