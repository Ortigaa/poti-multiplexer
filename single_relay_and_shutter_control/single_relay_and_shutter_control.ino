// This progam allows to control both a single relay and single servo motor with an analgo signal
// In EC-Lab a trigger technique has to be inserted with rising edge and t=0,2s

#include <Servo.h>

const int RELAY_PIN=34; // digital pin for control the relay
const int shutterPin = 2; // digital pin for control the servo
int analogPin = A0; // Analog pin for reading the analog signal
int analogValue = 0; // value of the analog signal

bool relayStatus = false; // state of relay. false: NC; true: NO

// Define servo object
Servo shutter;
int openPos = 90; // servo position for open
int closePos = 0; // servo position for close
bool shutterStatus = false; // state of the shutter. false: close; true: open

void setup() {
  // put your setup code here, to run once:
    Serial.begin(38400);
    Serial.println("Connection stablished!");
    pinMode(RELAY_PIN, OUTPUT);
    shutter.attach(shutterPin);
    shutter.write(closePos);
    
}

void loop() {
  analogValue = analogRead(analogPin);
  // put your main code here, to run repeatedly:
  if (analogValue >= 650 && relayStatus == false){
    //Serial.println("ON");
    digitalWrite(RELAY_PIN, HIGH);
    relayStatus = true;
    shutter.write(openPos);
    shutterStatus = true;
    delay(20);
  }
  else if (analogValue >= 650 && relayStatus == true){
    //Serial.println("OFF");
    digitalWrite(RELAY_PIN, LOW);
    relayStatus = false;
    shutter.write(closePos);
    shutterStatus = false;
    delay(20);
  }

}
