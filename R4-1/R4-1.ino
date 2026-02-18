#include <WiFiS3.h>
#include <WiFiUdp.h>

const char* ssid = "ArduinoNet";
const char* password = "";

const int udpPort = 4210;
WiFiUDP udp;

IPAddress receiverIP(192, 168, 4, 2);

const int buttonONPin = 2;   // signal wire
const int buttonONLED = 3;   // led wire

const int buttonOFFPin = 8;   // signal wire
const int buttonOFFLED = 9;   // led wire

bool doON = false;
bool doOFF = false;

bool buttonONLastState = HIGH;
bool buttonOFFLastState = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(buttonONPin, INPUT_PULLUP);
  pinMode(buttonOFFPin, INPUT_PULLUP);
  pinMode(buttonONLED, OUTPUT);
  pinMode(buttonOFFLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("Starting Access Point: ");
  Serial.println(ssid);
  WiFi.beginAP(ssid);
  Serial.print("AP IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(udpPort);
}

void loop() {
  bool buttonONState = digitalRead(buttonONPin);
  bool buttonOFFState = digitalRead(buttonOFFPin);

  if (buttonONLastState == LOW && buttonONState == HIGH) {
    delay(50);
    if (digitalRead(buttonONPin) == HIGH) {  // confirm it's still released
      udp.beginPacket(receiverIP, udpPort);
      udp.print("1");
      udp.endPacket();
      Serial.println("Sent: ON");
    }
  }

  if (buttonOFFLastState == LOW && buttonOFFState == HIGH) {
    delay(50);
    if (digitalRead(buttonOFFPin) == HIGH) {  // confirm it's still released
      udp.beginPacket(receiverIP, udpPort);
      udp.print("0");
      udp.endPacket();
      Serial.println("Sent: OFF");
    }
  }

  buttonONLastState = buttonONState;
  buttonOFFLastState = buttonOFFState;
}