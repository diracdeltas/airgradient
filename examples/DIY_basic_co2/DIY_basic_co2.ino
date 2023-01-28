/*
This is the code for the AirGradient DIY BASIC Air Quality Sensor with an ESP8266 Microcontroller.

It is a high quality sensor showing PM2.5, CO2, Temperature and Humidity on a small display and can send data over Wifi.

Build Instructions: https://www.airgradient.com/open-airgradient/instructions/diy/

Kits (including a pre-soldered version) are available: https://www.airgradient.com/open-airgradient/kits/

The codes needs the following libraries installed:
"ESP8266 and ESP32 OLED driver for SSD1306 displays by ThingPulse, Fabrice Weinberg" tested with Version 4.1.0

Configuration:
Please set in the code below the configuration parameters.

If you have any questions please visit our forum at https://forum.airgradient.com/

If you are a school or university contact us for a free trial on the AirGradient platform.
https://www.airgradient.com/

MIT License

*/


#include <AirGradient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SSD1306Wire.h"

AirGradient ag = AirGradient(true, 9600);

SSD1306Wire display(0x3c, SDA, SCL);

// CONFIGURATION START

const char* ssid = "YOUR SSID HERE";
const char* password = "YOUR PASSWORD HERE";
const int port = 80; // metrics will be displayed at http://$ip:$port

// CONFIGURATION END

unsigned long currentMillis = 0;

const int oledInterval = 2000;
unsigned long previousOled = 0;

const int co2Interval = 2000;
unsigned long previousCo2 = 0;

int Co2 = 0;
int displaypage = 0;

WiFiServer server(port);

void setup()
{
  Serial.begin(9600);

  display.init();
  display.flipScreenVertically();

  connectToWifi();

  showTextRectangle("Init", String(ESP.getChipId(), HEX), true);

  ag.CO2_Init();
}


void loop()
{
  currentMillis = millis();
  updateOLED();
  updateCo2();
  runServer();
}

void updateCo2()
{
    if (currentMillis - previousCo2 >= co2Interval) {
      previousCo2 += co2Interval;
      Co2 = ag.getCO2_Raw();
      Serial.println(String(Co2));
    }
}

void updateOLED() {
   if (currentMillis - previousOled >= oledInterval) {
     previousOled = currentMillis;

     switch (displaypage) {
       case 0:
           // Reconnect to wifi if needed
           if (WiFi.status() != WL_CONNECTED) {
             connectToWifi();
           } else {
             showTextRectangle("WiFi", String(WiFi.RSSI()), false);
           }
           displaypage = 1;
           break;
       case 1:
           showTextRectangle("CO2", String(Co2), false);
           displaypage = 2;
           break;
       case 2:
           IPAddress ip = WiFi.localIP();
           showTextRectangle(String(ip[0]) + String(".") + String(ip[1]),
                             String(ip[2]) + String(".") + String(ip[3]), true);
           displaypage = 0;
           break;
     }
   }
}

void showTextRectangle(String ln1, String ln2, boolean small) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  if (small) {
    display.setFont(ArialMT_Plain_16);
  } else {
    display.setFont(ArialMT_Plain_24);
  }
  display.drawString(32, 16, ln1);
  display.drawString(32, 38, ln2);
  display.display();
}

void runServer() {
  WiFiClient client = server.accept();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareHtmlPage());
          break;
        }
      }
    }

    while (client.available()) {
      // but first, let client finish its request
      // that's diplomatic compliance to protocols
      // (and otherwise some clients may complain, like curl)
      // (that is an example, prefer using a proper webserver library)
      client.read();
    }

    // close the connection:
    client.stop();
    Serial.println("[Client disconnected]");
  }
}

String prepareHtmlPage() {
  String htmlPage;
  htmlPage.reserve(1024);               // prevent ram fragmentation
  htmlPage = F("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"  // the connection will be closed after completion of the response
               "Refresh: 5\r\n"         // refresh the page automatically every 5 sec
               "\r\n"
               "<!DOCTYPE HTML>"
               "<html>"
               "Hello human. This page shows metrics for wherever your sensor is right now.<br>"
               "Co2 PPM:  ");
  htmlPage += String(Co2) + "<br>";
  htmlPage += String(ssid) + " RSSI: ";
  htmlPage += String(WiFi.RSSI()) + "<br>";
  htmlPage += F("</html>"
                "\r\n");
  return htmlPage;
}

// Wifi Manager
void connectToWifi() {
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }
   Serial.print("Connected, IP address: ");
   IPAddress ip = WiFi.localIP();
   Serial.println(ip);
   server.begin();
   Serial.println("Web server started on port " + String(port));
}
