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
    static unsigned long ultimoCambioEstado = 0;
    
    unsigned long tiempoActual = millis();
    
    // PROTECCIÓN: Evitar cambios de estado demasiado rápidos (debouncing)
    const unsigned long TIEMPO_MIN_ENTRE_CAMBIOS = 2000; // 2 segundos mínimo entre cambios
    
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
            if (estatusResistencia && (tiempoActual - ultimoCambioEstado >= TIEMPO_MIN_ENTRE_CAMBIOS)) {
                digitalWrite(CALEFACTORA_PIN, LOW);
                estatusResistencia = false;
                ultimoApagado = tiempoActual;
                enDescansoForzado = true;
                ultimoCambioEstado = tiempoActual;
            }
        }
    }
    
    // *** 2. Verificar tiempo mínimo de descanso ***
    if (enDescansoForzado) {
        unsigned long tiempoDescanso = tiempoActual - ultimoApagado;
        
        // Mantener apagada durante el descanso mínimo
        if (tiempoDescanso < TIEMPO_MINIMO_APAGADO_CALEFACCION) {
            if (estatusResistencia) { // Si por alguna razón estuviera encendida, apagarla.
                digitalWrite(CALEFACTORA_PIN, LOW);
                estatusResistencia = false;
                ultimoCambioEstado = tiempoActual; // Registrar el cambio
            }
            return;
        } else {
            enDescansoForzado = false; // Descanso completado
        }
    }
    
    // *** 3. Control por temperatura objetivo ***
    bool nuevoEstado = estatusResistencia;
    
    // Si temperatura supera objetivo + histéresis → APAGAR
    if (tempPromedio >= (tempObjetivo + tempHisteresis)) {
        nuevoEstado = false;
    }
    // Si temperatura está por debajo del objetivo - histéresis → ENCENDER
    else if (tempPromedio < (tempObjetivo - tempHisteresis)) {
        if (!enDescansoForzado && !estatusResistencia) { // Solo intentar encender si está apagada
            nuevoEstado = true;
        }
    }
    // Si está dentro del rango de histéresis, mantener estado actual
    // (no cambiar nuevoEstado)
    
    // *** 4. Aplicar cambio de estado SOLO si hay cambio real y ha pasado tiempo suficiente ***
    if (nuevoEstado != estatusResistencia && (tiempoActual - ultimoCambioEstado >= TIEMPO_MIN_ENTRE_CAMBIOS)) {
        estatusResistencia = nuevoEstado;
        ultimoCambioEstado = tiempoActual;

        if (estatusResistencia) {
            // ENCENDER
            digitalWrite(CALEFACTORA_PIN, HIGH);
            inicioCalefaccion = tiempoActual;
        } else {
            // APAGAR
            digitalWrite(CALEFACTORA_PIN, LOW);
            ultimoApagado = tiempoActual;
        }
    }
}

#endif // CONTROL_TEMPERATURA__H