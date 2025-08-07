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
#include "redes.h"

/*
 * Codigo principal del proyecto
 * todas las funciones se encuentran ordenas en sus respectivos archivos
 * el main se concentra en inicializar lo necesario y
 */

void setup()
{
  Serial.begin(115200);

  /*
   * Set up de los pines del ESP32  
   */

  //gps
  pinMode(led_interno, OUTPUT);

  //motores
  pinMode(motor_derecha_1, OUTPUT);
  pinMode(motor_derecha_2, OUTPUT);

  pinMode(motor_izquierda_1, OUTPUT);
  pinMode(motor_izquierda_2, OUTPUT);

  //sensores
  pinMode(sensor_de_temp, INPUT);
  pinMode(sensor_de_ph, INPUT);
  pinMode(sensor_de_turbidez, INPUT);

  //gps
  pinMode(gps_RX, INPUT);
  pinMode(gps_TX, OUTPUT);

  // set up del gps
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);

  // funcion en redes.h, conecta el esp32 con la red wifi
  conectar_wifi();

  // funcion en redes.h, conecta el esp32 con el broker mqtt
  mqtt_reconect();

  // indicamos que el setup se termino correctamente
  indicador(1, 2);
}

void loop()
{

  // comprovamo que el gps este conectado correctamente
  gps_coneccion();

  // creo un json el cual tendra los datos de los sensores
  String sensores_json = "{";
  sensores_json += "\"temperatura\":" + String(temperatura) + ",";
  sensores_json += "\"ph\":" + String(ph) + ",";
  sensores_json += "\"turbidez\":" + String(turbidez) + ",";
  sensores_json += "}";

  // envio el json con los datos de los sensores
  client.publish("sensores/ambiente", sensores_json.c_str());

  // creo un json con los datos del gps
  String gps_json = "{";
  gps_json += "\"latitud\":" + String(latitud) + ",";
  gps_json += "\"longitud\":" + String(longitud) + ",";
  gps_json += "\"velocidad\":" + String(velocidad) + ",";
  gps_json += "\"altitud\":" + String(altitud) + ",";
  gps_json += "}";

  // envio los datos
  client.publish("sensores/gps", gps_json.c_str());
}
