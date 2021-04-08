an.cpp

/* @ICAM Lille 2021 (MIA BOOAT PROJECT)
 * 
 * This code creates an interface between Arduino and a predefined HTML page
 * It uses asychronous HTTP requests to only POST datas over the HTML PAGE using Arduino listening on port 80
 * 
 * - Type of Arduino: Wifi Rev 2 with Wifinina firmware
 * - Baud rate: 115200 (Due to the GPS sensor)
 * - Total sketch size: Max 92% (With the code of sensors + actionners such as joystick, Button), 
 *   Min 80% (Without the code of sensors) °___°, maxi 95%;
 *   
 * - Fiability: 100% with an antenna connected to the Arduino and 50% in the opposite case 
 * - Connection stability: Not bad, not good === In the middle
 * 
 */

// Libraries for the Webserver
#include <WiFiNINA.h>
#include <aWOT.h>

// Libraries for other sensors (Hall effect sensor, Current sensor, 
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <bmm150.h>
#include <bmm150_defs.h>




////// Variables relatives to the Webserver setup ///////
char ssid[] = "test_5";        // your network SSID (name)
char pass[] = "simple_test";    // your network password
int status = WL_IDLE_STATUS;
WiFiServer server(80);
Application app;


////// Hall Effect sensor /////////
#define HALL_SENSOR 2


////// GPS SENSOR /////////
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  true


////// Current sensor: We just need to get the current value /////////
const int currentPin = A0;
const uint8_t sensitivity = 66;
int adcValue= 0;
int offsetVoltage = 2500;
float adcVoltage = 0;
float currentValue = 0;


///// Supplement variables ///////
uint32_t timer;


//// Variables to be sent over the internet ////
float lat_gps;
float long_gps;
long speed_boat;
int heading;
float current;
int rpm;


