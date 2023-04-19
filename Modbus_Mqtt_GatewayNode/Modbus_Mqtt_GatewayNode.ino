//mail me for doubts  sarath2@gmail.com
// Replace xxxx with appt values 
#include <ModbusRTU.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "xxxx"
#define WLAN_PASS       "xxxxx"
/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "xxxxx"
#define AIO_KEY         "xxxxx" 

#define DEBUG 1
/*********************************************/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
/*********************************************/
SoftwareSerial S(D3, D2);
#define TEMP_VAL_REG 10
#define COIL_STAT_REG 15
#define SLAVE_ID 1
ModbusRTU mb;
/*************************************************************/
void MQTT_connect();
uint16_t coils[2];
bool coilval[2];
bool coil_stat= false;
float temp_publish;
unsigned long previousMillis = 0;
const long interval = 5000; 
/*************************************************************/

bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) { // Callback to monitor errors
  if (event != Modbus::EX_SUCCESS) {
    Serial.print("Request result: 0x");
    Serial.print(event, HEX);
  }
  return true;
}
void setup() {
  Serial.begin(9600);
/***************Wifi Connection***************************/
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
  /***************Modbus initialization***************************/
  S.begin(9600, SWSERIAL_8N1);
  mb.begin(&S,D0); // RE/DI connected to D0 of ESP8266
  mb.master();
}

/*******************************************************************************/

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(50))) 
  {
    if (subscription == &onoffbutton) 
    {
      if(strcmp((char *)onoffbutton.lastread, "ON")==0)
      {
        #if DEBUG
        Serial.println(F("Recieved ON Signal from dashboard"));
        #endif
        coil_stat=1;
      }
      else
      {
        #if DEBUG
        Serial.println(F("Recieved OFF Signal from dashboard"));
        #endif
      coil_stat=0;
      }
    }
  }
  
  if (!mb.slave()) {

    mb.readHreg(SLAVE_ID, TEMP_VAL_REG, coils, 1, cb);
    while(mb.slave()) { // Check if transaction is active
      mb.task();
      delay(10);
    }    
    temp_publish=(float)coils[0]/100;
    #if DEBUG
    Serial.print("Temperature =");
    Serial.println(temp_publish);
    
    #endif
    

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
    
    if (! Temperature.publish(temp_publish)) {
      #if DEBUG
      Serial.println(F("Failed to publish"));
      #endif
     } 
     else {
      #if DEBUG
      Serial.println(F("OK! Published"));
      #endif 
     }
     }
    //write to coil reg
    mb.writeCoil(SLAVE_ID,COIL_STAT_REG,coil_stat,cb);
    #if DEBUG
    Serial.print("coil stat is " );
    Serial.println( coil_stat);
    #endif 
    while(mb.slave()) { // Check if transaction is active
    mb.task();
    delay(10);
    }
      
  }
  delay(50);
}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
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
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
