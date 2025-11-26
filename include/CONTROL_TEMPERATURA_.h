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

/**
 * @brief Controla la resistencia de calefacción
 * Objetivos:
 * - Mantener temperatura de día (initDia-finDia) o de noche (finDia-initDia)
 * - Si temperatura supera objetivo + histéresis → apagar resistencia
 * - Respetar tiempo máximo de encendido continuo (MAX_TIEMPO_CALEFACCION)
 * - Respetar tiempo mínimo de descanso después de apagarse (TIEMPO_MINIMO_APAGADO_CALEFACCION)
 */
void controlCalefaccion(){
    static unsigned long ultimoApagado = 0;
    static bool enDescansoForzado = false;
    
    unsigned long tiempoActual = millis();
    
    // Determinar si es día o noche según el reloj
    DateTime ahora = reloj.now();
    int horaActual = ahora.hour();
    bool esDia = (horaActual >= initDia && horaActual < finDia);
    
    // Seleccionar temperatura objetivo según la hora
    float tempObjetivo = esDia ? tempDia : tempNoche;
    
    // *** 1. Verificar tiempo máximo de calefacción continua ***
    if (estatusResistencia) {
        unsigned long tiempoEncendido = tiempoActual - inicioCalefaccion;
        
        // Si excede el tiempo máximo, apagar y forzar descanso
        if (tiempoEncendido >= MAX_TIEMPO_CALEFACCION) {
            digitalWrite(CALEFACTORA_PIN, LOW);
            estatusResistencia = false;
            ultimoApagado = tiempoActual;
            enDescansoForzado = true;
            return;
        }
    }
    
    // *** 2. Verificar tiempo mínimo de descanso ***
    if (enDescansoForzado) {
        unsigned long tiempoDescanso = tiempoActual - ultimoApagado;
        
        // Mantener apagada durante el descanso mínimo
        if (tiempoDescanso < TIEMPO_MINIMO_APAGADO_CALEFACCION) {
            digitalWrite(CALEFACTORA_PIN, LOW);
            estatusResistencia = false;
            return;
        } else {
            enDescansoForzado = false; // Descanso completado
        }
    }
    
    // *** 3. Control por temperatura objetivo ***
    
    // Si temperatura supera objetivo + histéresis → APAGAR
    if (tempPromedio >= (tempObjetivo + tempHisteresis)) {
        if (estatusResistencia) {
            digitalWrite(CALEFACTORA_PIN, LOW);
            estatusResistencia = false;
            ultimoApagado = tiempoActual;
        }
    }
    // Si temperatura está por debajo del objetivo - histéresis → ENCENDER
    else if (tempPromedio < (tempObjetivo - tempHisteresis)) {
        if (!estatusResistencia && !enDescansoForzado) {
            digitalWrite(CALEFACTORA_PIN, HIGH);
            estatusResistencia = true;
            inicioCalefaccion = tiempoActual;
        }
    }
    // Si está dentro del rango de histéresis, mantener estado actual
    // (no hacer nada, evita oscilaciones)
}

#endif // CONTROL_TEMPERATURA__H