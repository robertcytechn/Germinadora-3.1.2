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
    unsigned long tiempoActual = millis();
    // verificamos si ya paso el tiempo de reaccion para seguir con la funcion
    if (tiempoActual - ultimoCambioVentilacion < tiempoReaccionVentilacion) {
        // No ha pasado suficiente tiempo desde el último cambio salimos de la función y no hacemos nada
        // no saturamos i2c
        return;
    }
    ultimoCambioVentilacion = tiempoActual;

    // Determinar la temperatura objetivo según si es de día o de noche
    int minutosActuales = reloj.now().hour() * 60 + reloj.now().minute();
    bool esDia = (minutosActuales >= initDia && minutosActuales < finDia);
    float tempObjetivo = esDia ? tempDia : tempNoche; 

    // verificamos la temperatura maxima no supere el umbral de seguridad
    // si es asi, ventilacion externa en modo rafaga y ventilacion interna al maximo
    // mantenemos hasta que temperatura promedio baje a obejtivo menos histeresis dependiendo si es de dia o de noche
    if (temperaturaMax >= tempMaxSeguridad) {
        analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
        analogWrite(VENTINTER_PIN, PWM_INT_VENT_MAX);
        return;     // salimos de la función para mantener este estado
    }

    // si la resistencia esta encendida, ventilacion interna a maximo para ayudar a distribuir el calor
    if (estatusResistencia) {
        analogWrite(VENTINTER_PIN, PWM_INT_VENT_MAX);
    } else {
        // control de ventilacion interna segun ciclos de encendido y apagado
        if (estadoVentInt) {
            // ventilacion interna encendida: comprobar si debe apagarse
            if (tiempoActual - ultimaVentInterno >= T_VENT_INT_ON) {
                analogWrite(VENTINTER_PIN, PWM_INT_VENT_OFF);
                estadoVentInt = false;
                ultimaVentInterno = tiempoActual;
            }
        } else {
            // ventilacion interna apagada: comprobar si debe encenderse
            if (tiempoActual - ultimaVentInterno >= T_VENT_INT_OFF) {
                analogWrite(VENTINTER_PIN, PWM_INT_VENT_MED);
                estadoVentInt = true;
                ultimaVentInterno = tiempoActual;
            }
        }
    }

    // control de ventilacion externa segun ciclos: apagado -> basal -> rafaga -> descanso, 0 apagado 1 basal 2 rafaga
    // revisamos que la humedad promedio no baje del umbral minimo de seguridad antes de activar ventilacion externa
    if (humPromedio <= humMinSeguridad) {
        analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
        estadoVentExt = 0; // reiniciamos el estado a apagado
        inicioCicloExt = tiempoActual; // reiniciamos el ciclo
        return; // salimos de la función para mantener ventilación externa apagada
    }
    else{
        // continuamos con el ciclo de ventilación externa normal, si la humedad es segura
        switch (estadoVentExt) {
        case 0: // apagado -> basal
            analogWrite(VENTILADOR_PIN, PWM_EXT_BASAL);
            estadoVentExt = 1;
            inicioCicloExt = tiempoActual;
            break;
        case 1: // basal -> rafaga
            if (tiempoActual - inicioCicloExt >= T_EXT_BASAL) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_RAFAGA);
                estadoVentExt = 2;
                inicioCicloExt = tiempoActual;
            }
            break;
        case 2: // rafaga -> descanso
            if (tiempoActual - inicioCicloExt >= T_EXT_RAFAGA) {
                analogWrite(VENTILADOR_PIN, PWM_EXT_OFF);
                estadoVentExt = 0;
                inicioCicloExt = tiempoActual;
            }
            break;
        default:
            break;
        }
    }
    
}

#endif // CONTROL_VENTILACION__H