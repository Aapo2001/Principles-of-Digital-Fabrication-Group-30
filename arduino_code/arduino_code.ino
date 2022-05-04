char receivedChar;
boolean newData = false;
int solepin = 2;

void setup() {
    Serial.begin(9600);
    pinMode(solepin, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
      receivedChar = Serial.read();
      Serial.println(receivedChar);
      if(receivedChar == 'A'){
        digitalWrite(solepin, HIGH);
        Serial.println("Solenoid open");
      }
      if(receivedChar == 'B'){
        digitalWrite(solepin, LOW);
        Serial.println("Solenoid closed");
      }
    }
}
