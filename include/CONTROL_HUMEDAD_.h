#ifndef CONTROL_HUMEDAD__H
#define CONTROL_HUMEDAD__H

#include <Arduino.h>
#include <VARS_.h>
#include <PINS_.h>

// Variable local de estado lógico
static bool estatusHumidificador = false; 

// Flag para saber si ya superamos el arranque
static bool sistemaEstabilizado = false; 

void controlHumedad() {
    unsigned long ahora = TIEMPO_ACTUAL_MS; 

    // ============================================================
    // 🛡️ PROTECCIÓN DE ARRANQUE
    // ============================================================
    if (!sistemaEstabilizado) {
        // 1. Esperar al menos 10 segundos desde el encendido para que el DHT lea bien
        if (ahora < 10000UL) { 
            return; // No hacemos nada, esperamos.
        }
        
        // 2. Verificar que la humedad no sea 0 (Lectura inválida o sensor desconectado)
        if (humPromedio == 0.0 || isnan(humPromedio)) {
            return; // No confiamos en sensores vacíos.
        }

        // Si pasamos los filtros, marcamos el sistema como listo
        sistemaEstabilizado = true;
        Serial.println("CONTROL HUM: Sistema estabilizado. Iniciando logica automatica.");
    }
    // ============================================================


    // MÁQUINA DE ESTADOS FINITOS
    switch (etapaHumidificador) {
        
        // ============================================================
        // ESTADO 0: MONITOREO
        // ============================================================
        case ESTADO_HUM_MONITOREO:
            
            // Lógica normal de encendido (< 40%)
            if (humPromedio <= humTriggerNormal) {
                Serial.print("CONTROL HUM: Humedad baja ("); Serial.print(humPromedio); 
                Serial.println("%). Iniciando ciclo de 2 min.");
                
                digitalWrite(HUMIDIFICADOR_PIN, RELAY_ENCENDIDO); 
                timerHumidificador = ahora;
                etapaHumidificador = ESTADO_HUM_PULSANDO_ON;
            }
            // Lógica de seguridad (> 90%) - CON RESTRICCIÓN DE TIEMPO
            // Solo activamos esta seguridad si el sistema lleva ya rato encendido (> 60 seg)
            // para evitar que al arrancar en un día húmedo lo encendamos por error.
            else if (humPromedio >= humMaxSeguridad && ahora > 60000UL) {
                    Serial.println("CONTROL HUM: ALERTA >90% (Desfase detectado). Forzando apagado.");
                    digitalWrite(HUMIDIFICADOR_PIN, RELAY_ENCENDIDO); 
                    timerHumidificador = ahora;
                    etapaHumidificador = ESTADO_HUM_PULSANDO_OFF; 
            }
            break;

        // ============================================================
        // ESTADO 1: PULSANDO PARA ENCENDER
        // ============================================================
        case ESTADO_HUM_PULSANDO_ON:
            if (ahora - timerHumidificador >= DURACION_PULSO_BOTON) {
                digitalWrite(HUMIDIFICADOR_PIN, RELAY_APAGADO); 
                estatusHumidificador = true;
                
                Serial.println("CONTROL HUM: Encendido. Trabajando por 2 min obligatorios.");
                timerHumidificador = ahora; 
                etapaHumidificador = ESTADO_HUM_TRABAJANDO;
            }
            break;

        // ============================================================
        // ESTADO 2: TRABAJANDO (2 Minutos Obligatorios)
        // ============================================================
        case ESTADO_HUM_TRABAJANDO:
            if (ahora - timerHumidificador >= TIEMPO_MIN_ENCENDIDO_HUM) {
                Serial.println("CONTROL HUM: 2 minutos completados. Iniciando apagado.");
                
                digitalWrite(HUMIDIFICADOR_PIN, RELAY_ENCENDIDO); 
                timerHumidificador = ahora;
                etapaHumidificador = ESTADO_HUM_PULSANDO_OFF;
            }
            break;

        // ============================================================
        // ESTADO 3: PULSANDO PARA APAGAR
        // ============================================================
        case ESTADO_HUM_PULSANDO_OFF:
            if (ahora - timerHumidificador >= DURACION_PULSO_BOTON) {
                digitalWrite(HUMIDIFICADOR_PIN, RELAY_APAGADO); 
                estatusHumidificador = false;
                
                Serial.println("CONTROL HUM: Apagado. Entrando en descanso de 10 min.");
                timerHumidificador = ahora; 
                etapaHumidificador = ESTADO_HUM_DESCANSO;
            }
            break;

        // ============================================================
        // ESTADO 4: DESCANSO (10 Minutos)
        // ============================================================
        case ESTADO_HUM_DESCANSO:
            // Emergencia (< 30%) - Rompe el descanso
            if (humPromedio <= humTriggerEmergencia && humPromedio > 0) { // Validamos >0
                Serial.print("CONTROL HUM: ¡EMERGENCIA! Humedad critica ("); 
                Serial.print(humPromedio);
                Serial.println("%). Cancelando descanso y encendiendo.");
                
                digitalWrite(HUMIDIFICADOR_PIN, RELAY_ENCENDIDO); 
                timerHumidificador = ahora;
                etapaHumidificador = ESTADO_HUM_PULSANDO_ON;
                return;
            }

            // Esperar fin del descanso
            if (ahora - timerHumidificador >= TIEMPO_MIN_DESCANSO_HUM) {
                Serial.println("CONTROL HUM: Descanso terminado. Volviendo a vigilancia.");
                etapaHumidificador = ESTADO_HUM_MONITOREO;
            }
            break;
    }
}

#endif // CONTROL_HUMEDAD__H