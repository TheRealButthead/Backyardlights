#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "";
const char *password = "";

ESP8266WebServer server(80);
const int led = 13;

char temp[3000];

void handleRoot()
{
  digitalWrite(led, 1);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 3000,
             "<!DOCTYPE html>\
<html lang=\"en\">\
  <head>\
    <meta charset=\"utf-8\">\
    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
    <title>Backyard lights</title>\
    <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css\" rel=\"stylesheet\">\
    <!--[if lt IE 9]>\
      <script src=\"https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js\"></script>\
      <script src=\"https://oss.maxcdn.com/respond/1.4.2/respond.min.js\"></script>\
    <![endif]-->\
  </head>\
  <body>\
    <div class=\"container\">\
      <div class=\"row\">\
        <div class=\"col-md-12\">\
          <h1>Backyard lights</h1>\
        </div>\
      </div>\
      <div class=\"row\">\
        <div class=\"col-md-12\">\
          <button id=\"on_button\" class=\"btn btn-success\">On</button>\
          <button id=\"off_button\" class=\"btn btn-danger\">Off</button>\
        </div>\
      </div>\
    </div>\
    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>\
    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js\"></script>\
    <script>\
      $('#on_button').click(function(){\
        $.get('/on', function(data){\
        });\
      });\
      $('#off_button').click(function(){\
        $.get('/off', function(data){\
        });\
      });\
    </script>\
  </body>\
</html>", hr, min % 60, sec % 60);
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() 
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for(uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void)
{
  pinMode(led, OUTPUT);
  pinMode(5, OUTPUT); 
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if(MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/on", turnRelayOnCb);
  server.on("/off", turnRelayOffCb);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) 
{
  server.handleClient();
}

void turnRelayOn(void)
{
  digitalWrite(5, 1);
}

void turnRelayOff(void)
{
  digitalWrite(5, 0);
}

void turnRelayOnCb()
{
  turnRelayOn();
  server.send(200, "text/html", "ON!");
}

void turnRelayOffCb()
{
  turnRelayOff();
  server.send(200, "text/html", "OFF!");
}

