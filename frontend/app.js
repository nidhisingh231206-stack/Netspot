const toast = document.getElementById("toast");
function notify(message) {
    toast.textContent = message;
    toast.style.display = "block";
    clearTimeout(window.toastTimer);
    window.toastTimer = setTimeout(() => {
        toast.style.display = "none";
    }, 2200);
}

// Sidebar navigation


// Network service toggles
document.querySelectorAll(".toggle").forEach(toggle => {
    toggle.addEventListener("click", () => {
        toggle.classList.toggle("off");
        const status = toggle.classList.contains("off")
            ? "OFF"
            : "ON";
        notify("Service status changed to " + status);
    });
});

// Map access point details
document.querySelectorAll(".pin").forEach(pin => {
    pin.addEventListener("click", () => {
        notify("Access point location: " + pin.dataset.place);
    });
});

// Refresh demo readings
document.getElementById("refresh").addEventListener("click", () => {
    const time = new Date().toLocaleTimeString();
    document.querySelectorAll("#readings tr").forEach(row => {
        row.cells[6].title = "Last refreshed: " + time;
    });
    notify("Demo readings refreshed at " + time);
});

// Map zoom
let zoom = 1;
document.getElementById("zoomIn").addEventListener("click", () => {
    zoom = Math.min(zoom + 0.1, 1.5);
    document.getElementById("campusMap").style.backgroundSize =
        (zoom * 100) + "%";
    notify("Map zoom: " + Math.round(zoom * 100) + "%");
});

document.getElementById("zoomOut").addEventListener("click", () => {
    zoom = Math.max(zoom - 0.1, 0.7);
    document.getElementById("campusMap").style.backgroundSize =
        (zoom * 100) + "%";
    notify("Map zoom: " + Math.round(zoom * 100) + "%");
});

// Export table data
document.getElementById("export").addEventListener("click", () => {
    const rows = [...document.querySelectorAll("table tr")];
    const csv = rows.map(row =>
        [...row.querySelectorAll("th, td")]
            .map(cell => '"' + cell.innerText.replace(/"/g, '""') + '"')
            .join(",")
    ).join("\n");
    const blob = new Blob([csv], {
        type: "text/csv;charset=utf-8;"
    });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.href = url;
    link.download = "netspot-readings.csv";
    link.click();
    URL.revokeObjectURL(url);
    notify("CSV file exported");
});

// Live clock
function updateClock() {
    document.getElementById("clock").textContent =
        new Date().toLocaleTimeString("en-IN", {
            hour: "2-digit",
            minute: "2-digit",
            second: "2-digit"
        });
}
updateClock();
setInterval(updateClock, 1000);

// NETSPOT LIVE API INTEGRATION
async function loadReadings() {
    try {
        const response = await fetch(
            "http://127.0.0.1:8080/api/readings?limit=20",
            { cache: "no-store" }
        );

        if (!response.ok) {
            throw new Error("API connection failed");
        }

        const data = await response.json();

        const readings = data.readings || [];
        const table = document.getElementById("readings");

        // Update live metrics
        if (readings.length > 0) {
            const latest = readings[0];

            const metrics = document.querySelectorAll(".metric .value");

            // Signal strength
            if (metrics[2] && latest.signalStrengthDbm != null) {
                metrics[2].innerHTML =
                    latest.signalStrengthDbm + " <small>dBm</small>";
            }

            // Download speed
            if (metrics[3] && latest.downloadMbps != null) {
                metrics[3].innerHTML =
                    Number(latest.downloadMbps).toFixed(2) +
                    " <small>Mbps</small>";
            }
        }

        // Update recent readings
        table.innerHTML = "";

        readings.forEach((item, index) => {
            const row = document.createElement("tr");

            const signal = item.signalStrengthDbm;
            const ping = item.latencyMs;

            let status = "N/A";
            if (signal != null) {
                status = signal >= -60 ? "GOOD" :
                         signal >= -70 ? "FAIR" : "WEAK";
            }

            const values = [
                index + 1,
                item.location || "Unknown",
                item.device || item.source || "Unknown",
                signal != null ? signal + " dBm" : "N/A",
                item.downloadMbps != null
                    ? Number(item.downloadMbps).toFixed(2) + " Mbps"
                    : "N/A",
                ping != null ? ping + " ms" : "N/A",
                status
            ];

            values.forEach(value => {
                const cell = document.createElement("td");
                cell.textContent = value;
                row.appendChild(cell);
            });

            table.appendChild(row);
        });

    } catch (error) {
        console.error("NetSpot API Error:", error);
    }
}

// Initial readings
loadReadings();

// Automatic update every 3 seconds
setInterval(loadReadings, 3000);
// Initial load
loadReadings();
// Refresh every 3 seconds
setInterval(loadReadings, 3000);

navigator.geolocation.watchPosition(async function(position) {
    const gps = {
        source: "browser-geolocation",
        location: "Current MacBook location",
        latitude: position.coords.latitude,
        longitude: position.coords.longitude
    };
    try {
        const response = await fetch("/api/reading", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(gps)
        });
        console.log("GPS sent:", await response.json());
    } catch (error) {
        console.error("GPS API error:", error);
    }
}, function(error) {
    console.error("GPS permission/error:", error.message);
}, {
    enableHighAccuracy: true
});
