#include <Arduino.h>
#include <stdbool.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>

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

// ================== Set up de los pines ==================

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

// ================== Set up de sensores, etc ==================

  // set up del gps
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);

  // funcion en redes.h, conecta el esp32 con la red wifi
  client.setCallback(callback);
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

  //================== Creacion de un Json para enviar los datos ==================
  StaticJsonDocument<256> doc;

  doc["Dispositivo"] = "Esp32-1";

  JsonObject sensores = doc.createNestedObject("Sensores");
  sensores["temperatura"] = temperatura;
  sensores["pH"] = ph;
  sensores["turbidez"] = turbidez;

  JsonObject gps = doc.createNestedObject("Gps");
  gps["latitud"] = latitud;
  gps["longitud"] = longitud;
  gps["altitud"] = altitud;
  gps["velocidad"] = velocidad;

  char buffer[256];
  size_t n = serializeJson(doc, buffer);

  client.publish("esp32/sensores", buffer, n);

  //en el apartado redes, se realiza la llamada y recibe el mensaje en la variable "mensaje"
  if(mensaje == "adelante"){
    motores_adelante();
  }
  else if(mensaje == "izquierda"){
    motores_izquierda();
  }
  else if(mensaje == "derecha"){
    motores_derecha();
  }
  else{
    motores_detener();
  }

}


