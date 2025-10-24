#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>

// Hotspot
const char* ssid = "YungHub";
const char* password = "yungyung";

// I2C slave addresses for the two Arduinos
#define FRONT_ADDRESS 8
#define BACK_ADDRESS 9

// HTTP server on port 80
WebServer server(80);
// WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Send a command via I2C to both Arduino slaves
void sendI2CCommand(uint8_t command) {
  Serial.print("Sending I2C command: ");
  Serial.println(command);
  
  Wire.beginTransmission(FRONT_ADDRESS);
  Wire.write(command);
  Wire.endTransmission();

  Wire.beginTransmission(BACK_ADDRESS);
  Wire.write(command);
  Wire.endTransmission();
}

void handleRoot() {
  File htmlFile = SPIFFS.open("/index.html");
  if (!htmlFile) {
      server.send(500, "text/plain", "Failed to load page");
      Serial.println("Failed to open index.html!");
      return;
  }
  
  Serial.println("Serving main page:");
  
  // Read and print the content for debugging
  String content = htmlFile.readString();
  // Serial.println(content);  // <--- Check if the content is actually there

  server.send(200, "text/html", content);  // Send the file content as a string
  htmlFile.close();
}

void handleStyle() {
  File htmlFile = SPIFFS.open("/style.css");
  if (!htmlFile) {
      server.send(500, "text/plain", "Failed to load page");
      Serial.println("Failed to open style.css!");
      return;
  }
  
  Serial.println("Serving style:");
  
  // Read and print the content for debugging
  String content = htmlFile.readString();
  // Serial.println(content);  // <--- Check if the content is actually there

  server.send(200, "text/css", content);  // Send the file content as a string
  htmlFile.close();
}

void handleScript() {
  File htmlFile = SPIFFS.open("/script.js");
  if (!htmlFile) {
      server.send(500, "text/plain", "Failed to load page");
      Serial.println("Failed to open style.js!");
      return;
  }
  
  Serial.println("Serving script:");
  
  // Read and print the content for debugging
  String content = htmlFile.readString();
  // Serial.println(content);  // <--- Check if the content is actually there

  server.send(200, "text/js", content);  // Send the file content as a string
  htmlFile.close();
}

void handlePlanner() {
  File htmlFile = SPIFFS.open("/planner.html");
  if (!htmlFile) {
      server.send(500, "text/plain", "Failed to load page");
      Serial.println("Failed to open planner.html!");
      return;
  }
  
  Serial.println("Serving planner:");
  
  // Read and print the content for debugging
  String content = htmlFile.readString();
  // Serial.println(content);  // <--- Check if the content is actually there

  server.send(200, "text/html", content);  // Send the file content as a string
  htmlFile.close();
}

// WebSocket event callback with improved logging and payload conversion
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("[%u] WebSocket Connected\n", num);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u] WebSocket Disconnected\n", num);
      break;
    case WStype_TEXT: {
      // Ensure payload is null-terminated
      char commandStr[32];
      if (length < sizeof(commandStr)) {
        memcpy(commandStr, payload, length);
        commandStr[length] = '\0';
      } else {
        // Too long; truncate.
        memcpy(commandStr, payload, sizeof(commandStr) - 1);
        commandStr[sizeof(commandStr)-1] = '\0';
      }
      Serial.printf("[%u] Received Text: %s\n", num, commandStr);
      int cmd = atoi(commandStr);
      Serial.printf("Converted command: %d\n", cmd);
      sendI2CCommand(cmd);
      break;
    }
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println("SPIFFS Initialized");

  // List all files to verify if they exist
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
      Serial.print("File found: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize I2C bus
  Wire.begin();

  // Set up HTTP server endpoints
  server.on("/", handleRoot);
  server.on("/style.css", handleStyle);
  server.on("/script.js", handleScript);
  server.on("/planner", handlePlanner);
  server.begin();
  Serial.println("HTTP server started.");

  // Start WebSocket server on port 81
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}