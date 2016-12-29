#include <Arduino.h>

/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it ba sharing http://youtube.com/bitlunislab
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Adafruit_NeoPixel.h>

#include "PinStates.h"
#include "LedStates.h"
#include "Fader.h"
#include "RainbowFunction.h"
#include "SimpleRGBFunction.h"
#include "WaveFunction.h"
#include "CatrunningFunction.h"
#include "FlashingFunction.h"
#include "RandomFunction.h"

const char* ssid = "***********";
const char* password = "***********";

ESP8266WebServer server(80);

const int LED_PIN = 2;
//@pepsilla 20161206
//Egg proyect only 47 unts of ws2812b
//Picture gets 54 units of ws2812b
//Led strib just rest 199 of 300 five meter.
const int LED_COUNT = 199;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

LedStates currentLedStates(strip);
LedStates targetLedStates(strip);
Fader<LedStates> ledFader(currentLedStates, targetLedStates);
PinStates currentPinStates;
PinStates targetPinStates;
Fader<PinStates> pinFader(currentPinStates, targetPinStates);

//int counter = 0;
//unsigned long initMillis;

void handleRoot() {
  String message = "<html><head></head><body style='font-family: sans-serif; font-size: 12px'>Following functions are available:<br><br>";
  message += "<a href='/Random?fade=3000&delay=10000'>/Random</a> a random color pixel generator<br>";
  message += "<a href='/CatRunner?r=255&g=0&b=0&fade=3000&delay=4000'>/CatRunner</a> a crazy cats runner <br>";
  message += "<a href='/flasher?r=255&g=0&b=0&fade=3000&delay=5000'>/flasher</a> a flash strip <br>";
  message += "<a href='/rainbow?fade=3000'>/rainbow</a> a rainbow animation on LEDs<br>";
  message += "<a href='/wave?r=255&g=32&b=10&fade=5000'>/wave</a> a slow wave animation on LED on base color specified by arguments: r=<0..255> g=<0..255> b=<0..255><br>";
  message += "<a href='/setleds?r=32&g=64&b=32&fade=1000'>/setleds</a> sets LEDs to the color from arguments: r=<0..255> g=<0..255> b=<0..255><br>";
  message += "<a href='/ledsoff?fade=500'>/ledsoff</a> turns off LEDs<br>";
  message += "<a href='/setpins?D1=128&D2=256&D3=512'>/setpins</a> sets to any of the in arguments specified pins (D0..D8) to their PWM values (0..1023). To use them digital: 0=off, 1023=on<br>";
  message += "<a href='/togglepins'>/togglepins</a> inverts all pin values form pins used before.<br><br>";
  message += "All functions except togglepins support the argument 'fade' which specifies the milliseconds it takes to fade to the new specified state. ...nice blending ;-)<br>";
  message += "<br>Syntax is as follows: http://&ltip>/&ltcommand>?&ltargument1>=&ltvalue1>&&ltargument2>=&ltvalue2>&...<br>";
  message += "You can click on each link to see an example.<br><br>";
  message += "have fun -<a href='http://youtube.com/bitlunislab'>bitluni</a></body></html>";
  message += "<br>- @pepsilla add Random, CatRunner and flasher <a href='https://ecomred.es'>ecomred.es</a></body></html>";
  server.send(200, "text/html", message);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

int getArgValue(String name)
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

bool checkFadeAndSetLedFunction(LedFunction *f)
{
  int fade = getArgValue("fade");
  if(fade > -1)
  {
    targetLedStates.setFunction(f);
    ledFader.start(fade);
  }
  else
    currentLedStates.setFunction(f);
}

void setup(void){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //find it as http://lights.local
  /*if (MDNS.begin("lights"))
  {
    Serial.println("MDNS responder started");
  }*/

  server.on("/", handleRoot);

  server.on("/rainbow", [](){
    server.send(200, "text/plain", "rainbow");
    checkFadeAndSetLedFunction(new RainbowFunction());
  });

  server.on("/CatRunner", [](){
    server.send(200, "text/plain", "CatRunner");
    CatrunningFunction *f = new CatrunningFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/flasher", [](){
    server.send(200, "text/plain", "flasher");
    FlashingFunction *f = new FlashingFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/Random", [](){
    server.send(200, "text/plain", "random");
    RandomFunction *f = new RandomFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/wave", [](){
    server.send(200, "text/plain", "wave");
    WaveFunction *f = new WaveFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/setleds", [](){
    server.send(200, "text/plain", "setleds");
    SimpleRGBFunction *f = new SimpleRGBFunction();
    f->init(server);
    checkFadeAndSetLedFunction(f);
  });

  server.on("/ledsoff", [](){
    server.send(200, "text/plain", "ledsoff");
    checkFadeAndSetLedFunction(new SimpleRGBFunction());
  });

  server.on("/togglepins", [](){
    server.send(200, "text/plain", "togglepins");
    currentPinStates.toggle();
    currentPinStates.commit();
  });

  server.on("/setpins", [](){
    server.send(200, "text/plain", "setpins");
    int fade = getArgValue("fade");
    if(fade > -1)
    {
      targetPinStates.loadValues(server);
      pinFader.start(fade);
    }
    else
    {
      currentPinStates.loadValues(server);
      currentPinStates.commit();
    }
  });

  server.on("/pinsoff", [](){
    server.send(200, "text/plain", "insoff");
    currentPinStates.setAllTo(0);
    currentPinStates.commit();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void)
{
  //if(counter == 0) initMillis = millis();
  server.handleClient();
  //MDNS.update();
  currentLedStates.render();
  if(ledFader.active)
    targetLedStates.render();
  if(!ledFader.fade())
    currentLedStates.commit();
  pinFader.fade();
  //delay(4);
  yield();
/*
  unsigned long now = millis();
  counter++;
  if(counter == 999){
    Serial.print(ESP.getFreeHeap()/1024);
    Serial.println("K.");
    Serial.print((now - initMillis)/100);
    Serial.println(" ms.");
    counter = 0;
  }
  */
}
