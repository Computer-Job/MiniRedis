#include <chrono>
#include <iostream>
#include <string>
#include "httplib.h"

// HTTP
httplib::Server svr;

int main()
{
    std::string input{};

    // HTTP
    httplib::Client cli("http://localhost:8080");
    cli.set_keep_alive(true);
    cli.set_tcp_nodelay(true);

    for (int i = 0; i < 100000; i++)
    {
        input = "SET id" + std::to_string(i) + " " + std::to_string(i);

        const auto start = std::chrono::steady_clock::now();

        cli.Post("/command", input, "text/plain");

        const auto end = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "Retrieval time: " << elapsed << "ms\n";
    }

    std::cout << "Database filled.\n";

    while (true)
    {
        std::getline(std::cin, input);

        if (input == "Exit")
            break;

        const auto start = std::chrono::steady_clock::now();

        auto res = cli.Post("/command", input, "text/plain");

        const auto end = std::chrono::steady_clock::now();

        if (res)
        {
            const auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

            std::cout << "Status: " << res->status << "\n";
            std::cout << res->body << "\n";
            std::cout << "Retrieval time: " << elapsed << "ms\n";
        }
        else
        {
            std::cout << "Request failed" << "\n";
        }
    }
}
