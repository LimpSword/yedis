#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>
#include <vector>

enum CommandName {
    k_get,
    k_set,
    k_del,
    k_unknown,
};

struct Command {
    CommandName command_name;
    std::vector<std::string> args;
};

inline CommandName get_command_name(const std::string &command) {
    if (command.empty()) {
        return k_unknown;
    }
    if (command == "get") {
        return k_get;
    }
    if (command == "set") {
        return k_set;
    }
    if (command == "del") {
        return k_del;
    }
    return k_unknown;
}

inline Command parse_command(const std::string_view &line) {
    std::vector<std::string> split;
    std::string token;
    for (const char c: line) {
        if (c == ' ') {
            split.push_back(token);
            token = "";
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        split.push_back(token);
    }
    Command cmd;
    cmd.command_name = get_command_name(split[0]);
    cmd.args = std::vector(split.begin() + 1, split.end());
    std::cout << "command: " << cmd.command_name;
    for (const auto &arg: cmd.args) {
        std::cout << " arg: " << arg;
    }
    std::cout << std::endl;
    return cmd;
}

#endif //COMMANDS_H
