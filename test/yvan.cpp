/* @ICAM Lille 2021 (MIA BOOAT PROJECT)
 * 
 * This code creates an interface between Arduino and a predefined HTML page
 * It uses asychronous HTTP requests to GET and POST datas over the Arduino Web SERVER listening on port 80
 * 
 * - Type of Arduino: Wifi Rev 2 with Wifinina firmware
 * - Total sketch size: Max 92% (With the code of sensors + actionners such as joystick, Button), 
 *   Min 80% (Without the code of sensors) °___°;
 *   
 * - Fiability: Around 90% with an antenna connected to the Arduino and 20% in the opposite case 
 * - Connection stability: Not bad, not good === In the middle
 * 
 */

// As from a Test we interfaced a servo which follows the heading provided by the joystick in the webpage in case of WIFI mode

#define LOW_MEMORY_MCU
#include <WiFiNINA.h>
#include <aWOT.h>
#include <Servo.h> // Servo Library


char ssid[] = "test_5";        // your network SSID (name)
char pass[] = "simple_test";    // your network password
int status = WL_IDLE_STATUS;    // Connection status
boolean incomingData = false;   // Check if there is incomming datas from POST request or not
WiFiServer server(80);  // To start the server on port 80
Application app; // Object referencing the web app
// HTML content
P(index_html) = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta charset=\"utf-8\">\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t</head>\n\t<style>\n\t\tbody {\n\t\t\tmargin: 0;\n\t\t    padding: 0;\n\t\t    font-size: 1.3rem;\n\t\t}\n\t\t*, *::before, *::after {\n\t\t\tmargin: 0;\n\t\t\tpadding:0;\n\t\t\tbox-sizing: border-box; \n\t\t}\n\n\t\thtml {\n\t\t\tfont-family: Arial, Helvetica, sans-serif;\n\t\t\tfont-size: 1.2rem;\n\t\t\toverflow-x: hidden;\n\t\t}\n\n\t\t.navbar a {\n\t\t\tcolor: #FCD34D;\n\t\t}\n\t\theader {\n\t\t\tpadding: 0.8em;\n\t\t\tbackground: #374151;\n\t\t\tflex: 1;\n\t\t}\n\n\t\t.navbar {\n\t\t\tcolor: #FCD34D;\n\t\t\tfont-family: cursive;\n\t\t}\n\t\t\n\t\t.d-flex {\n\t\t\tdisplay: flex;\n\t\t}\n\t\t.align-items-center {\n\t\t\talign-items: center;\n\t\t}\n\t\t.justify-content-between {\n\t\t\tjustify-content: space-between;\n\t\t}\n\n\t\t.hero {\n\t\t\tflex-direction: column;\n\t\t}\n\t\t.hero * {\n\t\t\tmargin-top: 0.5rem;\n\t\t}\n\n\t\t.layout-1, .layout-2 {\n\t\t\tpadding: 2rem;\n\t\t}\n\n\t\t.layout-1 div:last-of-type {\n\t\t\tfont-size: 0.75rem;\n\t\t}\n\t\th6, .layout-2 p {\n\t\t\tfont-weight: normal\n\t\t}\n\t\th6 span, .layout-2 .autonomie_bateau {\n\t\t\tfont-weight: bold;\n\t\t}\n\t\t.layout-2 p {\n\t\t\tfont-size: 0.75rem;\n\t\t}\n\t\th4 {\n\t\t\tmargin-top: 2em;\n\t\t}\n\t\t.layout-2 {\n\t\t\tmargin:  auto;\n\t\t\tmargin-top: 2rem;\n\t\t\tposition: relative;\n\t\t}\n\t\tbutton {\n\t\t\tbox-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgba(0,0,0,0.02),0 4px 6px -2px rgba(0,0,0,0.05);\n\t\t\tbackground: #FBBF24;\n\t\t}\n\n\t\tbutton:hover {\n\t\t\tbackground: #FCD34D;\n\t\t\tbox-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgba(0,0,0,0.1),0 4px 6px -2px rgba(0,0,0,0.05);\n\t\t\t\n\t\t}\n\n\t\tbutton:focus, button:focus-within {\n\t\t\toutline: none;\n\t\t}\n\n\t\t@media only screen and (min-width: 800px) {\n\t\t\th4 {\n\t\t\t\tfont-size: 1.5rem !important;\n\t\t\t}\n\n\t\t\t.hero {\n\t\t\t\theight: 87.4vh;\n\t\t\t}\n\n\t\t\t.unexpected_modifier {\n\t\t\t\tmargin-top: -3rem !important;\n\t\t\t}\n\t\t\t.layout-1 {\n\t\t\t\tmargin-top: -1rem;\n\t\t\t}\n\t\t}\n\n\t\theader, .layout-2 {\n\t\t\tposition: relative;\n\t\t\toverflow:  hidden;\n\t\t}\n\n\t\t@media only screen and (min-width: 900px) {\n\t\t\thtml {\n\t\t\t\toverflow: hidden;\n\t\t\t\theight: 100%;\n\t\t\t}\n\t\t\t.navbar {\n\t\t\t\tposition: relative;\n\t\t\t\tz-index: -2;\n\t\t\t}\n\t\t\t.current_mode {\n\t\t\t\tcolor: black !important;\n\t\t\t}\n\t\t\t.hero {\n\t\t\t\tflex-direction: row;\n\t\t\t}\n\t\t\t.hero > div:last-of-type {\n\t\t\t\theight: unset !important;\n\t\t\t}\t\n\n\t\t\t.hero > div {\n\t\t\t\twidth: 50%;\n\t\t\t}\n\t\t\theader {\n\t\t\t\tmax-width: 45%;\n\t\t\t\tposition: relative;\n\t\t\t\tz-index: -1;\n\t\t\t}\n\n\t\t\t.hero > div:last-of-type {\n\t\t\t\tbackground: white;\n\t\t\t\tz-index: 1;\n\t\t\t\twidth: 55%;\n\t\t\t\tposition: relative;\n\t\t\t}\n\n\t\t\theader::after, .hero > div:last-of-type::after {\n\t\t\t\tposition: absolute;\n\t\t\t\twidth: 100%;\n\t\t\t\theight: 100%;\n\t\t\t\tbackground: white;\n\t\t\t\tz-index: -1;\n\t\t\t\tbottom: 0;\n\t\t\t\tleft: 0;\n\t\t\t\tcontent: \"\";\n\t\t\t\ttransform-origin: right top;\n\t\t\t\ttransform: skewY(75deg);\n\t\t\t}\n\t\t\t.hero > div:first-of-type {\n\t\t\t\tmargin-top:  0;\n\t\t\t\tz-index: 2;\n\t\t\t\twidth: 45%;\n\t\t\t}\n\t\t\t.hero > div:last-of-type::after {\n\t\t\t\ttransform-origin: left top;\n\t\t\t    transform: skewY(75deg);\n\t\t\t    background: #dbeafe;\n\t\t\t}\n\t\t\t.layout-1, .layout-2 {\n\t\t\t\tposition: relative;\n\t\t\t\ttop: 35%;\n\t\t\t\ttransform: translateY(-35%);\n\t\t\t}\n\t\t\n\t\t\t.layout-1 {\n\t\t\t\tmargin-top: 0rem;\n\t\t\t}\n\t\t\t.layout-2{\n\t\t\t\tposition: relative;\n\t\t\t    left: 45px;\n\t\t\t    top: 11.5rem;\n\t\t\t}\n\t\t\t.unexpected_modifier {\n\t\t\t\theight: 87vh;\n\t\t\t\ttransform-origin: top;\n\t\t\t\ttransform: translateY(3rem);\n\t\t\t}\n\n\t\t\t.joystick_l, .joystick_r {\n\t\t\t\tborder: 2px solid #DBEAFE !important;\n\t\t\t}\n\t\t}\n\n\t\t.hero div .layout-2:last-of-type {\n\t\t\tmargin-bottom: 2rem; \n\t\t}\n\n\t\t@media only screen and (min-height: 1000px) {\n\t\t\t.hero {\n\t\t\t\theight: 89vh;\n\t\t\t}\n\t\t\t.layout-2{\n\t\t\t    top: 13.5rem;\n\t\t\t}\n\t\t}\n\n\t\t.layout-2 {\n\t\t\tmargin-top: 3.35rem;\n\t\t}\n\t\t.container_joystick {\n\t\t    display: none;\n\t\t}\n\t\t.joystick_l, .joystick_r {\n\t\t\twidth: 80px;\n\t\t\theight: 80px;\n\t\t\tborder: 2px solid #37415175;\n\t\t\ttop: 50%;\n\t\t\tborder-radius: 50%;\n\t\t\tposition: relative;\n\n\t\t\t/* display: none; */ \n\t\t}\n\n\t\t.bawl_joystick {\n\t\t\twidth: 50px;\n\t\t\theight: 50px;\n\t\t\tborder-radius: 50%;\n\t\t    top: 50%;\n\t\t    left: 50%;\n\t\t    transform: translate(-50%, -50%);\n\t\t    margin-top: 0;\n\t\t    box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 38%), 0 4px 6px -2px rgb(0 0 0 / 5%);\n\t\t    background: white;\n\t\t    position: relative;\n\t\t    justify-content: center;\n\t\t}\n\n\t\t.bawl_joystick:focus, .bawl_joystick:focus-within {\n\t\t\toutline: none;\n\t\t}\n\t\t.bawl_joystick:hover {\n\t\t\tcursor: grab;\n\t\t}\n\t</style>\n\t<body>\n\t\t<div class=\"navbar\" style=\"background: white;display: flex;justify-content: space-between;align-items: center;\t\">\n\t\t\t<header class=\"d-flex justify-content-between align-items-center\">\n\t\t\t\t<a class=\"logo\" href=\"#\" style=\"text-decoration: none;font-size: 0.9rem;\">\n\t\t\t\t\t<h3>Brünhilde</h3>\n\t\t\t\t</a>\t\n\t\t\t</header>\n\t\t\t<div class=\"version\" style=\"position: absolute;right: 15px;background: #f4f5ff;padding: 1px 6px;font-size: 0.6rem;border-radius: 14px;color: black;font-family: unset;box-shadow: 0 0 transparent, 0 0 transparent, 0 10px 15px -3px rgb(0 0 0 / 15%), 0 4px 6px -2px rgb(0 0 0 / 5%);\">\n\t\t\t\t<div>bêta</div>\n\t\t\t</div>\n\t\t\t<div class=\"current_mode\" style=\"position: absolute;right: 65px;font-size: 0.6rem;color: white;font-family: unset;\tdisplay: flex;align-items: center;\">\n\t\t\t\t<div>\n\t\t\t\t\t<span class=\"type_mode\">Radio</span> mode activated\t\n\t\t\t\t</div>\n\t\t\t</div>\t\n\t\t</div>\n\n\t\t<div class=\"hero\" style=\"background: #DBEAFE;display: flex;text-align: center;position: relative;\">\n\t\t\t<div>\n\t\t\t\t<div class=\"layout-1\">\n\t\t\t\t\t<h4>Your boat is at</h4>\n\t\t\t\t\t<div>\n\t\t\t\t\t\t<h6>Lat: <span class=\"latitude_gps\">0.0000</span></h6>\n\t\t\t\t\t\t<h6>Lon: <span class=\"longitude_gps\">0.0000</span></h6>\n\t\t\t\t\t</div>\n\t\t\t\t\t<h4 style=\"font-size: 1.1rem;margin-top: 2.9rem;\">Cap</h4>\n\t\t\t\t\t<div>\n\t\t\t\t\t\t<h6><span class=\"cap\">0</span>°</h6>\n\t\t\t\t\t</div>\n\t\t\t\t\t<div class=\"justify-content-between d-flex\">\n\t\t\t\t\t\t<p class=\"\"><span class=\"current_consumption\">0</span> A</p>\n\t\t\t\t\t\t<p class=\"\"><span class=\"rotation_speed\">0</span> Rpm</p>\n\t\t\t\t\t</div>\n\t\t\t\t</div>\n\t\t\t</div>\n\t\t\t<div style=\"margin-top: 0;\">\n\t\t\t\t<div class=\"layout-2\">\n\t\t\t\t\t<div class=\"gauge_viewer mt-2\">\n\t\t\t\t\t\t<h4>Speed</h4>\n\t\t\t\t\t\t<div>0 m/s</div>\n\t\t\t\t\t\t<h6 style=\"margin-top: 0.25rem;\">Max: <span class=\"max_speed\">3.77 m/s</span></h6>\n\t\t\t\t\t</div>\n\t\t\t\t\t<div class=\"container_joystick d-flex align-items-center justify-content-between\">\n\t\t\t\t\t\t<div class=\"joystick_l\">\n\t\t\t\t\t\t\t<div class=\"bawl_joystick d-flex align-items-center\" id=\"bawl_joystick_1\">T</div>\n\t\t\t\t\t\t</div>\n\t\t\t\t\t\t<div class=\"joystick_l\">\n\t\t\t\t\t\t\t<div class=\"bawl_joystick align-items-center d-flex\" id=\"bawl_joystick_2\">D</div>\n\t\t\t\t\t\t</div>\n\t\t\t\t\t</div>\n\t\t\t\t\t<button class=\"commad_boat\" data-isOpen=\"No\" style=\"color: white;padding: 0.5rem 0.5rem;font-size: 0.9rem;border: none;margin-top: 1.2rem !important;border-radius: 5px;position: relative;z-index: 2;cursor: pointer;\">Command Brünhilde</button>\n\t\t\t\t\t<p><span class=\"autonomie_bateau\">5h</span> of autonomy</p>\n\t\t\t\t</div>\n\t\t\t</div>\n\t\t</div>\n\t\t\n\t\t<div class=\"footer\" style=\"background: #f4f5ff;font-size: 0.75rem;padding: 0.9rem;position: relative;z-index: 1;\">\n\t\t\t<p style=\"text-align: center;\">&#64; Icam 2021</p>\n\t\t</div>\n\t\n\t\t<script>\n\t\t\tvar req = new XMLHttpRequest();\n\t\t\treq.open(\"POST\", \"http://192.168.4.1/\", true);\n\t\t\treq.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n\t\t\treq.send(\"t=\" + 0 + \"&c=\" + 0 + \"&d=\" + \"none\" + \"&r=\" + \"none\" + \"&m=\" + 1);\n\n\t\t\tdocument.querySelector('button').onclick = function() {\n\t\t\t   if(this.getAttribute(\"data-isOpen\") == \"No\"){\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.display = \"flex\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.width = \"320px\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.zIndex = \"1\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.position = \"absolute\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.top = \"0%\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.left = \"50%\";\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.transform = \"translate(-50%, -0%)\";\n\n\n\t\t\t\t\t\n\t\t\t\t\tthis.setAttribute(\"data-isOpen\", \"Yes\");\n\t\t\t\t\tdocument.querySelector(\"button\").innerHTML = \"Switch to Radio\";\n\t\t\t\t\tdocument.querySelector(\".type_mode\").innerHTML = \"Wifi\";\n\n\n\n\t\t\t\t\treq.open(\"POST\", \"http://192.168.4.1/\", true);\n\t\t\t\t\treq.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n\t\t\t\t\treq.send(\"t=\" + 0 + \"&c=\" + 0 + \"&d=\" + \"none\" + \"&r=\" + \"none\" + \"&m=\" + 2);\n\n\t\t\t\t}\n\t\t\t\telse {\n\t\t\t\t\tdocument.querySelector(\".container_joystick\").style.display = \"none\";\n\t\t\t\t\tthis.setAttribute(\"data-isOpen\", \"No\");\n\t\t\t\t\tdocument.querySelector(\"button\").innerHTML = \"Command brünhilde\";\n\t\t\t\t\tdocument.querySelector(\".type_mode\").innerHTML = \"Radio\";\n\n\n\t\t\t\t\treq.open(\"POST\", \"http://192.168.4.1/\", true);\n\t\t\t\t\treq.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n\t\t\t\t\treq.send(\"t=\" + 0 + \"&c=\" + 0 + \"&d=\" + \"none\" + \"&r=\" + \"none\" + \"&m=\" + 1);\n\t\t\t\t}\n\t\t\t}\n\n\n\t\t\tclass JoystickController\n\t\t\t{\n\t\t\t\tconstructor( stickID, maxDistance, deadzone, stick_type)\n\t\t\t\t{\n\t\t\t\t\tthis.id = stickID;\n\t\t\t\t\tlet stick = document.getElementById(stickID);\n\t\t\t\t\tthis.dragStart = null;\n\t\t\t\t\tthis.touchId = null;\n\t\t\t\t\tthis.active = false;\n\t\t\t\t\tthis.value = { x: 0, y: 0 };\n\t\t\t\t\tlet self = this;\n\n\t\t\t\t\tfunction handleDown(event)\n\t\t\t\t\t{\n\t\t\t\t\t    self.active = true;\n\t\t\t\t\t\tstick.style.transition = '0s';\n\t\t\t\t\t\tevent.preventDefault();\n\n\t\t\t\t\t    if (event.changedTouches)\n\t\t\t\t\t    \tself.dragStart = { x: event.changedTouches[0].clientX, y: event.changedTouches[0].clientY };\n\t\t\t\t\t    else\n\t\t\t\t\t    \tself.dragStart = { x: event.clientX, y: event.clientY };\n\t\t\t\t\t    if (event.changedTouches)\n\t\t\t\t\t    \tself.touchId = event.changedTouches[0].identifier;\n\t\t\t\t\t}\n\n\t\t\t\t\tfunction handleMove(event)\n\t\t\t\t\t{\n\t\t\t\t\t    if ( !self.active ) return;\n\t\t\t\t\t\t    let touchmoveId = null;\n\t\t\t\t\t\t    if (event.changedTouches)\n\t\t\t\t\t\t    {\n\t\t\t\t\t\t    \tfor (let i = 0; i < event.changedTouches.length; i++)\n\t\t\t\t\t\t    \t{\n\t\t\t\t\t\t    \t\tif (self.touchId == event.changedTouches[i].identifier)\n\t\t\t\t\t\t    \t\t{\n\t\t\t\t\t\t    \t\t\ttouchmoveId = i;\n\t\t\t\t\t\t    \t\t\tevent.clientX = event.changedTouches[i].clientX;\n\t\t\t\t\t\t    \t\t\tevent.clientY = event.changedTouches[i].clientY;\n\t\t\t\t\t\t    \t\t}\n\t\t\t\t\t\t    \t}\n\n\t\t\t\t\t\t    \tif (touchmoveId == null) return;\n\t\t\t\t\t\t    }\n\t\t\t\t\t\t    const xDiff = event.clientX - self.dragStart.x;\n\t\t\t\t\t\t    const yDiff = event.clientY - self.dragStart.y;\n\t\t\t\t\t\t    const angle = Math.atan2(yDiff, xDiff);\n\t\t\t\t\t\t\tconst distance = Math.min(maxDistance, Math.hypot(xDiff, yDiff));\n\t\t\t\t\t\t\tvar xPosition;\n\t\t\t\t\t\t\tvar yPosition;\n\t\t\t\t\t\t\tif(stick_type == 1){\n\t\t\t\t\t\t\t\txPosition = 0;\n\t\t\t\t\t\t\t\tyPosition = distance * Math.sin(angle);\t\n\t\t\t\t\t\t\t}\n\t\t\t\t\t\t\telse {\n\t\t\t\t\t\t\t\txPosition = distance * Math.cos(angle);\n\t\t\t\t\t\t\t\tyPosition = 0;\n\t\t\t\t\t\t\t}\n\t\t\t\t\t    stick.style.transform = `translate3d(${xPosition + (- 25)}px, ${yPosition+ (- 25)}px, 0px)`;\n\t\t\t\t\t\tconst distance2 = (distance < deadzone) ? 0 : maxDistance / (maxDistance - deadzone) * (distance - deadzone);\n\t\t\t\t\t    const xPosition2 = distance2 * Math.cos(angle);\n\t\t\t\t\t\tconst yPosition2 = distance2 * Math.sin(angle);\n\t\t\t\t\t    const xPercent = parseFloat((xPosition2 / maxDistance).toFixed(4));\n\t\t\t\t\t    const yPercent = parseFloat((yPosition2 / maxDistance).toFixed(4));\n\t\t\t\t\t    self.value = { x: xPercent, y: yPercent };\n\n\t\t\t\t\t    var throttle = parseInt(255*Math.abs(yPosition)/29);\n\t\t\t\t\t    var angle_f = parseInt(90*Math.abs(xPosition)/29);\n\t\t\t\t\t    if(throttle > 255){\n\t\t\t\t\t    \tthrottle = 255;\n\t\t\t\t\t    }\n\t\t\t\t\t    else if(throttle < 0){\n\t\t\t\t\t    \tthrottle = 0;\n\t\t\t\t\t    }\n\t\t\t\t\t    if(angle_f > 90){\n\t\t\t\t\t    \tangle_f = 90;\n\t\t\t\t\t    }\n\t\t\t\t\t    else if(angle_f < 0){\n\t\t\t\t\t    \tangle_f = 0;\n\t\t\t\t\t    }\n\n\t\t\t\t\t    var direction = xPosition > 0 ? \"Right\" : \"Left\";\n\t\t\t\t\t    var rotation = yPosition > 0 ? \"backward\" : \"Forward\";\n\t\t\t\t\t\t\n\t\t\t\t\t    console.log(angle_f);\n\n\t\t\t\t\t\treq.open(\"POST\", \"http://192.168.4.1/\", true);\n\t\t\t\t\t\treq.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n\t\t\t\t\t\treq.send(\"t=\" + throttle + \"&c=\" + angle_f + \"&d=\" + direction + \"&r=\" + rotation + \"&m=\" + 2);\n\t\t\t\t\t  }\n\n\t\t\t\t\tfunction handleUp(event)\n\t\t\t\t\t{\n\t\t\t\t\t    if ( !self.active ) return;\n\t\t\t\t\t    if (event.changedTouches && self.touchId != event.changedTouches[0].identifier) return;\n\t\t\t\t\t    stick.style.transition = '.2s';\n\t\t\t\t\t    stick.style.transform = `translate3d(-25px, -25px, 0px)`;\n\t\t\t\t\t    self.value = { x: 0, y: 0 };\n\t\t\t\t\t    self.touchId = null;\n\t\t\t\t\t    self.active = false;\n\n\t\t\t\t\t    var throttle = 0;\n\t\t\t\t\t    var angle_f = 0;\n\t\t\t\t\t    var direction = \"Left\";\n\t\t\t\t\t    var rotation = \"Forward\";\n\n\n\t\t\t\t\t    req.open(\"POST\", \"http://192.168.4.1/\", true);\n\t\t\t\t\t\treq.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n\t\t\t\t\t\treq.send(\"t=\" + throttle + \"&c=\" + angle_f + \"&d=\" + direction + \"&r=\" + rotation + \"&m=\" + 2);\n\t\t\t\t\t}\n\n\t\t\t\t\tstick.addEventListener('mousedown', handleDown);\n\t\t\t\t\tstick.addEventListener('touchstart', handleDown);\n\t\t\t\t\tdocument.addEventListener('mousemove', handleMove, {passive: false});\n\t\t\t\t\tdocument.addEventListener('touchmove', handleMove, {passive: false});\n\t\t\t\t\tdocument.addEventListener('mouseup', handleUp);\n\t\t\t\t\tdocument.addEventListener('touchend', handleUp);\n\t\t\t\t    \n\t\t\t\t}\n\t\t\t}\n\n\t\t\tvar xhr = new XMLHttpRequest();\n\t\t\txhr.onreadystatechange = function() {\n\t\t\t\tif(xhr.readyState == 4){\n\t\t\t\t\tif (xhr.status == 200){\n\t\t\t\t\t\tvar tab = xhr.responseText.split(\",\");\n\t\t\t\t\t\tdocument.querySelector(\".latitude_gps\").innerHTML = tab[0];\n\t\t\t\t\t\tdocument.querySelector(\".longitude_gps\").innerHTML = tab[1];\n\t\t\t\t\t\tdocument.querySelector(\".gauge_viewer div\").innerHTML = tab[2] + \" m/s\";\n\t\t\t\t\t\tdocument.querySelector(\".cap\").innerHTML = tab[3];\n\t\t\t\t\t\tdocument.querySelector(\".current_consumption\").innerHTML = tab[4];\n\t\t\t\t\t\tdocument.querySelector(\".rotation_speed\").innerHTML = tab[5];\n\t\t\t\t\t}\n\t\t\t\t}\n\n\t\t\t\tif(xhr.status == 404){\n\t\t\t\t\tconsole.log(\"Error\", xhr.statusText);\n\t\t\t\t}\n\t\t\t}\n\n\t\t\tsetInterval(function(){\n\t\t\t\txhr.open(\"GET\", \"http://192.168.4.1/datas\", true);\n\t\t\t\txhr.send();\n\t\t\t}, 250);\n\n\t\t\tlet myStick_1 = new JoystickController(\"bawl_joystick_1\", 30, 2, 1);\n\t\t\tlet myStick_2 = new JoystickController(\"bawl_joystick_2\", 30, 2, 0);\n\t\t</script>\n\t</body>\n</html>";


