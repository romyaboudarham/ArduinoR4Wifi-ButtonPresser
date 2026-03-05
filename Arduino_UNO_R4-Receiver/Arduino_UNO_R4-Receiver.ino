#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "Arduino_LED_Matrix.h"
#include <IRremote.h>
#include <Servo.h>

Servo servo;
int servoPos = 90;

const int SERVO_PIN = 9;
const int IR_RECEIVE_PIN = 7;
const int IR_COMMAND_ON = 12;
const int IR_COMMAND_OFF = 24;

ArduinoLEDMatrix matrix;

IPAddress apIP(192, 168, 4, 1);
const char* ssid = "ArduinoNet";
const char* password = "";

const int udpPort = 4210;
WiFiUDP udp;
char packetBuffer[64];

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

  WiFi.begin(ssid); // no password arg — matches what worked before
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected! My IP: ");
  Serial.println(WiFi.localIP()); // confirm this prints 192.168.4.2

  udp.begin(udpPort);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver Ready");

  servo.attach(SERVO_PIN);
  servo.write(90);
}

void resetServo(int delta) {
  for (servoPos = servoPos; servoPos != 90; servoPos += delta) {
    servo.write(servoPos);
    delay(15);
  }
}

void sendUDP(IPAddress ip, const char* message) {
  udp.beginPacket(ip, udpPort);
  udp.print(message);
  udp.endPacket();
}

void moveServoOFF() {
  for (servoPos = 90; servoPos <= 180; servoPos += 1) {
    servo.write(servoPos);
    delay(15);
  }
  resetServo(-1);
  sendUDP(apIP, "CONFIRM_OFF"); // confirm to AP
}

void moveServoON() {
  for (servoPos = 90; servoPos >= 0; servoPos -= 1) {
    servo.write(servoPos);
    delay(15);
  }
  resetServo(1);
  sendUDP(apIP, "CONFIRM_ON"); // confirm to AP
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("IR command: ");
    Serial.println(IrReceiver.decodedIRData.command);
    switch (IrReceiver.decodedIRData.command) {
      case IR_COMMAND_ON:
        Serial.println("IR: ON");
        moveServoON();
        break;
      case IR_COMMAND_OFF:
        Serial.println("IR: OFF");
        moveServoOFF();
        break;
      default:
        break;
    }
    IrReceiver.resume();
  }

  int packetSize = udp.parsePacket();
  if (packetSize) {
    memset(packetBuffer, 0, sizeof(packetBuffer));
    udp.read(packetBuffer, sizeof(packetBuffer));
    Serial.print("UDP Received: ");
    Serial.println(packetBuffer);

    if (packetBuffer[0] == '1') {
      Serial.println("ON");
      matrix.loadFrame(onFrame);
      moveServoON();
      delay(1000);
      matrix.clear();
    } else if (packetBuffer[0] == '0') {
      Serial.println("OFF");
      matrix.loadFrame(offFrame);
      moveServoOFF();
      delay(1000);
      matrix.clear();
    }
  }
}