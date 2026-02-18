#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

const char* ssid     = "ArduinoNet";
const char* password = "";

const int udpPort = 4210;
WiFiUDP udp;
char packetBuffer[64];

// define these above setup()
const uint32_t onFrame[3] = {
  0x791cd,
  0x985d8578,
  0x53cd1791
};

const uint32_t offFrame[3] = {
  0x677f4494,
  0x49449779,
  0x44f44644
};

void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.clear();

  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected! My IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(udpPort);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    memset(packetBuffer, 0, sizeof(packetBuffer));
    udp.read(packetBuffer, sizeof(packetBuffer));
    Serial.print("Received: ");
    //Serial.println(packetBuffer);

    if (packetBuffer[0] == '1') {
      Serial.println("ON");
      matrix.loadFrame(onFrame);
      delay(1000);
      matrix.clear();
    } else if (packetBuffer[0] == '0') {
      Serial.println("OFF");
      matrix.loadFrame(offFrame);
      delay(1000);
      matrix.clear();
    }
  }
}