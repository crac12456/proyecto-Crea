#ifndef CONFIG_H
#define CONFIG_H

/////////////////////////////////////////////////////////////////////////////////////////
///// Header de configuracion, aqui se declaran las variables gobales que se usaran /////
/////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// ================== Configuración de los pines ==================

// Pines de los motores
#define motor_derecha_1 27
#define motor_derecha_2 26
#define motor_derecha_velocidad 13

#define motor_izquierda_1 33
#define motor_izquierda_2 25
#define motor_izquierda_velocidad 14

// Sensores
#define sensor_de_temp 4
#define sensor_de_ph 35
#define sensor_de_turbidez 32

// GPS
#define gps_RX 17
#define gps_TX 16
#define gps_bauds 9600

// Leds
#define led_interno 2

// ================== Constantes Globales ==================

// Constantes globales para wifi
extern const char *ssid;     // Ingresar la web
extern const char *password; // Ingresar contraseña

// Constantes de la conexión con mqtt
extern const char *mqtt_server;
extern const int mqtt_port;
extern const char *mqtt_password;
extern const char *mqtt_user;
extern const byte *mqtt_id;

// Constantes de los temas
extern const char *topic_pub;
extern const char *topic_sub;

// Set up del sensor de temperatura
extern OneWire sensor_de_temperatura;
extern DallasTemperature sensores;

// Set up del GPS
extern HardwareSerial gpsSerial;
extern TinyGPSPlus gps;

// Variables de tiempo para comprovacion y testeo
extern unsigned long tiempo_desde_inicio;
extern int tiempo_max_ms;

// Set up de las comunicaciones, Wifi y MQTT
extern WiFiClient espClient;
extern PubSubClient client;
extern String mensaje;

// Declaración de las variables del GPS, estas se enviaran por mqtt
extern double latitud;
extern double longitud;
extern float velocidad;
extern float altitud;

// Variables de los sensores con el mismo propisito
extern float ph;
extern float turbidez;
extern float temperatura;

extern unsigned long ultimo_envio_mqtt;
extern unsigned long ultimo_debug;
extern const unsigned long intervalo_envio;     
extern const unsigned long intervalo_debug; 

#endif