#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Add ArduinoJson library for JSON creation

// Wi-Fi Credentials
const char* ssid = "Skye Main";        // Replace with your Wi-Fi SSID
const char* password = "11111111";     // Replace with your Wi-Fi Password

// MQTT Broker
const char* mqttServer = "www.somha-iot.com"; // Correct MQTT broker address
const int mqttPort = 1883;                    // Default MQTT port
const char* mqtt_username = "ajbear";
const char* mqtt_password = "ajbear1969";

// MQTT Topic
const char* mqttTopic = "ajbear/bar";

// Define Digital Input Pin for Vibration Sensor
const int Vibration_signal = 35;

WiFiClient espClient;
PubSubClient client(espClient);

// Constants for JSON payload
const char* SITE_ID = "KMb827eb3fe41f";  // Replace with actual site ID
const char* DEVICE_ID = "3"; // Replace with actual device ID
const char* CONNECTION = "MQTT"; // Example connection type

void setup() {
  pinMode(Vibration_signal, INPUT);    // Set pin as input
  Serial.begin(9600);                  // Start the serial communication

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqttServer, mqttPort); // Set the MQTT server
}

void loop() {
  // Reconnect to MQTT broker if not connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Maintain the MQTT connection

  int sensorState = digitalRead(Vibration_signal); // Read the sensor state

  // Get the current date and time in ISO format
  String iso_format = String("2024-11-06T12:34:56Z"); // Replace with actual time source if available
  
  // Create JSON object
  StaticJsonDocument<256> doc;
  doc["siteID"] = SITE_ID;
  doc["deviceID"] = DEVICE_ID;
  doc["date"] = iso_format;
  doc["offset"] = -420;
  doc["connection"] = CONNECTION;

  // Add tagObj array
  JsonArray tagArray = doc.createNestedArray("tagObj");

  // Add first tag object with sensor value
  JsonObject tag1 = tagArray.createNestedObject();
  tag1["status"] = (sensorState == HIGH) ? 1 : 0;;
  tag1["label"] = "status";
  tag1["value"] = sensorState;

  // Add second tag object with a test label
  JsonObject tag2 = tagArray.createNestedObject();
  tag2["status"] = true;
  tag2["label"] = "test";
  tag2["value"] = 0;

  // Convert JSON document to string
  char message[256];
  serializeJson(doc, message);

  // Publish the JSON message to the MQTT topic
  if (client.publish(mqttTopic, message)) {
    Serial.print("Published message: ");
    Serial.println(message);
  } else {
    Serial.println("Failed to publish message");
  }

  delay(500); // Adjust delay as needed
}

// Function to reconnect to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect with credentials
    if (client.connect("ArduinoClient", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(client.state());
      Serial.println(" - Trying again in 2 seconds");
      delay(2000);
    }
  }
}
