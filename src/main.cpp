/*=======================================================================

  /$$$$$$                                      /$$
 /$$__  $$                                    | $$
| $$  \__/ /$$$$$$/$$$$   /$$$$$$   /$$$$$$  /$$$$$$
|  $$$$$$ | $$_  $$_  $$ |____  $$ /$$__  $$|_  $$_/
 \____  $$| $$ \ $$ \ $$  /$$$$$$$| $$  \__/  | $$
 /$$  \ $$| $$ | $$ | $$ /$$__  $$| $$        | $$ /$$
|  $$$$$$/| $$ | $$ | $$|  $$$$$$$| $$        |  $$$$/
 \______/ |__/ |__/ |__/ \_______/|__/         \___/

 /$$$$$$$  /$$
| $$__  $$| $$
| $$  \ $$| $$ /$$   /$$  /$$$$$$
| $$$$$$$ | $$| $$  | $$ /$$__  $$
| $$__  $$| $$| $$  | $$| $$$$$$$$
| $$  \ $$| $$| $$  | $$| $$_____/
| $$$$$$$/| $$|  $$$$$$/|  $$$$$$$
|_______/ |__/ \______/  \_______/

  /$$$$$$                        /$$     /$$                     /$$
 /$$__  $$                      | $$    |__/                    | $$
| $$  \__/  /$$$$$$  /$$$$$$$  /$$$$$$   /$$ /$$$$$$$   /$$$$$$ | $$
|  $$$$$$  /$$__  $$| $$__  $$|_  $$_/  | $$| $$__  $$ /$$__  $$| $$
 \____  $$| $$$$$$$$| $$  \ $$  | $$    | $$| $$  \ $$| $$$$$$$$| $$
 /$$  \ $$| $$_____/| $$  | $$  | $$ /$$| $$| $$  | $$| $$_____/| $$
|  $$$$$$/|  $$$$$$$| $$  | $$  |  $$$$/| $$| $$  | $$|  $$$$$$$| $$
 \______/  \_______/|__/  |__/   \___/  |__/|__/  |__/ \_______/|__/

========================= INFORMACION DEL PROYECTO ==========================
*                                                                           *
*             Unidad Educativa Particular "Siete de Mayo"                   *
*        3ro de Bachillerato Técnico Unificado - Paralelo "A"               *
*                                                                           *
=============================================================================
*                                                                           *
*  - Creado por: Eliel González                                             *
*  - Creado en: Visual Studio Code con platformio                           *
*  - Github (Código): https://github.com/crac12456/proyecto-Crea            *
*  - Github (Sitio Web): https://github.com/crac12456/ProyectoCREA-web.git  *
*                                                                           *
============================================================================*/

// ================== Headers ==================
#include <Arduino.h>
#include <stdbool.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <stdbool.h>
#include <BluetoothSerial.h>

#include "funciones.h"
#include "config.h"

// ================== Headers de las funciones ==================
bool test_gps();
void conectar_wifi();
void mqtt_reconect();
void gps_coneccion();
void callback(char *topic, byte *payload, unsigned int lenght);
void envio_de_datos();
void debug_info();
void test_motores();

// ================== Set up del proyecto ==================

void setup()
{
  Serial.begin(115200); // Inicio de la comunicacion por el terminal
  Serial.println("================== Proyecto Crea ==================");
  Serial.println(" Debuggin ");
  Serial.println("===================================================");

  // ================== Set up de los pines ==================

  // GPS
  pinMode(led_interno, OUTPUT);

  // Motores
  pinMode(motor_derecha_1, OUTPUT); // Derecho
  pinMode(motor_derecha_2, OUTPUT);

  pinMode(motor_izquierda_1, OUTPUT); // Izquierdo
  pinMode(motor_izquierda_2, OUTPUT);

  // Sensores
  pinMode(sensor_de_temp, INPUT);
  pinMode(sensor_de_ph, INPUT);
  pinMode(sensor_de_turbidez, INPUT);

  // GPS
  pinMode(gps_RX, INPUT);
  pinMode(gps_TX, OUTPUT);

  // ================== Set up de sensores y conecciones ==================

  client.setCallback(callback);

  // Set up del gps
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);

  // Conecta el esp32 con la red wifi
  conectar_wifi();

  // Conecta el esp32 con el broker mqtt
  mqtt_reconect();

  serialbt.begin("Smart Blue Sentinel");

  // Indicamos que el setup se termino correctamente
  indicador(1, 2);

  // Apago los motores por seguridad
  motores_detener();
  test_motores();
}

// ================== Loop prinicpal del proyecto ==================

