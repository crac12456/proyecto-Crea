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

#include "funciones.h"
#include "config.h"

bool test_gps();
void conectar_wifi();
void mqtt_reconect();
void gps_coneccion();
void callback(char *topic, byte *payload, unsigned int lenght);
void envio_de_datos();
void debug_info();

/*
 * Codigo principal del proyecto
 * todas las funciones se encuentran ordenas en sus respectivos archivos
 * el main se concentra en inicializar lo necesario y
 */

void setup()
{
  Serial.begin(115200);

  // ================== Set up de los pines ==================

  // gps
  pinMode(led_interno, OUTPUT);

  // motores
  pinMode(motor_derecha_1, OUTPUT);
  pinMode(motor_derecha_2, OUTPUT);

  pinMode(motor_izquierda_1, OUTPUT);
  pinMode(motor_izquierda_2, OUTPUT);

  // sensores
  pinMode(sensor_de_temp, INPUT);
  pinMode(sensor_de_ph, INPUT);
  pinMode(sensor_de_turbidez, INPUT);

  // gps
  pinMode(gps_RX, INPUT);
  pinMode(gps_TX, OUTPUT);

  // ================== Set up de sensores, etc ==================

  client.setCallback(callback);

  // set up del gps
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);

  // funcion en redes.h, conecta el esp32 con la red wifi
  conectar_wifi();

  // funcion en redes.h, conecta el esp32 con el broker mqtt
  mqtt_reconect();

  // indicamos que el setup se termino correctamente
  indicador(1, 2);

  motores_detener();
}

void loop()
{

  if (!client.connected())
  {
    Serial.print("conectando a mqtt");
    mqtt_reconect();
  }

  client.loop();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Conecting wifi");
    conectar_wifi();
  }

  gps_coneccion();

  temperatura = medicion_temperatura();
  ph = medicion_de_ph();
  turbidez = medicion_de_turbidez();

  // comproamo que el gps este conectado correctamente
  gps_coneccion();

  static unsigned long ultimo_envio = 0;
  if (millis() - ultimo_envio >= 2000)
  {
    ultimo_envio = millis();

    if (gps.location.isValid())
    {
      latitud = gps.location.lat();
      longitud = gps.location.lng();
      altitud = gps.altitude.meters();
      velocidad = gps.speed.kmph();

      temperatura = medicion_temperatura();
      ph = medicion_de_ph();
      turbidez = medicion_de_turbidez();

      envio_de_datos();

      digitalWrite(led_interno, !digitalRead(led_interno));
    }
    else
    {
      Serial.println("esperando coneccion con el gps");
    }
  }

  // para debuggin
  Serial.println("la temperatura es: ");
  Serial.print(temperatura);
  Serial.println("la latitud es: ");
  Serial.print(latitud);
  Serial.println("la longitud es: ");
  Serial.print(longitud);
  Serial.println("la altitud es: ");
  Serial.print(altitud);
  Serial.println("la velocidad es: ");
  Serial.print(velocidad);
}

bool test_gps()
{
  unsigned long tiempo_desde_inicio = millis();
  const int tiempo_max_ms = 1000;

  while (millis() - tiempo_desde_inicio < tiempo_max_ms)
  {
    if (gpsSerial.available())
    {
      return true;
    }
    delay(10);
  }
  return false;
}

void conectar_wifi()
{
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30)
  {
    delay(500);
    indicador(0, 0);
    intentos++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {

    Serial.println("Wifi conectado");
    indicador(2, 1);
  }
  else
  {
    Serial.println("la coneccion a wifi fallado");
    indicador_fallo(5);
  }
}

void mqtt_reconect()
{

  client.setServer(mqtt_server, mqtt_port);

  int intentos = 0;
  while (!client.connected() && intentos < 5)
  {
    intentos++;
    Serial.println("Conectando al servidor mqtt");
  }

  if (client.connect("ESP32_principal", mqtt_user, mqtt_password))
  {
    Serial.println("esp conectado");

    bool suscrito = client.subscribe(topic_sub);
    if (suscrito)
    {
      Serial.println("suscrito a");
      Serial.print(topic_sub);
    }
    else
    {
      Serial.println("error en la subscripcion");
    }
    indicador(3, 2);
  }
  else
  {
    Serial.println("no se ha podido conectar al servidor mqtt");
    indicador_fallo(10);
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

void callback(char *topic, byte *payload, unsigned int length)
{
  mensaje = "";

  for (int i = 0; i < length; i++)
  {
    mensaje += (char)payload[i];
  }

  // control de los motores
  if (mensaje == "adelante")
  {
    motores_adelante();
  }
  else if (mensaje == "izquierda")
  {
    motores_izquierda();
  }
  else if (mensaje == "derecha")
  {
    motores_derecha();
  }
  else
  {
    motores_detener();
  }
}

void envio_de_datos()
{
  //================== Creacion de un Json para enviar los datos ==================
  StaticJsonDocument<200> doc;

  // declaramos el dispositivo que esamos utilizando
  doc["Dispositivo"] = "Esp32-1";

  // declaracion de los sensores en el json para enviarlos al backend del sitio web
  JsonObject sensoresObj = doc.createNestedObject("Sensores");
  sensoresObj["temperatura"] = temperatura;
  sensoresObj["pH"] = ph;
  sensoresObj["turbidez"] = turbidez;

  // declaramos los datos del gps en el json
  JsonObject gpsObj = doc.createNestedObject("Gps");
  gpsObj["latitud"] = latitud;
  gpsObj["longitud"] = longitud;
  gpsObj["altitud"] = altitud;
  gpsObj["velocidad"] = velocidad;

  // variables temporales para conseguir el tamaño del documento y otras cosas para enviarlo
  char buffer[256];
  size_t n = serializeJson(doc, buffer);

  // envia por mqtt el json
  client.publish(topic_sub, (uint8_t *)buffer, (unsigned int)n);
}

void debug_info()
{

  Serial.println("la temperatura es: ");
  Serial.print(temperatura);
  Serial.println("la latitud es: ");
  Serial.print(latitud);
  Serial.println("la longitud es: ");
  Serial.print(longitud);
  Serial.println("la altitud es: ");
  Serial.print(altitud);
  Serial.println("la velocidad es: ");
  Serial.print(velocidad);
}
