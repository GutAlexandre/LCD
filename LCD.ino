#include <LiquidCrystal_I2C.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Adafruit_DotStar.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h> 
#define WLAN_SSID       "Bbox-A3E72FC8"             // Your SSID
#define WLAN_PASS       "fjnzJ6ANXdFJC7194b"        // Your password
String next;

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "unknow0441"            // Replace it with your username
#define AIO_KEY         "aio_XWdh309ubEh0qCQbkwxptBfPq5Zp"   // Replace with your Project Auth Key

/************ Global State (you don't need to change this!) ******************/

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe Mode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/lcdwrite");



/************************************************************************************/


void setup() {
Serial.begin(115200);
Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 

  mqtt.subscribe(&Mode);

lcd.init();
lcd.home();
lcd.backlight(); // Enable or Turn On the backlight 
lcd.print("Hello, NodeMCU");

}
/************************************************************************************/



void loop() {
 
  MQTT_connect();
 Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(2))) {
   
    if (subscription == &Mode) {
      Serial.print("Mode > ");
      Serial.print(F("Got: "));
      Serial.println((char *)Mode.lastread);
      byte Mode1_state = atoi((char *)Mode.lastread);
      EEPROM.commit();
      next = ((char *)Mode.lastread);
      TextChange();
    }
  }
 
}
/************************************************************************************/

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    //Serial.println("Connected");
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      //while (1);
      Serial.println("Wait 10 min to reconnect");
      delay(600000);
    }
  }
  Serial.println("MQTT Connected!");
}

/************************************************************************************/

void TextChange(){
  lcd.clear();
  lcd.setCursor(0, 0);
  if(next.length() >=17){
    for(int i=0;i<16;i++){
     lcd.print(next[i]);
    }
    lcd.setCursor(0, 1);
    for(int i=16;i<next.length();i++){
     lcd.print(next[i]);
    }

  }
  else{
    for(int i=0;i<next.length();i++){
      lcd.print(next[i]);
    }
  }
}
