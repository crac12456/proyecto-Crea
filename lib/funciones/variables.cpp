#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdbool.h>
#include <HTTPClient.h>

#include "config.h"
#include "funciones.h"

/*========================= Variables globales =========================

    Estas son las variables que se utilizar en todo el codigo,
    principalmente cosas relacionadas con las conecciones tanto wifi
    asi como constructures de las librerias.

=======================================================================*/

// Inicialización de variables del GPS
double latitud = 0;
double longitud = 0;
float velocidad = 0.0f;
float altitud = 0.0f;

// Inizialicación de variables de los sensores
float ph = 0.0f;
float turbidez = 0.0f;
float temperatura = 0.0f;

// Constantes globales para wifi
const char *ssid = "PRUEBA";       // Ingresar la web
const char *password = "12345678"; // Ingresar contraseña

// Constantes de la conexión con mqtt
const char *mqtt_server = "192.168.0.113";
const int mqtt_port = 1883;
const char *mqtt_password = "ProyectoCREA_UEPSIMA_1";
const char *mqtt_user = "Esp32_robot";
const char *mqtt_id = "ESP32_Robot_001";

// Informacion para el envio de datos por HTTP
const char *server = "";
const String *api_key = "";

WiFiClient client_WiFi;
HTTPClient http;

// Temas para el envio e ingreso de datos
const char *topic_pub = "esp32/robot/control";
const char *topic_sub = "esp32/robot/sensores";

// Set up del sensor de temperatura
OneWire sensor_de_temperatura(sensor_de_temp);
DallasTemperature sensores(&sensor_de_temperatura);

// Set up del GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// Set up del MQTT y conexión con la red
WiFiClient espClient;
PubSubClient client(espClient);

String mensaje = "";

// Conseguimos el tiempo desde que se empezo el arduino
unsigned long tiempo_desde_inicio = millis();
int tiempo_max_ms = 1000;

// Variables para testing de las conexiones
unsigned long ultimo_envio_mqtt = 0;
unsigned long ultimo_debug = 0;
const unsigned long intervalo_envio = 2000;
const unsigned long intervalo_debug = 5000;
bool subscrito = false;