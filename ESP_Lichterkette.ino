#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "FS.h"
#define TIMER1_TICKS_PER_US (APB_CLK_FREQ / 1000000L)
#define DEBUG_ESP_HTTP_SERVER
uint32_t usToTicks(uint32_t us) {
  return (TIMER1_TICKS_PER_US / 16 * us);     // converts microseconds to tick
}

void ICACHE_RAM_ATTR pwm_timer_isr() {
  Serial.print('.');
 /* Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 6);
  Serial1.write((uint8_t) 128);
  Serial1.write((uint8_t) 1);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) 2);*/
  TEIE |= TEIE1;
}

void pwm_start_timer() {
  timer1_disable();
  timer1_attachInterrupt(pwm_timer_isr);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(usToTicks(1000000));
}

extern "C" {
  #include "user_interface.h"
}

/* Set these to your desired credentials. */
const char *ssid = "Lichterkette";
const char *password = "12345678";
const uint8_t channel = 12;

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {

File f = SPIFFS.open("/index.html", "r");
  if (!f) {
    Serial.println("file open failed");
  }
  char* fileBuffer = (char*) malloc(sizeof(char)*f.size());

  f.readBytes(fileBuffer, f.size());
  server.send(200, "text/html", (const char*) fileBuffer);
  free(fileBuffer);
}


class TestHandler : public RequestHandler{

public:
TestHandler() { Serial.println("Handler am Start");}
   static String getContentType(const String& path) {
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
    
bool canHandle(HTTPMethod requestMethod, String requestUri) override  {
  Serial.println("canHandle");
  Serial.print(requestMethod);
  Serial.println();
        if (requestMethod != HTTP_GET) {
            return false;
        }
        return true;
    }

  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
      Serial.println("handle");
      /*  if (!canHandle(requestMethod, requestUri))
            return false;*/

         Serial.println( requestUri.c_str()) ;
        //  Serial.println(_uri.c_str());

      //  String path("/");

      //  if (!_isFile) {
            // Base URI doesn't point to a file.
            // If a directory is requested, look for index file.
            if (requestUri.endsWith("/")) requestUri += "index.html";

            // Append whatever follows this URI in request to get the file path.
             String path(requestUri);/*.substring(_baseUriLength);*/
    //    }
       // DEBUGV("TestHandler::handle: path=%s, isFile=%d\r\n", path.c_str(), _isFile);
       
Serial.println(  path.c_str());

if(server.args() == 3) {

uint8_t _color = 128;
uint8_t _effect = 128;
String color = server.arg(0);
String effect = server.arg(1);
Serial.println("color:" + color); 
Serial.println("effect:" + effect); 
 if(color == "RED"){ 
      _color = 128;
 }  else if(color == "GREEN"){
      _color = 16;
 }  else if(color == "BLUE"){
      _color = 2;
 }   else if(color == "TURQUOIS"){
      _color = 25;
 }   else if(color == "ORANGE"){
      _color = 164;
 }   else if(color == "CYAN"){
      _color = 18;
 }  else if(color == "WHITE"){
      _color = 255;
 }  else if(color == "VIOLETT"){
      _color = 131;
 }  else if(color == "PINK"){
      _color = 129;
 }  else if(color == "YELLOW"){
      _color = 176;
 }   else if(color == "BLACK"){
      _color = 0;
 }

if(effect == "SETFULLCOLOR") {
      _effect = 0;
      Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 6);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 1);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
    //  softuart.write(1,38400,254, 6, _effect ,1, _count, _color);
}else if(effect == "FILLCOLOR"){
      _effect = 1;
            Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 7);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 25);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
      Serial1.write((uint8_t) 0);
    //  softuart.write(1,38400,254, 7, _effect ,25, _count, _color,0);
} else if(effect == "BLINK"){
      _effect = 2;
                  Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 6);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 29);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
    //  softuart.write(1,38400,254, 6, _effect ,20, _count, _color);
} else if(effect == "RUNLED"){
      _effect = 3;
                        Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 7);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 55);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
         Serial1.write((uint8_t) 0);
    //  softuart.write(1,38400,254, 7, _effect ,55, _count, _color,0);   
} else if(effect == "ALTERNATING"){
      _effect = 5;
                              Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 7);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 75);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
         Serial1.write((uint8_t) 0);
    //  softuart.write(1,38400,254, 7, _effect ,75, _count, _color,0);
} else if(effect == "RECOLOR"){
      _effect = 7;
                        Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 6);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 50);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
    //  softuart.write(1,38400,254, 6, _effect ,50, _count, _color);
}  else if(effect == "FADE"){
      _effect = 8;
                        Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 6);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 25);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) _color);
    //  softuart.write(1,38400,254, 6, _effect ,25, _count, _color);
} else if(effect == "RAINBOW"){
      _effect = 9;
                        Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 5);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 150);
  Serial1.write((uint8_t) 50);

  delay(1000);
   _effect = 11;
                              Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 5);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 60);
  Serial1.write((uint8_t) 50);

   //   softuart.write(1,38400,254, 5, _effect ,150, _count);
}

Serial.println((uint8_t) _color);
Serial.println((uint8_t) _effect);
}
    
for(int i = 0;i< server.args();i++){
  Serial.print(server.argName(i));
   Serial.print(" : ");
  Serial.println(server.arg(i));
  
}
Serial.println();

        String contentType = getContentType(path);

        // look for gz file, only if the original specified path is not a gz.  So part only works to send gzip via content encoding when a non compressed is asked for
        // if you point the the path to gzip you will serve the gzip as content type "application/x-gzip", not text or javascript etc...
       /* if (!path.endsWith(".gz") && !_fs.exists(path))  {
            String pathWithGz = path + ".gz";
            if(_fs.exists(pathWithGz))
                path += ".gz";
        }*/

        File f = SPIFFS.open(path, "r");
        if (!f)
            return false;

        /*if (_cache_header.length() != 0)
            server.sendHeader("Cache-Control", _cache_header);*/

        server.streamFile(f, contentType);
        return true;
    }
};


void setup() {
  ESP.eraseConfig();
  SPIFFS.begin();
  
  delay(1000);

  Serial.begin(115200);
  Serial1.begin(38400);
  Serial.setDebugOutput(true); 
  Serial.println();

  WiFi.setAutoConnect(false);

  WiFi.begin(ssid, password); // store
  WiFi.begin(); // restore

   /* THESE BOTH BREAK AP JOINS FOR SOME REASON , they give time for the reconnect to start scanning if my guess ( you will see a rogue async "scandone" */
  // Serial.println("connect result: " + (String)WiFi.waitForConnectResult());
  delay(6000);

  WiFi.printDiag(Serial);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP); // this stops the autoconnect
  WiFi.printDiag(Serial);

  Serial.println();
  Serial.print("Configuring access point...");

  wifi_set_phy_mode(PHY_MODE_11G);
  WiFi.softAP(ssid, password, channel); // stick a channel 1 in there, and it still fails...
  delay(300);
  WiFi.printDiag(Serial);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //server.on("/", handleRoot);

  server.addHandler(new TestHandler());
  server.begin();
  Serial.println("HTTP server started");
  pwm_start_timer();
}

void loop() {
  server.handleClient();
}

