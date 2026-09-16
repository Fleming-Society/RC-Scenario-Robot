#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <WebSocketsServer.h>

// WiFi credentials
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

// HTML page to serve (loads external CSS & JS)
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Omni Directional Robot Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" type="text/css" href="https://raw.flemingsociety.com/robot/style.css">
</head>
<body>
  <div class="logo-container">
    <img src="https://raw.flemingsociety.com/logo.png" alt="Fleming Society Logo">
  </div>
  <div class="container">
    <h1>Omni Directional Robot Control</h1>
    <div class="row">
      <button id="btnUp" onmousedown="buttonPress('up')" onmouseup="buttonRelease()" ontouchstart="buttonPress('up')" ontouchend="buttonRelease()">&#8679;</button>
    </div>
    <div class="row">
      <button id="btnLeft" onmousedown="buttonPress('left')" onmouseup="buttonRelease()" ontouchstart="buttonPress('left')" ontouchend="buttonRelease()">&#8678;</button>
      <div class="spacer"></div>
      <button id="btnRight" onmousedown="buttonPress('right')" onmouseup="buttonRelease()" ontouchstart="buttonPress('right')" ontouchend="buttonRelease()">&#8680;</button>
    </div>
    <div class="row">
      <button id="btnDown" onmousedown="buttonPress('down')" onmouseup="buttonRelease()" ontouchstart="buttonPress('down')" ontouchend="buttonRelease()">&#8681;</button>
    </div>
  </div>
  <script src="https://raw.flemingsociety.com/robot/script.js"></script>
</body>
</html>
)rawliteral";

// HTTP endpoint: serve the main page
void handleRoot() {
  Serial.println("Serving main page");
  server.send(200, "text/html", htmlPage);
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
