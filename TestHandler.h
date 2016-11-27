#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include "FastLED.h"
#include "FS.h"
#include <ESP8266WebServer.h>

extern CRGB _color;
extern uint8_t _effect;
extern uint8_t effektzeit;

class TestHandler : public RequestHandler {

public:
TestHandler() { }
   static String getContentType(const String& path);
    
bool canHandle(HTTPMethod requestMethod, String requestUri) override;

  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override; 
};
