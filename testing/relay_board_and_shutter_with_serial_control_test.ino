// relay_board_control_with_serial_control.ino
//
// Controlling photocharge-discharge of photocaps or photobatteries
// This code is used to control a relay board with 8 channels and switch between connected and disconencted states.
// there are two modes of control inplemented,
// 1. Using analog inputs provided by the potentiostat (Rising edge time = 90ms) --> for 8 channels this should be 150ms
// 2. Using the serial communication. This is useful to restart certain relays

// TO DO
// send the status of shutters and relays back through serial so it can be displayed in the GUI

// Additional libraries
#include <Servo.h>

const int numChannels = 5; // change this depending on the number of channels to control. **NOTE** this will affect the reading times, and the pulse width must me modified accordingly
// As a rule of thumb, the pulse width must be numChannels*20 - 10 ms
// Define the pins for controlling the relais board
// const int RELAY_PINS[] = {34, 36, 38, 40, 42, 44, 46, 48};
// Here they will control pins with LEDs in the UNO
const int RELAY_PINS[] = {2, 4, 7, 8, 12}; 
// Define pins for reading the analog input data
static const uint8_t analog_pins[] = {A0, A1, A2, A3, A4, A5};
// Define pins for controlling shutters
// const int SHUTTER_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int SHUTTER_PINS[] = {3, 5, 6, 9, 10};
// Create a list with instances of servo objects (shutters)
Servo servos[numChannels];

// Variables to store the data
int analogValues[numChannels];
bool relayStatus[numChannels];
bool shutterStatus[numChannels];

// variables to store parsed data
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];    
int state_relay = 0;
int state_shutter = 0;
int relay = 0;
boolean newData = false;

//============
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}
//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars, ","); //get the first part to define the relay
    relay = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ","); // get the second part to define the state of relay.
    state_relay = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ","); // get the second part to define the state of shutter.
    state_shutter = atoi(strtokIndx);
}

//============

void showParsedData() {
    Serial.print("Relay ");
    Serial.println(relay);
    Serial.print("State relay ");
    Serial.println(state_relay);
    Serial.print("State shutter ");
    Serial.println(state_shutter);
}


//============
// Read the analog channels one at a time and operate the relays and shutters accordingly
void analogReadings() {
  for (int i = 0; i < numChannels; i++){
    analogValues[i] = analogRead(analog_pins[i]);
    delay(10);
    analogValues[i] = analogRead(analog_pins[i]);
    if (analogValues[i] >= 650 && relayStatus[i] == false){
      //Serial.println("ON");
      digitalWrite(RELAY_PINS[i], HIGH);
      relayStatus[i] = true;
      servos[i].write(90);
      shutterStatus[i] = true;
      delay(10);
    }
    else if (analogValues[i] >= 650 && relayStatus[i] == true){
      //Serial.println("OFF");
      digitalWrite(RELAY_PINS[i], LOW);
      relayStatus[i] = false;
      servos[i].write(0);
      shutterStatus[i] = false;
      delay(10);
    }
    delay(10);
  }
}
//============
// Read the serial connection and operate the relay and shutter accordingly
void serialReadings() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    // because strtok() used in parseData() replaces the commas with \0
    parseData();
    if (state_relay == 0){
      digitalWrite(RELAY_PINS[relay], LOW);
      relayStatus[relay] = false;
    }
    if (state_shutter == 0){
      servos[relay].write(0);
      shutterStatus[relay] = false;
    }
    if (state_relay > 0){
      digitalWrite(RELAY_PINS[relay], HIGH);
      relayStatus[relay] = true;
    }
    if (state_shutter > 0){
      servos[relay].write(90);
      shutterStatus[relay] = true;
    }
    newData = false;
  }
}

//=============
// To send the information about the relay and shutter states back to the GUI through serial
// Print a list with the state of relays and shutters
// The integers of the list represent the state. One integer for channel
// 0: both relay and shutter false
// 1: relay on and shutter off
// 2: relay off and shutter on
// 3: both relay and shutter on
void serialWriting(){
  int state = 0;
  Serial.print("<");
  for (int i = 0; i<numChannels; i++){
    if (relayStatus[i] == true && shutterStatus[i] == true){
      state = 3;
    }
    else if (relayStatus[i] == false && shutterStatus[i] == true){
      state = 2;
    }
    else if (relayStatus[i] == true && shutterStatus[i] == false){
      state = 1;
    }
    else {
      state = 0;
    }
    Serial.print(state);
    if (i<numChannels-1){
      Serial.print(",");
    }
  }
  Serial.println(">");
}
//==========

void setup() {
  for (int i = 0; i < numChannels; i++) {
    // Set the pins used for relais as output and all to low, so the start in NC state.
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
    // Update the list with the relay status
    relayStatus[i] = false;
    // Attach shutters to pins and close all of them, moving to 0
    servos[i].attach(SHUTTER_PINS[i]);
    servos[i].write(0);
    // Update the list of shutter status with all closed
    shutterStatus[i] = false;
    // SEt all reading pins to input
    pinMode(analog_pins[i], INPUT);
  }
  // Start the serial communication
  Serial.begin(38400);
  Serial.println("Connection stablished!");
}

void loop() {
  // Read serial inputs and change relays accordingly
  serialReadings();
  //showParsedData();
  // Read all analog inputs and change relays accordingly
  analogReadings();
  serialWriting();
}
