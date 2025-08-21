#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <math.h>

#include "funciones.h"
#include "config.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// FUNCIONES DE LOS SENSORES /////////////////////////////////////////////
////////////////////////// Aqui esta la programacion de los drivers de los sensores //////////////////////////
////////////////// Las librerias se encuetran arriba, facilmenta accesibles desde platformIO /////////////////
////////////////////////////////// estan programadas con sintaxis de C ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// variable para un mejor entendimiento 
const float voltaje_del_esp = 3.3;
const float lectura_analogica_max = 4095.0;

// ================== Driver del sensor de Turbidez ==================

float medicion_de_turbidez()
{

    float voltaje = 0;
    float NTU = 0;
    const int cant_de_lecturas = 800;

    // se convierte a voltaje para entenderlo mas facilmente
    voltaje = analogRead(sensor_de_turbidez);
    delayMicroseconds(100);

    // repetimos 800 veces la medicion para tener el dato mas veridico posible
    for (int i = 0; i < cant_de_lecturas; i++)
    {
        voltaje += ((float)analogRead(sensor_de_turbidez) * (voltaje_del_esp / lectura_analogica_max));
    }

    // Promedio de repetir la medicion
    voltaje /= cant_de_lecturas;

    voltaje = (voltaje / lectura_analogica_max) * voltaje_del_esp;
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
float redondeo(float medicion)
{
    float multiplicador = powl(10.0f, 1);
    medicion = roundf(medicion * multiplicador) / multiplicador;
    return medicion;
}

// ================== Driver del sensor de pH ==================

float medicion_de_ph()
{

    float voltaje = 0;
    const int cantidad_de_lecturas = 20;

    for (int i = 0; i < cantidad_de_lecturas; i++)
    {
        voltaje = +analogRead(sensor_de_ph);
        delay(10);
    }

    voltaje /= cantidad_de_lecturas;

    voltaje = (voltaje / lectura_analogica_max) * voltaje_del_esp;

    // lee la entrada del sensor y la convierte a PH
    float ph = 7.0 - ((voltaje - 1.65) / 0.18);

    if (ph < 0)
    {
        ph = 0;
    }
    if (ph > 14)
    {
        ph = 14;
    }

    return ph;
}

// ================== Driver del medidor de temperatura ==================

float medicion_temperatura()
{
    float temp;

    sensores.requestTemperatures();     // le indicamos a los senores que consigan la temperatura
    temp = sensores.getTempCByIndex(0); // conseguimos la temperatura en °C de los sensores

    if (temp == DEVICE_DISCONNECTED_C)
    {
        Serial.println("sensor de temperatura desconectado");
        return -999.0;
    }

    return temp;
}

// ================== Controles de los motores ==================

// Adelante 
void motores_adelante()
{
    digitalWrite(motor_derecha_1, HIGH);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, HIGH);
}

// Derecha 
void motores_derecha()
{
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, HIGH);
}

// Izquierda
void motores_izquierda()
{
    digitalWrite(motor_derecha_1, HIGH);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, LOW);
}

// Detener 
void motores_detener()
{
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, LOW);
}

// ================== Indicadores de luz ==================

// para indicar que halla cargado todo 
void indicador(int cant, int vel)
{

    vel *= 100;

    if (cant == 0 && vel == 0)
    {
        digitalWrite(led_interno, HIGH);
    }

    for (int i = 0; i < cant; i++)
    {
        digitalWrite(led_interno, HIGH);
        delay(vel);
        digitalWrite(led_interno, LOW);
        delay(vel);
    }
    delay(1000);
}

// Indicador de fallo 
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