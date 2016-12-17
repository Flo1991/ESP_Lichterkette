#include "FastLED.h"
#include "FS.h"
#include <ESP8266WebServer.h>

extern CRGB _color;
extern uint8_t _effect;
extern uint8_t effektzeit;
extern float brightness;

class TestHandler : public RequestHandler {
 public:
  TestHandler() {}

  static String getContentType(const String& path);

  bool canHandle(HTTPMethod requestMethod, String requestUri) override { return true; }
  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override; 
};
