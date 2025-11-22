#include <Arduino.h>
#include "sdControl.h"
#include "config.h"
#include "serial.h"
#include "network.h"
#include "FSWebServer.h"
#include "bluetooth.h"
#include <SPIFFS.h>

void setup() {
  SERIAL_INIT(115200);
  SPIFFS.begin();
  sdcontrol.setup();
  
  // Initialize Bluetooth
  BT.begin("ESP32-SD-WiFi");
  
  network.start();
  server.begin(&SPIFFS);
  
  DEBUG_LOG("Setup complete\n");
}

void loop() {
  network.loop();
  
  // Handle Bluetooth data if available
  if (BT.available()) {
    String data = BT.readString();
    // Echo back for testing
    BT.write("Received: ");
    BT.write(data.c_str());
    BT.write("\n");
  }
}