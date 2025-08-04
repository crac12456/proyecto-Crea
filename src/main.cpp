#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <esp_camera.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "funciones.h"
#include "config.h"
 
//set up del sensor de temperatura 
OneWire sensor_de_temperatura(sensor_de_temp);
DallasTemperature sensores(&sensor_de_temperatura);

//set up del gps
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

unsigned long tiempo_desde_inicio = millis(); //conseguimos el tiempo desde que se empezo el arduino

void setup() {
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);
}

void loop() {

//comprovamo que el gps este conectado correctamente 
while (millis() - tiempo_desde_inicio < 1000){ 
   //leemos el gps
  while(gpsSerial.available() > 0){ 
    gps.encode(gpsSerial.read());
    }
 }  

 //variables del gps
double latitud = gps.location.lat(); //asignacion la latitud del gps a una variable
double longitud = gps.location.lng(); //asigna la longitud del gps a una variable 
float velocidad = gps.speed.kmph(); //asigna la velocidad del gps a una variable
float altitud = gps.altitude.meters(); //asigna la altura del gps a una variable 
  
}
