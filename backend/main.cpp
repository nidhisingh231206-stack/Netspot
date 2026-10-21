
#include <iostream>
#include <string>
#include "httplib.h"
#include "ReadingList.h"
#include "json.hpp"
#include <cmath>

using json = nlohmann::json;

int main()
{
   httplib::Server server;

    // Linked-list storage for incoming readings
    ReadingList readings;

    // 1. Health check endpoint
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

    // 3. Handle browser CORS preflight requests
    server.Options("/api/reading",
        [](const httplib::Request& req, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.set_header(
                "Access-Control-Allow-Methods",
                "GET, POST, OPTIONS"
            );
        });

    // 4. Receive, validate, and store readings
    server.Post("/api/reading",
        [&readings](const httplib::Request& req, httplib::Response& res)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.set_header(
                "Access-Control-Allow-Methods",
                "GET, POST, OPTIONS"
            );

            // Reject empty requests
            if (req.body.empty())
            {
                res.status = 400;
                res.set_content(
                    R"({"error":"Request body is empty"})",
                    "application/json"
                );
                return;
            }

            // Parse JSON
            json reading;

            try
            {
                reading = json::parse(req.body);
            }
            catch (const json::parse_error&)
            {
                res.status = 400;
                res.set_content(
                    R"({"error":"Invalid JSON"})",
                    "application/json"
                );
                return;
            }

            // Ensure JSON is an object
            if (!reading.is_object())
            {
                res.status = 400;
                res.set_content(
                    R"({"error":"JSON must be an object"})",
                    "application/json"
                );
                return;
            }

            // Validate required coordinates
            if (!reading.contains("latitude") ||
                !reading.contains("longitude") ||
                !reading["latitude"].is_number() ||
                !reading["longitude"].is_number())
            {
                res.status = 400;
                res.set_content(
                    R"({"error":"Valid numeric latitude and longitude are required"})",
                    "application/json"
                );
                return;
            }

            double latitude = reading["latitude"].get<double>();
            double longitude = reading["longitude"].get<double>();

            if (!std::isfinite(latitude) ||
                !std::isfinite(longitude) ||
                latitude < -90 || latitude > 90 ||
                longitude < -180 || longitude > 180)
            {
                res.status = 400;
                res.set_content(
                    R"({"error":"Latitude or longitude is out of range"})",
                    "application/json"
                );
                return;
            }

            // Validate optional network measurements
            for (const char* field :
                 {"downloadMbps", "uploadMbps", "latencyMs"})
            {
                if (reading.contains(field) &&
                    !reading[field].is_null() &&
                    !reading[field].is_number())
                {
                    res.status = 400;
                    res.set_content(
                        R"({"error":"Network measurements must be numeric or null"})",
                        "application/json"
                    );
                    return;
                }

                if (reading.contains(field) &&
                    reading[field].is_number())
                {
                    double value = reading[field].get<double>();

                    if (!std::isfinite(value) || value < 0)
                    {
                        res.status = 400;
                        res.set_content(
                            R"({"error":"Measurements must be finite and non-negative"})",
                            "application/json"
                        );
                        return;
                    }
                }
            }

            // Store the validated JSON in the linked list
            readings.append(req.body);

            res.set_content(
                R"({"status":"received","stored":true})",
                "application/json"
            );
        });

    // 5. Start the server
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
