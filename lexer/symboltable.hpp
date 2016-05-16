#include <map>
#include <string>
#include <regex>
#include <fstream>
#include <functional>

void populateTokenMap(std::string tokenfname);
std::string getPattern(std::string tokenname);
void mapAccess(std::function<void(std::string, std::string)> perform);