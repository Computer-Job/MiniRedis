#include <iostream>
#include "httplib.h"

// HTTP
httplib::Server svr;

int main() 
{
    std::string input {};

    //HTTP
    httplib::Client cli("http://localhost:8080");
 
    while (true) 
    {
        std::cin >> input;

        if (input == "Exit") 
            break;

        if (auto res = cli.Post("/echo", input, "text/plain"))
        {
            std::cout << "Status: " << res->status << "\n";
            std::cout << res->body << "\n";
        }
        else
        {
            std::cout << "Request failed" << std::endl;
        }
    }
}
