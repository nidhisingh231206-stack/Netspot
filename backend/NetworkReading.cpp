
#include <iostream>
#include <iomanip>
#include <ctime>
#include "NetworkReading.h"

using namespace std;

NetworkReading::NetworkReading(
    string location,
    optional<double> signalStrength,
    optional<double> downloadSpeed,
    optional<double> uploadSpeed,
    optional<double> latency,
    optional<double> latitude,
    optional<double> longitude
)
    : signalStrength(signalStrength),
      downloadSpeed(downloadSpeed),
      uploadSpeed(uploadSpeed),
      latency(latency),
      latitude(latitude),
      longitude(longitude),
      location(location),
      timestamp(chrono::system_clock::now())
{
}

string NetworkReading::getSignalQuality() const
{
    if (!signalStrength.has_value())
        return "Unavailable";

    double signal = signalStrength.value();

    if (signal >= -50)
        return "Excellent";
    else if (signal >= -60)
        return "Good";
    else if (signal >= -70)
        return "Fair";
    else
        return "Weak";
}

void NetworkReading::display() const
{
    cout << fixed << setprecision(2);

    cout << "\n========== NETSPOT READING ==========\n";
    cout << "Location: " << location << '\n';

    cout << "Signal strength: ";
    if (signalStrength)
        cout << *signalStrength << " dBm";
    else
        cout << "Unavailable";
    cout << '\n';

    cout << "Signal quality: " << getSignalQuality() << '\n';

    cout << "Download speed: ";
    if (downloadSpeed)
        cout << *downloadSpeed << " Mbps";
    else
        cout << "Unavailable";
    cout << '\n';

    cout << "Upload speed: ";
    if (uploadSpeed)
        cout << *uploadSpeed << " Mbps";
    else
        cout << "Unavailable";
    cout << '\n';

    cout << "Latency: ";
    if (latency)
        cout << *latency << " ms";
    else
        cout << "Unavailable";
    cout << '\n';

    cout << "Latitude: ";
    if (latitude)
        cout << *latitude;
    else
        cout << "Unavailable";
    cout << '\n';

    cout << "Longitude: ";
    if (longitude)
        cout << *longitude;
    else
        cout << "Unavailable";
    cout << '\n';

    cout << "=====================================\n";
}

optional<double> NetworkReading::getSignalStrength() const
{
    return signalStrength;
}

optional<double> NetworkReading::getDownloadSpeed() const
{
    return downloadSpeed;
}

optional<double> NetworkReading::getUploadSpeed() const
{
    return uploadSpeed;
}

optional<double> NetworkReading::getLatency() const
{
    return latency;
}

optional<double> NetworkReading::getLatitude() const
{
    return latitude;
}

optional<double> NetworkReading::getLongitude() const
{
    return longitude;
}

string NetworkReading::getLocation() const
{
    return location;
}

long long NetworkReading::getTimestamp() const
{
    auto seconds = chrono::system_clock::to_time_t(timestamp);
    return static_cast<long long>(seconds);
}
