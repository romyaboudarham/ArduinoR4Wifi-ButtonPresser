#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>

const char* ssid = "ArduinoNet";
const char* password = "";

const int udpPort = 4210;

WiFiUDP udp;
IPAddress receiverIP(192, 168, 4, 2);

const int buttonONPin = 14;
const int buttonONLED = 32;
const int buttonOFFPin = 15;
const int buttonOFFLED = 33;

bool buttonONLastState = HIGH;
bool buttonOFFLastState = HIGH;

NetworkServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(buttonONPin, INPUT_PULLUP);
  pinMode(buttonOFFPin, INPUT_PULLUP);
  pinMode(buttonONLED, OUTPUT);
  pinMode(buttonOFFLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("Starting Access Point: ");
  Serial.println(ssid);

  if (!WiFi.softAP(ssid)) {
    log_e("Soft AP creation failed.");
    while (1);
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(myIP);

  udp.begin(udpPort);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  bool buttonONState = digitalRead(buttonONPin);
  bool buttonOFFState = digitalRead(buttonOFFPin);

  if (buttonONLastState == LOW && buttonONState == HIGH) {
    delay(50);
    if (digitalRead(buttonONPin) == HIGH) {
      udp.beginPacket(receiverIP, udpPort);
      udp.print("1");
      udp.endPacket();
      Serial.println("Sent: ON");
    }
  }

  if (buttonOFFLastState == LOW && buttonOFFState == HIGH) {
    delay(50);
    if (digitalRead(buttonOFFPin) == HIGH) {
      udp.beginPacket(receiverIP, udpPort);
      udp.print("0");
      udp.endPacket();
      Serial.println("Sent: OFF");
    }
  }

  buttonONLastState = buttonONState;
  buttonOFFLastState = buttonOFFState;
}