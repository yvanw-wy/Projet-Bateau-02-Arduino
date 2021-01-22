//Include all the libraries used throughout the program
#include <Wire.h> //Include the Wire.h library
#include <LiquidCrystal_I2C.h> //This libraru controls the display via the I2C bus
#include <Adafruit_GPS.h> //This library controls the GPS
#include <SoftwareSerial.h> //Thi slibrary is called by the Adafruit GPS to send and recieve data
#include <Arduino.h> //
#include <Servo.h> //This library is used to control the servo, which will be connected to our rudder
#include <SPI.h> //This library is needed by WiFiNINA
#include <WiFiNINA.h> //This library enables the creation of a WebServer

#include "bmm150.h"
#include "bmm150_defs.h" //Both of these libraries are called by the magnetometer - No idea why they don't use the regular ²

//define global paths that link a pin to any set of characters for future use
#define enA 11 //Whenever enA is called, it will refer to pin D11 no matter in which function
#define in1 6 //Whenever in1 is called, it will refer to pin D6 no matter in which fucntion
#define in2 7 //Whenever in2 is called, it will refer to pin D7 no matter in which function
#define button 4 //Whenever button is called, it will refer to pin D4 no matter in which loop
#define GPSECHO  true //We define this path as a boolean. The value is set to true

BMM150 bmm = BMM150(); //The string bmm will reffer to the BMM150 function in the library BMM150
Servo myservo; //
SoftwareSerial mySerial(3, 2); //
Adafruit_GPS GPS(&mySerial); //
int val; //We define a globabl variable val, which will always be an integer
int rotDirection = 0; //We define a globabl variable rotDirection, which will always be an integer
int pressed = false; //We define a globabl variable pressed, which will always be an integer (a boolean is considered an integer)


const int currentPin = A0; //we define a global path to A0 - the #define dunction can only be used for digital pins
int sensitivity = 66; //We de fine a global variable sensitivity with a defualt value of 66, which will always be an integer
int adcValue = 0; //We define a globabl variable adcValue with a default value of 0, which will always be an integer
int offsetVoltage = 2500; //We define a globabl variable offsetVoltage with a default value of 2500, which will always be an integer
double adcVoltage = 0;
double currentValue = 0;



LiquidCrystal_I2C lcd(0x27,16,2); //Tell the display to use 0x27 as its I2C address, and set it up as 16 column / 2 line


void setup() {


  Serial.begin(115200); //Starts a serial output with a baud rate of 115200 on the default port

  pinMode(enA, OUTPUT); //Sets the pin mode to output for enA (11)
  pinMode(in1, OUTPUT); //Sets the pin mode to output for in1 (6)
  pinMode(in2, OUTPUT); //Sets the pin mode to output for in2 (7)
  pinMode(button, INPUT); //Sets the pin mode to input for button (4)
  digitalWrite(in1, LOW); //Sets the default output of pin in1 to low (off)
  digitalWrite(in2, HIGH); //Sets the default ouput of pin in2 to high (on)

  myservo.attach(9); //attach the myservo command to pin D9

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);

  if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) { //If the initialization of the magnetometer fails, stop here
    Serial.println("Magnetometer not found");
    while(1);
  }

  else { //If it does not fail

    Serial.println("Magnetometer ready"); //Say it's redy and carry on
  }




  lcd.init(); //Initiate the LCD display
  lcd.backlight(); //Turn on the backlight of the diplay

  lcd.setCursor(1,0); //Place the cursor at position 1,0 (Line 1, column 0)
  lcd.print(" Boat Project"); //Print the sting there
  lcd.setCursor(1,1); //Move the cursor over to position 1,1 (Line 1, column 1)
  lcd.print("   Group D"); //Print this string

  myservo.write(0); //Send 0 via the pin attached to myservo (D9)

  delay(2000); //Wait for 2000ms (2s)

  mySerial.println(PMTK_Q_RELEASE); //Print the output of the command PMTK_Q_RELEASE in the console


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
