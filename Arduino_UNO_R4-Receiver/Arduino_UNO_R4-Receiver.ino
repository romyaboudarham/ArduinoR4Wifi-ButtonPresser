#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "Arduino_LED_Matrix.h"

#include <Servo.h>

Servo servo;  // create Servo object to control a servo
int servoPos = 90;    // variable to store the servo position

const int SERVO_PIN = 9;

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

  servo.attach(SERVO_PIN);
  servo.write(90);
}

void resetServo(int delta) {
  for (servoPos=servoPos; servoPos != 90; servoPos += delta) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}

void moveServoON() {
  for (servoPos = 90; servoPos <= 180; servoPos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  resetServo(-1);
}

void moveServoOFF() {
  for (servoPos = 90; servoPos >= 0; servoPos -= 1) { // goes from 180 degrees to 0 degrees
    servo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
   resetServo(1);
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