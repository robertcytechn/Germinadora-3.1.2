#ifndef MOSTRAR_PANTALLA__H
#define MOSTRAR_PANTALLA__H

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>

extern Adafruit_SSD1306 display;
extern RTC_DS1307 reloj;

extern float tempPromedio;
extern float humPromedio;
extern float temperaturaMax;
extern float humedadMax;
extern float temperaturas[3];
extern float humedades[3];

extern bool lucesRojasOn;
extern int potenciadeluces;
extern bool estatusResistencia;
extern int potenciaVentiladorexterno;

extern bool alarmaActiva;
extern int codigoAlarma;

// =================================================================
//  FUNCIÓN PARA MOSTRAR INFORMACIÓN EN LA PANTALLA OLED
// =================================================================
void mostrarPantalla() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    // Obtener hora actual del RTC
    DateTime ahora = reloj.now();
    
    // ===== LÍNEA 1: HORA Y FECHA =====
    display.setTextSize(1);
    display.setCursor(0, 0);
    
    // Formato: HH:MM:SS DD/MM
    if (ahora.hour() < 10) display.print("0");
    display.print(ahora.hour());
    display.print(":");
    if (ahora.minute() < 10) display.print("0");
    display.print(ahora.minute());
    display.print(":");
    if (ahora.second() < 10) display.print("0");
    display.print(ahora.second());
    
    display.print(" ");
    if (ahora.day() < 10) display.print("0");
    display.print(ahora.day());
    display.print("/");
    if (ahora.month() < 10) display.print("0");
    display.print(ahora.month());
    
    // ===== LÍNEA 2: SEPARADOR =====
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);
    
    // ===== LÍNEA 3-4: TEMPERATURA Y HUMEDAD PROMEDIO =====
    display.setTextSize(1);
    display.setCursor(0, 12);
    display.print("T:");
    display.print(tempPromedio, 1);
    display.print("C");
    
    display.setCursor(65, 12);
    display.print("H:");
    display.print(humPromedio, 0);
    display.print("%");
    
    // ===== LÍNEA 5-6: TEMPERATURA Y HUMEDAD MÁXIMA =====
    display.setCursor(0, 21);
    display.print("Max:");
    display.print(temperaturaMax, 1);
    display.print("C");
    
    display.setCursor(65, 21);
    display.print(humedadMax, 0);
    display.print("%");
    
    // ===== LÍNEA 7: SEPARADOR =====
    display.drawLine(0, 30, 128, 30, SSD1306_WHITE);
    
    // ===== LÍNEA 8-9: ESTADO DE SENSORES =====
    display.setTextSize(1);
    display.setCursor(0, 33);
    display.print("S1:");
    display.print(temperaturas[0], 0);
    display.print("C ");
    display.print(humedades[0], 0);
    display.print("%");
    
    display.setCursor(0, 42);
    display.print("S2:");
    display.print(temperaturas[1], 0);
    display.print("C ");
    display.print(humedades[1], 0);
    display.print("%");
    
    // ===== LÍNEA 10: SEPARADOR =====
    display.drawLine(0, 51, 128, 51, SSD1306_WHITE);
    
    // ===== LÍNEA 11: ESTADO DE ACTUADORES =====
    display.setCursor(0, 54);
    
    // Icono de luz
    if (potenciadeluces > 0) {
        display.print("LUZ:");
        display.print(map(potenciadeluces, 0, 255, 0, 100));
        display.print("%");
    } else {
        display.print("LUZ:OFF");
    }
    
    // Icono de calefacción
    display.setCursor(65, 54);
    if (estatusResistencia) {
        display.print("CAL:ON");
    } else {
        display.print("CAL:--");
    }
    
    // ===== ALARMA (si está activa) =====
    if (alarmaActiva) {
        // Mostrar alarma parpadeante
        if (millis() % 1000 < 500) {
            display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
            display.setCursor(20, 1);
            display.print("ALARMA:");
            display.print(codigoAlarma);
            display.setTextColor(SSD1306_WHITE);
        }
    }
    
    display.display();
}

#endif // MOSTRAR_PANTALLA__H
