#define Current_sensor A0

float i;


void setup() {
    Serial.begin(9600);
    pinMode(Current_sensor, INPUT);

}

void loop() {
  i = analogRead(Current_sensor);
  Serial.println(i);
  delay(100);
}