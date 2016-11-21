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

const int R = 15;
//const int R = 14;
const int G = 12;
const int B = 13;

String red;
String green;
String blue;

int count;


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
    String strData = String(strState).substring(1, msglen);


    strData = GetRidOfurlCharacters(strData);
    Serial.println(strData);

    
    
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
    Serial.print(count++);
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

String GetRidOfurlCharacters(String urlChars)
{

  urlChars.replace("%0D%0A", String('\n'));

  urlChars.replace("+",   " ");
  urlChars.replace("%20", " ");
  urlChars.replace("%21", "!");
  urlChars.replace("%22", String(char('\"')));
  urlChars.replace("%23", "#");
  urlChars.replace("%24", "$");
  urlChars.replace("%25", "%");
  urlChars.replace("%26", "&");
  urlChars.replace("%27", String(char(39)));
  urlChars.replace("%28", "(");
  urlChars.replace("%29", ")");
  urlChars.replace("%2A", "*");
  urlChars.replace("%2B", "+");
  urlChars.replace("%2C", ",");
  urlChars.replace("%2D", "-");
  urlChars.replace("%2E", ".");
  urlChars.replace("%2F", "/");
  urlChars.replace("%30", "0");
  urlChars.replace("%31", "1");
  urlChars.replace("%32", "2");
  urlChars.replace("%33", "3");
  urlChars.replace("%34", "4");
  urlChars.replace("%35", "5");
  urlChars.replace("%36", "6");
  urlChars.replace("%37", "7");
  urlChars.replace("%38", "8");
  urlChars.replace("%39", "9");
  urlChars.replace("%3A", ":");
  urlChars.replace("%3B", ";");
  urlChars.replace("%3C", "<");
  urlChars.replace("%3D", "=");
  urlChars.replace("%3E", ">");
  urlChars.replace("%3F", "?");
  urlChars.replace("%40", "@");
  urlChars.replace("%41", "A");
  urlChars.replace("%42", "B");
  urlChars.replace("%43", "C");
  urlChars.replace("%44", "D");
  urlChars.replace("%45", "E");
  urlChars.replace("%46", "F");
  urlChars.replace("%47", "G");
  urlChars.replace("%48", "H");
  urlChars.replace("%49", "I");
  urlChars.replace("%4A", "J");
  urlChars.replace("%4B", "K");
  urlChars.replace("%4C", "L");
  urlChars.replace("%4D", "M");
  urlChars.replace("%4E", "N");
  urlChars.replace("%4F", "O");
  urlChars.replace("%50", "P");
  urlChars.replace("%51", "Q");
  urlChars.replace("%52", "R");
  urlChars.replace("%53", "S");
  urlChars.replace("%54", "T");
  urlChars.replace("%55", "U");
  urlChars.replace("%56", "V");
  urlChars.replace("%57", "W");
  urlChars.replace("%58", "X");
  urlChars.replace("%59", "Y");
  urlChars.replace("%5A", "Z");
  urlChars.replace("%5B", "[");
  urlChars.replace("%5C", String(char(65)));
  urlChars.replace("%5D", "]");
  urlChars.replace("%5E", "^");
  urlChars.replace("%5F", "_");
  urlChars.replace("%60", "`");
  urlChars.replace("%61", "a");
  urlChars.replace("%62", "b");
  urlChars.replace("%63", "c");
  urlChars.replace("%64", "d");
  urlChars.replace("%65", "e");
  urlChars.replace("%66", "f");
  urlChars.replace("%67", "g");
  urlChars.replace("%68", "h");
  urlChars.replace("%69", "i");
  urlChars.replace("%6A", "j");
  urlChars.replace("%6B", "k");
  urlChars.replace("%6C", "l");
  urlChars.replace("%6D", "m");
  urlChars.replace("%6E", "n");
  urlChars.replace("%6F", "o");
  urlChars.replace("%70", "p");
  urlChars.replace("%71", "q");
  urlChars.replace("%72", "r");
  urlChars.replace("%73", "s");
  urlChars.replace("%74", "t");
  urlChars.replace("%75", "u");
  urlChars.replace("%76", "v");
  urlChars.replace("%77", "w");
  urlChars.replace("%78", "x");
  urlChars.replace("%79", "y");
  urlChars.replace("%7A", "z");
  urlChars.replace("%7B", String(char(123)));
  urlChars.replace("%7C", "|");
  urlChars.replace("%7D", String(char(125)));
  urlChars.replace("%7E", "~");
  urlChars.replace("%7F", "Â");
  urlChars.replace("%80", "`");
  urlChars.replace("%81", "Â");
  urlChars.replace("%82", "â");
  urlChars.replace("%83", "Æ");
  urlChars.replace("%84", "â");
  urlChars.replace("%85", "â¦");
  urlChars.replace("%86", "â");
  urlChars.replace("%87", "â¡");
  urlChars.replace("%88", "Ë");
  urlChars.replace("%89", "â°");
  urlChars.replace("%8A", "Å");
  urlChars.replace("%8B", "â¹");
  urlChars.replace("%8C", "Å");
  urlChars.replace("%8D", "Â");
  urlChars.replace("%8E", "Å½");
  urlChars.replace("%8F", "Â");
  urlChars.replace("%90", "Â");
  urlChars.replace("%91", "â");
  urlChars.replace("%92", "â");
  urlChars.replace("%93", "â");
  urlChars.replace("%94", "â");
  urlChars.replace("%95", "â¢");
  urlChars.replace("%96", "â");
  urlChars.replace("%97", "â");
  urlChars.replace("%98", "Ë");
  urlChars.replace("%99", "â¢");
  urlChars.replace("%9A", "Å¡");
  urlChars.replace("%9B", "âº");
  urlChars.replace("%9C", "Å");
  urlChars.replace("%9D", "Â");
  urlChars.replace("%9E", "Å¾");
  urlChars.replace("%9F", "Å¸");
  urlChars.replace("%A0", "Â");
  urlChars.replace("%A1", "Â¡");
  urlChars.replace("%A2", "Â¢");
  urlChars.replace("%A3", "Â£");
  urlChars.replace("%A4", "Â¤");
  urlChars.replace("%A5", "Â¥");
  urlChars.replace("%A6", "Â¦");
  urlChars.replace("%A7", "Â§");
  urlChars.replace("%A8", "Â¨");
  urlChars.replace("%A9", "Â©");
  urlChars.replace("%AA", "Âª");
  urlChars.replace("%AB", "Â«");
  urlChars.replace("%AC", "Â¬");
  urlChars.replace("%AE", "Â®");
  urlChars.replace("%AF", "Â¯");
  urlChars.replace("%B0", "Â°");
  urlChars.replace("%B1", "Â±");
  urlChars.replace("%B2", "Â²");
  urlChars.replace("%B3", "Â³");
  urlChars.replace("%B4", "Â´");
  urlChars.replace("%B5", "Âµ");
  urlChars.replace("%B6", "Â¶");
  urlChars.replace("%B7", "Â·");
  urlChars.replace("%B8", "Â¸");
  urlChars.replace("%B9", "Â¹");
  urlChars.replace("%BA", "Âº");
  urlChars.replace("%BB", "Â»");
  urlChars.replace("%BC", "Â¼");
  urlChars.replace("%BD", "Â½");
  urlChars.replace("%BE", "Â¾");
  urlChars.replace("%BF", "Â¿");
  urlChars.replace("%C0", "Ã");
  urlChars.replace("%C1", "Ã");
  urlChars.replace("%C2", "Ã");
  urlChars.replace("%C3", "Ã");
  urlChars.replace("%C4", "Ã");
  urlChars.replace("%C5", "Ã");
  urlChars.replace("%C6", "Ã");
  urlChars.replace("%C7", "Ã");
  urlChars.replace("%C8", "Ã");
  urlChars.replace("%C9", "Ã");
  urlChars.replace("%CA", "Ã");
  urlChars.replace("%CB", "Ã");
  urlChars.replace("%CC", "Ã");
  urlChars.replace("%CD", "Ã");
  urlChars.replace("%CE", "Ã");
  urlChars.replace("%CF", "Ã");
  urlChars.replace("%D0", "Ã");
  urlChars.replace("%D1", "Ã");
  urlChars.replace("%D2", "Ã");
  urlChars.replace("%D3", "Ã");
  urlChars.replace("%D4", "Ã");
  urlChars.replace("%D5", "Ã");
  urlChars.replace("%D6", "Ã");
  urlChars.replace("%D7", "Ã");
  urlChars.replace("%D8", "Ã");
  urlChars.replace("%D9", "Ã");
  urlChars.replace("%DA", "Ã");
  urlChars.replace("%DB", "Ã");
  urlChars.replace("%DC", "Ã");
  urlChars.replace("%DD", "Ã");
  urlChars.replace("%DE", "Ã");
  urlChars.replace("%DF", "Ã");
  urlChars.replace("%E0", "Ã");
  urlChars.replace("%E1", "Ã¡");
  urlChars.replace("%E2", "Ã¢");
  urlChars.replace("%E3", "Ã£");
  urlChars.replace("%E4", "Ã¤");
  urlChars.replace("%E5", "Ã¥");
  urlChars.replace("%E6", "Ã¦");
  urlChars.replace("%E7", "Ã§");
  urlChars.replace("%E8", "Ã¨");
  urlChars.replace("%E9", "Ã©");
  urlChars.replace("%EA", "Ãª");
  urlChars.replace("%EB", "Ã«");
  urlChars.replace("%EC", "Ã¬");
  urlChars.replace("%ED", "Ã­");
  urlChars.replace("%EE", "Ã®");
  urlChars.replace("%EF", "Ã¯");
  urlChars.replace("%F0", "Ã°");
  urlChars.replace("%F1", "Ã±");
  urlChars.replace("%F2", "Ã²");
  urlChars.replace("%F3", "Ã³");
  urlChars.replace("%F4", "Ã´");
  urlChars.replace("%F5", "Ãµ");
  urlChars.replace("%F6", "Ã¶");
  urlChars.replace("%F7", "Ã·");
  urlChars.replace("%F8", "Ã¸");
  urlChars.replace("%F9", "Ã¹");
  urlChars.replace("%FA", "Ãº");
  urlChars.replace("%FB", "Ã»");
  urlChars.replace("%FC", "Ã¼");
  urlChars.replace("%FD", "Ã½");
  urlChars.replace("%FE", "Ã¾");
  urlChars.replace("%FF", "Ã¿");

  return urlChars;
}
