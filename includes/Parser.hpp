#ifndef PARSER_HPP
# define PARSER_HPP

# ifdef FT_IRC_DEBUG
  #  define DBG(someCode) do { someCode; } while (0)
# else
  #  define DBG(someCode) do { } while (0)
# endif

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

//Data Structure to hold the final parsed command
struct IrcCommand {
	std::string					command;
	std::vector<std::string>	parameters;
	std::string					prefix;
	bool						has_trailing;

	IrcCommand() {}
	void print() const {
		std::cout << "--- Parsed Command ---" << std::endl;
		std::cout << "Command: " << command << std::endl;
		std::cout << "Prefix: " << (prefix.empty() ? "(none)" : prefix) << std::endl;
		std::cout << "Parameters (" << parameters.size() << "):" << std::endl;
		std::cout << "Has traling :" << has_trailing << std::endl;
		for (size_t i = 0; i < parameters.size(); ++i) {
			std::cout << "  [" << i << "]: '" << parameters[i] << "'" << std::endl;
		}
		std::cout << "----------------------" << std::endl;
	}
};

IrcCommand parseMessage(const std::string& raw_message);

#endif