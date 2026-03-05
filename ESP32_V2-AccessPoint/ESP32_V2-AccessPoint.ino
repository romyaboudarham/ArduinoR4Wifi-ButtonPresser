#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>

const char* ssid = "ArduinoNet";
const char* password = "";

const int udpPort = 4210;
WiFiUDP udp;
IPAddress receiverIP(192, 168, 4, 2); // back to fixed IP that worked

const int buttonONPin = 33;
const int buttonONLED = 14;
const int buttonOFFPin = 12;
const int buttonOFFLED = 15;

bool buttonONLastState = HIGH;
bool buttonOFFLastState = HIGH;

bool isOn = false;
bool ledBlinkState = false;
unsigned long lastBlinkTime = 0;

NetworkServer server(80);

void setup() {
  Serial.begin(115200); // back to 115200 that worked
  pinMode(buttonONPin, INPUT_PULLUP);
  pinMode(buttonOFFPin, INPUT_PULLUP);
  pinMode(buttonONLED, OUTPUT);
  pinMode(buttonOFFLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(buttonONLED, LOW);
  digitalWrite(buttonOFFLED, LOW);

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

void checkConfirmation() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char buf[64];
    memset(buf, 0, sizeof(buf));
    udp.read(buf, sizeof(buf));

    if (strcmp(buf, "CONFIRM_ON") == 0) {
      if (!isOn) {
        Serial.println("MISMATCH: syncing AP to ON");
        isOn = true;
      } else {
        Serial.println("Confirmed: ON state matches");
      }
    } else if (strcmp(buf, "CONFIRM_OFF") == 0) {
      if (isOn) {
        Serial.println("MISMATCH: syncing AP to OFF");
        isOn = false;
      } else {
        Serial.println("Confirmed: OFF state matches");
      }
    }
  }
}

void loop() {
  bool buttonONState = digitalRead(buttonONPin);
  bool buttonOFFState = digitalRead(buttonOFFPin);

  if (buttonONLastState == LOW && buttonONState == HIGH) {
    delay(50);
    if (digitalRead(buttonONPin) == HIGH && !isOn) {
      isOn = true;
      udp.beginPacket(receiverIP, udpPort);
      udp.print("1");
      udp.endPacket();
      Serial.println("Sent: ON");
    }
  }

  if (buttonOFFLastState == LOW && buttonOFFState == HIGH) {
    delay(50);
    if (digitalRead(buttonOFFPin) == HIGH && isOn) {
      isOn = false;
      udp.beginPacket(receiverIP, udpPort);
      udp.print("0");
      udp.endPacket();
      Serial.println("Sent: OFF");
    }
  }

  buttonONLastState = buttonONState;
  buttonOFFLastState = buttonOFFState;

  checkConfirmation();

  if (isOn) {
    unsigned long now = millis();
    if (now - lastBlinkTime >= 500) {
      ledBlinkState = !ledBlinkState;
      digitalWrite(buttonONLED, ledBlinkState ? HIGH : LOW);
      lastBlinkTime = now;
    }
    digitalWrite(buttonOFFLED, HIGH);
  } else {
    digitalWrite(buttonONLED, LOW);
    digitalWrite(buttonOFFLED, LOW);
  }
}