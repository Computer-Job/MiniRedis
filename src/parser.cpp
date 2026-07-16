#include "parser.h"

httplib::Params Parser::Parse(const std::vector<std::string>& tokens, const std::unordered_set<std::string>& commands)
{
    httplib::Params params {};
    int count {0};
    std::string num {std::to_string(count)}; 
    std::string type {"command" + num};
    
    for (const std::string& token : tokens)
    {
        params.emplace(type, token);

        if (type == "command" + num)
        {
            type = "key" + num;
        }
        else if (type == "key" + num)
        {
            auto find = commands.find(token);

            if (find == commands.end())
            {
                type = "value" + num;
            }
            else
            {
                count++;
                std::string num = std::to_string(count); 
                type = "command" + num;
            }
        }
        else if (type == "value" + num)
        {
            count++;
            std::string num = std::to_string(count); 
            type = "command" + num;
        }
    }

    return params;
}
