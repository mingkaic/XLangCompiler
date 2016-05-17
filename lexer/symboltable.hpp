#pragma once

#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <functional>

#define TOKENFILE "decafTokens.hpp"
#include TOKENFILE

// associate tokens to patterns from TOKENFILE
void populateTokenMap();

// get pattern if it exists
// @remark: please deallocate returned string
std::string* getPattern(TOKEN tok);

// iterate through map performing input function
void mapAccess(std::function<void(TOKEN, std::string)> perform);