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

====================== INFORMACION DEL PROYECTO =======================

 - Unidad educativa particular siete de mayo
 - 3ro BTU "A"
 - Creado por: Eliel González
 - Creado en: VS code con platformio
 - Github: https://github.com/crac12456/proyecto-Crea 

=======================================================================*/

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

// ================== Set up del proyecto ==================

void setup()
{
  Serial.begin(115200); //inicio de la comunicacion por el terminal

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

  // ================== Set up de sensores y conecciones ==================

  client.setCallback(callback);

  // set up del gps
  sensores.begin();
  gpsSerial.begin(gps_bauds, SERIAL_8N1, gps_RX, gps_TX);

  //conecta el esp32 con la red wifi
  conectar_wifi();

  //conecta el esp32 con el broker mqtt
  mqtt_reconect();

  // indicamos que el setup se termino correctamente
  indicador(1, 2);

  //apag0 los motores por seguridad
  motores_detener();
}

// ================== Loop prinicpal del proyecto ==================

void loop()
{
  // ================== Coneccion con las redes y broker ==================

  // Se asegura de que este conectado al broker 
  if (!client.connected())
  {
    Serial.print("conectando a mqtt");
    mqtt_reconect();
  }

  // Loop de envio de mensajes del mqtt 
  client.loop();

  // se asegura de que el wifi este conectado y lo reconecta
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Conecting wifi");
    conectar_wifi();
  }

  // Funcion de coneccion con el gps 
  gps_coneccion();

  // Declaracion de variables
  temperatura = medicion_temperatura();
  ph = medicion_de_ph();
  turbidez = medicion_de_turbidez();

  // comproamo que el gps este conectado correctamente
  gps_coneccion();

  // ================== Envio y recibo de datos ==================

  // variables para verificacion 
  static unsigned long ultimo_envio = 0; 
  const int tiempo_maximo = 2000;

  //comprovamos que se este enviando la informacion 
  if (millis() - ultimo_envio >= tiempo_maximo)
  {
    ultimo_envio = millis();

    //conseguimos los datos del gps si este esta disponible 
    if (gps.location.isValid())
    {
      latitud = gps.location.lat();
      longitud = gps.location.lng();
      altitud = gps.altitude.meters();
      velocidad = gps.speed.kmph();

      envio_de_datos(); //funcion que envia los datos por mqtt al backend

      digitalWrite(led_interno, !digitalRead(led_interno));
    }
    else
    {
      Serial.println("esperando coneccion con el gps");
    }
  }

  //informacion para debuggin 
  debug_info();

}

// ================== Coneccion con la red ==================
void conectar_wifi()
{
  WiFi.begin(ssid, password);

  const int max_intentos = 30; 
  int intentos = 0;

  //comprueba la cantidad de intentos de coneccion 
  while (WiFi.status() != WL_CONNECTED && intentos < max_intentos)
  {
    delay(500);
    indicador(0, 0);
    intentos++;
  }

  //indicador de coneccion
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

// ================== Coneccion con el broker mqtt ==================
void mqtt_reconect()
{

  client.setServer(mqtt_server, mqtt_port);

  //comprueva la cantidad de intentos de conectarse
  int intentos = 0;
  while (!client.connected() && intentos < 5)
  {
    intentos++;
    Serial.println("Conectando al servidor mqtt");
  }

  //si se ha conectado, se suscribe al broker 
  if (client.connect("ESP32_principal", mqtt_user, mqtt_password))
  {
    Serial.println("esp conectado");

    bool suscrito = client.subscribe(topic_sub);

    // ================== Subscripcion al broker ==================

    //comprueva que este suscrito al broker 
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

//se conecta con el gps y lee los datos 
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

//recibir los mensajes del broker 
void callback(char *topic, byte *payload, unsigned int length)
{
  mensaje = "";

  for (int i = 0; i < length; i++)
  {
    mensaje += (char)payload[i];
  }

  //================== Control de los motores ==================

  // despues de recibir los mensajtes, procesa la informacion para controlar los motores 
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
  StaticJsonDocument<256> doc; // Aumenté un poco por los objetos anidados

  // Información del dispositivo
  doc["Dispositivo"] = "Esp32-1";

  // Sensores
  JsonObject sensoresObj = doc["Sensores"].to<JsonObject>();
  sensoresObj["temperatura"] = temperatura;
  sensoresObj["pH"] = ph;
  sensoresObj["turbidez"] = turbidez;

  // GPS
  JsonObject gpsObj = doc["Gps"].to<JsonObject>();
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

//funcion para debuggin 
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
