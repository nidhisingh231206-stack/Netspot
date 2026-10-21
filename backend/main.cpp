
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
    // Handle browser CORS preflight requests
server.Options("/api/reading",
    [](const httplib::Request& req, httplib::Response& res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
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

if (!reading.is_object())
{
    res.status = 400;
    res.set_content(
        R"({"error":"JSON must be an object"})",
        "application/json"
    );
    return;
}
            
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

if (latitude < -90 || latitude > 90 ||
    longitude < -180 || longitude > 180)
{
    res.status = 400;
    res.set_content(
        R"({"error":"Latitude or longitude is out of range"})",
        "application/json"
    );
    return;
}
            
if (!reading.contains("downloadMbps") ||
    !reading.contains("uploadMbps") ||
    !reading.contains("latencyMs") ||
    !reading["downloadMbps"].is_number() ||
    !reading["uploadMbps"].is_number() ||
    !reading["latencyMs"].is_number())
{
    res.status = 400;
    res.set_content(
        R"({"error":"Download, upload, and latency must be numeric"})",
        "application/json"
    );
    return;
}

double download = reading["downloadMbps"].get<double>();
double upload = reading["uploadMbps"].get<double>();
double latency = reading["latencyMs"].get<double>();

if (!std::isfinite(download) ||
    !std::isfinite(upload) ||
    !std::isfinite(latency) ||
    download < 0 ||
    upload < 0 ||
    latency < 0)
{
    res.status = 400;
    res.set_content(
        R"({"error":"Network measurements must be finite and non-negative"})",
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