void loop()
{
  // ================== Conexión con las redes y broker ==================

  // Verifica que este conectado al broker
  if (!client.connected())
  {
    Serial.print("conectando a mqtt");
    mqtt_reconect();
  }

  if(serialbt.available())
  {
    mensaje = serialbt.read();
    control_motores(mensaje);
  }

  // Loop de envio de mensajes del mqtt
  client.loop();

  // Verifica la conexión con el wifi y lo reconecta de ser necesario
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Conecting to Wifi");
    conectar_wifi();
  }

  // Coneccion con el GPS
  gps_coneccion();

  // Declaración de variables
  temperatura = medicion_temperatura();
  ph = medicion_de_ph();
  turbidez = medicion_de_turbidez();

  // ================== Envio y recibo de datos ==================

  // Variables para verificación
  static unsigned long ultimo_envio = 0;
  const int tiempo_maximo = 2000;

  // Si se estan enviando los datos, consegue los datos del GPS
  if (millis() - ultimo_envio >= tiempo_maximo)
  {
    ultimo_envio = millis();

    // Conseguimos los datos del GPS si este esta disponible
    if (gps.location.isValid())
    {
      latitud = gps.location.lat();
      longitud = gps.location.lng();
      altitud = gps.altitude.meters();
      velocidad = gps.speed.kmph();

      envio_de_datos(); // Función que envia los datos por mqtt al backend

      digitalWrite(led_interno, !digitalRead(led_interno));
    }
    else
    {
      Serial.println("Esperando coneccion con el gps");
    }
  }

  // debug
  test_motores();
}

// ================== Conexión con la red ==================
void conectar_wifi()
{
  // Set up necesario para empezar la conexión
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  const int max_intentos = 30;
  int intentos = 0;

  Serial.print("Conectando: ");

  // Verifica la cantidad de intentos de conectarse con la red
  while (WiFi.status() != WL_CONNECTED && intentos < max_intentos)
  {
    delay(500);
    Serial.print(".");
    indicador(0, 0);
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWifi conectado");
    indicador(2, 1);
  }
  else
  {
    Serial.println("\nla coneccion a wifi a fallado");
    indicador_fallo(5);
  }
}

// ================== Conexión con el broker mqtt ==================
void mqtt_reconect()
{
  // Set up de la conexión
  client.setServer(mqtt_server, mqtt_port);

  // Verifica la cantidad de intentos para realizar la conexión
  int intentos = 0;
  Serial.println("Conectando al servidor mqtt");
  while (!client.connected() && intentos < 5)
  {
    Serial.print(" . ");
    intentos++;
  }

  // Si se ha conectado, se suscribe al broker
  if (client.connect(mqtt_id, mqtt_user, mqtt_password))
  {
    Serial.println("\nEsp32 conectado al broker");

    // Se subscribe al broker
    //subscrito = client.subscribe(topic_sub);

    // Comprueba que este suscrito al broker
    if (subscrito)
    {
      Serial.println("suscrito a: ");
      Serial.print(topic_sub);
    }
    else
    {
      Serial.println("\nerror en la subscripcion");
    }
    indicador(3, 2);
  }
  else
  {
    Serial.println("\nno se ha podido conectar al servidor mqtt");
    indicador_fallo(10);
  }
}

// Se conecta con el GPS y lee los datos
void gps_coneccion()
{
  while (gpsSerial.available() > 0)
  {
    while (gpsSerial.available() > 0)
    {
      gps.encode(gpsSerial.read());
    }
  }
}

// Recibir los mensajes del broker
void callback(char *topic, byte *payload, unsigned int length)
{
  if (subscrito)
  {  mensaje = "";

    for (int i = 0; i < length; i++)
    {
      mensaje += (char)payload[i];
    }

    control_motores(mensaje);
  }
}

void envio_de_datos()
{
  //================== Creacion de un Json para enviar los datos ==================
  JsonDocument doc;

  // Información del dispositivo
  doc["Dispositivo"] = "Esp32-1";

  // Sensores
  doc["temperatura"] = temperatura;
  doc["pH"] = ph;
  doc["turbidez"] = turbidez;

  // GPS
  doc["latitud"] = latitud;
  doc["longitud"] = longitud;
  doc["altitud"] = altitud;
  doc["velocidad"] = velocidad;

  // Envio del documentos Json
  if (subscrito)
  {
    char buffer[256];
    size_t n = serializeJson(doc, buffer);

    client.publish(topic_sub, (uint8_t *)buffer, (unsigned int)n);
  }
  else
  {
   // http.begin(client_WiFi, server);

    //http.addHeader("content type");
  }
}

// ================== Debugging ==================

void debug_info()
{
  Serial.println("La temperatura es: ");
  Serial.print(temperatura);
  Serial.println("La latitud es: ");
  Serial.print(latitud);
  Serial.println("La longitud es: ");
  Serial.print(longitud);
  Serial.println("La altitud es: ");
  Serial.print(altitud);
  Serial.println("La velocidad es: ");
  Serial.print(velocidad);
  delay(1000);
}

void test_motores()
{
  Serial.println("Adelante");
  motores_adelante();
  delay(100);
  Serial.println("Derecha");
  motores_izquierda();
  delay(100);
  Serial.println("Izquierda");
  motores_izquierda();
  delay(100);
  Serial.println("Detener");
  motores_izquierda();
  delay(100);
}