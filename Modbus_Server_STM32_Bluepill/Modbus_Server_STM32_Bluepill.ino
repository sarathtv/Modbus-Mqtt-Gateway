//mail me at sarath2@gmail.com 
/************** Header Files **********************/
#include <ModbusRTU.h>
#include "DHT.h"
/***************Macro Definitions******************/
#define DHTPIN PB12
#define RELAYPIN PB1
#define DHTTYPE DHT11   // DHT 11
#define COIL_STAT_REG 15
#define TEMP_VAL_REG 10
#define SLAVE_ID 1
/**************Function declarations***************/
void read_Temperature();
/**************Object Creations********************/
HardwareSerial S(PA3,PA2);
DHT dht(DHTPIN, DHTTYPE);
ModbusRTU mb;
/**************Global Variables********************/
float t;
bool prev_status=false;
unsigned long previousMillis = 0;
const long interval = 2000; 
/************************************/
void setup() {
  pinMode(RELAYPIN,OUTPUT);
  digitalWrite(RELAYPIN, HIGH);
  Serial.begin(9600);
  dht.begin();
  S.begin(9600, SERIAL_8N1);
  mb.begin(&S,PA5); // RE/DI connected to PA5 bluepill
  
  mb.slave(SLAVE_ID);
// Add Holding register and write 0 value
  mb.addHreg(TEMP_VAL_REG);
  mb.Hreg(TEMP_VAL_REG, 0);
// Add coil register and write 0  value
  mb.addCoil(COIL_STAT_REG);
  mb.Coil(COIL_STAT_REG,0);
  Serial.println("Server code");
}

void loop() 
{
  read_Temperature();//read temperature value
  mb.Hreg(TEMP_VAL_REG,t*100);
  readCoilReg();
  delay(1000);
  mb.task();
     
}

void read_Temperature()
{
  t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.println();
  }
}

void readCoilReg()
{
  bool current_stat=mb.Coil(COIL_STAT_REG);
  if(current_stat!=prev_status)
  {
    prev_status=current_stat;
    Serial.print("Coil stat is :");
    Serial.println(current_stat);
    //write to relay  
     digitalWrite(RELAYPIN, !current_stat);
  }
}
