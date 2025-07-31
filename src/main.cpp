#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <esp_camera.h>
#include <SPI.h>
#include <WiFi.h>

#include <funciones.h>
#include <config.h>
 
float NTU = 0.0f;
float temp = 0.0f;

OneWire sensor_de_temperatura(sensor_de_temp);
DallasTemperature sensores(&sensor_de_temperatura);

TinyGPSPlus gps;

HardwareSerial gpsSerial(2);

void setup() {
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);
}

void loop() {


  
}
