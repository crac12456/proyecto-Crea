#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <Arduino.h>

// Control de los motores
void control_motores(char mensaje);
void motores_adelante();
void motores_atras();
void motores_izquierda();
void motores_derecha();
void motores_detener();

// Sensores
float medicion_de_turbidez(); 
float medicion_de_ph();
float medicion_temperatura();

float redondeo(float medicion);

// Para indicar mediante la luz
void indicador(int cant, int vel);
void indicador_fallo(int cant);

bool test_gps();

#endif

