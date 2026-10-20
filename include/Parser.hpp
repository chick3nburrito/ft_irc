#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "ToolsHelper.hpp"

#define MAX_CAR 512

struct IrcMessage
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;

    bool hasTrailing;
};

class Parser
{
    public :
        Parser();
        Parser(const Parser& other);
        Parser &operator=(const Parser &other);
        ~Parser();

        static IrcMessage parse(const std::string &line);
};

