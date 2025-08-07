#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>

#include "funciones.h"
#include "config.h"

/*
 * Medidor de turbidez
 * el codigo toma el input del sensor de turvidez y lo convierte a voltaje y luego a NTU
 * o la "Unidad Nefelometrica de turbidez"
 */

float medicion_de_turbidez()
{

    float voltaje = 0;
    float NTU = 0;

    // se convierte a voltaje para entenderlo mas facilmente
    voltaje = analogRead(sensor_de_turbidez) * (3.3 / 4095.0);

    // repetimos 800 veces la medicion para tener el dato mas veridico posible
    for (int i = 0; i < 800; i++)
    {
        voltaje += ((float)analogRead(sensor_de_turbidez) * (3.3 / 4095.0));
    }

    // Promedio de repetir la medicion
    voltaje /= 800;
    voltaje = redondeo(voltaje);

    // asignamos NTU
    if (voltaje < 2.5)
    {
        // NTU maximo
        NTU = 3000;
    }
    else if (voltaje < 4.2)
    {
        // NTU minimo
        NTU = 0;
        voltaje = 4.2;
    }
    else
    {
        // calcula el NTU si no es el maximo o minimo
        NTU = -1120.4 * sqrt(voltaje) + 5742.3 * voltaje - 4353.8;
    }

    return NTU;
}

// Hacemos esta funcion para redondear el voltaje
float redondeo(int medicion)
{
    float multiplicador = powl(10.0f, 1);
    medicion = roundf(medicion * multiplicador) / multiplicador;
    return medicion;
}

/*
 * Funcion que mide el ph, esta lee la entrada analogica del sensor y lo convierte a ph
 * luego regresa el ph
 */

float medicion_de_ph()
{

    float ph;

    // lee la entrada del sensor y la convierte a PH
    ph = 4095.0 - (analogRead(sensor_de_ph) / 73.07);

    return ph;
}

/*
 * Funcion para medir la temperatura, 
 * utiliza la libreria dallas temperatura y one wire 
 */

float medicion_temperatura()
{
    float temp;

    sensores.requestTemperatures();     // le indicamos a los senores que consigan la temperatura
    temp = sensores.getTempCByIndex(0); // conseguimos la temperatura en °C de los sensores

    return temp;
}

/* 
 * Seccion del los motores
 * aqui se prenden y se apagan los motores para dirigir el robot
 */

// seccion motores
void motores_adelante()
{

    // le decimos al motor derecho acelerar
    digitalWrite(motor_derecha_1, HIGH);
    digitalWrite(motor_derecha_2, LOW);

    // le decimos al motor izquierdo a acelerar
    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, HIGH);
}

void motores_izquierda()
{

    // le decimos al motor derecho acelerar
    digitalWrite(motor_derecha_1, HIGH);
    digitalWrite(motor_derecha_2, LOW);

    // le decimos al motor izquierdo detenerse
    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, LOW);
}

void motores_derecha()
{
    // le decimos al motor derecho detenerse
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, LOW);

    // le decimos al motor derecho acelerar
    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, HIGH);
}

void motores_detener(){
    // le decimos al motor derecho detenerse
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, LOW);

    // le decimos al motor derecho acelerar
    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, LOW);
}

/*
 * Funciones con los leds, utilizamos estas para comprovar que algo funcione, identificar errores, etc.
 */

void indicador(int cant, int vel)
{

    vel *= 100;

    if (cant == 0 && vel == 0)
    {
        goto permanente;
    }

    for (int i = 0; i >= cant; i++)
    {
        digitalWrite(led_interno, HIGH);
        delay(vel);
        digitalWrite(led_interno, LOW);
        delay(vel);
    }
    delay(1000);

permanente:
    digitalWrite(led_interno, HIGH);
}

void indicador_fallo(int cant)
{

    for (int i = 0; i <= cant; i++)
    {
        digitalWrite(led_interno, HIGH);
        delay(1000);
        digitalWrite(led_interno, LOW);
        delay(100);
        digitalWrite(led_interno, HIGH);
        delay(2000);
        digitalWrite(led_interno, LOW);
        delay(100);
        digitalWrite(led_interno, HIGH);
        delay(1000);
        digitalWrite(led_interno, LOW);
        delay(100);
    }
}