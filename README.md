# 🚀 VEHGA CONNECT – ESP32 Smart Vehicle Monitoring System

This project uses an ESP32 to monitor **battery status, temperature, UV intensity, speed, GPS location, and distance traveled**.  
Data is sent in real-time to the **Blynk IoT app** for remote tracking and display.

## ✅ Features
- Battery voltage monitoring & percentage estimation
- Real-time remaining range calculation (km)
- DS18B20 temperature sensing
- UV intensity measurement
- Distance & speed tracking
- GPS Latitude & Longitude using Neo-6M
- Live data updates to Blynk
- Voltage smoothing using moving-average filter

## 🧰 Hardware Required
| Component | Quantity |
|---|:---:|
| ESP32 Development Board | 1 |
| DS18B20 Temperature Sensor | 1 |
| Hall Effect Sensor | 1 |
| Neo-6M GPS Module | 1 |
| UV Sensor (analog) | 1 |
| Battery voltage divider | 1 |
| Jumper wires | — |

## 📌 Pin Connections

### ✅ Sensor Pins
| Function | GPIO Pin |
|---|---|
| DS18B20 (One-Wire) | 32 |
| Hall Effect Sensor | 17 |
| UV Sensor | 34 |
| Speed Sensor | 33 |
| Battery Voltage Input | 34 |

### ✅ GPS Module (Neo-6M)
| GPS Pin | ESP32 Pin |
|---|---|
| RX | 4 |
| TX | 5 |

## 📦 Required Libraries
Install via Arduino Library Manager:
- BlynkSimpleEsp32
- WiFi
- OneWire
- DallasTemperature
- TinyGPS++
- SoftwareSerial
- Wire
- MPU6050 (included but unused)

## 🔧 Setup Instructions
1. Open the `.ino` file in Arduino IDE
2. Select board:
   **Tools → Board → ESP32 Dev Module**
3. Update Wi-Fi & Blynk credentials:
   ```cpp
   char auth[] = "YOUR_BLYNK_TOKEN";
   char ssid[] = "YOUR_WIFI";
   char pass[] = "YOUR_PASSWORD";
