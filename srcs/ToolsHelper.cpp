#include "../include/ToolsHelper.hpp"
#include <iostream>

bool is_whitespace(char c)
{
    if(c == ' ')
        return (true);
    return false;
}

void skip_whitespace(const std::string &line, size_t *_posw)
{
    for (size_t i = *_posw; i < line.size(); i++, (*_posw)++)
    {
        if(!is_whitespace(line[i]))
            break;
    }
}

bool IsNicknameValid(const std::string &nickname) // section 2.3.1 RFC 1459 NICK GRAMMAR
{
    if(nickname.empty() || nickname.size() > NICKMAX)
        return false;
    if(!std::isalpha(static_cast<unsigned char>(nickname[0])))
        return (false);
    for (size_t i = 1; i < nickname.size(); i++)
    {
        if(!std::isalnum(static_cast<unsigned char>(nickname[i])) 
            && !std::strchr("-[]\\`^{}_", nickname[i]))
            return (false);
    }
    return (true);
}

std::vector<std::string> splitDelimiter(const std::string &str, char delim)
{
    std::vector<std::string> res;
    std::string s = "";
    for (size_t i = 0; i < str.size(); i++)
    {
        if(str[i] == delim)
        {
            res.push_back(s);
            s = "";
        }
        else
            s.push_back(str[i]);
    }
    if(!s.empty())
        res.push_back(s);
    return (res);
}