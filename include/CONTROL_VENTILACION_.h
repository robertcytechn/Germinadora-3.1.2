#ifndef CONTROL_VENTILACION__H
#define CONTROL_VENTILACION__H


#include <Arduino.h>
#include <RTClib.h>
#include <VARS_.h>
#include <PINS_.h>

// =================================================================
//  CONTROL DE VENTILACIÓN (Interna y Externa)
// =================================================================

extern RTC_DS1307 reloj;


/**
 * @brief Controla el ventilador de entrada de aire a la cámara
 * Prioridades:
 * 1. Si temperatura > tempMaxSeguridad → Ventilador al 100%
 * 2. Si humedad < humMinSeguridad → Apagar ventilación externa
 * 3. Si todo normal → Ciclo normal (basal/ráfaga)
 */
void controlVentilacion(){
    unsigned long tiempoActual = millis();

    static bool ventExternoForzado = false;
    static bool ventExternoApagadoPorHumedad = false;
    
    // *** PRIORITARIO: Control por temperatura máxima de seguridad ***
    if (temperaturaMax > tempMaxSeguridad) {
        // Temperatura crítica: activar ventilador al 100%
        if (potenciaVentiladorexterno != PWM_EXT_RAFAGA) {
            analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
            potenciaVentiladorexterno = PWM_EXT_RAFAGA;
        }
        ventExternoForzado = true;
        ventExternoApagadoPorHumedad = false;
        return;
    } 
    // Aplicar histéresis para temperatura
    else if (ventExternoForzado && temperaturaMax > (tempMaxSeguridad - tempHisteresis)) {
        // Mantener al 100% hasta que baje por debajo del umbral con histéresis
        if (potenciaVentiladorexterno != PWM_EXT_RAFAGA) {
            analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
            potenciaVentiladorexterno = PWM_EXT_RAFAGA;
        }
        return;
    } 
    else {
        ventExternoForzado = false;
    }
    
    // *** Control por humedad mínima de seguridad ***
    if (humPromedio < humMinSeguridad) {
        // Humedad muy baja: apagar ventilación externa
        if (potenciaVentiladorexterno != PWM_EXT_OFF) {
            analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
            potenciaVentiladorexterno = PWM_EXT_OFF;
        }
        ventExternoApagadoPorHumedad = true;
        estadoVentExt = 0; // Reiniciar estado del ciclo
        inicioCicloExt = tiempoActual;
        return;
    } 
    // Aplicar histéresis para humedad
    else if (ventExternoApagadoPorHumedad && humPromedio < (humMinSeguridad + humHisteresis)) {
        // Mantener apagado hasta que suba por encima del umbral con histéresis
        if (potenciaVentiladorexterno != PWM_EXT_OFF) {
            analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
            potenciaVentiladorexterno = PWM_EXT_OFF;
        }
        return;
    } 
    else {
        ventExternoApagadoPorHumedad = false;
    }
    
    // *** Ventilación normal: Ciclo basal/ráfaga/descanso ***
    unsigned long tiempoTranscurrido = tiempoActual - inicioCicloExt;
    // solo si es de dia
    DateTime ahora = reloj.now();
    int horaActual = ahora.hour();
    bool esDia = (horaActual >= initDia && horaActual < finDia);
    if (!esDia) return;
    
    switch (estadoVentExt) {
        case 0: // Estado inicial / Ventilación basal
            if (potenciaVentiladorexterno != PWM_EXT_BASAL) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_BASAL);
                potenciaVentiladorexterno = PWM_EXT_BASAL;
            }
            if (tiempoTranscurrido >= T_EXT_BASAL) {
                estadoVentExt = 1; // Pasar a ráfaga
                inicioCicloExt = tiempoActual;
            }
            break;
            
        case 1: // Ráfaga de ventilación
            if (potenciaVentiladorexterno != PWM_EXT_RAFAGA) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
                potenciaVentiladorexterno = PWM_EXT_RAFAGA;
            }
            if (tiempoTranscurrido >= T_EXT_RAFAGA) {
                estadoVentExt = 2; // Pasar a descanso
                inicioCicloExt = tiempoActual;
            }
            break;
            
        case 2: // Descanso (ventilador apagado)
            if (potenciaVentiladorexterno != PWM_EXT_OFF) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
                potenciaVentiladorexterno = PWM_EXT_OFF;
            }
            if (tiempoTranscurrido >= T_EXT_DESCANSO) {
                estadoVentExt = 0; // Volver a ventilación basal
                inicioCicloExt = tiempoActual;
            }
            break;
            
        default:
            estadoVentExt = 0;
            inicioCicloExt = tiempoActual;
            break;
    }
    
    // *** VENTILACIÓN INTERNA ***
    // Si la resistencia está encendida se activa la ventilación interna sin conteo
    // Si está apagada se sigue el ciclo on/off
    const unsigned long TIEMPO_MIN_ENTRE_CAMBIOS_VENT_INT = 2000; // 2 segundos
    static int ultimaPotenciaVentInterno = -1;
    static unsigned long ultimoCambioVentInterno = 0;
    
    if (estatusResistencia) {
        if (ultimaPotenciaVentInterno != PWM_INT_VENT_MAX && (tiempoActual - ultimoCambioVentInterno > TIEMPO_MIN_ENTRE_CAMBIOS_VENT_INT)) {
            analogWrite(VENTINTER_PIN, PWM_INT_VENT_MAX); // Ventilación interna al máximo
            ultimaPotenciaVentInterno = PWM_INT_VENT_MAX;
            ultimoCambioVentInterno = tiempoActual;
        }
        ventInternoActivo = true;
    } else {
        if (ventInternoActivo) {
            // Actualmente encendida, verificar si debe apagarse
            if (tiempoActual - ultimaVentInterno >= T_VENT_INT_ON) {
                if (ultimaPotenciaVentInterno != PWM_INT_VENT_MIN && (tiempoActual - ultimoCambioVentInterno > TIEMPO_MIN_ENTRE_CAMBIOS_VENT_INT)) {
                    analogWrite(VENTINTER_PIN, PWM_INT_VENT_MIN); // Apagar ventilación interna
                    ultimaPotenciaVentInterno = PWM_INT_VENT_MIN;
                    ultimoCambioVentInterno = tiempoActual;
                    ultimaVentInterno = tiempoActual; // Reiniciar temporizador de ciclo
                    ventInternoActivo = false;
                }
            }
        } else {
            // Actualmente apagada, verificar si debe encenderse
            if (tiempoActual - ultimaVentInterno >= T_VENT_INT_OFF) {
                if (ultimaPotenciaVentInterno != PWM_INT_VENT_MED && (tiempoActual - ultimoCambioVentInterno > TIEMPO_MIN_ENTRE_CAMBIOS_VENT_INT)) {
                    analogWrite(VENTINTER_PIN, PWM_INT_VENT_MED); // Encender ventilación interna a potencia media
                    ultimaPotenciaVentInterno = PWM_INT_VENT_MED;
                    ultimoCambioVentInterno = tiempoActual;
                    ultimaVentInterno = tiempoActual; // Reiniciar temporizador de ciclo
                    ventInternoActivo = true;
                }
            }
        }
    }
}

#endif // CONTROL_VENTILACION__H