# 🚭 Smart Non-Smoking Zone Guard (IoT Project)

> **Course:** CPC357 - IoT Architecture & Smart Applications  
> **School:** School of Computer Sciences, Universiti Sains Malaysia  
> **Target SDG:** Goal 11 - Sustainable Cities and Communities




## 📖 Project Overview

The **Smart Non-Smoking Zone Guard** is an IoT-based monitoring system designed to enforce smoke-free policies in smart buildings. Unlike traditional smoke detectors, this system utilizes the **Cytron Maker Feather AIoT S3** and **MQ-2 gas sensors** to distinguish between **Electronic Cigarettes (Vaping)** and **Traditional Tobacco Smoke** based on gas concentration levels.

The system provides real-time feedback via:
**Cloud Visualization:** A web dashboard displaying real-time smoke levels using MQTT protocol.

## ✨ Key Features

* **Dual-Type Detection:** Distinguishes between Vaping (Medium concentration) and Tobacco Smoking (High concentration).
* **Real-time Dashboard:** Visualizes smoke levels and temperature/humidity via a web interface.
* **Optimized Visualization:** Custom Y-axis scale (**0 to 1300**) to clearly show minor fluctuations caused by e-cigarette vapor.

## 🛠️ Hardware Architecture

### Components List
* **Microcontroller:** Cytron Maker Feather AIoT S3 (ESP32-S3)
* **Gas Sensor:** MQ-2 (Smoke, LPG, CO)
* **Env Sensor:** DHT11 (Temperature & Humidity)


### 🔌 Pin Configuration (Wiring)

| Component | Sensor Pin | Connection Type |
| :--- | :--- |  :--- |
| **MQ-2** | A0  |  Analog Input |
| **MQ-2** | VCC |  Power (For better heating) |
| **MQ-2** | GND |  Ground |
| **DHT11** | DATA/SIG |  Digital Input |
| **RGB LED** | - |  Built-in |
| **Buzzer** | - |  Built-in |



## 💻 Software & Logic

### 1. Firmware Logic (ESP32)
The system uses a "Stepped Threshold" approach to categorize smoke types (Calibrated Baseline: ~999):

* **Safe Zone:** `< 800`
    * Status: Safe

* **Vaping Zone:** `800 - 1000` humidity ** `increse > 2.0`
    * Status: Vaping Detected
   
* **Danger Zone:** `> 1000`
    * Status: Tobacco Smoke / Fire
    * Buzzer Alarm 🔊

### 2. Frontend (Web Dashboard)
* **Tech Stack:** HTML5, JavaScript, Chart.js.
* **Protocol:** MQTT over WebSocket (`broker.emqx.io`).
* **Visualization:** Line chart with fixed Y-axis `min: 0` and `max: 1300` to highlight the specific signature of vaping.

## 🚀 Installation & Setup

### Prerequisites
* Arduino IDE (Board: Cytron Maker Feather)
* Libraries: `DHT sensor library`, `Adafruit NeoPixel`, `PubSubClient`.

### Step 1: Firmware Upload
1.  Open `firmware/main.ino` in Arduino IDE.
2.  Update WiFi credentials:
    ```cpp
    const char* ssid = "YOUR_WIFI_NAME";
    const char* password = "YOUR_WIFI_PASSWORD";
    ```
3.  **Verify Pin Definitions:** Ensure `MQ2PIN` and `DHTPIN` match your physical wiring.
4.  Upload to the board.

### Step 2: Web Dashboard
1.  Navigate to the `web/` folder.
2.  Open `index.html` in any modern browser.
3.  node db_saver.js connect to Mango DB.
4.  The dashboard will automatically connect to the public MQTT broker and start streaming data.

## 📊 Testing Instructions

1.  **Power Up:** Connect the board via USB. Wait **3-5 minutes** for the MQ-2 sensor to pre-heat and stabilize (Reading should drop to ~750).
2.  **Vape Test:** Gently blow e-cigarette vapor or hold the sensor above a vapor source.
3.  **Smoke/Gas Test:** Use a lighter (gas only, no flame) near the sensor.


## 👥 Group Members

* **Student 1:** Li Xinfeng 
* **Student 2:** Lao Jun 
