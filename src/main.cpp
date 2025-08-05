#include <Arduino.h>
#include <stdbool.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi.h>

#include "funciones.h"
#include "config.h"
#include "test.h"

void setup() {
  Serial.begin(115200);

  //set up del gps
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);
  
  //comprobamos que el wifi se conecte correctamente
  WiFi.begin(ssid, password);
  if(WiFi.status() != WL_CONNECTED){
    indicador(2, 1);
  }

  client.setServer(mqtt_server, mqtt_port);
  while(!client.connected()){
    indicador(0, 0);
    if(client.connect("ESP32cliente", mqtt_user, mqtt_pasword)){
      indicador(3, 2);
    }
    else{
      indicador_fallo(10);
    }
  }

  
  indicador(1, 2); //indicamos que el setup se termino correctamente 
}

void loop() {

  //comprovamo que el gps este conectado correctamente 
  while (test_gps()){ 
    //leemos el gps
    while(gpsSerial.available() > 0){ 
      gps.encode(gpsSerial.read());
    }
  }  

  //mas info: https://www.prometec.net/esp32-mqtt/

  
}
