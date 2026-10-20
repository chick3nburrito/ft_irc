#pragma once

#include <string>
#include <cstring>
#include <vector>

#define NICKMAX 30

bool                        is_whitespace(char c);
void                        skip_whitespace(const std::string &line, size_t *_posw);
bool                        IsNicknameValid(const std::string &nickname);
std::vector<std::string>    splitDelimiter(const std::string &str, char delim);
