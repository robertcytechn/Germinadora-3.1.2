#ifndef CONTROL_LUCES__H
#define CONTROL_LUCES__H

#include <Arduino.h>
#include <RTClib.h>

#include <PINS_.h>

// =================================================================
//  CONTROL DE ILUMINACIÓN (Blanca + Roja)
// =================================================================
// Configuración de iluminación
int initDia = 7 * 60; // inicio del cilo del dia en minutos (7:00 AM)
int finDia = 21 * 60; // fin del ciclo del dia en minutos (9:00 PM)
int duracionAmanecer = 90; // duración del amanecer/atardecer en minutos
int potenciadeluces = 0;
unsigned long tiempoReaccion = 1 * 60000UL; // tiempo de reaccion de las luces en milisegundos (1 minuto)
unsigned long ultimoCambio = 0;

extern RTC_DS1307 reloj;

void controlLuces(){

    int tiempoActualMinutos = reloj.now().hour() * 60 + reloj.now().minute();

    // comprobar si ha pasado el tiempo de reaccion desde el último cambio
    unsigned long tiempoActual = millis();
    if (tiempoActual - ultimoCambio < tiempoReaccion) {
        // No ha pasado suficiente tiempo desde el último cambio salimos de la función y no hacemos nada
        return;
    }
    ultimoCambio = tiempoActual;

    // revisamos si es de dia o de noche
    bool esDia = (tiempoActualMinutos >= initDia && tiempoActualMinutos < finDia);
    //si es de noche salimos de la función y apagamos todo
    if (!esDia) {
        potenciadeluces = 0;
        analogWrite(LUCES_BLANCAS_PIN, potenciadeluces);
        digitalWrite(LEDS_ROJOS_PIN, LOW);
        return;
    }
    else{
        //si es de dia, calculamos la potencia de las luces blancas según la hora actual  - si estamos en amanecer o atardecer
        int minutosDesdeInicioDia = tiempoActualMinutos - initDia;
        if (minutosDesdeInicioDia < duracionAmanecer) {
            // Amanecer
            potenciadeluces = map(minutosDesdeInicioDia, 0, duracionAmanecer, 0, 255);
        }
        else if (minutosDesdeInicioDia > (finDia - initDia - duracionAmanecer)) {
            // Atardecer
            potenciadeluces = map(minutosDesdeInicioDia, finDia - initDia - duracionAmanecer, finDia - initDia, 255, 0);
        }
        else {
            // Luz plena durante el día tambien encendemos luces rojas
            potenciadeluces = 255;
        }
        analogWrite(LUCES_BLANCAS_PIN, potenciadeluces);
        //revisamos que las luces rojas solo se enciendan despues del amanecer completo y se apaguen antes del atardecer
        if (minutosDesdeInicioDia >= duracionAmanecer && minutosDesdeInicioDia < (finDia - initDia - duracionAmanecer)) {
            digitalWrite(LEDS_ROJOS_PIN, HIGH);
        }
        else {
            digitalWrite(LEDS_ROJOS_PIN, LOW);
        }
        return;
    }

}



#endif // CONTROL_LUCES__H