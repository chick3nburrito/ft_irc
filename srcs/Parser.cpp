#include "../include/Parser.hpp"

Parser::Parser() {}
Parser::Parser(const Parser& other)
{
    (void)other;
}
Parser &Parser::operator=(const Parser &other)
{
    (void)other;
    return (*this);
}
Parser::~Parser() {}

IrcMessage Parser::parse(const std::string &line)
{
    IrcMessage message;
    size_t pos = 0;

    message.hasTrailing = false;

    skip_whitespace(line, &pos);

    if (pos < line.size() && line[pos] == ':')
    {
        pos++;

        while (pos < line.size() && !is_whitespace(line[pos]))
        {
            message.prefix.push_back(line[pos]);
            pos++;
        }
    }

    skip_whitespace(line, &pos);

    while (pos < line.size() && !is_whitespace(line[pos]))
    {
        message.command.push_back(line[pos]);
        pos++;
    }

    while (pos < line.size())
    {
        skip_whitespace(line, &pos);

        if (pos >= line.size())
            break;

        if (line[pos] == ':')
        {
            pos++;
            message.params.push_back(line.substr(pos));
            message.hasTrailing = true;
            break;
        }

        std::string param;

        while (pos < line.size() && !is_whitespace(line[pos]))
        {
            param.push_back(line[pos]);
            pos++;
        }

        if (!param.empty())
            message.params.push_back(param);
    }
    return message;
}