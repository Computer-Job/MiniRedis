#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <optional>
#include <unordered_map>
#include "httplib.h"

class StorageHandler
{
private:
    std::unordered_map<std::string, std::string> storage;
    std::mutex storage_mutex;

public:
    std::optional<std::string> getStorage(const std::string &key)
    {
        std::lock_guard<std::mutex> lock(storage_mutex);
        auto entry = storage.find(key);

        if (entry == storage.end())
        {
            return std::nullopt;
        }

        return entry->second;
    };

    void setStorage(const std::string &key, const std::string &value)
    {
        {
            std::lock_guard<std::mutex> lock(storage_mutex);
            storage.insert_or_assign(key, value);
        }

        return;
    };

    std::size_t delStorage(const std::string &key)
    {
        std::size_t removed;

        {
            std::lock_guard<std::mutex> lock(storage_mutex);
            removed = storage.erase(key);
        }

        return removed;
    };
};

class CommandHandler
{
private:
    using Handler = void (CommandHandler::*)(const std::vector<std::string> &, httplib::Response &);
    StorageHandler storage;

    void getCommand(const std::vector<std::string> &parts, httplib::Response &res)
    {
        if (parts.size() != 2)
        {
            res.status = 400;
            res.set_content("Usage: GET <key>", "text/plain");

            return;
        }
        auto get = storage.getStorage(parts[1]);

        if (!get.has_value())
        {
            res.status = 404;
            res.set_content("(nil)", "text/plain");

            return;
        }
        res.status = 200;
        res.set_content(*get, "text/plain");

        return;
    };

    void setCommand(const std::vector<std::string> &parts, httplib::Response &res)
    {
        int value_index {2};
        int i {value_index};
        std::size_t size {parts.size()};
        std::string value {};

        if (size > value_index)
        {
            value = parts[value_index];
        }
        else
        {
            value = parts[size - 1];
        }

        if (size > value_index && parts[value_index].front() == '"') 
        {
            while (value.size() < 2 || parts[i].back() != '"')
            {
                i++;

                if (i >= size)
                {
                    res.status = 400;
                    res.set_content("End quote missing", "text/plain");

                    return;
                }

                value += " " + parts[i];
            }
            value.erase(value.begin());
            value.pop_back();
        }

        if (size - i != 1)
        {
            res.status = 400;
            res.set_content("Usage: SET <key> <value>", "text/plain");

            return;
        }
        storage.setStorage(parts[1], value);
        res.status = 200;
        res.set_content("OK", "text/plain");

        return;
    };

    void delCommand(const std::vector<std::string> &parts, httplib::Response &res)
    {
        if (parts.size() != 2)
        {
            res.status = 400;
            res.set_content("Usage: DEL <key>", "text/plain");

            return;
        }
        auto removed = storage.delStorage(parts[1]);

        res.status = 200;
        res.set_content(std::to_string(removed), "text/plain");

        return;
    };

    const std::unordered_map<std::string, Handler> handlers{
        {"GET", &CommandHandler::getCommand},
        {"SET", &CommandHandler::setCommand},
        {"DEL", &CommandHandler::delCommand},
    };

    void executeCommand(const std::vector<std::string> &parts, httplib::Response &res)
    {
        std::string command = parts[0];
        auto it = handlers.find(command);

        if (it == handlers.end())
        {
            res.status = 400;
            res.set_content("Unknown command: " + command, "text/plain");

            return;
        }

        (this->*(it->second))(parts, res);

        return;
    };

public:
    void parseCommand(const httplib::Request &req, httplib::Response &res)
    {
        std::vector<std::string> parts{};
        std::string part{};

        std::istringstream stream(req.body);

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
        const std::string &command = parts[0];

        executeCommand(parts, res);

        return;
    };
};

// HTTP
httplib::Server svr;

int main()
{
    CommandHandler handler;

    svr.Post("/command", [&handler](const httplib::Request &req, httplib::Response &res)
             { handler.parseCommand(req, res); });

    svr.Get("/health", [](const httplib::Request &, httplib::Response &res)
            { res.set_content("Connected", "text/plain"); });

    svr.listen("0.0.0.0", 8080);

    return 0;
}
