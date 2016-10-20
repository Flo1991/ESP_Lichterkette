#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include "FS.h"
#define FASTLED_INTERRUPT_RETRY_COUNT 0 
#include "FastLED.h"
#define DATA_PIN 4
#define NUM_LEDS 148
#define TIMER1_TICKS_PER_US (APB_CLK_FREQ / 1000000L)
#define DEBUG_ESP_HTTP_SERVER

//EFFECTS
#define SETFULLCOLOR 0
#define AUFFUELLEN 1
#define BLINK 2
#define RUNLED 3
#define LAUFLED 5
#define NEUFAERBEN 7
#define FADEN 8
#define INITRAINBOW 9
#define ROTATE_R 10
#define ROTATE_L 11
#define CUSTOM 12
#define EASTEREGG 13
uint32_t usToTicks(uint32_t us) {
  return (TIMER1_TICKS_PER_US / 16 * us);     // converts microseconds to tick
}
/*
void ICACHE_RAM_ATTR pwm_timer_isr() {
  Serial.print('.');
 /* Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 6);
  Serial1.write((uint8_t) 128);
  Serial1.write((uint8_t) 1);
  Serial1.write((uint8_t) 50);
   Serial1.write((uint8_t) 2);*/
  //TEIE |= TEIE1;
//}
CRGB _color = CRGB::Red;
uint8_t _effect = 128;
uint8_t effektzeit = 0;

  CRGB lightdata[NUM_LEDS];  

/*
void pwm_start_timer() {
  timer1_disable();
  timer1_attachInterrupt(pwm_timer_isr);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(usToTicks(1000000));
}*/

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


String color = server.arg(0);
String effect = server.arg(1);
Serial.println("color:" + color); 
Serial.println("effect:" + effect); 
 if(color == "RED"){ 
      _color = CRGB::Red;
 }  else if(color == "GREEN"){
      _color = CRGB::Green;
 }  else if(color == "BLUE"){
      _color = CRGB::Blue;
 }   else if(color == "TURQUOIS"){
      _color = CRGB::Turquoise;
 }   else if(color == "ORANGE"){
      _color = CRGB::Orange;
 }   else if(color == "CYAN"){
      _color = CRGB::Cyan;
 }  else if(color == "WHITE"){
      _color = CRGB::White;
 }  else if(color == "VIOLETT"){
      _color = CRGB::Violet;
 }  else if(color == "PINK"){
      _color = CRGB::Pink;
 }  else if(color == "YELLOW"){
      _color = CRGB::Yellow;
 }   else if(color == "BLACK"){
      _color = CRGB::Black;
 }

if(effect == "SETFULLCOLOR") {
      _effect = 0;
//      Serial1.write((uint8_t) 254);
//  Serial1.write((uint8_t) 6);
 // Serial1.write((uint8_t) _effect);
 // Serial1.write((uint8_t) 1);
//  Serial1.write((uint8_t) 50);
//   Serial1.write((uint8_t) _color);
   effektzeit = 1;
    //  softuart.write(1,38400,254, 6, _effect ,1, _count, _color);
}else if(effect == "FILLCOLOR"){
      _effect = 1;
//            Serial1.write((uint8_t) 254);
//  Serial1.write((uint8_t) 7);
 // Serial1.write((uint8_t) _effect);
 // Serial1.write((uint8_t) 25);
//  Serial1.write((uint8_t) 50);
 //  Serial1.write((uint8_t) _color);
  //    Serial1.write((uint8_t) 0);
         effektzeit = 25;
    //  softuart.write(1,38400,254, 7, _effect ,25, _count, _color,0);
} else if(effect == "BLINK"){
      _effect = 2;
      effektzeit = 29;
} else if(effect == "RUNLED"){
      _effect = 3;
            effektzeit = 55;
} else if(effect == "ALTERNATING"){
      _effect = 5;
            effektzeit = 75;
} else if(effect == "RECOLOR"){
      _effect = 7;
      effektzeit = 50;
}  else if(effect == "FADE"){
      _effect = 8;
      effektzeit = 25;
} else if(effect == "RAINBOW"){
      _effect = 9;
 /* Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 5);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 150);
  Serial1.write((uint8_t) 50);*/
     effektzeit = 150;

// TODO
//  delay(1000);
 //  _effect = 11;
/*                              Serial1.write((uint8_t) 254);
  Serial1.write((uint8_t) 5);
  Serial1.write((uint8_t) _effect);
  Serial1.write((uint8_t) 60);
  Serial1.write((uint8_t) 50);*/

 //  effektzeit = 60;
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
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(lightdata, NUM_LEDS);


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
//  pwm_start_timer();
}

void setfullcolor(CRGB color, CRGB *lightdata) {
  for (uint8_t i=0;i<NUM_LEDS;i++) {
    lightdata[i] = color;
  }
}

