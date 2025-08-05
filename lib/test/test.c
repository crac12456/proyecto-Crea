#include <Arduino.h>
#include "config.h"
#include <stdbool.h>


bool test_gps(){
    while(millis() - tiempo_desde_inicio < tiempo_max_ms){
        return true;
    }
    return false;
}