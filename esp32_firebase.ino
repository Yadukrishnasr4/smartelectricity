#include <WiFi.h>
#include <FirebaseESP32.h>

// WiFi and Firebase Config (REPLACE WITH YOUR OWN)
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
#define FIREBASE_HOST "YOUR_PROJECT_ID.firebaseio.com"
#define FIREBASE_AUTH "YOUR_DATABASE_SECRET"

// Pin Definitions
#define RELAY_PIN 4
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastHistoryPush = 0;
const unsigned long historyInterval = 60000; // 1 minute interval for historical logging

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Start with load ON

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Firebase Init
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Simulate Sensor Readings (Replace with actual sensor code)
  float voltage = 230.0 + random(-5, 5);
  float current = 5.0 + random(-10, 10) / 10.0;
  float power = voltage * current;

  // Real-time Push
  Firebase.setFloat(firebaseData, "/energy_monitoring/voltage", voltage);
  Firebase.setFloat(firebaseData, "/energy_monitoring/current", current);
  Firebase.setFloat(firebaseData, "/energy_monitoring/power", power);

  // Historical Logging (Push to /history list every minute)
  if (millis() - lastHistoryPush > historyInterval) {
    lastHistoryPush = millis();
    FirebaseJson json;
    json.set("power", power);
    json.set("timestamp", String(millis())); // Or use server timestamp with .pushJSON
    Firebase.pushJSON(firebaseData, "/energy_monitoring/history", json);
    Serial.println("Historical data pushed to cloud.");
  }

  // Read Relay & Threshold from Firebase
  if (Firebase.getBool(firebaseData, "/energy_monitoring/relay")) {
    bool relayState = firebaseData.boolData();
    digitalWrite(RELAY_PIN, relayState ? LOW : HIGH);
  }

  delay(3000); // 3-second real-time loop
}
