#!/usr/bin/env python3
""" NetSpot macOS Wi-Fi RSSI collector. Reads the Mac's real Wi-Fi RSSI with sudo wdutil info, obtains the latest browser GPS coordinates from the local NetSpot API, and POSTs both to C++. """
import json
import re
import subprocess
import time
from datetime import datetime, timezone
from urllib.request import Request, urlopen
from urllib.error import URLError, HTTPError
BASE = "http://127.0.0.1:8080"
INTERVAL_SECONDS = 10
def get_json(path):
    req = Request(BASE + path, headers={"Cache-Control": "no-store"})
    with urlopen(req, timeout=5) as response:
        return json.loads(response.read().decode("utf-8"))
def get_rssi():
    result = subprocess.run(
        ["sudo", "wdutil", "info"],
        check=True, capture_output=True, text=True, timeout=15
    )
    match = re.search(r"^\s*RSSI\s*:\s*(-?\d+)\s*dBm", result.stdout,
                      flags=re.MULTILINE | re.IGNORECASE)
    if not match:
        raise RuntimeError("Could not find RSSI in sudo wdutil info output.")
    return int(match.group(1))

# Actual internet speed measurement
_last_speed_check = 0
_cached_download = None
_cached_upload = None

def get_network_quality():
    global _last_speed_check, _cached_download, _cached_upload

    # Run speed test only once every 60 seconds
    if time.monotonic() - _last_speed_check < 60:
        return _cached_download, _cached_upload

    try:
        result = subprocess.run(
            ["networkQuality", "-v"],
            capture_output=True,
            text=True,
            timeout=120,
            check=True
        )

        output = result.stdout

        down = re.search(
            r"Downlink capacity:\s*([\d.]+)\s*Mbps",
            output,
            re.IGNORECASE
        )

        up = re.search(
            r"Uplink capacity:\s*([\d.]+)\s*Mbps",
            output,
            re.IGNORECASE
        )

        if down and up:
            _cached_download = float(down.group(1))
            _cached_upload = float(up.group(1))
            _last_speed_check = time.monotonic()

    except (subprocess.SubprocessError, OSError, ValueError) as error:
        print("Network quality error:", error)
    
    return _cached_download, _cached_upload

def post_reading(payload):
    body = json.dumps(payload).encode("utf-8")
    req = Request(
        BASE + "/api/reading",
        data=body,
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    with urlopen(req, timeout=5) as response:
        return response.status, response.read().decode("utf-8")

def get_ping_ms():
    try:
        result = subprocess.run(
            ["ping", "-c", "1", "1.1.1.1"],
            capture_output=True,
            text=True,
            timeout=4
        )

        match = re.search(
            r"time[=<]([\d.]+)\s*ms",
            result.stdout
        )

        if match:
            return float(match.group(1))

        return None

    except (subprocess.TimeoutExpired, OSError):
        return None
    
def main():
    print("NetSpot Mac Wi-Fi collector")
    print("Keep the C++ NetSpot server running and keep the dashboard open.")
    print("Approve the macOS password prompt once if requested.")
    subprocess.run(["sudo", "-v"], check=True)
    while True:
        try:
            data = get_json("/api/readings?limit=20")
            gps = next(
                (item for item in data.get("readings", [])
                 if isinstance(item.get("latitude"), (int, float))
                 and isinstance(item.get("longitude"), (int, float))
                 and item.get("source") == "browser-geolocation"),
                None
            )
            if gps is None:
                print("Waiting for browser GPS. Open http://127.0.0.1:8080 and allow Location.")
                time.sleep(INTERVAL_SECONDS)
                continue
            rssi = get_rssi()
            download_mbps, upload_mbps = get_network_quality();
            ping_ms = get_ping_ms()
            payload = {
                "device": "MacBook",
                "source": "macOS-wdutil",
                "location": gps.get("location", "Current Mac location"),
                "latitude": gps["latitude"],
                "longitude": gps["longitude"],
                "signalStrengthDbm": rssi,
                "downloadMbps": download_mbps,
                "uploadMbps": upload_mbps,
                "latencyMs": ping_ms,
                "timestamp": datetime.now(timezone.utc).isoformat(),
                "simulated": False,
            }
            status, response_text = post_reading(payload)
            print(f"Sent real Wi-Fi RSSI: {rssi} dBm | HTTP {status} | {response_text}")
        except (HTTPError, URLError, subprocess.SubprocessError, RuntimeError, KeyError, ValueError) as exc:
            print(f"Collector error: {exc}")
        except KeyboardInterrupt:
            print("\nNetSpot Wi-Fi collector stopped.")
            break
        time.sleep(INTERVAL_SECONDS)
if __name__ == "__main__":
    main()