//// Generate the raw html page ////
void index(Request &req, Response &res) {
  Serial.println("answer to index =)");
  res.set("Status", "HTTP/1.1 200 OK");
  res.set("Content-type", "text/html");
  res.print("<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta charset=\"utf-8\">\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t</head>\n\t<style>\n\t\tbody {\n\t\t\tmargin: 0;\n\t\t    padding: 0;\n\t\t    font-size: 1.3rem;\n\t\t}\n\t\t*, *::before, *::after {\n\t\t\tmargin: 0;\n\t\t\tpadding:0;\n\t\t\tbox-sizing: border-box; \n\t\t}\n\n\t\thtml {\n\t\t\tfont-family: Arial, Helvetica, sans-serif;\n\t\t\tfont-size: 1.2rem;\n\t\t\toverflow-x: hidden;\n\t\t}\n\n\t\t.navbar a {\n\t\t\tcolor: #FCD34D;\n\t\t}\n\t\theader {\n\t\t\tpadding: 0.8em;\n\t\t\tbackground: #374151;\n\t\t\tflex: 1;\n\t\t}\n\t\t\n\t\t.d-flex {\n\t\t\tdisplay: flex;\n\t\t}\n\t\t.align-items-center {\n\t\t\talign-items: center;\n\t\t}\n\t\t.justify-content-between {\n\t\t\tjustify-content: space-between;\n\t\t}\n\n\t\t.hero {\n\t\t\tflex-direction: column;\n\t\t}\n\t\t.hero * {\n\t\t\tmargin-top: 0.5rem;\n\t\t}\n\n\t\t.layout-1, .layout-2 {\n\t\t\tpadding: 2rem;\n\t\t}\n\n\t\t.layout-1 div:last-of-type {\n\t\t\tfont-size: 0.75rem;\n\t\t}\n\t\th6, .layout-2 p {\n\t\t\tfont-weight: normal\n\t\t}\n\t\th6 span, .layout-2 .autonomie_bateau {\n\t\t\tfont-weight: bold;\n\t\t}\n\t\t.layout-2 p {\n\t\t\tfont-size: 0.75rem;\n\t\t}\n\t\th4 {\n\t\t\tmargin-top: 2em;\n\t\t}\n\t\t.layout-2 {\n\t\t\tmargin:  auto;\n\t\t\tmargin-top: 2rem;\n\t\t\tposition: relative;\n\t\t}\n\n\t\tbutton:focus, button:focus-within {\n\t\t\toutline: none;\n\t\t}\n\n\t\theader, .layout-2 {\n\t\t\tposition: relative;\n\t\t\toverflow:  hidden;\n\t\t}\n\n\t\t.hero div .layout-2:last-of-type {\n\t\t\tmargin-bottom: 2rem; \n\t\t}\n\n\t\t@media only screen and (min-height: 1000px) {\n\t\t\t.hero {\n\t\t\t\theight: 89vh;\n\t\t\t}\n\t\t\t.layout-2{\n\t\t\t    top: 13.5rem;\n\t\t\t}\n\t\t}\n\n\t\t.layout-2 {\n\t\t\tmargin-top: 3.35rem;\n\t\t}\n\t\t.container_joystick {\n\t\t    display: none;\n\t\t}\n\n\t\t.bawl_joystick:focus, .bawl_joystick:focus-within {\n\t\t\toutline: none;\n\t\t}\n\t</style>\n\t<body>\n\t\t<div class=\"navbar\" style=\"background: white;display: flex;justify-content: space-between;align-items: center;color: #FCD34D;font-family: cursive;\">\n\t\t\t<header class=\"d-flex justify-content-between align-items-center\">\n\t\t\t\t<a class=\"logo\" href=\"#\" style=\"text-decoration: none;font-size: 0.9rem;\">\n\t\t\t\t\t<h3>Brünhilde</h3>\n\t\t\t\t</a>\t\n\t\t\t</header>\n\t\t\t<div class=\"version\" style=\"position: absolute;right: 15px;background: #f4f5ff;padding: 1px 6px;font-size: 0.6rem;border-radius: 14px;color: black;font-family: unset;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 15%), 0 4px 6px -2px rgb(0 0 0 / 5%);\">\n\t\t\t\t<div>bêta</div>\n\t\t\t</div>\n\t\t\t<div class=\"current_mode\" style=\"position: absolute;right: 65px;font-size: 0.6rem;color: white;font-family: unset;\tdisplay: flex;align-items: center;\">\n\t\t\t\t<div>\n\t\t\t\t\t<span class=\"type_mode\">Radio</span> mode activated\t\n\t\t\t\t</div>\n\t\t\t</div>\t\n\t\t</div>\n\n\t\t<div class=\"hero\" style=\"background: #DBEAFE;display: flex;text-align: center;position: relative;\">\n\t\t\t<div>\n\t\t\t\t<div class=\"layout-1\">\n\t\t\t\t\t<h4>Your boat is at</h4>\n\t\t\t\t\t<div>\n\t\t\t\t\t\t<h6>Lat: <span class=\"latitude_gps\">0.0000</span></h6>\n\t\t\t\t\t\t<h6>Lon: <span class=\"longitude_gps\">0.0000</span></h6>\n\t\t\t\t\t</div>\n\t\t\t\t\t<h4 style=\"font-size: 1.1rem;margin-top: 2.9rem;\">Cap</h4>\n\t\t\t\t\t<div>\n\t\t\t\t\t\t<h6><span class=\"cap\">0</span>°</h6>\n\t\t\t\t\t</div>\n\t\t\t\t\t<div class=\"justify-content-between d-flex\">\n\t\t\t\t\t\t<p class=\"\"><span class=\"current_consumption\">0</span> A</p>\n\t\t\t\t\t\t<p class=\"\"><span class=\"rotation_speed\">0</span> Rpm</p>\n\t\t\t\t\t</div>\n\t\t\t\t</div>\n\t\t\t</div>\n\t\t\t<div style=\"margin-top: 0;\">\n\t\t\t\t<div class=\"layout-2\">\n\t\t\t\t\t<div class=\"gauge_viewer mt-2\">\n\t\t\t\t\t\t<h4>Speed</h4>\n\t\t\t\t\t\t<div>0 m/s</div>\n\t\t\t\t\t\t<h6 style=\"margin-top: 0.25rem;\">Max: <span class=\"max_speed\">3.77 m/s</span></h6>\n\t\t\t\t\t</div>\n\t\t\t\t\t<div class=\"container_joystick d-flex align-items-center justify-content-between\">\n\t\t\t\t\t\t<div class=\"joystick_l\" style=\"width: 80px;height: 80px;border: 2px solid #37415175;top: 50%;border-radius: 50%;position: relative;\">\n\t\t\t\t\t\t\t<div class=\"bawl_joystick d-flex align-items-center\" id=\"bawl_joystick_1\" style=\"width: 50px;height: 50px;border-radius: 50%;top: 50%;left: 50%;transform: translate(-50%, -50%);margin-top: 0;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 38%), 0 4px 6px -2px rgb(0 0 0 / 5%);background: white;position: relative;justify-content: center;\">T</div>\n\t\t\t\t\t\t</div>\n\t\t\t\t\t\t<div class=\"joystick_l\" style=\"width: 80px;height: 80px;border: 2px solid #37415175;top: 50%;border-radius: 50%;position: relative;\">\n\t\t\t\t\t\t\t<div class=\"bawl_joystick align-items-center d-flex\" id=\"bawl_joystick_2\" style=\"width: 50px;height: 50px;border-radius: 50%;top: 50%;left: 50%;transform: translate(-50%, -50%);margin-top: 0;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 38%), 0 4px 6px -2px rgb(0 0 0 / 5%);background: white;position: relative;justify-content: center;\">D</div>\n\t\t\t\t\t\t</div>\n\t\t\t\t\t</div>\n\t\t\t\t\t<button class=\"commad_boat\" data-isOpen=\"No\" style=\"color: white;padding: 0.5rem 0.5rem;font-size: 0.9rem;border: none;margin-top: 1.2rem !important;border-radius: 5px;position: relative;z-index: 2;cursor: pointer;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgba(0,0,0,0.02),0 4px 6px -2px rgba(0,0,0,0.05);background: #FBBF24;\">Command Brünhilde</button>\n\t\t\t\t\t<p><span class=\"autonomie_bateau\">5h</span> of autonomy</p>\n\t\t\t\t</div>\n\t\t\t</div>\n\t\t</div>\n\t\t\n\t\t<div class=\"footer\" style=\"background: #f4f5ff;font-size: 0.75rem;padding: 0.9rem;position: relative;z-index: 1;\">\n\t\t\t<p style=\"text-align: center;\">&#64; Icam 2021</p>\n\t\t</div>\n\t\n\t\t<script>\n\t\t\tdocument.querySelector('button').onclick = function() {\n\t\t\t   if(this.getAttribute(\"data-isOpen\") == \"No\"){\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.display = \"flex\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.width = \"320px\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.zIndex = \"1\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.position = \"absolute\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.top = \"0%\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.left = \"50%\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.transform = \"translate(-50%, -0%)\";\n\n\n\t\t\t\t\t\n\t\t\t\t\tthis.setAttribute(\"data-isOpen\", \"Yes\");\n\t\t\t\t\tdocument.querySelector(\"button\").innerHTML = \"Switch to Radio\";\n\t\t\t\t\tdocument.querySelector(\".type_mode\").innerHTML = \"Wifi\";\n\n\t\t\t\t}\n\t\t\t\telse {\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.display = \"none\";\n\t\t\t\t\tthis.setAttribute(\"data-isOpen\", \"No\");\n\t\t\t\t\tdocument.querySelector(\"button\").innerHTML = \"Command brünhilde\";\n\t\t\t\t\tdocument.querySelector(\".type_mode\").innerHTML = \"Radio\";\n\n\t\t\t\t}\n\t\t\t}\n\n\t\t\tvar xhr = new XMLHttpRequest();\n\t\t\txhr.onreadystatechange = function() {\n\t\t\t\tif(xhr.readyState == 4){\n\t\t\t\t\tif (xhr.status == 200){\n\t\t\t\t\t\tvar tab = xhr.responseText.split(\",\");\n\t\t\t\t\t\tdocument.querySelector(\".latitude_gps\").innerHTML = tab[0];\n\t\t\t\t\t\tdocument.querySelector(\".longitude_gps\").innerHTML = tab[1];\n\t\t\t\t\t\tdocument.querySelector(\".gauge_viewer div\").innerHTML = tab[2] + \" m/s\";\n\t\t\t\t\t\tdocument.querySelector(\".cap\").innerHTML = tab[3];\n\t\t\t\t\t\tdocument.querySelector(\".current_consumption\").innerHTML = tab[4];\n\t\t\t\t\t\tdocument.querySelector(\".rotation_speed\").innerHTML = tab[5];\n\t\t\t\t\t}\n\t\t\t\t}\n\n\t\t\t\tif(xhr.status == 404){\n\t\t\t\t\tconsole.log(\"Error\", xhr.statusText);\n\t\t\t\t}\n\t\t\t}\n\n\t\t\tsetInterval(function(){\n\t\t\t\txhr.open(\"GET\", \"http://192.168.4.1/datas\", true);\n\t\t\t\txhr.send();\n\t\t\t}, 250);\n\t\t</script>\n\t</body>\n</html>");
}



