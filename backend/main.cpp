
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

            res.set_content(
                R"({
                    "location": "Test Location",
                    "signalStrengthDbm": null,
                    "downloadMbps": 86.0,
                    "uploadMbps": 42.0,
                    "latencyMs": 18.0,
                    "latitude": null,
                    "longitude": null,
                    "simulated": true
                })",
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
