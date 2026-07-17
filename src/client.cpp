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

    const auto start = std::chrono::steady_clock::now();

    Lexer lexer;
    std::vector<std::string> tokens {lexer.Lex(input)};

    if (tokens.empty())
        std::cout << lexer.getAlert() << "\n";

    const auto parser_start = std::chrono::steady_clock::now();

    Parser parser;

    httplib::Params params {parser.Parse(tokens, commands)};

    const auto server_start = std::chrono::steady_clock::now();

    auto res = client.Post("/command", params);

    const auto end = std::chrono::steady_clock::now();

    const std::chrono::duration<double, std::milli> lexer_time = parser_start - start;
    const std::chrono::duration<double, std::milli> parser_time = server_start - parser_start;
    const std::chrono::duration<double, std::milli> server_time = end - server_start;
    const std::chrono::duration<double, std::milli> total_elapsed = end - start;

    if (!res)
    {
        std::cout << "Request failed" << "\n";

        return 0;
    }
    std::cout << expandNewlines(res->body);
    std::cout << std::to_string(res->status) << "\n";
    std::cout << "Lexer speed:  " << lexer_time.count() << "ms\n";
    std::cout << "Parser speed: " << parser_time.count() << "ms\n";
    std::cout << "Server speed: " << server_time.count() << "ms\n";
    std::cout << "Total speed:  " << total_elapsed.count() << "ms\n";

    return 0;
}
