#include <ESP8266WebServer.h>


/**
 * Same as ESP8266WebServer but with _handleRequest() and handleClient() returning if a client was successfully handled by the request handler.
 */
class Webserver : public ESP8266WebServer {
 public:
  Webserver(uint16_t port) : ESP8266WebServer(port) {}

  bool _handleRequest();
  bool handleClient();
};

