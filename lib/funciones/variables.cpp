#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"
#include "funciones.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Declaracion de variables globales del codigo //////////////////////////
/////////////////////////// Tanto coneccion de redes como utilidades ////////////////////////////

//Inicializacion de variables del gps
double latitud = 0;
double longitud = 0;
float velocidad = 0.0f;
float altitud = 0.0f;

//Inizialicacion de variables de los sensores 
float ph = 0.0f;
float turbidez = 0.0f;
float temperatura = 0.0f;


// Constantes globales para wifi
const char *ssid = "PRUEBA"; // ingresar la web
const char *password = "12345678"; // ingresar contraseña

// Constantes de la coneccion con mqt
const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_password = "password";
const char *mqtt_user = "esp_1_user";

//Temas para el envio e ingreso de datos 
const char *topic_pub = "esp32/control";
const char *topic_sub = "esp32/sensores";

// Set up del sensor de temperatura
OneWire sensor_de_temperatura(sensor_de_temp);
DallasTemperature sensores(&sensor_de_temperatura);

// Set up del gps
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// Set up del MQTT y conexión con la red
WiFiClient espClient;
PubSubClient client(espClient);

String mensaje = "";

// Conseguimos el tiempo desde que se empezo el arduino
unsigned long tiempo_desde_inicio = millis();
int tiempo_max_ms = 1000;

// Variables para testing de las coneccions
unsigned long ultimo_envio_mqtt = 0;
unsigned long ultimo_debug = 0;
const unsigned long intervalo_envio = 2000;     
const unsigned long intervalo_debug = 5000; 
