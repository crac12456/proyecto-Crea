#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <math.h>

#include "funciones.h"
#include "config.h"

/*=======================================================================
|                    * Funciones de los sensores *                      |
=========================================================================
 *                                                                      *
 *   Aqui esta la programacion de los drivers de los sensores           *
 *   Las librerias se encuetran arriba, accesibles desde platformIO     *
 *   estan programadas con sintaxis de C para mayor simplicidad         *
 *                                                                      *
=======================================================================*/

// Variable para un mejor entendimiento 
const float voltaje_del_esp = 3.3;
const float lectura_analogica_max = 4095.0;

// ================== Driver del sensor de Turbidez ==================

float medicion_de_turbidez()
{
    float voltaje = 0;
    float NTU = 0;
    const int cant_de_lecturas = 800;

    // Se convierte a voltaje para entenderlo mas facilmente
    voltaje = analogRead(sensor_de_turbidez);
    temporizador(1);

    // Repetimos 800 veces la medicion para tener el dato mas veridico posible
    for (int i = 0; i < cant_de_lecturas; i++)
    {
        voltaje += ((float)analogRead(sensor_de_turbidez) * (voltaje_del_esp / lectura_analogica_max));
    }

    // Promedio de repetir la medicion
    voltaje /= cant_de_lecturas;

    voltaje = (voltaje / lectura_analogica_max) * voltaje_del_esp;
    voltaje = redondeo(voltaje);

    // Asignamos NTU
    if (voltaje < 2.5)
    {
        // NTU maximo
        //NTU = 3000;
    }
    else if (voltaje < 4.2)
    {
        // NTU minimo
        NTU = 0;
        voltaje = 4.2;
    }
    else
    {
        // Calcula el NTU si no es el maximo o minimo
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
        temporizador(10);
    }

    voltaje /= cantidad_de_lecturas;

    voltaje = (voltaje / lectura_analogica_max) * voltaje_del_esp;

    // lee la entrada del sensor y la convierte a pH
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

    sensores.requestTemperatures();     // Le indicamos a los senores que consigan la temperatura
    temp = sensores.getTempCByIndex(0); // Conseguimos la temperatura en °C de los sensores

    if (temp == DEVICE_DISCONNECTED_C)
    {
        Serial.println("sensor de temperatura desconectado");
        return -999.0;
    }

    return temp;
}

// ================== Controles de los motores ==================

void control_motores(char mensaje) {    
    Serial.println("procesando mensaje");

    switch (mensaje)
    {
        case 'F':
            Serial.println("Mensaje recibido, motores adelante");
            motores_adelante();
            break;
        case 'B':
            Serial.println("Mensaje recibido, motores atras");
            motores_atras();
            break;
        case 'R':
            Serial.println("Mensaje recibido, motores derecha");
            motores_derecha();
            break;
        case 'L':
            Serial.println("Mensaje recibido, motores izquierda");
            motores_izquierda();
            break;
        case 'S':
            Serial.println("Mensaje recibido, motores detener");
            motores_detener();
            break;
        default:
            break;
    }
}
// Adelante 
void motores_adelante()
{
    digitalWrite(motor_derecha_1, HIGH);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, HIGH);

    Serial.println("motores adelante");
}

void motores_atras()
{
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, HIGH);

    digitalWrite(motor_izquierda_1, HIGH);
    digitalWrite(motor_izquierda_2, LOW);

    Serial.println("motores atras");
}

// Derecha 
void motores_derecha()
{
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, HIGH);

    Serial.println("motores deracha");
}

// Izquierda
void motores_izquierda()
{
    digitalWrite(motor_derecha_1, HIGH);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, LOW);

    Serial.println("motores izquierda");
}

// Detener 
void motores_detener()
{
    digitalWrite(motor_derecha_1, LOW);
    digitalWrite(motor_derecha_2, LOW);

    digitalWrite(motor_izquierda_1, LOW);
    digitalWrite(motor_izquierda_2, LOW);

    Serial.println("motores detener");
}

// ================== Indicadores de luz ==================

// Para indicar que haya cargado todo 
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
        temporizador(vel);
        digitalWrite(led_interno, LOW);
        temporizador(vel);
    }
    temporizador(1000);
}

// Indicador de fallos
void indicador_fallo(int cant)
{
    for (int i = 0; i <= cant; i++)
    {
        digitalWrite(led_interno, HIGH);
        temporizador(1000);
        digitalWrite(led_interno, LOW);
        temporizador(100);
        digitalWrite(led_interno, HIGH);
        temporizador(2000);
        digitalWrite(led_interno, LOW);
        temporizador(100);
        digitalWrite(led_interno, HIGH);
        temporizador(1000);
        digitalWrite(led_interno, LOW);
        temporizador(100);
    }
}

bool test_gps()
{
    return gpsSerial.available() > 0;
}

bool temporizador (unsigned long intervalo) {
    static unsigned long tiempo_anterior = 0;
    unsigned long ahora = millis();

    if (ahora - tiempo_anterior >= intervalo)
    {
        tiempo_anterior = ahora;
        return true;
    }
    return false;
}