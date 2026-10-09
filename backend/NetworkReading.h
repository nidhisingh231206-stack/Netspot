
#ifndef NETWORK_READING_H
#define NETWORK_READING_H

#include <string>
#include <optional>
#include <chrono>

class NetworkReading
{
private:
    std::optional<double> signalStrength;
    std::optional<double> downloadSpeed;
    std::optional<double> uploadSpeed;
    std::optional<double> latency;

    std::optional<double> latitude;
    std::optional<double> longitude;

    std::string location;
    std::chrono::system_clock::time_point timestamp;

public:
    NetworkReading(
        std::string location,
        std::optional<double> signalStrength,
        std::optional<double> downloadSpeed,
        std::optional<double> uploadSpeed,
        std::optional<double> latency,
        std::optional<double> latitude,
        std::optional<double> longitude
    );

    void display() const;
    std::string getSignalQuality() const;

    std::optional<double> getSignalStrength() const;
    std::optional<double> getDownloadSpeed() const;
    std::optional<double> getUploadSpeed() const;
    std::optional<double> getLatency() const;

    std::optional<double> getLatitude() const;
    std::optional<double> getLongitude() const;

    std::string getLocation() const;
    long long getTimestamp() const;
};

#endif
