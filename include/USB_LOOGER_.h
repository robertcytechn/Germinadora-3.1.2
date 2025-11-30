#ifndef USB_LOGGER__H
#define USB_LOGGER__H

#include <Arduino.h>
#include <Ch376msc.h>
#include <RTClib.h>
#include "VARS_.h"
#include "PINS_.h"

// =================================================================
//  CONFIGURACIÓN DEL LOGGER USB
// =================================================================

// Configuración para usar el puerto Serie 1 (Hardware Serial) a 9600 baudios.
// Conectar el CH376 a los pines TX1 y RX1 del Arduino Mega.
Ch376msc usb(Serial1, 9600);

// Intervalo de logging: 15 minutos en milisegundos
//const unsigned long LOG_INTERVAL = 15 * 60 * 1000UL;
const unsigned long LOG_INTERVAL = 10 * 1000UL; //para pruebas rapidas de 10 segundos
unsigned long lastLogTime = 0;

// Objeto RTC externo (declarado en main.cpp)
extern RTC_DS1307 reloj;

// =================================================================
//  FUNCIONES DEL LOGGER
// =================================================================

void diagnoseCH376() {
    Serial.println("Diagnóstico del módulo CH376...");
    if (usb.pingDevice() == ANSW_USB_INT_SUCCESS) {
        Serial.println("CH376: Ping exitoso. El módulo responde.");
        uint8_t ver = usb.getChipVer();
        Serial.print("CH376: Versión del chip: ");
        Serial.println(ver);
        Serial.println("CH376: ¡El módulo parece estar conectado correctamente por Serial1!");
    } else {
        Serial.println("************************************************************");
        Serial.println("CH376: ERROR CRÍTICO: No se pudo comunicar con el módulo CH376.");
        Serial.println("  - Verifique el cableado en los pines TX1 y RX1 del Arduino Mega.");
        Serial.println("  - Asegúrese de que la velocidad en baudios (9600) es la correcta.");
        Serial.println("************************************************************");
    }
}

/**
 * @brief Inicializa el módulo CH376 para el logging en USB.
 *        Debe ser llamado en la función setup() principal.
 */
void setupLogger() {
    Serial.println("Inicializando logger USB via Serial1...");
    Serial1.begin(9600);
    delay(100);
    usb.init();
    diagnoseCH376(); // Llamada a la función de diagnóstico
}

/**
 * @brief Escribe los datos actuales de sensores y actuadores en un archivo CSV en la USB.
 */
void logData() {
    if (!usb.driveReady()) {
        Serial.println("ERROR Logger: No se detecta unidad USB.");
        return;
    }

    Serial.println("Logger: Guardando todos los datos del sistema en formato CSV en USB...");

    // Establecer el nombre del archivo de log
    usb.setFileName("LOG.CSV");

    // Abrir el archivo. La librería lo crea si no existe.
    if (usb.openFile() == ANSW_USB_INT_SUCCESS) {
        
        // Si el archivo está vacío, escribimos la cabecera del CSV.
        if (usb.getFileSize() == 0) {
            char header[] = "Timestamp,Temp_Sup,Temp_Inf,Temp_Pue,Hum_Sup,Hum_Inf,Hum_Pue,Temp_Prom,Hum_Prom,Temp_Max,Hum_Max,Pot_Luces,Calefaccion,Vent_Int,Vent_Ext_PWM,Alarma,Cod_Alarma,Temp_Obj_Dia,Temp_Obj_Noche,Hum_Obj,Hum_Histeresis,Hum_Max_Seg,Hum_Min_Seg,Init_Dia,Fin_Dia,Dur_Amanecer,Luz_Tiempo_Reac,Luz_Ultimo_Cambio,Temp_Histeresis,Temp_Max_Seg,Temp_Min_Seg,Calef_Ultimo_Cambio,Calef_Tiempo_Reac,Vent_Int_Ultima,Vent_Ext_Inicio_Ciclo,Vent_Tiempo_Reac,Vent_Ultimo_Cambio\r\n";
            if(!usb.writeFile(header, strlen(header))) {
                Serial.println("ERROR Logger: No se pudo escribir la cabecera del CSV.");
                usb.closeFile();
                return;
            }
        }

        // Mover el cursor al final del archivo para añadir datos (append)
        usb.moveCursor(usb.getFileSize());

        // Obtener la fecha y hora del RTC
        DateTime now = reloj.now();
        char timestamp[20];
        sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

        // Preparar el buffer para los datos
        char logBuffer[512];
        
        // Formatear la cadena de log en formato CSV
        snprintf(logBuffer, sizeof(logBuffer),
                    "%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%lu,%lu,%.2f,%.2f,%.2f,%lu,%lu,%lu,%lu,%lu,%lu\r\n",
                    timestamp,
                    (double)temperaturas[0], (double)temperaturas[1], (double)temperaturas[2],
                    (double)humedades[0], (double)humedades[1], (double)humedades[2],
                    (double)tempPromedio, (double)humPromedio, (double)temperaturaMax, (double)humedadMax,
                    potenciadeluces,
                    estatusResistencia ? 1 : 0,
                    estadoVentInt ? 1 : 0,
                    (estadoVentExt == 1) ? PWM_EXT_BASAL : ((estadoVentExt == 2) ? PWM_EXT_RAFAGA : 0),
                    alarmaActiva ? 1 : 0,
                    codigoAlarma,
                    (double)tempDia, (double)tempNoche, (double)humObjetivo, (double)humHisteresis, (double)humMaxSeguridad, (double)humMinSeguridad,
                    initDia, finDia, duracionAmanecer,
                    tiempoReaccion, ultimoCambio,
                    (double)tempHisteresis, (double)tempMaxSeguridad, (double)tempMinSeguridad,
                    tiempoUltimoCambioCalefaccion, tiempoReaccionCalefaccion,
                    ultimaVentInterno, inicioCicloExt, tiempoReaccionVentilacion, ultimoCambioVentilacion
                    );

        // Escribir los datos en el archivo
        if (usb.writeFile(logBuffer, strlen(logBuffer))) {
            Serial.println("Logger: Datos CSV guardados correctamente.");
        } else {
            Serial.println("ERROR Logger: No se pudo escribir en el archivo CSV.");
        }

        // Cerrar el archivo
        usb.closeFile();
    } else {
        Serial.println("ERROR Logger: No se pudo abrir o crear el archivo CSV.");
    }
}

/**
 * @brief Gestiona el temporizador para el logging.
 *        Debe ser llamado en la función loop() principal.
 */
void handleLogging() {
    if (millis() - lastLogTime >= LOG_INTERVAL) {
        logData();
        lastLogTime = millis();
    }
     // revisamos si hay algun mensaje del ch376
    if (usb.checkIntMessage())
    {
        Serial.println("Logger: Mensaje de interrupcion del CH376.");
    }
}

#endif // USB_LOGGER__H
