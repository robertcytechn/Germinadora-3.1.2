#ifndef CONTROL_LUCES__H
#define CONTROL_LUCES__H

#include <Arduino.h>
#include <RTClib.h>

#include <VARS_.h>
#include <PINS_.h>

// =================================================================
//  CONTROL DE ILUMINACIÓN (Blanca + Roja)
// =================================================================

extern RTC_DS1307 reloj;

void controlLuces(){
    static int ultimaPotenciaLuces = -1;
    static bool ultimoEstadoLucesRojas = false;
    static unsigned long ultimoCambioLucesRojas = 0;
    
    DateTime ahora = reloj.now();
    int minutosActuales = ahora.hour() * 60 + ahora.minute();
    int minutosInicio = initDia * 60;
    int minutosFin = finDia * 60;

    // 1. Control Luces Blancas (Amanecer/Atardecer suave)
    if (minutosActuales >= minutosInicio && minutosActuales <= minutosFin) {
        // Amanecer gradual
        if (minutosActuales < minutosInicio + duracionAmanecer) {
            potenciadeluces = map(minutosActuales, minutosInicio, minutosInicio + duracionAmanecer, 0, 255);
        }
        // Atardecer gradual
        else if (minutosActuales > minutosFin - duracionAmanecer) {
            potenciadeluces = map(minutosActuales, minutosFin - duracionAmanecer, minutosFin, 255, 0);
        }
        // Pleno día
        else {
            potenciadeluces = 255;
        }
    } else {
        potenciadeluces = 0; // Noche
    }
    
    // Solo escribir si hay cambio en la potencia
    if (potenciadeluces != ultimaPotenciaLuces) {
        analogWrite(LUCES_BLANCAS_PIN, potenciadeluces);
        ultimaPotenciaLuces = potenciadeluces;
    }


    // 2. Control LEDs Rojos (Espectro rojo para fotosíntesis)
    // Se encienden despues de la duracionAmanecer y se apagan antes de la duracionAmanecer
    // Esto da un periodo de luz roja más corto pero intenso
    bool nuevoEstadoRojas = false;
    if (minutosActuales >= (minutosInicio + duracionAmanecer) && minutosActuales <= (minutosFin - duracionAmanecer)) {
        nuevoEstadoRojas = true;
    }
    
    // Solo escribir si hay cambio en el estado y ha pasado el tiempo de debouncing
    if (nuevoEstadoRojas != ultimoEstadoLucesRojas) {
        unsigned long tiempoActual = millis();
        if (tiempoActual - ultimoCambioLucesRojas > 2000) { // Debounce de 2 segundos
            digitalWrite(LEDS_ROJOS_PIN, nuevoEstadoRojas ? HIGH : LOW);
            lucesRojasOn = nuevoEstadoRojas;
            ultimoEstadoLucesRojas = nuevoEstadoRojas;
            ultimoCambioLucesRojas = tiempoActual;
        }
    }
}



#endif // CONTROL_LUCES__H