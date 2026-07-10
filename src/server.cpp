#include <iostream>
#include "httplib.h"

// HTTP
httplib::Server svr;

int main() 
{
    svr.Post("/echo", [](const httplib::Request &req, httplib::Response &res)
    {
        res.set_content(req.body, "text/plain");
    });

    svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) 
    {
        res.set_content("Hello World!", "text/plain");
    });

    svr.Get("/test", [](const httplib::Request &, httplib::Response &res) 
    {
        res.set_content("Buzz OFF", "text/plain");
    });

    svr.listen("0.0.0.0", 8080);
}
