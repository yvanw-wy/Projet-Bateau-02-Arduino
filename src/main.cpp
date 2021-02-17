#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Servo.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <bmm150.h>
#include <bmm150_defs.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 


#define MotorEnable 11
#define in1 6
#define ChangeRotation 4


//WIFI AP
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
//WIFI AP

//GPS
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  true
//GPS

// HTML code
String html ="<!doctype html> <html> <head> <title>Cursor Hover Effects</title> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css\"> <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"> </head> <body> <ul> <li><a href=\"#\"><i class=\"fa fa-gift\" aria-hidden=\"true\"></i></a></li> <li><a href=\"#\"><i class=\"fa fa-glass\" aria-hidden=\"true\"></i></a></li> <li><a href=\"#\"><i class=\"fa fa-heart\" aria-hidden=\"true\"></i></a></li> <li><a href=\"#\"><i class=\"fa fa-globe\" aria-hidden=\"true\"></i></a></li> <li><a href=\"#\"><i class=\"fa fa-graduation-cap\" aria-hidden=\"true\"></i></a></li> <div class=\"cursor\"></div> </ul> <script type=\"text/javascript\"> const cursor = document.querySelector('.cursor'); document.addEventListener('mousemove', (e) => { cursor.style.left = e.pageX + \"px\"; cursor.style.top = e.pageY + \"px\"; }); </script> </body> </html> <style> * { margin: 0; padding: 0; } body { background: #000; display: flex; justify-content: center; align-items: center; min-height: 100vh; } ul { position: relative; display: flex; } ul li { list-style: none; margin: 0 20px; } ul li a { position: relative; display: inline-block; font-size: 2em; text-decoration: none; color: #fff; transition: 0.2s; } ul li:hover a { color: #2196f3; } ul .cursor { position: fixed; width: 0; height: 0; border: 4px solid #fff; border-radius: 50%; transform: translate(-50%,-50%); transition: width 0.2s,height 0.2s; pointer-events: none; } ul li:hover ~ .cursor { transform: translate(-50%,-50%); width: 60px; height: 60px; border: 2px solid #2196f3; } </style>";
//HTML code

void setup() {

  //WIFI AP ------------------------------------------------------------------------

  Serial.begin(9600);
  while (!Serial) {
  }

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    while (true);
  }

  delay(10000);

  server.begin();

  printWiFiStatus();
//WIFI AP ------------------------------------------------------------------------

//GPS ----------------------------------------------------------------------------
 Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
//GPS ----------------------------------------------------------------------------



}


uint32_t timer = millis();


void GPSCODE() {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

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
}

void START_WIFI(){
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println(html);

            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(led, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void SCREENAUTOPILOT() {}

void SCREENREMOTE() {}

void loop() {
  START_WIFI();
}