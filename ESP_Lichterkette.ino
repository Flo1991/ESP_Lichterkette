#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include "Webserver.h"

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

CRGB _color = CRGB::Red;
uint8_t _effect = 128;
uint8_t effektzeit = 0;
float brightness = 1.0f;

  CRGB lightdata[NUM_LEDS];  


extern "C" {
  #include "user_interface.h"
}

/* Set these to your desired credentials. */
const char *ssid = "Lichterkette";
const char *password = "12345678";
const uint8_t channel = 12;

Webserver server(80);

void setup() {
  ESP.eraseConfig();
  SPIFFS.begin();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(lightdata, NUM_LEDS);
  Serial.begin(115200);

  delay(1000);


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
}

void setfullcolor(CRGB color, CRGB *lightdata) {
  for (uint8_t i=0;i<NUM_LEDS;i++) {
    lightdata[i] = color;
  }
}

void blinkled(CRGB color, CRGB *lightdata) {
  setfullcolor(color, lightdata);
  FastLED.show();
  delay(effektzeit);
  //_delay_ms(50);
  setfullcolor(CRGB::Black, lightdata);
  FastLED.show();
  delay(effektzeit);
  //_delay_ms(50);
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
      color.r *= brightness;
      color.g *= brightness;
      color.b *= brightness;
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
      delay(effektzeit);
      //_delay_ms(40);
    } 
    //_delay_ms(40);
    server.handleClient();
    // TODO break if there's a client
    delay(effektzeit);
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
        delay(effektzeit);
        server.handleClient();
        // TODO break if there's a client
      }
      for (i=0;i<NUM_LEDS;i++)
      {
          
        rotate(lightdata,0);
        FastLED.show();
        //_delay_ms(10);
        delay(effektzeit);
           server.handleClient();
        // TODO break if there's a client
      }
        
    } 
    else
    {
      rotate(lightdata,richtung);
     FastLED.show();
      //_delay_ms(10);
      delay(effektzeit);
      
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
    delay(effektzeit);
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
    delay(effektzeit);
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
    delay(effektzeit);
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
    delay(effektzeit);
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
       delay(effektzeit); 
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
     delay(50);
  } 
  eastereggbase(color,lightdata);
  for (i=0;i<100;i++)
  {
  //  if (PacketComplete==1)
 //   break;
     delay(50);
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
          delay(effektzeit);
          FastLED.show();
          break;
        case ROTATE_L:
          rotate(lightdata,1);
          delay(effektzeit);
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

