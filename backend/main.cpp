
#include <iostream>
#include "httplib.h"

using namespace std;

int main()
{
    httplib::Server server;

    server.Get("/health",
        [](const httplib::Request&, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(
                R"({"status":"running","project":"NetSpot"})",
                "application/json"
            );
        });

   server.Get("/api/reading",
    [](const httplib::Request&, httplib::Response& res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");

        res.status = 404;
        res.set_content(
            R"({"error":"No real reading available yet"})",
            "application/json"
        );
    });

    server.Post("/api/reading", [](const httplib::Request& req,
                               httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");

    if (req.body.empty()) {
        res.status = 400;
        res.set_content(
            R"({"error":"Request body is empty"})",
            "application/json"
        );
        return;
    }

    res.set_content(
        R"({"status":"received","message":"Reading received by NetSpot"})",
        "application/json"
    );
});

    cout << "NetSpot API server starting..." << endl;
    cout << "Health: http://127.0.0.1:8080/health" << endl;
    cout << "Reading: http://127.0.0.1:8080/api/reading" << endl;

    if (!server.listen("127.0.0.1", 8080))
    {
        cerr << "Server failed to start. Check port 8080." << endl;
        return 1;
    }

    return 0;
}
