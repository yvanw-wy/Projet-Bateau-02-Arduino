//Include all the libraries used throughout the program
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Servo.h>

#include "bmm150.h"
#include "bmm150_defs.h"

//define global variables that link a pin to any set of characters for future use
#define enA 11
#define in1 6
#define in2 7
#define button 4
#define GPSECHO  true

BMM150 bmm = BMM150();
Servo myservo;
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);
int val;
int rotDirection = 0;
int pressed = false;


const int currentPin = A0;
int sensitivity = 66;
int adcValue= 0;
int offsetVoltage = 2500;
double adcVoltage = 0;
double currentValue = 0;



LiquidCrystal_I2C lcd(0x27,16,2); //Tell the display to use 0x27 as its I2C address, and set it up as 16 column / 2 line


void setup() {


  Serial.begin(115200);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  myservo.attach(9);

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);

  if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
    Serial.println("Magnetometer not found");
    while(1);
  }

  else {

    Serial.println("Magnetometer ready");
  }




  lcd.init();
  lcd.backlight();

  lcd.setCursor(1,0);
  lcd.print(" Boat Project");
  lcd.setCursor(1,1);
  lcd.print("   Group D");

  myservo.write(0);

  delay(2000);

  mySerial.println(PMTK_Q_RELEASE);


}



uint32_t timer = millis();



void loop() {


  char c = GPS.read();
  if ((c) && (GPSECHO))
    Serial.write(c);

  if (GPS.newNMEAreceived()) {

    if (!GPS.parse(GPS.lastNMEA()))
      return;
  }

  if (millis() - timer > 2000) {
    timer = millis();

    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }

  int potValue = analogRead(A7);
  int pwmOutput = map(potValue, 0, 1023, 0 , 255);
  analogWrite(enA, pwmOutput);

  if (digitalRead(button) == true) {
    pressed = !pressed;
  }

  while (digitalRead(button) == true);
  delay(20);

  if ((pressed == true)  & (rotDirection == 0)) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rotDirection = 1;
    delay(20);
  }

  if ((pressed == false) & (rotDirection == 1)) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    rotDirection = 0;
    delay(20);
  }

  lcd.clear();

  bmm150_mag_data value;
  bmm.read_mag_data();
  value.x = bmm.raw_mag_data.raw_datax;
  value.y = bmm.raw_mag_data.raw_datay;
  value.z = bmm.raw_mag_data.raw_dataz;
  float xyHeading = atan2(value.x, value.y);
  float heading = xyHeading;
  if(heading < 0)
  heading += 2*PI;
  if(heading > 2*PI)
  heading -= 2*PI;
  float headingDegrees = heading * 180/M_PI;
  delay(100);

lcd.setCursor(0,0);

  if((337.6 <= headingDegrees && headingDegrees <= 360) || (0 <= headingDegrees && headingDegrees <= 22.5)) {
    lcd.print(headingDegrees);
    lcd.setCursor(15,0);
    lcd.print("N");
  }

  else if(22.51 <= headingDegrees && headingDegrees <= 67.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(14,0);
    lcd.print("NE");
  }

  else if(67.51 <= headingDegrees && headingDegrees <= 112.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(15,0);
    lcd.print("E");
  }

  else if(112.51 <= headingDegrees && headingDegrees <= 157.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(14,0);
    lcd.print("SE");
  }

  else if(157.51 <= headingDegrees && headingDegrees <= 202.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(15,0);
    lcd.print("S");
  }

  else if(202.51 <= headingDegrees && headingDegrees <= 247.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(14,0);
    lcd.print("SW");
  }

  else if(247.51 <= headingDegrees && headingDegrees <= 292.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(15,0);
    lcd.print("W");
  }

  else if(292.51 <= headingDegrees && headingDegrees <= 337.5) {
    lcd.print(headingDegrees);
    lcd.setCursor(14,0);
    lcd.print("NW");
  }

  else {
    lcd.print("error - bad data");
  }


  val = map(headingDegrees, 0, 360, 0, 45);
  myservo.write(val*4);

  lcd.setCursor(0,1);
  lcd.print("Angle:");
  lcd.setCursor(7,1);
  lcd.print(val*4);

  delay(500);

}
