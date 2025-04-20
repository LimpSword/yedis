#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>
#include <vector>

enum CommandName {
    GET,
    SET,
    DEL,
    UNKNOWN,
};

struct Command {
    CommandName command_name;
    std::vector<std::string> args;
};

inline std::vector<std::string> split_space(const std::string_view &line) {
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
    return split;
}

inline CommandName get_command_name(const std::string &command) {
    if (command.empty()) {
        return UNKNOWN;
    }
    if (command == "get") {
        return GET;
    }
    if (command == "set") {
        return SET;
    }
    if (command == "del") {
        return DEL;
    }
    return UNKNOWN;
}

inline Command parse_command(const std::string_view &line) {
    std::vector<std::string> split = split_space(line);
    Command cmd;

    if (split.empty()) {
        cmd.command_name = UNKNOWN;
        return cmd;
    }

    cmd.command_name = get_command_name(split[0]);
    cmd.args = std::vector(split.begin() + 1, split.end());

    return cmd;
}

#endif //COMMANDS_H
