# Driver y funciones del proyeto

## Sensor de turbidez

en esta funcion tengo 2 parametros, una variable NTU (unidad nefelométrica de turbidez)
y el pin utilizado (posiblemente lo borre)

En esta funcion lo primero que hago es conseguir una funcion de voltaje del pin del sensor de turvidez, lo divido para 1023 y multiplico para 5, esto para convertir la entrada analogica en voltaje para una facil conversion

Despues de esto tenemos un bucle for el cual repetira la medida 800 veces para asegurarnos de que esta sea lo mas correcta posible

De ahi conseguimos el promeio de las 800 repeticiones utilizando una funcion que he creado para redondear la funcion voltaje

Despues de eso asigno tengo un switch que revisa el voltaje y le asigna el NTU correspondiente en caso de que la lectura exeda el maximo o minimo

En caso de que no sea asi, esta leera el voltaje y lo convertira en NTU

## Sensor de PH

El ph es la cantidad de particulas de hidrogeno en un liquido o sustancia,
es una nidad logaritmica que muestra la acides o basicidad de un liquido

en la programacion, primero se leen 20 veces el sensor para luego redondearlo, obetiendo una medicion mas fiable
Convertimos el valor analogico a ph y nos aseguramos de que la medicion sea correcta

## Sensor de temperatura

El sensor de temperatura DS18B20 utiliza un protocolo one wire, el cual permite enviar y recibir informacion mediante un cable
con las librerias **OneWire** podemoss controlar esto facilmente
ademas, la libreria DallasTemperature permite conseguir facilmente la temperatura del sensor solamente llamando un par de funciones

Estas funciones son:

- **OneWire nombre(pin)**: esta es la declaracion del pin, indica a la libreria OneWire cual pin es e que teine el sensor
- **sensors.begin()**: esta indica a los sensores que deben de iniziar
- **sensors.recuestTemperatures()**: esta funcion indica a los sensores que empiezen a funcionar
- **sensors.getTempXVyIndex(pin)**: esta funcion consigue la temperatura del sensor en °C, la almacenamos en una variable

Aparte de esto, el codigo tiene un bucle if el cual comprueba que el sensor funcione correctamente

## Funciones de motores

para que el robot se mueva, tenemos 3 funciones para cada direccion, y en caso de que se este detenido, simplemente no se llamara a ninguna

estas funciones tienen 2 partes, la del motor derecho y del motor izquierdo, utilizo un **digitalWrite**.
esto le indica al controlador L982 en que dirreccion girara este motor
para que valla hacia delante activo ambos al mismo tiempo en dirreciones contrarias
para que valla a la izquierda desactivo el izquierdo y activo el derecho, para la derecha hago lo contrario

## Funciones de los leds

funciones para comprovar que todo funcione correctamente, mostrar que este transcurriendo correcamente el set up o mostrar un fallo
