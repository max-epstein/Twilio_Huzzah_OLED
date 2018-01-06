/*
 * Twilio SMS and MMS on ESP8266 Example.
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "twilio.hpp"

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>



// OLED display TWI address
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(-1);

#if (SSD1306_LCDHEIGHT != 32)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// Use software serial for debugging?
#define USE_SOFTWARE_SERIAL 1

// Your network SSID and password
const char* ssid = "middle_aged-middle_school";
const char* password = "frenchbistrochili";

// Find the api.twilio.com SHA1 fingerprint, this one was valid as 
// of January 2017.
const char* fingerprint = "47 18 D6 BE F5 D0 BF CE 01 B7 AD BD 96 3A AA 46 F1 8C F1 A5";

// Twilio account specific details, from https://twilio.com/console
// Please see the article: 
// https://www.twilio.com/docs/guides/receive-and-reply-sms-and-mms-messages-esp8266-c-and-ngrok

// If this device is deployed in the field you should only deploy a revocable
// key. This code is only suitable for prototyping or if you retain physical
// control of the installation.
const char* account_sid = "AC2f42c5571f04858bf254094aa65cb2ed";
const char* auth_token = "609c6be50ef230c27fa93f7d71b476b8";



// Global twilio objects
Twilio *twilio;
ESP8266WebServer twilio_server(8000);

//  Optional software serial debugging
#if USE_SOFTWARE_SERIAL == 1
#include <SoftwareSerial.h>

#endif

/*
 * Callback function when we hit the /message route with a webhook.
 * Use the global 'twilio_server' object to respond.
 */
 void handle_message() {
        #if USE_SOFTWARE_SERIAL == 1
        Serial.println("Incoming connection!  Printing body:");
        // Parse Twilio's request to the ESP
        for (int i = 0; i < twilio_server.args(); ++i) {
                #if USE_SOFTWARE_SERIAL == 1
                Serial.print(twilio_server.argName(i));
                Serial.print(": ");
                Serial.println(twilio_server.arg(i));
                #endif
        }
     
     //display number for 3secs, then body for 7sec 3 times, then back to IP screen
     for (int j = 0; j < 3; j++) {
      #if USE_SOFTWARE_SERIAL == 1
        //display the message
        display.setCursor(0,0);
        //display.println("New message");
        display.println("New message from: ");
        //arg(17) is who the number is from
        display.setCursor(27,17);
        display.print(twilio_server.arg(17));
        display.display();
        delay(3000);
        display.clearDisplay();

        display.setCursor(0,0);
        //arg(10) is the message body
        display.println(twilio_server.arg(10));
        display.display();
        delay(7000);
        display.clearDisplay();
        #endif
  }


        display.setCursor(0,0);
        display.print("Connected to WiFi, IP address: "); 
        display.setCursor(27,17);
        display.println(WiFi.localIP());
        display.display();
        display.clearDisplay();
       
        #endif

        // Response will be filled with connection info and Twilio API responses
        // from this initial SMS send.
        String response = "Word";
        twilio_server.send(200, "application/xml", response);
  
}

/*
 * Setup function for ESP8266 Twilio Example.
 * 
 * Here we connect to a friendly wireless network, instantiate our twilio 
 * object, optionally set up software serial, then send a SMS or MMS message.
 */
void setup() {

  
        WiFi.begin(ssid, password);
        twilio = new Twilio(account_sid, auth_token, fingerprint);
          

        #if USE_SOFTWARE_SERIAL == 1
        Serial.begin(115200);

        display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
        display.clearDisplay();
        display.display();

        display.setTextSize(1);   
        display.setTextColor(WHITE);
 
        
        while (WiFi.status() != WL_CONNECTED) {
                delay(1000);
                Serial.print(".");
        }
        Serial.println("");
        Serial.println("Connected to WiFi. IP address: ");
        Serial.println(WiFi.localIP());

        display.setCursor(0,0);
        display.println("WiFi Connected");
        display.println("IP address: ");
        display.setCursor(27,20);
        display.println(WiFi.localIP());
        display.display();
        display.clearDisplay();
        
      

        #else
        while (WiFi.status() != WL_CONNECTED) delay(1000);
        #endif

        
      

        // Set up a route to /message which will be the webhook url
        twilio_server.on("/message", handle_message);
        twilio_server.begin();

}


/* 
 *  In our main loop, we listen for connections from Twilio in handleClient().
 */
void loop() {
        twilio_server.handleClient();
}
