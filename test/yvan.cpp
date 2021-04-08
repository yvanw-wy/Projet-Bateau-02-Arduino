/* @ICAM Lille 2021 (MIA BOOAT PROJECT)
 * 
 * This code creates an interface between Arduino and a predefined HTML page
 * It uses asychronous HTTP requests to only POST datas over the HTML PAGE using Arduino listening on port 80
 * 
 * - Type of Arduino: Wifi Rev 2 with Wifinina firmware
 * - Baud rate: 115200 (Due to the GPS sensor)
 * - Total sketch size: 99%
 *   
 * - Fiability: 100% with an antenna connected to the Arduino and 50% in the opposite case 
 * - Connection stability: Not bad, not good === In the middle
 * 
 */

// Libraries for the Webserver
#define LOW_MEMORY_MCU
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
P(index_html) = "<!DOCTYPE html><html lang=\"en\" style=\"font-family: Arial, Helvetica, sans-serif;font-size: 1.2rem;overflow-x: hidden;\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head><style>*, *::before, *::after {margin: 0;padding:0;box-sizing: border-box; }.hero * {margin-top: 0.5rem;}.layout-1 div:last-of-type {font-size: 0.75rem;}h6, .layout-2 p {font-weight: normal}h6 span, .layout-2 .autonomie_bateau {font-weight: bold;}button:focus, button:focus-within {outline: none;}.hero div .layout-2:last-of-type {margin-bottom: 2rem; }@media only screen and (min-height: 1000px) {.hero {height: 89vh;}.layout-2{ top: 13.5rem;}.footer {position: fixed;bottom: 0;width: 100%;}}.bawl_joystick:focus, .bawl_joystick:focus-within {outline: none;}</style><body style=\"margin: 0;padding: 0;font-size: 1.3rem;\"><div class=\"navbar\" style=\"background: white;display: flex;justify-content: space-between;align-items: center;color: #FCD34D;font-family: cursive;\"><header style=\"padding: 0.8em;background: #374151;flex: 1;justify-content: space-between;display: flex;align-items: center;position: relative;overflow: hidden;\"><a class=\"logo\" href=\"#\" style=\"text-decoration: none;font-size: 0.9rem;color: #FCD34D;\"><h3>Brünhilde</h3></a></header><div class=\"version\" style=\"position: absolute;right: 15px;background: #f4f5ff;padding: 1px 6px;font-size: 0.6rem;border-radius: 14px;color: black;font-family: unset;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 15%), 0 4px 6px -2px rgb(0 0 0 / 5%);\"><div>bêta</div></div><div class=\"current_mode\" style=\"position: absolute;right: 65px;font-size: 0.6rem;color: white;font-family: unset;display: flex;align-items: center;\"><div><span class=\"type_mode\">Radio</span> mode activated</div></div></div><div class=\"hero\" style=\"background: #DBEAFE;display: flex;text-align: center;position: relative;flex-direction: column;\"><div><div class=\"layout-1\" style=\"padding: 2rem;\"><h4 style=\"margin-top: 2em;\">Your boat is at</h4><div><h6>Lat: <span class=\"latitude_gps\">0.0000</span></h6><h6>Lon: <span class=\"longitude_gps\">0.0000</span></h6></div><h4 style=\"font-size: 1.1rem;margin-top: 2.9rem;margin-top: 2em;\">Cap</h4><div><h6><span class=\"cap\">0</span>°</h6></div><div style=\"display: flex;justify-content: space-between\"><p class=\"\"><span class=\"current_consumption\">0</span> A</p><p class=\"\"><span class=\"rotation_speed\">0</span> Rpm</p></div></div></div><div style=\"margin-top: 0;\"><div class=\"layout-2\" style=\"margin:auto;margin-top: 3.35rem;position: relative;padding: 2rem;position: relative;overflow: hidden;\"><div class=\"gauge_viewer mt-2\"><h4 style=\"margin-top: 2em;\">Speed</h4><div>0 m/s</div><h6 style=\"margin-top: 0.25rem;\">Max: <span class=\"max_speed\">3.77 m/s</span></h6></div><div class=\"container_joystick d-flex align-items-center justify-content-between\" style=\"justify-content: space-between;display: none;align-items: center;\"><div class=\"joystick_l\" style=\"width: 80px;height: 80px;border: 2px solid #37415175;top: 50%;border-radius: 50%;position: relative;\"><div class=\"bawl_joystick d-flex align-items-center\" id=\"bawl_joystick_1\" style=\"width: 50px;height: 50px;border-radius: 50%;top: 50%;left: 50%;transform: translate(-50%, -50%);margin-top: 0;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 38%), 0 4px 6px -2px rgb(0 0 0 / 5%);background: white;position: relative;justify-content: center;display: flex;align-items: center;\">T</div></div><div class=\"joystick_l\" style=\"width: 80px;height: 80px;border: 2px solid #37415175;top: 50%;border-radius: 50%;position: relative;\"><div class=\"bawl_joystick align-items-center d-flex\" id=\"bawl_joystick_2\" style=\"width: 50px;height: 50px;border-radius: 50%;top: 50%;left: 50%;transform: translate(-50%, -50%);margin-top: 0;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 38%), 0 4px 6px -2px rgb(0 0 0 / 5%);background: white;position: relative;justify-content: center;display: flex;align-items: center;\">D</div></div></div><button class=\"commad_boat\" data-isOpen=\"No\" style=\"color: white;padding: 0.5rem 0.5rem;font-size: 0.9rem;border: none;margin-top: 1.2rem !important;border-radius: 5px;position: relative;z-index: 2;cursor: pointer;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgba(0,0,0,0.02),0 4px 6px -2px rgba(0,0,0,0.05);background: #FBBF24;\">Command Brünhilde</button><p style=\"font-size: 0.75rem;\"><span class=\"autonomie_bateau\">5h</span> of autonomy</p></div></div></div><div class=\"footer\" style=\"background: #f4f5ff;font-size: 0.75rem;padding: 0.9rem;position: relative;z-index: 1;\"><p style=\"text-align: center;\">&#64; Icam 2021</p></div><script>document.querySelector('button').onclick = function() { if(this.getAttribute(\"data-isOpen\") == \"No\"){document.querySelector(\".container_joystick\").style.display = \"flex\";document.querySelector(\".container_joystick\").style.width = \"320px\";document.querySelector(\".container_joystick\").style.zIndex = \"1\";document.querySelector(\".container_joystick\").style.position = \"absolute\";document.querySelector(\".container_joystick\").style.top = \"0%\";document.querySelector(\".container_joystick\").style.left = \"50%\";document.querySelector(\".container_joystick\").style.transform = \"translate(-50%, -0%)\";this.setAttribute(\"data-isOpen\", \"Yes\");document.querySelector(\"button\").innerHTML = \"Switch to Radio\";document.querySelector(\".type_mode\").innerHTML = \"Wifi\";}else {document.querySelector(\".container_joystick\").style.display = \"none\";this.setAttribute(\"data-isOpen\", \"No\");document.querySelector(\"button\").innerHTML = \"Command brünhilde\";document.querySelector(\".type_mode\").innerHTML = \"Radio\";}}var xhr = new XMLHttpRequest();xhr.onreadystatechange = function() {if(xhr.readyState == 4){if (xhr.status == 200){var tab = xhr.responseText.split(\",\");document.querySelector(\".latitude_gps\").innerHTML = tab[0];document.querySelector(\".longitude_gps\").innerHTML = tab[1];document.querySelector(\".gauge_viewer div\").innerHTML = tab[2] + \" m/s\";document.querySelector(\".cap\").innerHTML = tab[3];document.querySelector(\".current_consumption\").innerHTML = tab[4];document.querySelector(\".rotation_speed\").innerHTML = tab[5];}}if(xhr.status == 404){console.log(\"Error\", xhr.statusText);}}setInterval(function(){xhr.open(\"GET\", \"http://192.168.4.1/datas\", true);xhr.send();}, 100);</script></body></html>";

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


