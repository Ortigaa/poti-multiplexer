#include <Servo.h>

int analogPin = A0;
int shutterPin = 6;
int analogValue = 0;

Servo shutter;
int openPos = 90;
int closePos = 0;
bool shutterStatus = false;

void setup() {
  Serial.begin(38400);
  Serial.println("Connection Stablished");
  shutter.attach(shutterPin);
  shutter.write(closePos);

}

void loop() {
  analogValue = analogRead(analogPin);
  // put your main code here, to run repeatedly:
  if (analogValue >= 650 && shutterStatus == false){
    Serial.println("ON");
    shutter.write(openPos);
    shutterStatus = true;
    delay(200);
  }
  else if (analogValue >= 650 && shutterStatus == true){
    Serial.println("OFF");
    shutter.write(closePos);
    shutterStatus = false;
    delay(200);
  }
}
