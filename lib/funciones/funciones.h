#ifndef FUNCIONES_H
#define FUNCIONES_H

#ifdef __cplusplus
extern "C" {
#endif

//control de los motores
void motores_adelante();
void motores_izquierda();
void motores_derecha();

//sensores
float medicion_de_turbidez(); 
float medicion_de_ph();
float medicion_temperatura();

#ifdef __cplusplus
}
#endif

#endif