///// Supplement variables ///////
long available_time; // Estimated available time if the user no later receive datas from the app in case of web-server controll


// Variables to the webserver: To be modified in the code by their appropriate values ----------------------------
float latitude;
float longitude;
long speed_boat;
int heading;
float current;
int rpm;

//// Variables to get from the webserver ------------------------------------------------
int throttleVal;
int headingVal;
int typeVal;
String directionVal = "";
String rotationVal = "";


Servo servo; // Servo object instanciated
int startingValue_servo; // Starting value of the servo


//// Generate the raw html page ////
void index(Request &req, Response &res) {
  Serial.println("answer to index =)");
  res.set("Status", "HTTP/1.1 200 OK");
  res.set("Content-type", "text/html");
  res.printP(index_html);
}


//// Post datas from the arduino to that html page dynamically //// 
void postDatas(Request &req, Response &res) {
  res.print(String(latitude) + "," + String(longitude) + "," + String(speed_boat) + "," + String(heading) + "," + String(current) + "," + String(rpm));
} 

//// Custom message to receive a POST request ////
void getDatas(Request &req, Response &res) {
  res.print("Received");
} 

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
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

  // Check if the firware has been updated or not
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // local IP address of will be 10.0.0.1
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  //// Initialisation of pins ////
  pins_init();
  
  //// Here we assume it will make 10 seconds to the user to connect to the new Access Point ///
  delay(10000);


  //// Start Web Server, Send periodically datas to it over a period of 0.25s, Get datas Asynchronously
  app.get("/", &index);
  app.get("/datas", &postDatas);
  app.post("/", &getDatas);
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();


  //// Initialisation of variables
  latitude = 0;
  longitude = 0;
  speed_boat = 0;
  heading = 0;
  current = 0;
  rpm = 0;

  throttleVal = 0;
  headingVal = 0;
  typeVal = 0;
  
  //// Millis variable /////
  available_time= millis();
}


