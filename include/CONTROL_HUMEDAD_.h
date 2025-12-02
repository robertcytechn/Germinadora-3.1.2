#ifndef CONTROL_HUMEDAD__H
#define CONTROL_HUMEDAD__H

#include <Arduino.h>
#include <RTClib.h>
#include <VARS_.h>
#include <PINS_.h>

// =================================================================
//  FUNCIÓN DE PULSO (El dedo fantasma)
// =================================================================
void pulsarBotonHumidificador() {
    // Ajusta RELAY_ENCENDIDO / APAGADO según tu configuración en VARS_.h
    // Si usas lógica inversa (módulo azul): LOW activa, HIGH apaga
    digitalWrite(HUMIDIFICADOR_PIN, RELAY_ENCENDIDO); 
    delay(DURACION_PULSO_BOTON);
    digitalWrite(HUMIDIFICADOR_PIN, RELAY_APAGADO);
}

// =================================================================
//  LÓGICA PRINCIPAL
// =================================================================
void controlHumedad() {
    // ============================================================
    // 1. ZONA DE SEGURIDAD Y RESINCRONIZACIÓN (Tu idea genial)
    // ============================================================
    // Si la humedad es EXTREMADAMENTE ALTA (>93%), asumimos que el humidificador 
    // está encendido aunque el Arduino piense que está apagado (por un reinicio o fallo).
    // Mandamos un pulso para intentar apagarlo.
    
    if (humPromedio >= HUM_UMBRAL_DESINCRONIZACION) {
        // Solo actuamos si ha pasado un tiempo prudente desde el último intento de emergencia
        // (para dar tiempo a que la humedad baje y no estar prendiendo/apagando a lo loco)
        if (TIEMPO_ACTUAL_MS - ultimoIntentoResync >= TIEMPO_ESPERA_RESYNC) {
            Serial.print("CONTROL HUM: ALERTA DE DESFASE. Humedad al ");
            Serial.print(humPromedio);
            Serial.println("%. Forzando APAGADO de humidificador.");
            
            pulsarBotonHumidificador(); // ¡Click!
            
            // Forzamos la variable a FALSE, porque acabamos de intentar apagarlo
            estatusHumidificador = false; 
            
            // Actualizamos timers para que no vuelva a entrar aquí inmediatamente
            ultimoIntentoResync = TIEMPO_ACTUAL_MS;
            tiempoUltimoCambioHumedad = TIEMPO_ACTUAL_MS; 
        }
        return; // Salimos de la función. La seguridad es prioridad.
    }

    // ============================================================
    // 2. PROTECCIONES NORMALES
    // ============================================================
    
    // Anti-Jitter (Tiempo de reacción normal)
    if (TIEMPO_ACTUAL_MS - tiempoUltimoCambioHumedad < TIEMPO_ENCENDIDO_HUMIDIFICADOR) {
        return; 
    }

    // Protección Viento en Contra (Si hay ráfaga, no humidificar)
    if (estadoVentExt == 2) {
        if (estatusHumidificador) {
            Serial.println("CONTROL HUM: Viento fuerte (Rafaga). Apagando por eficiencia.");
            pulsarBotonHumidificador(); 
            estatusHumidificador = false;
            tiempoUltimoCambioHumedad = TIEMPO_ACTUAL_MS;
        }
        return;
    }

    // ============================================================
    // 3. CONTROL NORMAL (Histéresis)
    // ============================================================
    
    // ENCENDER (Humedad Baja)
    if (humPromedio <= (humObjetivo - humHisteresis)) {
        if (!estatusHumidificador) { // Solo si creemos que está apagado
            Serial.print("CONTROL HUM: Humedad baja (");
            Serial.print(humPromedio);
            Serial.println("%). Encendiendo.");
            
            pulsarBotonHumidificador();
            estatusHumidificador = true;
            tiempoUltimoCambioHumedad = TIEMPO_ACTUAL_MS;
        }
    }
    // APAGAR (Objetivo Alcanzado)
    else if (humPromedio >= humObjetivo) {
        if (estatusHumidificador) { // Solo si creemos que está encendido
            Serial.print("CONTROL HUM: Objetivo alcanzado (");
            Serial.print(humPromedio);
            Serial.println("%). Apagando.");
            
            pulsarBotonHumidificador();
            estatusHumidificador = false;
            tiempoUltimoCambioHumedad = TIEMPO_ACTUAL_MS;
        }
    }
}

#endif // CONTROL_HUMEDAD__H