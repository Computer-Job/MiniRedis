#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <optional>
#include <unordered_map>
#include <iomanip>
#include "httplib.h"

struct Entry
{
    std::string value{};
    std::optional<int> time{}; // Time in terms of seconds
    std::optional<std::chrono::steady_clock::time_point> expires_at{};
};

class StorageHandler
{
private:
    std::unordered_map<std::string, Entry> storage;
    std::mutex storage_mutex;

public:
    std::optional<std::string> getStorage(const std::string &key)
    {
        std::lock_guard<std::mutex> lock(storage_mutex);
        auto find = storage.find(key);

        if (find == storage.end())
        {
            return std::nullopt;
        }

        return find->second.value;
    };

    void setStorage(const std::string &key, const std::string &value)
    {
        {
            std::lock_guard<std::mutex> lock(storage_mutex);
            storage.insert_or_assign(key, Entry{value});
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

    void expireStorage(const std::string &key, const int &time)
    {
        std::lock_guard<std::mutex> lock(storage_mutex);
        auto find = storage.find(key);

        if (find != storage.end())
        {
            find->second.time = time;
        }

        return;
    };

    std::optional<int> ttlStorage(const std::string &key)
    {
        std::lock_guard<std::mutex> lock(storage_mutex);
        auto find = storage.find(key);

        if (find == storage.end())
        {
            return std::nullopt;
        }

        return find->second.time;
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
        if (parts.size() != 3)
        {
            res.status = 400;
            res.set_content("Usage: SET <key> <value>", "text/plain");

            return;
        }
        storage.setStorage(parts[1], parts[2]);
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

    void expireCommand(const std::vector<std::string> &parts, httplib::Response &res)
    {
        if (parts.size() != 3)
        {
            res.status = 400;
            res.set_content("Usage: EXPIRE <key> <time(seconds)>", "text/plain");

            return;
        }
        try
        {
            std::size_t pos{};
            int time = std::stoi(parts[2], &pos);

            if (pos != parts[2].size())
            {
                res.status = 400;
                res.set_content("Usage: EXPIRE <key> <time(seconds)>", "text/plain");

                return;
            }
            storage.expireStorage(parts[1], time);
            res.status = 200;
            res.set_content("OK", "text/plain");

            return;
        }
        catch (const std::exception &)
        {
            res.status = 400;
            res.set_content("Usage: EXPIRE <key> <time(seconds)>", "text/plain");

            return;
        }
    };

    void ttlCommand(const std::vector<std::string> &parts, httplib::Response &res)
    {
        if (parts.size() != 2)
        {
            res.status = 400;
            res.set_content("Usage: TTL <key>", "text/plain");

            return;
        }
        auto get = storage.ttlStorage(parts[1]);

        if (!get.has_value())
        {
            res.status = 404;
            res.set_content("(nil)", "text/plain");

            return;
        }
        res.status = 200;
        res.set_content(std::to_string(*get), "text/plain");

        return;
    };

    const std::unordered_map<std::string, Handler> handlers{
        {"GET", &CommandHandler::getCommand},
        {"SET", &CommandHandler::setCommand},
        {"DEL", &CommandHandler::delCommand},
        {"EXPIRE", &CommandHandler::expireCommand},
        {"TTL", &CommandHandler::ttlCommand}};

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
            stream >> std::ws;

            if (stream.peek() == '"')
            {
                if (!(stream >> std::quoted(part)))
                {
                    res.status = 400;
                    res.set_content("Invalid string syntax", "text/plain");

                    return;
                }
                parts.push_back(part);

                stream >> std::ws;

                if (stream.peek() != std::char_traits<char>::eof())
                {
                    res.status = 400;
                    res.set_content("Unexpected argument", "text/plain");

                    return;
                }
                break;
            }
        }

        if (parts.empty())
        {
            res.status = 400;
            res.set_content("No command provided", "text/plain");

            return;
        }

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
