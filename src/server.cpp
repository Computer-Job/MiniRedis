#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include "httplib.h"

// HTTP
httplib::Server svr;

int main() 
{
    std::unordered_map<std::string, std::string> storage;
    std::mutex storage_mutex;

    svr.Post("/echo", [&storage, &storage_mutex](const httplib::Request &req, httplib::Response &res)
    {
        std::istringstream stream(req.body);
        std::vector<std::string> parts;
        std::string part;

        while (stream >> part)
        {
            parts.push_back(part);
        }

        if (parts.empty())
        {
            res.status = 400;
            res.set_content("No command provided", "text/plain");

            return;
        }

        std::string command = parts[0];

        if (command == "SET")
        {
            if (parts.size() != 3)
            {
                res.status = 400;
                res.set_content("Usage: SET <key> <value>", "text/plain");

                return;
            }
            
            {
                std::lock_guard<std::mutex> lock(storage_mutex);
                storage.insert_or_assign(parts[1], parts[2]);
            }

            res.status = 200;
            res.set_content("OK", "text/plain");

            return;
        }
        else if (command == "GET")
        {
            if (parts.size() != 2)
            {
                res.status = 400;
                res.set_content("Usage: GET <key>", "text/plain");

                return;
            }

            std::string value;
            bool found = false;

            {
                std::lock_guard<std::mutex> lock(storage_mutex);

                auto entry = storage.find(parts[1]);

                if (entry != storage.end())
                {
                    value = entry->second;
                    found = true;
                }
            }

            if (!found)
            {
                res.status = 404;
                res.set_content("(nil)", "text/plain");

                return;
            }

            res.status = 200;
            res.set_content(value, "text/plain");

            return;
        }
        else if (command == "DEL")
        {
            if (parts.size() != 2)
            {
                res.status = 400;
                res.set_content("Usage: DEL <key>", "text/plain");

                return;
            }

            std::size_t removed;

            {
                std::lock_guard<std::mutex> lock(storage_mutex);
                removed = storage.erase(parts[1]);
            }

            res.status = 200;
            res.set_content(std::to_string(removed), "text/plain");

            return;
        }
        else
        {
            res.status = 400;
            res.set_content("Unknown command: " + command, "text/plain");

            return;
        }
    });

    svr.Get("/test", [](const httplib::Request &, httplib::Response &res) 
    {
        res.set_content("Connected", "text/plain");
    });

    svr.listen("0.0.0.0", 8080);
}
