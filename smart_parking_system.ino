#include <WiFi.h>
#include <WebServer.h>

// Replace with your WiFi credentials
const char* ssid = "######";
const char* password = "########";

// Web server runs on port 80
WebServer server(80);

// Define number of slots
#define SLOTS 4  

// IR sensor pins
int irPins[SLOTS] = {34,35,32,26};  // example GPIOs
// LED pins
int ledPins[SLOTS] = {2,4,12,13};  

// Slot states
bool slotOccupied[SLOTS];

void setup() {
  Serial.begin(115200);

  // Initialize pins
  for (int i = 0; i < SLOTS; i++) {
    pinMode(irPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Define web routes
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  // Update slot states
  for (int i = 0; i < SLOTS; i++) {
    int sensorValue = digitalRead(irPins[i]);
    // adjust logic depending on sensor output
    slotOccupied[i] = (sensorValue == LOW);  
    
    digitalWrite(ledPins[i], slotOccupied[i] ? HIGH : LOW);
  }

  server.handleClient();
}

// Webpage
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Smart Parking</title>";
  html += "<meta http-equiv='refresh' content='1'>";
  html += "<style>";
  html += "body{background:#eef2f3;font-family:Arial,Helvetica,sans-serif;text-align:center;margin:0;padding:0;}";
  html += "header{background:linear-gradient(90deg,#003366,#006699);color:white;padding:20px;font-size:24px;font-weight:bold;}";
  html += ".summary{margin:20px auto;padding:15px;width:80%;background:#fff;border-radius:12px;box-shadow:0 4px 10px rgba(0,0,0,0.2);}";
  html += ".summary span{margin:0 15px;font-size:18px;font-weight:bold;}";
  html += ".slots{display:flex;flex-wrap:wrap;justify-content:center;margin-top:20px;}";
  html += ".slot{padding:20px;margin:15px;width:180px;border-radius:15px;font-size:18px;font-weight:bold;box-shadow:0 4px 8px rgba(0,0,0,0.2);transition:0.3s;}";
  html += ".slot:hover{transform:scale(1.05);}";
  html += ".empty{background:#4CAF50;color:white;}";    // Green for empty
  html += ".occupied{background:#F44336;color:white;}"; // Red for occupied
  html += "</style></head><body>";
  
  // Header
  html += "<header>🚗 Smart Parking Dashboard</header>";

  // Summary
  int total = SLOTS, occupied = 0, empty = 0;
  for (int i = 0; i < SLOTS; i++) {
    if (slotOccupied[i]) occupied++;
    else empty++;
  }
  html += "<div class='summary'>";
  html += "<span>Total Slots: " + String(total) + "</span>";
  html += "<span style='color:#4CAF50;'>Empty: " + String(empty) + "</span>";
  html += "<span style='color:#F44336;'>Occupied: " + String(occupied) + "</span>";
  html += "</div>";

  // Slot status cards
  html += "<div class='slots'>";
  for (int i = 0; i < SLOTS; i++) {
    html += "<div class='slot ";
    html += slotOccupied[i] ? "occupied'>" : "empty'>";
    html += "🚗 Slot " + String(i + 1);
    html += slotOccupied[i] ? "<br>Occupied" : "<br>Empty";
    html += "</div>";
  }
  html += "</div>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