///// Magnetometer //////
BMM150 bmm = BMM150();

///// Supplement variables ///////
uint32_t timer;


//// Variables to be sent over the internet ////
float lat_gps;
float long_gps;
long speed_boat;
int heading;
float current;
uint16_t rpm;


//// Generate the raw html page ////
void index(Request &req, Response &res) {
  //Serial.println("answer to index =)");
  res.set("Status", "HTTP/1.1 200 OK");
  res.set("Content-type", "text/html");
  res.printP(index_html);
}



//// Post datas from the arduino to that html page dynamically //// 
void postDatas(Request &req, Response &res) {
  res.print(String(lat_gps) + "," + String(long_gps) + "," + String(speed_boat) + "," + String(heading) + "," + String(current) + "," + String(rpm));
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Serial.println("Access Point Web Server");
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // local IP address of will be 10.0.0.1
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  //Serial.println("Creating access point named: " + String(ssid));

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
  //Serial.print("To see this page in action, open a browser to http://");
  //Serial.println(ip);



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
    // HALL SENSOR -------------------------------------------------------------------
    pinMode(HALL_SENSOR, INPUT);


    // GPS ----------------------------------------------------------------------------
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


    // MAGNETOMETER ---------------------------------------------------------------------
    if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
      //Serial.println("Chip ID can not read!");
      while(1);
    } else {
      //Serial.println("Initialize done!");
    }
}


//// Magnetometer function ////
float heading_func(){
    bmm150_mag_data value;
    bmm.read_mag_data();
   
    value.x = bmm.raw_mag_data.raw_datax;
    value.y = bmm.raw_mag_data.raw_datay;
    value.z = bmm.raw_mag_data.raw_dataz;
   
    float xyHeading = atan2(value.x, value.y);
    float zxHeading = atan2(value.z, value.x);
    float heading = xyHeading;
   
    if(heading < 0)
      heading += 2*PI;
    if(heading > 2*PI)
      heading -= 2*PI;
    float headingDegrees = heading * 180/M_PI;
    float xyHeadingDegrees = xyHeading * 180 / M_PI;
    float zxHeadingDegrees = zxHeading * 180 / M_PI;
   
    return headingDegrees;
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

      /* For debugging purpose */
      //Serial.println("Latitude: " + String(lat_gps) + ", longitude: " + long_gps + ", Boat speed: " + speed_boat); 
      
    }
}


///// Current function ////
float current_sensor(){
  adcValue = analogRead(currentPin);
  adcVoltage = (adcValue / 1024.0) * 5000;
  currentValue = ((adcVoltage - offsetVoltage) / sensitivity);
  return currentValue;
}


//// Hall function ////
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
      //Serial.println("Device disconnected from AP");
    }
  }

  GPSCODE();
  heading = heading_func();
  current = current_sensor();
  rpm = isNearMagnet();

  // For deebugging purpose
  //Serial.println("Heading: " + String(heading) + ", current: " + String(current) + ", Rpm: " + String(rpm)); 

  WiFiClient client = server.available();

  if (client.connected()) {
    //Serial.println("Serving connected client : "  + String(client));

    app.process(&client);

    // close the connection:
    client.stop();
    //Serial.println("client disconnected");
  }
}
