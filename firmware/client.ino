#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "DESKTOP-9SAQDQ1 7721";       // Your WiFi name
const char* password = "t685M97="; // Your WiFi password

const char* websocket_server_ip = "192.168.137.1"; // Laptop IP running the Python server
const uint16_t websocket_server_port = 8765;       // Port where the server listens

WebSocketsClient webSocket;

// Sensor settings
const int CH_A = 0;
const int CH_B = 1;
const int CH_C = 3;
const int CH_D = 4;
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 500;  // 10Hz
int zeroA = 0;
int zeroB = 0;
int zeroC = 0;
int zeroD = 0;
int avg = 10; // Number of readings to average on startup to compute offset
float scale = 0.015; 

// LED pin
const int redLED = 10;

// Unique ESP32 ID
const int esp_id = 4;  // CHANGE THIS for each ESP32: 1, 2, 3, 4

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to %s\n", websocket_server_ip);
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] Received: %s\n", payload);
      break;
  }
}

void zero() {
  for (int i = 0; i < avg; i++) { // Average first avg readings
    zeroA += analogRead(CH_A);
    zeroB += analogRead(CH_B);
    zeroC += analogRead(CH_C);
    zeroD += analogRead(CH_D);
    delay(100);
  }
  zeroA /= avg;
  zeroB /= avg;
  zeroC /= avg;
  zeroD /= avg;
}

float process(int raw, int zero) {
  if (raw - zero < 0) {
    return 0;
  }
  return (raw - zero) * scale; 
}

void setup() {
  Serial.begin(115200);

  // LED
  pinMode(redLED, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Setup WebSocket
  webSocket.begin(websocket_server_ip, websocket_server_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000); // Try to reconnect every 5 seconds if disconnected

  // Zero sensors
  zero();
}

void loop() {
  webSocket.loop();  // Handle WebSocket events

  unsigned long now = millis();
  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;

    int A = analogRead(CH_A);
    int B = analogRead(CH_B);
    int C = analogRead(CH_C);
    int D = analogRead(CH_D);

    if (A > 3500 || B > 3500 || C > 3500 || D > 3500) {
      digitalWrite(redLED, HIGH);
    }
    else {
      digitalWrite(redLED, LOW);
    }

    String message = "{\"esp_id\":" + String(esp_id) + ",\"A\":" + String(process(A, zeroA)) + \
    ",\"B\":" + String(process(B, zeroB)) + ",\"C\":" + String(process(C, zeroC)) + ",\"D\":" + String(process(D, zeroD)) + "}";
    
    webSocket.sendTXT(message);
    Serial.println("Sent: " + message);
  }
}
