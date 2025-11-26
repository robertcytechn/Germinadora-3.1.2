#ifndef CONTROL_VENTILACION__H
#define CONTROL_VENTILACION__H


#include <Arduino.h>

#include <VARS_.h>
#include <PINS_.h>

// =================================================================
//  CONTROL DE VENTILACIÓN (Interna y Externa)
// =================================================================

void controlVentilacion(){

    // /ventilacion Externa (ciclo basal/ráfaga)
    unsigned long tiempoActual = millis();
    if (estadoVentExt == 0) { // Estado: Apagado
        if (tiempoActual - inicioCicloExt >= T_EXT_DESCANSO) {
            // Cambiar a estado de ráfaga
            estadoVentExt = 1;
            inicioCicloExt = tiempoActual;
            potenciaVentiladorexterno = PWM_EXT_RAFAGA;
            analogWrite(VENTILADOR_PIN, potenciaVentiladorexterno);
        }
    } 
    else if (estadoVentExt == 1) { // Estado: Ráfaga
        if (tiempoActual - inicioCicloExt >= T_EXT_RAFAGA) {
            // Cambiar a estado basal
            estadoVentExt = 2;
            inicioCicloExt = tiempoActual;
            potenciaVentiladorexterno = PWM_EXT_BASAL;
            analogWrite(VENTILADOR_PIN, potenciaVentiladorexterno);
        }
    } 
    else if (estadoVentExt == 2) { // Estado: Basal
        if (tiempoActual - inicioCicloExt >= T_EXT_BASAL) {
            // Cambiar a estado apagado
            estadoVentExt = 0;
            inicioCicloExt = tiempoActual;
            potenciaVentiladorexterno = PWM_EXT_OFF;
            analogWrite(VENTILADOR_PIN, potenciaVentiladorexterno);
        }
    }

    // ventilacion Interna (ciclo on/off) - si la resistencia esta encendida se activa la ventilación interna sin conteo, si está apagada se sigue el ciclo
    if(estatusResistencia){
        analogWrite(VENTINTER_PIN, 255); // Ventilación interna al máximo
        ventInternoActivo = true;
    } else {
        unsigned long tiempoActual = millis();
        if (ventInternoActivo) {
            // Actualmente encendida, verificar si debe apagarse
            if (tiempoActual - ultimaVentInterno >= T_VENT_INT_ON) {
                analogWrite(VENTINTER_PIN, 0); // Apagar ventilación interna
                ventInternoActivo = false;
                ultimaVentInterno = tiempoActual;
            }
        } else {
            // Actualmente apagada, verificar si debe encenderse
            if (tiempoActual - ultimaVentInterno >= T_VENT_INT_OFF) {
                analogWrite(VENTINTER_PIN, 128); // Encender ventilación interna a potencia media
                ventInternoActivo = true;
                ultimaVentInterno = tiempoActual;
            }
        }
    }


}

#endif // CONTROL_VENTILACION__H