#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include "FastLED.h"
#include "FS.h"
#include "TestHandler.h"
#include <ESP8266WebServer.h>

class Webserver : public ESP8266WebServer {
  public:
  Webserver(uint16_t port) : ESP8266WebServer(port){}

  void _handleRequest() {
    ESP8266WebServer::_handleRequest();
  }
};
