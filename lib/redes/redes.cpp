#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdbool.h>
#include <TinyGPSPlus.h>

#include "config.h"
#include "funciones.h"


void conectar_wifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        indicador(0, 0);
    }
    indicador(2, 1);
}

void mqtt_reconect()
{
    client.setServer(mqtt_server, mqtt_port);
    while (!client.connected())
    {
        if (client.connect("ESP32_principal", mqtt_user, mqtt_pasword))
        {
            indicador(3, 2);
        }
        else
        {
            indicador_fallo(10);
        }
    }
}

void gps_coneccion()
{
    while (test_gps())
    {
        while (gpsSerial.available() > 0)
        {
            gps.encode(gpsSerial.read());
        }
    }
}

bool test_gps()
{
    unsigned long tiempo_desde_inicio = millis();
    int tiempo_max_ms = 1000;
    
    while (millis() - tiempo_desde_inicio < tiempo_max_ms)
    {
        return true;
    }
    return false;
}

void callback(char* topic, byte* payload, unsigned int lenght){
    String mensaje;
    for(int i = 0; i <= lenght; i++){
        mensaje += (char)payload[i];
    }

}