//// Post datas from the arduino to that html page dynamically //// 
void postDatas(Request &req, Response &res) {
  res.print(String(lat_gps) + "," + String(long_gps) + "," + String(speed_boat) + "," + String(heading) + "," + String(current) + "," + String(rpm));
} 

//// Read datas from the html page dynamically ////

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // local IP address of will be 10.0.0.1
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.println("Creating access point named: " + String(ssid));

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);


  //start web server
  app.get("/", &index);
  app.get("/datas", &postDatas);
  server.begin();

    // you're connected now, so print out where to go to the browser using Wifi's shield Ip address
  IPAddress ip = WiFi.localIP();

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);



  lat_gps = 0;
  long_gps = 0;
  speed_boat = 0;
  heading = 0;
  current = 0;
  rpm = 0;
  
  //// Millis variable /////
  timer = millis();
}


///// Function useful to initialise pin connections /////
void pinsInit()
{
    // MAGNETOMETER -------------------------------------------------------------------
    pinMode(HALL_SENSOR, INPUT);

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
}


//// GPS function ////
void GPSCODE() {
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

    // Since the Web page receive datas over a predefine period of time (each 0.5s), we don't need to use a millis in that case
    if (GPS.fix) {
      /*Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); 
      Serial.print(GPS.lat); */
      lat_gps = GPS.lat;
      
      /*Serial.print(", ");
      Serial.print(GPS.longitude, 4); 
      Serial.println(GPS.lon); */
      long_gps = GPS.lon;
      
      /*Serial.print("Speed (knots): "); 
      Serial.println(GPS.speed); */
      speed_boat = GPS.speed;
      
    }
}


///// Current function ////
float current_sensor(){
  adcValue = analogRead(currentPin);
  adcVoltage = (adcValue / 1024.0) * 5000;
  currentValue = ((adcVoltage - offsetVoltage) / sensitivity);
  return currentValue;
}


//// Magnetometer function ////
boolean isNearMagnet()
{
    int sensorValue = digitalRead(HALL_SENSOR);
    if(sensorValue == LOW)//if the sensor value is LOW?
    {
        return true;//yes,return ture
    }
    else
    {
        return false;//no,return false
    }
}



void loop() {


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

  GPSCODE();
  heading = 0;
  current = current_sensor();
  rpm = isNearMagnet();
  

  WiFiClient client = server.available();

  if (client.connected()) {
    Serial.println("Serving connected client : "  + String(client));

    app.process(&client);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
