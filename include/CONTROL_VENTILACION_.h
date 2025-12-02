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


void controlVentilacion(){
    // Si el modo anti-hongos está activo, forzamos ventilación externa al 80% y apagamos la interna
    if (modoAntiHongos) {
        analogWrite(VENTILADOR_PIN, 204);  // 80% de 255 = 204
        analogWrite(VENTINTER_PIN, PWM_INT_VENT_MAX);
        static bool mensajeMostrado = false;
        if (!mensajeMostrado) {
            Serial.println("CONTROL VENT: Modo ANTI-HONGOS - Ventilacion externa al 80%, interna desactivada.");
            mensajeMostrado = true;
        }
        return;
    }

    // verificamos si ya paso el tiempo de reaccion para seguir con la funcion
    if (TIEMPO_ACTUAL_MS - ultimoCambioVentilacion < TIEMPO_REACCION_VENTILACION) {
        // No ha pasado suficiente tiempo desde el último cambio salimos de la función y no hacemos nada
        // no saturamos i2c
        return;
    }
    ultimoCambioVentilacion = TIEMPO_ACTUAL_MS;

    // verificamos la temperatura maxima no supere el umbral de seguridad
    // si es asi, ventilacion externa en modo rafaga y ventilacion interna al maximo
    // mantenemos hasta que temperatura promedio baje a obejtivo menos histeresis dependiendo si es de dia o de noche
    if (temperaturaMax >= tempMaxSeguridad) {
        analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
        analogWrite(VENTINTER_PIN, PWM_INT_VENT_MAX);
        Serial.println("CONTROL VENT: ¡EMERGENCIA! Temp maxima superada. Ventilacion forzada.");
        return;     // salimos de la función para mantener este estado
    }

    // si la resistencia esta encendida, ventilacion interna a maximo para ayudar a distribuir el calor
    if (estatusResistencia) {
        analogWrite(VENTINTER_PIN, PWM_INT_VENT_MAX);
        Serial.println("CONTROL VENT: Calefaccion encendida, activando ventilacion interna para distribuir calor.");
    } else {
        // control de ventilacion interna segun ciclos de encendido y apagado
        if (estadoVentInt) {
            // ventilacion interna encendida: comprobar si debe apagarse
            if (TIEMPO_ACTUAL_MS - ultimaVentInterno >= T_VENT_INT_ON) {
                analogWrite(VENTINTER_PIN, PWM_INT_VENT_OFF);
                estadoVentInt = false;
                ultimaVentInterno = TIEMPO_ACTUAL_MS;
                Serial.println("CONTROL VENT: Fin ciclo ON ventilacion interna.");
            }
        } else {
            // ventilacion interna apagada: comprobar si debe encenderse
            if (TIEMPO_ACTUAL_MS - ultimaVentInterno >= T_VENT_INT_OFF) {
                analogWrite(VENTINTER_PIN, PWM_INT_VENT_MED);
                estadoVentInt = true;
                ultimaVentInterno = TIEMPO_ACTUAL_MS;
                Serial.println("CONTROL VENT: Inicio ciclo ON ventilacion interna.");
            }
        }
    }

    // control de ventilacion externa segun ciclos: apagado -> basal -> rafaga -> descanso, 0 apagado 1 basal 2 rafaga
    // revisamos que la humedad promedio no baje del umbral minimo de seguridad antes de activar ventilacion externa
    if (humPromedio <= humTriggerNormal) {
        analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
        estadoVentExt = 0; // reiniciamos el estado a apagado
        inicioCicloExt = TIEMPO_ACTUAL_MS; // reiniciamos el ciclo
        Serial.println("CONTROL VENT: Humedad minima de seguridad. Apagando ventilacion externa.");
        return; // salimos de la función para mantener ventilación externa apagada
    }
    else{
        // continuamos con el ciclo de ventilación externa normal, si la humedad es segura
        switch (estadoVentExt) {
        case 0: // apagado -> basal
            analogWrite(VENTILADOR_PIN, PWM_EXT_BASAL);
            estadoVentExt = 1;
            inicioCicloExt = TIEMPO_ACTUAL_MS;
            Serial.println("CONTROL VENT: Iniciando ciclo ventilacion externa -> BASAL.");
            break;
        case 1: // basal -> rafaga
            if (TIEMPO_ACTUAL_MS - inicioCicloExt >= T_EXT_BASAL) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
                estadoVentExt = 2;
                inicioCicloExt = TIEMPO_ACTUAL_MS;
                Serial.println("CONTROL VENT: Pasando a -> RAFAGA.");
            }
            break;
        case 2: // rafaga -> descanso
            if (TIEMPO_ACTUAL_MS - inicioCicloExt >= T_EXT_RAFAGA) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
                estadoVentExt = 0;
                inicioCicloExt = TIEMPO_ACTUAL_MS;
                Serial.println("CONTROL VENT: Pasando a -> DESCANSO.");
            }
            break;
        default:
            break;
        }
    }
    
}

#endif // CONTROL_VENTILACION__H