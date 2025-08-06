#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"
#include "funciones.h"

//set up del sensor de temperatura 
OneWire sensor_de_temperatura(sensor_de_temp);
DallasTemperature sensores(&sensor_de_temperatura);

//set up del gps
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

//set up del MQTT y coneccion con la red
WiFiClient espClient;
PubSubClient client(espClient);


//conseguimos el tiempo desde que se empezo el arduino
unsigned long tiempo_desde_inicio = millis();
int tiempo_max_ms = 1000;

//variables del gps
double latitud = gps.location.lat();
double longitud = gps.location.lng(); 
float velocidad = gps.speed.kmph(); 
float altitud = gps.altitude.meters(); 

float ph = medicion_de_ph();
float turbidez = medicion_de_turbidez();
float temperatura = medicion_temperatura();