//// Premade function which connects all the available sensors and actionners (servo, DC motor)
void pins_init() {
  servo.attach(6);
  startingValue_servo = servo.read(); //// Initialise it at position of the servo;
  servo.write(startingValue_servo); ////Moving the servo to that position
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

  //// Here we must change those raw values with those of the sensors ////
  latitude = latitude + 1.00000005;
  longitude = longitude + 1.0000000005;
  speed_boat = speed_boat + 0.00005;
  heading = heading + 0.05;
  current++;
  rpm++;

  //// Creation of the client object
  WiFiClient client = server.available();

  //// Check if a new client is connected or not and get the body of continuous POST request ////
  if (client.connected()) {
    // Serial.println("Serving connected client : "  + String(client));
    app.process(&client);
    
    
    
    
    
    
    
    // Receive POST datas from the webpage and treat them only if the user is still connected to the AP
    // Fiability: 50% (because of incomming deconnection of all users from the access point after 15s of manoeuvrability
    // Part to be reviewed for further optimisation
    String get_values = ""; // buffer to store datas comming from the user side
    while (client.available() && status == WL_AP_CONNECTED) {
      if (incomingData == false)
      {
         Serial.println();
         incomingData = true;
      }
      // Read post datas //
      char c = client.read();
      get_values += c;
    }
    
    //// If we actually ////
    if(get_values != ""){
        // Parse the values  
        
        int startIndex = get_values.indexOf("t");
        int endIndex = get_values.indexOf("c");
        String throttleStr = get_values.substring(startIndex + 2, endIndex - 1);
        char tempThrottle[4];
        throttleStr.toCharArray(tempThrottle, sizeof(tempThrottle));
        throttleVal = atoi(tempThrottle);
        
        startIndex = get_values.indexOf("c");
        endIndex = get_values.indexOf("d");
        String capStr = get_values.substring(startIndex + 2, endIndex -1);
        char tempCap[4];
        capStr.toCharArray(tempCap, sizeof(tempCap));
        headingVal = atoi(tempCap);
        
        startIndex = get_values.indexOf("d");
        endIndex = get_values.indexOf("r");
        String directionStr = get_values.substring(startIndex + 2, endIndex -1);
        char tempDirection[4];
        directionStr.toCharArray(tempDirection, sizeof(tempDirection));
        directionVal = String(tempDirection);

        startIndex = get_values.indexOf("r");
        endIndex = get_values.indexOf("m");
        String rotationStr = get_values.substring(startIndex + 2, endIndex -1);
        char tempRotation[4];
        rotationStr.toCharArray(tempRotation, sizeof(tempRotation));
        rotationVal = String(tempRotation);

        startIndex = get_values.indexOf("m");
        String modeStr = get_values.substring(startIndex + 2);
        char tempMode[4];
        modeStr.toCharArray(tempMode, sizeof(tempMode));
        typeVal = atoi(tempMode);

        // Format of values: throttleVal[0 to 255], RotationVal[For = Forward mode, bac = Backward mode], headingVal[0 90], , directionVal[Lef, Rig].  
        // For debugging purpose 
        // Serial.println("Throttle: " + throttleVal + ", heading: " + headingVal + "\n");


        
        // Dealing with those values ------------------------------------
        if(typeVal == 1){
          // Here we're still on RADIO MODE
          // We must put the associated code down below (for controlling the Servo and the 
        }
        
        else { 
          // Here we're on WIFI MODE
          // As a test, we implemented a basic manoeuvrability with a servo connected to pin 6 (Test purpose)
        
          if(directionVal == "Lef"){ // Move the servo Anticlockwise
             int diff = startingValue_servo - headingVal;
             
             // Taking into account inbound values
             if(diff < 0){
              diff = 0;
             }
             servo.write(diff);
             startingValue_servo = diff;
             Serial.println("Heading: " + String(headingVal) + "Diff_heading: " + String(diff));
          }
          else if(directionVal == "Rig") {
             int plus = startingValue_servo + headingVal;
             
             // Taking into accound inbound values
             if(plus > 180){
              plus = 180;
             }
             servo.write(plus); // Mode the servo ClockWise;
             startingValue_servo = plus;
             Serial.println("Heading: " + String(headingVal) + "Diff_heading: " + String(plus)); 
          }

          
        }
    }
    else {
      // Here we must set a timeout to be safe in case where we really lost connection with millis() function
      
    }
    
    
    // Debugging purpose --------------------------------------------------
    if (incomingData == true)
    {
      incomingData = false;
    }







    
    // close the connection:
    client.flush();
    client.stop();
  }
  // Else we must relay onto the radio mode
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
