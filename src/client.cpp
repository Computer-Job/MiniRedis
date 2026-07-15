#include <chrono>
#include <iostream>
#include <string>
#include <unordered_set>

#include "httplib.h"

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
    std::unordered_set<std::string> commands;
    std::string input{};
    std::string part{};

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
    std::getline(std::cin, input);
    size_t size = input.size();

    if (input[0] == '"' || input[0] == ' ' || size < 3)
    {
        std::cout << "Please input a command";

        return 0;
    }
    httplib::Params params {};
    std::string grammar {"command"};
    size_t begin {0};
    int quote_count {0}; //Quote(s) count
    int ws_count {0}; //White space count
    int count {0};

    for (size_t position = 0; position < input.size(); position++)
    {
        if (input[position] == ' ')
        {
            ws_count++;
        }
        else 
        {
            if (ws_count == 1)
            {
                begin = position;
                std::cout << begin << "\n";
                
                if (input[position] == '"')
                {
                    position++; 
                    
                    for (; position < input.size(); position++)
                    {
                        if (input[position] == '"')
                        {
                            size_t backslash_count {0};

                            for (size_t index = position; index > 0 && input[index - 1] == '\\'; index--)
                            {
                                backslash_count++;
                            }
                            if (backslash_count % 2 == 0)
                                break;
                        }
                    }

                    if (input[position] != '"')
                    {
                        std::cout << "Incorrect syntax\n";

                        return 0;
                    }
                    else if (position + 1 < input.size() && input[position + 1] != ' ')
                    {
                        std::cout << "Incorrect syntax\n";

                        return 0;
                    }
                }
            } 
            else if (input[position] == '"')
            {
                std::cout << "Incorrect syntax\n";

                return 0;
            }
            ws_count = 0;
        }

        if (ws_count == 1)
        {
            std::cout << "\"" << input.substr(begin, position - begin) << "\"" << "\n";
        }
        else if (position == input.size() - 1)
        {
            std::cout << "\"" << input.substr(begin) << "\"" << "\n";
        }
        else if (ws_count > 1)
        {
            std::cout << "Incorrect syntax\n";

            return 0;
        }
    }

    // while ((position = input.find(' ', position)) != std::string::npos)
    // {
    //     part = input.substr(begin, position);
        
    //     std::cout << position << "\n";
    //     if (input[position+=2] == ' ')
    //     {
    //         std::cout << "Invalid syntax\n";

    //         return 0;
    //     }
    //     std::cout << position << "\n";
    //     begin = position;

    //     if (grammar == "command")
    //     {
    //         auto find = commands.find(part);

    //         if (find == commands.end())
    //         {
    //             std::cout << "Invalid command";

    //             return 0;
    //         }
    //         params.emplace(grammar, part);
    //         grammar = "key";
    //         count ++;
    //     }
    //     else if (grammar == "key")
    //     {
    //         params.emplace(grammar, part);

    //         std::cout << "ma i made it\n";

    //         if ((position = input.find(' ', position)) != std::string::npos)
    //         {
    //             std::cout << "made it\n";
    //             part = input.substr(begin, position);

    //             auto find = commands.find(part);

    //             if (find == commands.end())
    //             {
    //                 if (part[0] != '"' || (part[0] == '"' && part[part.size() - 1] == '"'))
    //                 {
    //                     grammar = "value";
    //                     params.emplace(grammar, part);
    //                     grammar = "command";
    //                 }
    //             }
    //             else
    //             {
    //                 grammar = "command";
    //                 params.emplace(grammar, part);
    //                 grammar = "key";
    //                 count ++;
    //             }
    //         }
    //         else 
    //         {
    //             break;
    //         }
    //     }
    //     std::cout << grammar << "\n";
    // }

    auto command = params.find("command");
    if (command != params.end())
    {
        std::cout << command->second << "\n";
    }
    auto key = params.find("key");
    if (key != params.end())
    {
        std::cout << key->second << "\n";
    }
    auto value = params.find("value");
    if (value != params.end())
    {
        std::cout << value->second << "\n";
    }



    // for (int i = 0; i < count; i ++)
    // {
    //     auto command = params.find("command" + std::to_string(count));
    //     auto key = params.find("command" + std::to_string(count));
    //     auto value = params.find("command" + std::to_string(count));

    //     if (command != params.end())
    //     {
    //         std::cout << command->second; // GET
    //     }
    //     if (key != params.end())
    //     {
    //         std::cout << key->second; // GET
    //     }
    //     if (value != params.end())
    //     {
    //         std::cout << value->second; // GET
    //     }
    // }

    // while (input[end] != ' ')
    //     end++;
    
    // part = input.substr(begin, end - 1);
    // params.emplace("command" + std::to_string(count), part);

    // if (end + 1 > size && size < )
    // {
    //     std::cout << "Invalid argument";

    //     return 0;
    // }
    // end++;
    // begin = end;

    // for (size_t end : input)
    // {
    //     if (input[end] == ' ')
    //     {
    //         std::string part = input.substr(begin, end - 1);

    //         if (params.)
    //     }
    // }

    // const auto start = std::chrono::steady_clock::now();

    // auto res = client.Post("/test", params);
    // //auto res = cli.Post("/test", "s", "text/plain");

    // const auto end = std::chrono::steady_clock::now();
    // const auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    // if (res)
    // {
    //     std::cout << res->body << "\n";
    //     std::cout << "Retrieval time: " << elapsed << "ms\n";
    // }
    // else
    // {
    //     std::cout << "Request failed" << "\n";
    // }

    return 0;

    // for (int i = 0; i < 100000; i++)
    // {
    //     input = "SET id" + std::to_string(i) + " " + std::to_string(i);

    //     const auto start = std::chrono::steady_clock::now();

    //     cli.Post("/command", input, "text/plain");

    //     const auto end = std::chrono::steady_clock::now();
    //     const auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    //     std::cout << "Retrieval time: " << elapsed << "ms\n";
    // }

    // std::cout << "Database filled.\n";

    // while (true)
    // {
    //     std::getline(std::cin, input);

    //     if (input == "Exit")
    //         break;

    //     const auto start = std::chrono::steady_clock::now();

    //     auto res = cli.Post("/command", input, "text/plain");

    //     const auto end = std::chrono::steady_clock::now();

    //     if (res)
    //     {
    //         const auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    //         std::cout << "Status: " << res->status << "\n";
    //         std::cout << expandNewlines(res->body) << "\n";
    //         std::cout << "Retrieval time: " << elapsed << "ms\n";
    //     }
    //     else
    //     {
    //         std::cout << "Request failed" << "\n";
    //     }
    // }
}
