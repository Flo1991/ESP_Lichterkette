#include "TestHandler.h"

String  TestHandler::getContentType(const String& path) {
  if (path.endsWith(".html")) return "text/html";
  else if (path.endsWith(".htm")) return "text/html";
  else if (path.endsWith(".css")) return "text/css";
  else if (path.endsWith(".txt")) return "text/plain";
  else if (path.endsWith(".js")) return "application/javascript";
  else if (path.endsWith(".png")) return "image/png";
  else if (path.endsWith(".gif")) return "image/gif";
  else if (path.endsWith(".jpg")) return "image/jpeg";
  else if (path.endsWith(".ico")) return "image/x-icon";
  else if (path.endsWith(".svg")) return "image/svg+xml";
  else if (path.endsWith(".ttf")) return "application/x-font-ttf";
  else if (path.endsWith(".otf")) return "application/x-font-opentype";
  else if (path.endsWith(".woff")) return "application/font-woff";
  else if (path.endsWith(".woff2")) return "application/font-woff2";
  else if (path.endsWith(".eot")) return "application/vnd.ms-fontobject";
  else if (path.endsWith(".sfnt")) return "application/font-sfnt";
  else if (path.endsWith(".xml")) return "text/xml";
  else if (path.endsWith(".pdf")) return "application/pdf";
  else if (path.endsWith(".zip")) return "application/zip";
  else if(path.endsWith(".gz")) return "application/x-gzip";
  else if (path.endsWith(".appcache")) return "text/cache-manifest";
  return "application/octet-stream";
}

void TestHandler::upload(ESP8266WebServer& server, String requestUri, HTTPUpload& upload) {
  Serial.print("status: "); Serial.println(upload.status);
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

bool TestHandler::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri)  {
  // If a directory is requested, look for index file.
  if (requestUri.endsWith("/")) {
    requestUri += "index.html";
  }
  Serial.println(requestUri);
  if(requestMethod == HTTP_POST && requestUri == "/upload.html") {
     server.send(200);
     return true;
  }

  // Append whatever follows this URI in request to get the file path.
  String path(requestUri);

  if(server.args() == 4) {
    String color = server.arg(0);
    String effect = server.arg(1);
    String speedString = server.arg(2);
    String brightnessString = server.arg(3);
    brightness = atof(brightnessString.c_str())/100.0f;
    effektzeit = atoi(speedString.c_str());
    int r, g, b;
    int rgb = (int)strtol((const char*) &color.c_str()[1], NULL, 16);
    r = rgb >> 16 & 0xff;
    g = rgb >> 8 & 0xff;
    b = rgb & 0xff;
    r *= brightness;
    g *= brightness;
    b *= brightness;
    _color = CRGB( r, g, b);

    if (effect == "SETFULLCOLOR") {
          _effect = 0;
    } else if(effect == "FILLCOLOR") {
          _effect = 1;
    } else if(effect == "BLINK") {
          _effect = 2;
    } else if(effect == "RUNLED") {
          _effect = 3;
    } else if(effect == "ALTERNATING") {
          _effect = 5;
    } else if(effect == "RECOLOR") {
          _effect = 7;
    } else if(effect == "FADE") {
          _effect = 8;
    } else if(effect == "RAINBOW") {
          _effect = 9;
    } 
  }

  String contentType = getContentType(path);
  
  File f = SPIFFS.open(path, "r");
  if (!f)
      return false;
  
  server.streamFile(f, contentType);
  return true;
}
