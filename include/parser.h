#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include <iostream> //DELETE

#include "httplib.h"

class Parser
{
private:

public:
    httplib::Params Parse(const std::vector<std::string>& tokens, const std::unordered_set<std::string>& commands);
};
