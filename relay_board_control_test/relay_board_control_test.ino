// Define the pins for controlling the relais board
const int RELAY_PINS[] = {2, 3, 4, 5, 6, 7, 8, 9};
// Define pins for reading the analog input data
static const uint8_t analog_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
// Variables to store the data
int analogValues[8];
int analogValues_real[8];
bool relayState[8];
int sign = 0;
int i = 0;

void readInput() {
  for (int i = 0; i<9; i++){
    analogValues[i] = analogRead(analog_pins[i]);
    delay(10);
    analogValues[i] = analogRead(analog_pins[i]);
  }
}

void setup() {
  // Set all the digitial pins to control the relais to output and set them to low
  for (int i = 0; i < 9; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], LOW);
    relayState[i] = false;
  }
  // Start the serial communication
  Serial.begin(38400);
  Serial.println("Connection stablished!");
}

void loop() {
  // Here we must read all the analog inputs, possible stored in a list of some kind and then check wheter each one
  // of them changes
  // A second list is needed to keep track of the state of the relais
  // Also a way of restarting each relay individually have to be implemented
  while(Serial.available()){
    sign = Serial.read();
    if(sign>0){
      digitalWrite(RELAY_PINS[i], HIGH);
      relayState[i] = true;
    }
  }
}
