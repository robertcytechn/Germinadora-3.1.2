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
    unsigned long tiempoActual = millis();

    // Determinar la temperatura objetivo según si es de día o de noche
    int minutosActuales = reloj.now().hour() * 60 + reloj.now().minute();
    bool esDia = (minutosActuales >= initDia && minutosActuales < finDia);
    float tempObjetivo = esDia ? tempDia : tempNoche;

    // =================================================================
    //  PRIORIDAD 1: APAGADO DE EMERGENCIA POR TEMPERATURA MÁXIMA
    // =================================================================
    // Si la temperatura máxima supera el umbral de seguridad, apagamos la calefacción inmediatamente.
    // Esto previene el sobrecalentamiento y reinicia el ciclo de descanso para evitar que el relé se active y desactive rápidamente.
    if (temperaturaMax >= tempMaxSeguridad) {
        if (estatusResistencia) {
            digitalWrite(CALEFACTORA_PIN, LOW);
            estatusResistencia = false;
            tiempoUltimoCambioCalefaccion = tiempoActual; // Inicia un nuevo ciclo de descanso
        }
        return; // Salimos de la función para asegurar que permanezca apagada.
    }

    // =================================================================
    //  LÓGICA DE CICLOS DE ENCENDIDO Y DESCANSO
    // =================================================================

    if (estatusResistencia) {
        // --- CALEFACCIÓN ENCENDIDA: Comprobar si debe apagarse ---
        // Se apaga si se cumple el tiempo de encendido O si ya se alcanzó la temperatura objetivo.
        if (tiempoActual - tiempoUltimoCambioCalefaccion >= TIEMPO_ENCENDIDO_CALEFACCION || tempPromedio > (tempObjetivo + tempHisteresis)) {
            digitalWrite(CALEFACTORA_PIN, LOW);
            estatusResistencia = false;
            tiempoUltimoCambioCalefaccion = tiempoActual; // reiniciamos el ciclo de descanso para evitar encendido y apagado rápido de relé
        }
    } else {
        // --- CALEFACCIÓN APAGADA: Comprobar si debe encenderse ---
        // Se enciende si la temperatura está por debajo del objetivo Y ha transcurrido el tiempo de descanso.
        if (tempPromedio < (tempObjetivo - tempHisteresis) && tiempoActual - tiempoUltimoCambioCalefaccion >= TIEMPO_APAGADO_CALEFACCION) {
            digitalWrite(CALEFACTORA_PIN, HIGH);
            estatusResistencia = true;
            tiempoUltimoCambioCalefaccion = tiempoActual; // reiniciamos el ciclo de encendido para evitar encendido y apagado rápido de relé
        }
    }

}

#endif // CONTROL_TEMPERATURA__H