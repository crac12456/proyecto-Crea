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

extern OneWire sensor_de_temperatura;
extern DallasTemperature sensores;

//GPS
#define gps_RX 17
#define gps_TX 16
#define gps_bauds 9600

extern HardwareSerial gpsSerial;
extern TinyGPSPlus gps;

#endif