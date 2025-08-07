#ifndef REDES_H
#define REDES_H

void conectar_wifi();
void mqtt_reconect();
void gps_coneccion();
void callback(char* topic, byte* payload, unsigned int lenght);

extern String mensaje; 

#endif