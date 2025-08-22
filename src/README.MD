# MAIN

Este es el main, el archivo principal y donde se ejecuta todo el codigo, este tiene 4 partes:

- Headers: Son los *#include* donde declaramos las librerias y archivos que incorporamos al archivo.
- Setup: Donde se inician las diferentes partes del proyecto, asi como se conecta a la red, etc.
- Loop: Bucle principal del codigo, este se repetira infinitamente y define lo que hara el microcontrolador.
- funciones: Estos son distintos bloques de codigo esenciales para el funcionamiento del proyecto, se separan del resto para que sea mas facil leer el codigo, mantenerlo, expandirlo, etc. Esta hacen cosas como conectarse al Wifi.

Entre los Header y el main encontramos un apartado llamado "Headers de las funciones", este apartado esta para que el archivo sea mas limpio.

El lenguaje C es procedural, osea, va de arriba a abajo, por lo que las funciones deberian estar arriba del codigo en las que las usaremos, pero esto no es lo mas estetico ni facil de leer, por lo que podemos utilizar esta pre-declaraciones, las cuales le indican al programa que tenemos estas funciones al principio, para que al momento de ejecucion, valla a donde esta el codigo de la función, el cual esta al final del proyecto.

## Headers

Estas son las librerias necesarias para el funcionamiento del proyecto.
tambien incluye 2 Headers propios (funciones.h y config.h) creados para reducir el tamaño del main.

Como se puede ver, tenemos una considerable cantidad de librerias, aunque puede ser pequeña para proyectos de mayor magnitud, a continuacion sus descripcion:

- __Arduino.h:__ Framework de arduino, lo que utilizamos para programar el microcontrolador, a diferencia de Arduino IDE, en platformIO tenemos que declararla ya que tiene compatibilidad con multiples frameworks.
- __stdbool.h__ Parte de la libreria estandart de C, este permite utiliza booleanos (valores binarios que pueden ser positivos o falsos).
- __OneWire.h__ Esta libreria permite utilizar el protocolo OneWire que utiliza un solo cable para comunicarse con cosas como sensores.
- __DallasTemperature.h__ Libreria de la empresa creadora del *DS18B20*, sensor que utilizamos para medir la temperatura, este permite conseguir las mediciones de temperatura de forma sencilla.
- __TinyGPSPlus.h:__ Esta libreria nos permite utilizar el gps de for mas sensilla y entendible
- __HardwareSerial.h__ Utilizamos esta libreria para la comunicacion con el gps, es necesaria para utilizar TinyGPSPlus.
- __PubSubClient.h__ Esta libreria nos permite utilizar comunicaciones de tipo cliente, en este caso utilzamos mqtt, la cual utiliza un metodo de subscripcion y publicacion para las comunicaciones.
- __SPI:__ Libreria necesaria para la coneccion con Wifi.
- __Wifi:__ Esta libreria nos facilita en gran forma la coneccion a una red Wifi, necesaria para mqtt.
- __ArduinoJson:__ Esta libreria permite crear archivos json (el archivo de guardato de datos de javascript), el cual nos permite enviar datos a el backend de la pagina web de forma mas estructurada y legible
- __funciones.h__ Este es un archivo del proyecto, encontrado en la carpeta lib/funciones, en este se encuentra los controladores de los sensores, facilitando su uso, asi como un par de funciones utiles más..
- __config.h:__ En este Header estan declarados todos los pines, asi como distintas variables globales que se utilizan en diferentes partes del proyecto, cosas como constructures de los sensores y informacion necesaria para el WiFi como la red y contraseña, asi como distintas cosas necesarias para el uso de mqtt.

## Setup

Este apartado se uniza para iniciar las funciones, librerias y cosas necesarias para el funcionamiento del proyecto, este de divide en dos partes.

- __pines:__ En este declaramos para que usaremos los pines, si son input o output, esencial para el funcionamiento del proyecto, en el codigo opodremos ver para que es cada pin y todo tienen nombres descriptivos.
- __setup__ En esta parte se inician cosas como la coneccon con el wifi, con el broker mqtt y verificamos que los motores esten apagados.

## Loop

En arduino, el loop es el bucle que se ejecuta de forma infinita en el cual se realizan todos los procesos del microcontrolador, en este tenemos el codigo que controlara al robot y se llaman a las funciones o driver necesarios.

- __Coneccion con las redes:__ En este apartado, se conecta al wifi y a la red y comprueva continuamente que estas conecciones se mantengan, en este caso tenemos la coneccion con el broker mqtt, el loop del cliente, que envia los datos y recibe continuamente, la verificacion de la coneccion al internet y tambien el gps se conecta con los satelites para conseguir la informacion necesaria, aparte de eso tenemos unas funciones donde almacenamos los datos de los sensores para su uso posterior.

- __Envio y recepcion de datos:__ En este apartado, nos aseguramos de que la coneccion con mqtt se mantenga, tambien conseguimos los datos del gps en caso de que este funcione y llamamos a la funcion que envia los datos.

## Funciones del main

En este ultimo apartado tenemos diferentes funciones que utilizamos anteriormente, estas son:

- __conectar_wifi:__ Esta funcion se encarga de conectar el wifi y asegurarse de que se mantenga conectado.
- __mqtt_reconect:__ Esta funcion se encarga de conectarse al broker mqtt, esta necesita la ip del servidor y el puerto al cual conectarse, asi como se suscribe a su tema espesifico, tambien, verifica que la coneccion se mantenga en el tiempo.
- __gps_coneccion:__ Esta se asegura de que el gps se encuetre conectado en todo momento.
- __callback:__ Esta funcion es la que se encarga de recibir los mensajes del broker mqtt, y para ahorrar espacio tambien tiene el apartado en la que se controlan los motores segun la informacion que reciba del broker.
- __envio_de_datos:__ Esta es la parte en la cual se envia los datos de los sensores a la pagina web, lo primero que vemos es la creacion del documento json el cual sera enviado a la pagina, al fina de este tenemos el envio al broker.