void blinkled(CRGB color, CRGB *lightdata) {
  setfullcolor(color, lightdata);
  FastLED.show();
  sleep_ms(effektzeit);
  //_delay_ms(50);
  setfullcolor(CRGB::Black, lightdata);
  FastLED.show();
  sleep_ms(effektzeit);
  //_delay_ms(50);
}

void sleep_ms(uint16_t sleeptime)
{
  delay(sleeptime);
  
}


void initrainbow(CRGB *lightdata) {

  float factor = 148.0f/360.0f;


CRGB color = CRGB::Black;
uint16_t i= 0;
  for(;i<360;i++) {
      if(i<120) {
        color.blue = 0x00;
      } else if(i<=180) {
        color.blue = ((i-120)/60.0f)*0xff;
      } else if(i>300) {
        color.blue = ((360-i)/60.0f)*0xff;
      } else {
        color.blue = 0xff;
      }

      if(i>=120&&i<240) {
        color.red = 0x00;
      } else if(i>=240&&i<300) {
        color.red = ((i-240)/60.0f)*0xff;
      } else if(i<120&&i>=60) {
        color.red = ((120-i)/60.0f)*0xff;
      } else {
        color.red = 0xff;
      }

      if(i<60) {
        color.green = (i/60.0f)*0xff;
      } else if(i<=180) {
        color.green = 0xff;
      } else if(i<240) {
        color.green = ((240-i)/60.0f)*0xff;
      } else {
        color.green = 0x00;
      }
       lightdata[(int)(i*factor)] = color;
  }
  
}

void auffuellen( CRGB color, CRGB backcolor, CRGB *lightdata) {
  uint8_t i,j;
  for (i=0;i<NUM_LEDS;i++)
  {
    //setfullcolor(backcolor,lightdata);
    for (j=0;j<NUM_LEDS-i;j++)
    {
      //changeled(color,lightdata,j);
      lightdata[j] = color;
      if (j>0)
      {
        lightdata[j-1] = backcolor;
       // changeled(backcolor,lightdata,j-1);
      } 
      FastLED.show();
      sleep_ms(effektzeit);
      //_delay_ms(40);
    } 
    //_delay_ms(40);
    server.handleClient();
    // TODO break if there's a client
    sleep_ms(effektzeit);
  }
}


void rotate(CRGB *lightdata, uint8_t richtung)
{
  CRGB temp;
  uint16_t i;
  
  
  if (richtung==0)//array rechts rotieren
  {
    temp = lightdata[0]; // temp gets 0
    for (i=0;i<NUM_LEDS-1;i++) // 0 to 48
    {
      lightdata[i] = lightdata[i+1]; // 0 to 48 gets 1 to 49
    }
    lightdata[NUM_LEDS-1] = temp; // 49 gets 0
    
  } else {
    temp = lightdata[NUM_LEDS-1];  // temp gets 49
    
    for (i=NUM_LEDS-1;i>0;i--) // 49 to 1 
    {
      lightdata[i] = lightdata[i-1]; // 49 to 1 gets 48 to 0
    }
    lightdata[0] = temp; // 0 gets 49
  }
}

void initrunled(CRGB color, CRGB *lightdata, CRGB background)
{
    Serial.println("initrunled");
    setfullcolor(background,lightdata);
    lightdata[0] = color;
}

void runrunled(CRGB *lightdata, uint8_t richtung)
{
  uint8_t i;
    
    if (richtung==2)
    {
      for (i=0;i<NUM_LEDS;i++)
      {
       FastLED.show();
        rotate(lightdata,1);        
        //_delay_ms(10);
        sleep_ms(effektzeit);
        server.handleClient();
        // TODO break if there's a client
      }
      for (i=0;i<NUM_LEDS;i++)
      {
          
        rotate(lightdata,0);
        FastLED.show();
        //_delay_ms(10);
        sleep_ms(effektzeit);
           server.handleClient();
        // TODO break if there's a client
      }
        
    } 
    else
    {
      rotate(lightdata,richtung);
     FastLED.show();
      //_delay_ms(10);
      sleep_ms(effektzeit);
      
    }
  
  
  
}

void initlaufled(CRGB color, CRGB backcolor, CRGB *lightdata)
{
  uint16_t i;
  setfullcolor(backcolor, lightdata);
  for (i=0;i<NUM_LEDS;i++)
  {
    if(i%2==0)
    {
      lightdata[i] = color;
    }
  }
}

void runlaufled(CRGB *lightdata )
{ 
     FastLED.show();
    //_delay_ms(250);
    sleep_ms(effektzeit);
    rotate(lightdata,1);
}

