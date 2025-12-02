#ifndef CONTROL_LUCES__H
#define CONTROL_LUCES__H

#include <Arduino.h>
#include <RTClib.h>

#include <PINS_.h>
#include <VARS_.h>

// =================================================================
//  CONTROL DE ILUMINACIÓN (Blanca + Roja)
// =================================================================

extern RTC_DS1307 reloj;

void controlLuces(){
    // Si el modo anti-hongos está activo, encendemos todas las luces al 100%
    if (modoAntiHongos) {
        potenciadeluces = 255;
        analogWrite(LUCES_BLANCAS_PIN, 255);
        digitalWrite(LEDS_ROJOS_PIN, RELAY_ENCENDIDO);
        static bool mensajeMostrado = false;
        if (!mensajeMostrado) {
            Serial.println("CONTROL LUCES: Modo ANTI-HONGOS - Todas las luces al 100%.");
            mensajeMostrado = true;
        }
        return;
    }

    // comprobar si ha pasado el tiempo de reaccion desde el último cambio
    if (TIEMPO_ACTUAL_MS - ultimoCambio < TIEMPO_REACCION_LUCES) {
        // No ha pasado suficiente tiempo desde el último cambio salimos de la función y no hacemos nada
        return;
    }
    ultimoCambio = TIEMPO_ACTUAL_MS;
     int tiempoActualMinutos = RELOJ_GLOBAL.hour() * 60 + RELOJ_GLOBAL.minute();

    // revisamos si es de dia o de noche
    bool esDia = (tiempoActualMinutos >= initDia && tiempoActualMinutos < finDia);
    //si es de noche salimos de la función y apagamos todo
    static bool ultimoEstadoDia = !esDia; // Para detectar el cambio
    if (!esDia) {
        potenciadeluces = 0;
        analogWrite(LUCES_BLANCAS_PIN, potenciadeluces);
        digitalWrite(LEDS_ROJOS_PIN, RELAY_APAGADO);
        if (ultimoEstadoDia != esDia) { Serial.println("CONTROL LUCES: Es de noche, apagando luces."); ultimoEstadoDia = esDia; }
        return; 
    }
    else{
        //si es de dia, calculamos la potencia de las luces blancas según la hora actual  - si estamos en amanecer o atardecer
        int minutosDesdeInicioDia = tiempoActualMinutos - initDia;
        if (minutosDesdeInicioDia < duracionAmanecer) {
            // Amanecer
            potenciadeluces = map(minutosDesdeInicioDia, 0, duracionAmanecer, 0, 255);
            Serial.print("CONTROL LUCES: Amanecer, potencia: "); Serial.println(potenciadeluces);
        }
        else if (minutosDesdeInicioDia > (finDia - initDia - duracionAmanecer)) {
            // Atardecer
            potenciadeluces = map(minutosDesdeInicioDia, finDia - initDia - duracionAmanecer, finDia - initDia, 255, 0);
            Serial.print("CONTROL LUCES: Atardecer, potencia: "); Serial.println(potenciadeluces);
        }
        else {
            // Luz plena durante el día tambien encendemos luces rojas
            potenciadeluces = 255;
            if (ultimoEstadoDia != esDia) { Serial.println("CONTROL LUCES: Es de dia, luz a plena potencia."); }
        }
        analogWrite(LUCES_BLANCAS_PIN, potenciadeluces);
        //revisamos que las luces rojas solo se enciendan despues del amanecer completo y se apaguen antes del atardecer
        if (minutosDesdeInicioDia >= duracionAmanecer && minutosDesdeInicioDia < (finDia - initDia - duracionAmanecer)) {
            digitalWrite(LEDS_ROJOS_PIN, RELAY_ENCENDIDO);
            if (ultimoEstadoDia != esDia) { Serial.println("CONTROL LUCES: Encendiendo luces rojas."); }
        }
        else {
            digitalWrite(LEDS_ROJOS_PIN, RELAY_APAGADO);
            if (ultimoEstadoDia != esDia) { Serial.println("CONTROL LUCES: Apagando luces rojas."); }
        }
        if (ultimoEstadoDia != esDia) { ultimoEstadoDia = esDia; }
        return;
    }

}



#endif // CONTROL_LUCES__H