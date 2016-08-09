/**
    This is the ino file for ESP8266 with DHT11 temperature/humidity sensor working together, 
    ESP8266 as the transmitter/server for providing the sensor data.
    Primary function:
      ESP8266 can be configured by direct wifi connection with a html form.
      ESP8266 will catch and send or provide data as a server.

    This file was created base on ESP8266 example: AdvancedWebServer.
      
    @author Greg Huang
    @version 0.1 9/8/2016 
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

void configConnection(){
  int ssidIdx = -1;
  int psdIdx = -1;
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "testInput") {
         Serial.println("Test input from form:" + server.arg(i));
      } else if (server.argName(i) == "ssid") {
        ssidIdx = i;
        Serial.println("Ssid from form:" + server.arg(i));
      } else if (server.argName(i) == "psd") {
        psdIdx = i;
        Serial.println("Password from form:" + server.arg(i));
      } 
   }

   if(ssidIdx != -1 && psdIdx != -1){
    
     server.send(200, "text/plain", "Start configuration...");
    
     String ssidInputStr = server.arg(ssidIdx);
     String psdInputStr = server.arg(psdIdx);
     const char* ssidInput = ssidInputStr.c_str();
     const char* psdInput = psdInputStr.c_str();

     Serial.println("Connecting to ssid:");
     Serial.println(ssidInput);
     Serial.println("Connecting to psd: ");
     Serial.println(psdInput);
//     WiFi.disconnect(); 
//     delay(1000); /* wait for disconnection */ -> looks like disconnect and delay will trigger reset.
     WiFi.begin(ssidInput, psdInput);
     int totalTry = 0;
     while (WiFi.status() != WL_CONNECTED && totalTry <= 10) {
       delay(1000);
       Serial.println("Status: " + WiFi.status());
       totalTry++;
     }
     Serial.println("WL_CONNECTED: " + WL_CONNECTED);
     if(WiFi.status() == WL_CONNECTED){
      Serial.println("Successfully connected with configuration. IP: " + WiFi.localIP());
     } else {
      Serial.println("Failed connected with configuration.");
     }
  }
 }
}

void configPage(){
  char formStr[800];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  String ssid = WiFi.SSID();
  IPAddress ip = WiFi.localIP();
  String html = "<html>\
                  <head>\
                    <meta charset=\"UTF-8\"/>\
                    <title>ESP8266 Demo</title>\
                    <style>\
                      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
                    </style>\
                  </head>\
                  <body>\
                    <h1>ESP8266 Configuration</h1>\
                    <form id=\"form1\" action=\"config\" value=\"test value\"><br/>\
                      <span>ssid:&nbsp;</span><input type=\"text\" name=\"ssid\"><br/>\
                      <span>password:&nbsp;</span><input type=\"text\" name=\"psd\"><br/>\
                      <span>submit:&nbsp;</span><button type=\"submit\" form=\"form1\" value=\"Submit\">Submit</button>\
                    </form>\
                    <p>SSID: " + ssid + "</p>\
                    <p>IP: " + ip.toString() + "</p>\
                  </body>\
                </html>";
  server.send(200, "text/html", html);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin();
  Serial.println("begin...");

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/config", configConnection);

  server.on("/config-page", configPage);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}

