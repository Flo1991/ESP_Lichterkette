#include "Webserver.h"

#define HTTP_MAX_SEND_WAIT 5000 //ms to wait for data chunk to be ACKed

/**
 * Same as ESP8266WebServer::_handleRequest() but returning if a client was successfully handled by the request hander.
 */
bool Webserver::_handleRequest() {
  bool handled = false;
  if (!_currentHandler){
#ifdef DEBUG_ESP_HTTP_SERVER
    DEBUG_OUTPUT.println("request handler not found");
#endif
  }
  else {
    handled = _currentHandler->handle(*this, _currentMethod, _currentUri);
#ifdef DEBUG_ESP_HTTP_SERVER
    if (!handled) {
      DEBUG_OUTPUT.println("request handler failed to handle request");
    }
#endif
  }

  if (!handled) {
    if(_notFoundHandler) {
      _notFoundHandler();
    }
    else {
      send(404, "text/plain", String("Not found: ") + _currentUri);
    }
  }

  _currentUri = String();
  return handled;
}

/**
 * Same as ESP8266WebServer::handleClient() but returning if a client was successfully handled by the request hander.
 */
bool Webserver::handleClient() {
  bool wasHandled = false;
  if (_currentStatus == HC_NONE) {
    WiFiClient client = _server.available();
    if (!client) {
      return false;
    }

#ifdef DEBUG_ESP_HTTP_SERVER
    DEBUG_OUTPUT.println("New client");
#endif

    _currentClient = client;
    _currentStatus = HC_WAIT_READ;
    _statusChange = millis();
  }

  if (!_currentClient.connected()) {
    _currentClient = WiFiClient();
    _currentStatus = HC_NONE;
    return false;
  }

  // Wait for data from client to become available
  if (_currentStatus == HC_WAIT_READ) {
    if (!_currentClient.available()) {
      if (millis() - _statusChange > HTTP_MAX_DATA_WAIT) {
        _currentClient = WiFiClient();
        _currentStatus = HC_NONE;
      }
      yield();
      return false;
    }

    if (!_parseRequest(_currentClient)) {
      _currentClient = WiFiClient();
      _currentStatus = HC_NONE;
      return false;
    }
    _currentClient.setTimeout(HTTP_MAX_SEND_WAIT);
    _contentLength = CONTENT_LENGTH_NOT_SET;
    wasHandled = _handleRequest();

    if (!_currentClient.connected()) {
      _currentClient = WiFiClient();
      _currentStatus = HC_NONE;
      return wasHandled;
    } else {
      _currentStatus = HC_WAIT_CLOSE;
      _statusChange = millis();
      return wasHandled;
    }
  }

  if (_currentStatus == HC_WAIT_CLOSE) {
    if (millis() - _statusChange > HTTP_MAX_CLOSE_WAIT) {
      _currentClient = WiFiClient();
      _currentStatus = HC_NONE;
    } else {
      yield();
      return wasHandled;
    }
  }
  return wasHandled;
}

