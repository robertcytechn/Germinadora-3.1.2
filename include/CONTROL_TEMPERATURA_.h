#ifndef CONTROL_TEMPERATURA__H
#define CONTROL_TEMPERATURA__H

#include <Arduino.h>
#include <RTClib.h>

#include <VARS_.h>
#include <PINS_.h>

// =================================================================
//  CONTROL DE TEMPERATURA (Calefacción)
// =================================================================

extern RTC_DS1307 reloj;

void controlCalefaccion(){
    // verificamos si ya paso el tiempo de reaccion para seguir con la funcion
    if (TIEMPO_ACTUAL_MS - tiempoUltimoCambioCalefaccion < TIEMPO_REACCION_CALEFACCION) {
        // No ha pasado suficiente tiempo desde el último cambio salimos de la función y no hacemos nada
        // no saturamos i2c
        return;
    }

    // Determinar la temperatura objetivo según si es de día o de noche
    int minutosActuales = RELOJ_GLOBAL.hour() * 60 + RELOJ_GLOBAL.minute();
    bool esDia = (minutosActuales >= initDia && minutosActuales < finDia);
    float tempObjetivo = esDia ? tempDia : tempNoche;

    // =================================================================
    //  PRIORIDAD 1: APAGADO DE EMERGENCIA POR TEMPERATURA MÁXIMA
    // =================================================================
    // Si la temperatura máxima supera el umbral de seguridad, apagamos la calefacción inmediatamente.
    // Esto previene el sobrecalentamiento y reinicia el ciclo de descanso para evitar que el relé se active y desactive rápidamente.
    if (temperaturaMax >= tempMaxSeguridad) {
        if (estatusResistencia) {
            digitalWrite(CALEFACTORA_PIN, RELAY_APAGADO);
            estatusResistencia = false;
            tiempoUltimoCambioCalefaccion = TIEMPO_ACTUAL_MS; // Inicia un nuevo ciclo de descanso
            Serial.println("CONTROL TEMP: ¡EMERGENCIA! Temp maxima superada. Apagando calefaccion.");
        }
        return; // Salimos de la función para asegurar que permanezca apagada.
    }

    // =================================================================
    //  LÓGICA DE CICLOS DE ENCENDIDO Y DESCANSO
    // =================================================================

    if (estatusResistencia) {
        // --- CALEFACCIÓN ENCENDIDA: Comprobar si debe apagarse ---
        // Se apaga si se cumple el tiempo de encendido O si ya se alcanzó la temperatura objetivo.
        if (TIEMPO_ACTUAL_MS - tiempoUltimoCambioCalefaccion >= TIEMPO_ENCENDIDO_CALEFACCION || tempPromedio > (tempObjetivo + tempHisteresis)) {
            digitalWrite(CALEFACTORA_PIN, RELAY_APAGADO);
            estatusResistencia = false;
            tiempoUltimoCambioCalefaccion = TIEMPO_ACTUAL_MS; // reiniciamos el ciclo de descanso para evitar encendido y apagado rápido de relé
            Serial.println("CONTROL TEMP: Apagando calefaccion (ciclo/temp alcanzada).");
        }
    } else {
        // --- CALEFACCIÓN APAGADA: Comprobar si debe encenderse ---
        // Se enciende si la temperatura está por debajo del objetivo Y ha transcurrido el tiempo de descanso.
        if (tempPromedio < (tempObjetivo - tempHisteresis) && TIEMPO_ACTUAL_MS - tiempoUltimoCambioCalefaccion >= TIEMPO_APAGADO_CALEFACCION) {
            digitalWrite(CALEFACTORA_PIN, RELAY_ENCENDIDO);
            estatusResistencia = true;
            tiempoUltimoCambioCalefaccion = TIEMPO_ACTUAL_MS; // reiniciamos el ciclo de encendido para evitar encendido y apagado rápido de relé
            Serial.println("CONTROL TEMP: Encendiendo calefaccion (temp baja y descanso cumplido).");
        }
    }

}

#endif // CONTROL_TEMPERATURA__H