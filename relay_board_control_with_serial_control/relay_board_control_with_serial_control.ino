// Define the pins for controlling the relais board
const int RELAY_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9};
// Define pins for reading the analog input data
static const uint8_t analog_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};

// Variables to store the data
int analogValues[8];
bool relayStatus[8];

// variables to store parsed data
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];    
int state = 0;
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

    strtokIndx = strtok(NULL, ","); // get the second part to define the state. This we actually discard for now, because we just want to turn them off
    state = atoi(strtokIndx);
}


void setup() {
  // Set all the digitial pins to control the relais to output and set them to low
  for (int i = 0; i < 8; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
    relayStatus[i] = false;
  }
  // Start the serial communication
  Serial.begin(38400);
  Serial.println("Connection stablished!");
}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    digitalWrite(RELAY_PINS[relay], LOW);
    relayStatus[relay] = false;
  }
  // Here we must read all the analog inputs, possible stored in a list of some kind and then check wheter each one
  // of them changes
  // A second list is needed to keep track of the state of the relais
  // Also a way of restarting each relay individually have to be implemented
  for (int i = 0; i<9; i++){
    analogValues[i] = analogRead(analog_pins[i]);
    delay(10);
    analogValues[i] = analogRead(analog_pins[i]);
    if (analogValues[i] >= 650 && relayStatus[i] == false){
      Serial.println("ON");
      digitalWrite(RELAY_PINS[i], HIGH);
      relayStatus[i] = true;
      delay(50);
    }
    else if (analogValues[i] >= 650 && relayStatus[i] == true){
      Serial.println("OFF");
      digitalWrite(RELAY_PINS[i], LOW);
      relayStatus[i] = false;
      delay(50);
    }
  }
}
