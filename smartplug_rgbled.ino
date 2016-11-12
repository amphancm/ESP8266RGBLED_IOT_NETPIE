#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <ESP_Adafruit_SSD1306.h>
#include <ArduinoJson.h>


const char* ssid     = "YOUR NETWORK";
const char* password = "YOUR PASSWORD";

#define APPID       "APP ID"                // Need to Sign-in Netpie.io
#define GEARKEY     "YOUR KEY"
#define GEARSECRET  "YOUR SECRET"
//#define SCOPE       "r:/outdoor/temp,w:/outdoor/valve,name:logger,chat:plant"
#define SCOPE       ""

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);

const int R = 14;
const int G = 12;
const int B = 13;

String red;
String green;
String blue;


void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    Serial.print(topic);
    Serial.print(" : ");
    char strState[msglen];
    
    for (int i = 0; i < msglen; i++) {
      strState[i] = (char)msg[i];
      Serial.print((char)msg[i]);
    }
    Serial.println();
    String strData = String(strState).substring(0, msglen);

    if(strData != "") {
      String jsonReq = strData; //if send json data, it have only 1 argument
      int size = jsonReq.length() + 1;
      char json[size];
      jsonReq.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);

        if (json_parsed.containsKey("r")){
            String r = json_parsed["r"]; 
            //Serial.println(r);
            red = r;
        }
            
        if (json_parsed.containsKey("g")){
            String g = json_parsed["g"]; 
            //Serial.println(g);
            green = g;
        }  

        if (json_parsed.containsKey("b")){
            String b = json_parsed["b"]; 
            //Serial.println(b);
            blue = b;
        }  
    }

      analogWrite(R, red.toInt());
      analogWrite(G, green.toInt());
      analogWrite(B, blue.toInt());
  
      Serial.println(red.toInt());   // for TESTING
      Serial.println(green.toInt()); // for TESTING
      Serial.println(blue.toInt());  // for TESTING

      display.clearDisplay();
      display.setTextColor(WHITE);
        
      display.setTextSize(2); 
      display.setTextColor(WHITE); 
      display.setCursor(0,0);      
      display.print("R = ");  display.println(red.toInt());
      display.setCursor(0,18);
      display.print("G = ");  display.println(green.toInt());
      display.setCursor(0,36);
      display.print("B = ");  display.println(blue.toInt()); 
      display.display();
        
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
          Serial.print("Connected to NETPIE...");
          //microgear.setName("pieplug");
          microgear.setName("rgbled");
          Serial.print("OK\n");

          display.clearDisplay();
          display.setTextColor(WHITE);
            
          display.setTextSize(2); 
          display.setTextColor(WHITE); 
          display.setCursor(0,0);      
          display.print("R = ");  display.println(red.toInt());
          display.setCursor(0,18);
          display.print("G = ");  display.println(green.toInt());
          display.setCursor(0,36);
          display.print("B = ");  display.println(blue.toInt()); 
          display.display();
            
          display.setTextSize(1);
          display.setTextColor(WHITE);
          //display.setTextColor(BLACK, WHITE); // 'inverted' text
          display.setCursor(0,52);
          display.println("Connect NETPIE Ok");        
          display.display(); 
}




//============================================
void setup() {
//============================================
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  analogWrite(R, 1000);     // R 
  analogWrite(G, 1000);     // G 
  analogWrite(B, 1000);     // B 
  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  //pinMode(output1, OUTPUT);
  //digitalWrite(output1, LOW);

  //  dht.begin();            // dht begin

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3D);      // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x78>>1);     // init done
 
  display.clearDisplay();                 // Clear the buffer.
  display.setTextSize(2);
  display.setTextColor(WHITE);
  //display.setTextColor(BLACK, WHITE);   // 'inverted' text
  display.setCursor(0,0);
  display.println("  ESP8266");

  display.setTextSize(3);                                  //Size4 = 5 digit , size3 = 7 digits
  //display.setTextColor(BLACK, WHITE);   // 'inverted' text
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println("RGB LED");
    
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setCursor(0,52);
  display.println("Version 0.1"); 
  display.display();
  delay(2000);

  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");

          display.clearDisplay();                 // Clear the buffer.
          display.setTextSize(2);
          display.setTextColor(WHITE);
          //display.setTextColor(BLACK, WHITE);   // 'inverted' text
          display.setCursor(0,0);
          display.println("  ESP8266");
        
          display.setTextSize(3);                                  //Size4 = 5 digit , size3 = 7 digits
          //display.setTextColor(BLACK, WHITE);   // 'inverted' text
          display.setTextColor(WHITE);
          display.setCursor(0,20);
          display.println("RGB LED");
            
          display.setTextSize(1);
          display.setTextColor(WHITE);
          //display.setTextColor(BLACK, WHITE); // 'inverted' text
          display.setCursor(0,52);
          display.println("connecting... ");        
          display.display();  
    }

          Serial.println("WiFi connected");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          Serial.println("Connecting... NETPIE");             
            
          display.clearDisplay();                 // Clear the buffer.
          display.setTextSize(2);
          display.setTextColor(WHITE);
          //display.setTextColor(BLACK, WHITE);   // 'inverted' text
          display.setCursor(0,0);
          display.println("  ESP8266");
        
          display.setTextSize(3);                                  //Size4 = 5 digit , size3 = 7 digits
          //display.setTextColor(BLACK, WHITE);   // 'inverted' text
          display.setTextColor(WHITE);
          display.setCursor(0,20);
          display.println("RGB LED");
            
          display.setTextSize(1);
          display.setTextColor(WHITE);
          //display.setTextColor(BLACK, WHITE); // 'inverted' text
          display.setCursor(0,52);
          display.println("Connecting... NETPIE");        
          display.display();


          //uncomment the line below if you want to reset token -->
          microgear.resetToken();
          microgear.init(GEARKEY, GEARSECRET, SCOPE);
          microgear.connect(APPID);

          testRGB();
          
         
  }
}


void loop() {
  if (microgear.connected()) {
    microgear.loop();
    Serial.println("connect...OK");
    
  } else {
    Serial.println("connection lost, reconnect...");

    display.clearDisplay();                 // Clear the buffer.
    display.setTextSize(2);
    display.setTextColor(WHITE);
    //display.setTextColor(BLACK, WHITE);   // 'inverted' text
    display.setCursor(0,0);
    display.println("connection lost ! ");
     
    display.setTextSize(1);  
    display.println("reconnect...");     
    display.display();
    
    microgear.connect(APPID);
  }
  delay(1000);
}

void testRGB() { // fade in and out of Red, Green, Blue
    analogWrite(R, 1023);     // R off
    analogWrite(G, 1023);     // G off
    analogWrite(B, 1023);     // B off 
    fade(R); // R
    fade(G); // G
    fade(B); // B
}

void fade(int pin) {
    for (int u = 0; u < 1024; u++) {
      analogWrite(pin,  1023 - u);
      delay(1);
    }
    for (int u = 0; u < 1024; u++) {
      analogWrite(pin, u);
      delay(1);
    }
}

