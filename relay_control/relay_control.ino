const int RELAY_PIN=2; // digital pin for control the relay
int triggerPin = A1;
int analogPin = A0;
int triggerValue = 0;
int analogValue = 0;
int wtime = 5000; // waiting time in ms
bool relayStatus = false;

void setup() {
  // put your setup code here, to run once:
    pinMode(RELAY_PIN, OUTPUT);
    Serial.begin(38400);
    Serial.println("Connection stablished!");
    
}

void loop() {
  triggerValue = analogRead(triggerPin);
  analogValue = analogRead(analogPin);
  // put your main code here, to run repeatedly:
  if (analogValue >= 650 && relayStatus == false){
    Serial.println("ON");
    digitalWrite(RELAY_PIN, HIGH);
    relayStatus = true;
    delay(200);
  }
  else if (analogValue >= 650 && relayStatus == true){
    Serial.println("OFF");
    digitalWrite(RELAY_PIN, LOW);
    relayStatus = false;
    delay(200);
  }

}