void neufaerben(CRGB color, CRGB *lightdata)
{
  uint8_t i;
  for (i=0;i<NUM_LEDS;i++)
  {
    lightdata[i] = color;
     FastLED.show();
    //_delay_ms(40);
    sleep_ms(effektzeit);
   server.handleClient();
        // TODO break if there's a client
  }
}

void faden(CRGB color, CRGB *lightdata)
{
  uint8_t i;
  uint8_t maxgruen, maxrot, maxblau;
  maxgruen =color.green;
  maxblau = color.blue;
  maxrot = color.red;
  for (i=0;i<255;i++)
  {
    setfullcolor(color,lightdata);
     FastLED.show();
    //_delay_ms(1);
    sleep_ms(effektzeit);
    if (color.green > 0)
    {
      --color.green;
    }
    if (color.blue > 0)
    {
      --color.blue;
    }
    if (color.red > 0)
    {
      --color.red;
    }
    if (color.red == 0 && color.blue == 0 && color.green == 0)
    {
      break;
    }
    server.handleClient();
        // TODO break if there's a client
  }
  
  for (i=0;i<255;i++)
  {
    setfullcolor(color,lightdata);    
      FastLED.show();
    //_delay_ms(1);
    sleep_ms(effektzeit);
    if (color.green < maxgruen)
    {
      ++color.green;
    }
    if (color.blue < maxblau)
    {
      ++color.blue;
    }
    if (color.red < maxrot)
    {
      ++color.red;
    }
    if (color.red == maxrot && color.blue == maxblau && color.green == maxgruen)
    {
      break;
    }
    server.handleClient();
        // TODO break if there's a client
  }
  
    
  
}

void eastereggbase( CRGB color, CRGB *lightdata)
{
  uint8_t i,j;
  uint8_t n;
  j=NUM_LEDS;
  for (i=0;i<NUM_LEDS;i++)
  {
    n=(j-i);
    lightdata[0] = color;
    //changeled(color,lightdata,0);
    while(n-- > 0)
    {
      rotate(lightdata,1);
     // transmit2leds(lightdata);
     FastLED.show();
      //_delay_ms(40);
       sleep_ms(effektzeit); 
    }
   // if (PacketComplete==1)
  //    break;
  }
}

void easteregg(CRGB*lightdata)
{
   CRGB color, color2;
  uint8_t i;
  color=CRGB::Red | CRGB::Green;
  color2=CRGB::Green;
  //uint8_t color8=0b11111100;
  //uint8_t color82=201;
  //struct farbe24bit color;
  //struct farbe24bit color2;
  //color=colorconv8to24(color8);
  //color2=colorconv8to24(color82);
  eastereggbase(color2,lightdata);
  for (i=0;i<100;i++)
  {
  //  if (PacketComplete==1)
  //    break;
     sleep_ms(50);
  } 
  eastereggbase(color,lightdata);
  for (i=0;i<100;i++)
  {
  //  if (PacketComplete==1)
 //   break;
     sleep_ms(50);
  }
}

void loop() {
    uint16_t i,j;
  server.handleClient();

        switch(_effect)
      {
        case SETFULLCOLOR:
          setfullcolor(_color,lightdata);
         FastLED.show();
          break;
        case AUFFUELLEN:
          auffuellen(_color, CRGB::Black ,lightdata);
          FastLED.show();
          break;
        case BLINK:
          blinkled(_color,lightdata);
          break;
        case RUNLED:
          Serial.println("CASE");
          initrunled(_color,lightdata,CRGB::Black);
          _effect++;
        case 4:
          runrunled(lightdata,2);
          break;
        case LAUFLED:
          initlaufled(_color,CRGB::Black,lightdata);
          _effect++;
        case 6:
          runlaufled(lightdata);
          break;
        case NEUFAERBEN:
          neufaerben(_color,lightdata);
          _effect=255;
          break;
        case FADEN:
          faden(_color,lightdata);
          break;
        case INITRAINBOW:
          initrainbow(lightdata);
          FastLED.show();
          _effect = 10;
          effektzeit = 20;
          break;
        case ROTATE_R:
          rotate(lightdata,0);
          sleep_ms(effektzeit);
          FastLED.show();
          break;
        case ROTATE_L:
          rotate(lightdata,1);
          sleep_ms(effektzeit);
          FastLED.show();
          break;
        case CUSTOM:
          for (i=0;i<NUM_LEDS;i++)
          {
          //  j = i % (DataLen-5);
           // changeled(colorconv8to24(CompColorArray[j]),lightdata,i);
           //  lightdata[i] = colorconv8to24(CompColorArray[j]);
          }
          FastLED.show();
          _effect=255;
          break;
        case EASTEREGG:
          //easteregg(lightdata);
          break;
        default:
          break;
      }
}

