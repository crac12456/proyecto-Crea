#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <Arduino.h>

//control de los motores
void motores_adelante();
void motores_izquierda();
void motores_derecha();
void motores_detener();

//sensores
float medicion_de_turbidez(); 
float medicion_de_ph();
float medicion_temperatura();

float redondeo(float medicion);

//para indicar mediante la luz
void indicador(int cant, int vel);
void indicador_fallo(int cant);

#endif

