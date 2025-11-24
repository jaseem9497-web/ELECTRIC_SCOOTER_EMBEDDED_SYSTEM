#define BLYNK_TEMPLATE_ID "TMPL3K-ul4SLM"
#define BLYNK_TEMPLATE_NAME "VEGHA CONNECT "
#define BLYNK_AUTH_TOKEN "w1_kGhoQM1q6sMYleNHr2dRS6KuDk_Qj"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Blynk credentials
char auth[] = "w1_kGhoQM1q6sMYleNHr2dRS6KuDk_Qj";  // Replace with your Blynk Auth Token
char ssid[] = "vegha";         // Replace with your Wi-Fi SSID
char pass[] = "vegha2024";     // Replace with your Wi-Fi password
//reverse 26
// GPIO Pins Declaration
#define ONE_WIRE_BUS 32  // GPIO pin for DS18B20 (change if needed)
#define HALL_SENSOR_PIN 17  // GPIO pin for Hall Effect sensor (change if needed)
#define UV_SENSOR_PIN 34    // GPIO pin for UV sensor (change if needed)
#define SPEED_PIN 33       // GPIO pin for speed sensor (change if needed)
#define VOLTAGE_PIN 34     // GPIO pin for battery voltage (change if needed)

// DS18B20 temperature sensor setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature = 0.0;  // Variable to store temperature reading

// Neo-6M GPS setup
#define RX_PIN 4   // RX pin for GPS module
#define TX_PIN 5   // TX pin for GPS module
SoftwareSerial ss(RX_PIN, TX_PIN);  // Create a software serial object
TinyGPSPlus gps;

// Constants for battery calculations
const float totalBatteryCapacity = 1152.0;  // Total battery capacity in Wh
const float energyConsumptionRate = 28.57;  // Energy consumption rate in Wh/km
const float minBatteryVoltage = 46.0; // Minimum voltage for 0% charge
const float maxBatteryVoltage = 52.0; // Maximum voltage for 100% charge
const float dividerRatio = 54.0 / 3.2; // Voltage divider ratio (54V -> 3.2V)

// Variables for battery and distance calculations
int sensorValue = 0;          // Variable for storing battery sensor reading
float batteryVoltage = 0.0;   // Actual voltage after scaling
float batteryPercentage = 0.0;  // Battery percentage
float remainingBatteryCapacity = 0.0;
float remainingRange = 0.0;
float speed = 0.0;            // Speed from motor controller
float distanceTraveled = 0.0; // Distance traveled in kilometers

// UV sensor value
int uvIntensity = 0; // Variable to store the UV sensor reading

// Thresholds for accelerometer values (adjust these values based on your requirements)
const float accelThreshold = 2.0;  // Threshold for acceleration in m/s^2 (example)

// Variable for detecting alert condition
bool alertFlag = false;           // Flag to indicate if alert condition is met
unsigned long lastAlertTime = 0;  // Timer for the alert condition duration

void setup() {
  // Start Serial Monitor for debugging
  Serial.begin(115200);
  ss.begin(9600);  // Initialize GPS module communication
  
  // Connect to Wi-Fi
  Blynk.begin(auth, ssid, pass);
  
  // Initialize DS18B20 sensor
  sensors.begin();
  
  // Set the ADC width and resolution for battery voltage readings
  analogReadResolution(12);  // 12-bit resolution (4096 levels)
  
  // Set Hall Effect sensor pin as input
  pinMode(HALL_SENSOR_PIN, INPUT);

  // Set UV sensor pin as input
  pinMode(UV_SENSOR_PIN, INPUT);
}

void loop() {
  // Read the battery voltage from the voltage divider
  sensorValue = analogRead(VOLTAGE_PIN);
  
  // Convert the sensor value (0-4095 range) to actual voltage
  batteryVoltage = sensorValue * (3.3 / 4095.0) * dividerRatio;  // Convert to actual voltage
  
  // Apply voltage smoothing (simple moving average filter)
  static float voltageHistory[10];   // Store the last 10 readings
  static int index = 0;
  float sum = 0;
  
  voltageHistory[index] = batteryVoltage;  // Add current reading to history
  index = (index + 1) % 10;                // Circular index for history array
  
  // Calculate the average of last 10 readings
  for (int i = 0; i < 10; i++) {
    sum += voltageHistory[i];
  }
  batteryVoltage = sum / 10.0;  // Smoothed voltage
  
  // Map the battery voltage to percentage (you can adjust this based on your measurements)
  batteryPercentage = map(batteryVoltage, minBatteryVoltage, maxBatteryVoltage, 0, 100);
  
  // Ensure battery percentage stays within 0 to 100 range
  if (batteryPercentage < 0) batteryPercentage = 0;
  if (batteryPercentage > 100) batteryPercentage = 100;
  
  // Calculate the remaining battery capacity
  remainingBatteryCapacity = totalBatteryCapacity * (batteryPercentage / 100.0);
  
  // Calculate the remaining range
  remainingRange = remainingBatteryCapacity / energyConsumptionRate;
  
  // Read the speed from the motor controller
  speed = analogRead(SPEED_PIN) * (5.0 / 1023.0); // Assuming speed is mapped to an analog value (0-1023)
  
  // Convert the speed to km/h if needed
  speed = map(speed, 0, 1023, 0, 100); // Map analog values (0-1023) to speed in km/h
  
  // Calculate the distance traveled
  distanceTraveled += (speed / 3600.0); // Convert speed to km/s (speed in km/h / 3600 seconds)
  
  // Read the temperature from the DS18B20 sensor
  sensors.requestTemperatures(); // Request temperature readings
  temperature = sensors.getTempCByIndex(0); // Get the temperature from the first sensor
  
  // Read the Hall Effect sensor state
  int hallSensorState = digitalRead(HALL_SENSOR_PIN);
  
  // Read the UV sensor value (UV intensity)
  uvIntensity = analogRead(UV_SENSOR_PIN); // Get UV intensity from the sensor
  
  // GPS Data: Read from Neo-6M GPS module
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
  
  if (gps.location.isUpdated()) {
    // Get GPS Latitude and Longitude
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();
    
    // Send GPS data to Blynk
    Blynk.virtualWrite(V5, latitude);    // Latitude to V5
    Blynk.virtualWrite(V6, longitude);   // Longitude to V6
  }
  
  // Send the required values to Blynk
  Blynk.virtualWrite(V0, distanceTraveled);  // Send distance to V0
  Blynk.virtualWrite(V1, remainingRange);    // Send remaining range to V1
  Blynk.virtualWrite(V2, temperature);       // Send temperature to V2
  Blynk.virtualWrite(V3, uvIntensity);      // Send UV intensity to V3
  Blynk.virtualWrite(V4, batteryPercentage); // Send battery percentage to V4
  
  // Wait for 1 second before the next reading
  delay(1000);
}
