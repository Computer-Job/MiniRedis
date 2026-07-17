#include "parser.h"

httplib::Params Parser::Parse(const std::vector<std::string>& tokens, const std::unordered_set<std::string>& commands)
{
    httplib::Params params {};
    int count {0};
    std::string num {std::to_string(count)}; 
    std::string type {"command" + num};
    
    for (const std::string &token : tokens)
    {
        if (type == "undetermined")
        {
            const auto &find = commands.find(token);

            if (find == commands.end())
            {
                type = "value" + num;
            }
            else
            {
                count++;
                num = std::to_string(count); 
                type = "command" + num;
            }
        }
        params.emplace(type, token);

        if (type == "command" + num)
        {
            type = "key" + num;
        }
        else if (type == "key" + num)
        {
            type = "undetermined";
        }
        else if (type == "value" + num)
        {
            count++;
            num = std::to_string(count); 
            type = "command" + num;
        }
    }

    return params;
}
