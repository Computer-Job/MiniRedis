#include <chrono>
#include <iostream>
#include <string>
#include <unordered_set>

#include "httplib.h"
#include "parser.h"
#include "lexer.h"

std::string expandNewlines(std::string value)
{
    std::size_t pos = 0;

    while ((pos = value.find("\\n", pos)) != std::string::npos)
    {
        value.replace(pos, 2, "\n");
        pos ++;
    }

    return value;
}

// HTTP
httplib::Server svr;

int main()
{
    std::unordered_set<std::string> commands{};

    // HTTP
    httplib::Client client("http://localhost:8080");
    client.set_keep_alive(true);
    client.set_tcp_nodelay(true);
    {
        auto res = client.Get("/list");

        if (!res)
        {
            std::cout << "Request failed" << "\n";

            return 0;
        }
        std::istringstream stream(res->body);
        std::string command;

        while (stream >> command)
        {
            commands.insert(command);
        }
    }
    std::string input{};
    std::getline(std::cin, input);

    Lexer lexer;
    std::vector<std::string> tokens {lexer.Lex(input)};

    if (tokens.empty())
        std::cout << lexer.getAlert() << "\n";

    // for (const std::string& token : tokens)
    //     std::cout << token << "\n";

    Parser parser;

    httplib::Params params {parser.Parse(tokens, commands)};

    auto res = client.Post("/command", params);

    if (!res)
    {
        std::cout << "Request failed" << "\n";

        return 0;
    }
    std::istringstream stream(res->body);
    std::string command;

    while (stream >> command)
    {
        std::cout << command << " ";
    }

    return 0;
}
