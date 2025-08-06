
/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * * Header de configuracion, aqui se declaran las variables gobales que se usaran * * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */
#ifndef CONFIG_H
#define CONFIG_H

//pines motores
#define motor_derecha_1 27
#define motor_derecha_2 26
#define motor_derecha_velocidad 13

#define motor_izquierda_1 33
#define motor_izquierda_2 34
#define motor_izquierda_velocidad 14

//sensores
#define sensor_de_temp 4  
#define sensor_de_ph 35 
#define sensor_de_turbidez 32 

//GPS
#define gps_RX 17
#define gps_TX 16
#define gps_bauds 9600

//leds
#define led_interno 2

//constantes globales para wifi
const char* ssid = "AREA DE INFORMÀTICA"; //ingresar la web
const char* password = "UEPSIMAINFO*2025*@"; //ingresar contraseña

//constantes de la coneccion con mqtt
const char* mqtt_server = "server";
const int mqtt_port = 0000;
const char* mqtt_pasword = "password";
const char* mqtt_user = "esp_1";

//set up del sensor de temperatura
extern OneWire sensor_de_temperatura;
extern DallasTemperature sensores;

//set up del gps
extern HardwareSerial gpsSerial;
extern TinyGPSPlus gps;

//variables de tiempo para comprovacion y testeo 
extern unsigned long tiempo_desde_inicio;
extern int tiempo_max_ms;

//set up de las comunicaciones, wifi y MQTT
extern WiFiClient espClient;
extern PubSubClient client;

extern double latitud = gps.location.lat();
extern double longitud = gps.location.lng(); 
extern float velocidad = gps.speed.kmph(); 
extern float altitud = gps.altitude.meters(); 

extern float ph = medicion_de_ph();
extern float turbidez = medicion_de_turbidez();
extern float temperatura = medicion_temperatura();


#endif