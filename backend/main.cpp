
#include <iostream>
#include <string>
#include "httplib.h"
#include "ReadingList.h"
#include "json.hpp"

int main()
{
    httplib::Server server;

    // Linked-list storage for incoming readings
    ReadingList readings;

    // 1. Health check
    server.Get("/health",
        [](const httplib::Request& req, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(
                R"({"status":"running","project":"NetSpot"})",
                "application/json"
            );
        });

    // 2. Return the latest stored reading
    server.Get("/api/reading",
        [&readings](const httplib::Request& req, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");

            std::string latest;

            if (!readings.getLatest(latest))
            {
                res.status = 404;
                res.set_content(
                    R"({"error":"No reading stored yet"})",
                    "application/json"
                );
                return;
            }

            res.set_content(latest, "application/json");
        });

    // 3. Receive and store a reading
    server.Post("/api/reading",
        [&readings](const httplib::Request& req, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");

            if (req.body.empty())
            {
                res.status = 400;
                res.set_content(
                    R"({"error":"Request body is empty"})",
                    "application/json"
                );
                return;
            }

            // Store the submitted JSON text in the linked list
            readings.append(req.body);

            res.set_content(
                R"({"status":"received","stored":true})",
                "application/json"
            );
        });

    std::cout << "NetSpot server starting on "
              << "http://127.0.0.1:8080" << std::endl;

    if (!server.listen("127.0.0.1", 8080))
    {
        std::cerr << "Failed to start NetSpot server."
                  << std::endl;
        return 1;
    }

    return 0;
}
