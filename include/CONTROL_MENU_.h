#ifndef CONTROL_MENU__H
#define CONTROL_MENU__H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <VARS_.h>
#include <PINS_.h>

// =================================================================
//  CONFIGURACIÓN DEL ENCODER
// =================================================================

// Variables de control
bool menuActivo = false;
int estadoCLK_Ultimo;
int estadoCLK_Actual;
unsigned long ultimoMovimientoMenu = 0;
unsigned long tiempoBotonPresionado = 0;

// Referencia externa
extern Adafruit_SSD1306 display;

// =================================================================
//  ESTRUCTURA DE DATOS DEL MENÚ
// =================================================================

enum TipoDato { T_FLOAT, T_INT, T_TIEMPO_MS, T_HORA_MIN };

struct OpcionMenu {
    const char* nombre;
    void* variable;
    TipoDato tipo;
    float min;
    float max;
    float paso;
};

// --- SUBMENÚ: CLIMA ---
// Paso de temperatura ajustado a 0.1
OpcionMenu menuClima[] = {
    {"Temp Dia", &tempDia, T_FLOAT, 15.0, 35.0, 0.1},
    {"Temp Noche", &tempNoche, T_FLOAT, 10.0, 30.0, 0.1},
    {"Hum Objetivo", &humObjetivo, T_FLOAT, 30.0, 95.0, 1.0},
    {"Histeresis T", &tempHisteresis, T_FLOAT, 0.1, 5.0, 0.1},
    {"Histeresis H", &humHisteresis, T_FLOAT, 1.0, 10.0, 0.5}
};

// --- SUBMENÚ: LUZ Y CICLO ---
OpcionMenu menuLuz[] = {
    {"Inicio Dia", &initDia, T_HORA_MIN, 0, 1440, 15},
    {"Fin Dia", &finDia, T_HORA_MIN, 0, 1440, 15},
    {"Dur Amanecer", &duracionAmanecer, T_INT, 0, 180, 5}
};

// --- SUBMENÚ: TIEMPOS ACTUADORES ---
// Minimos y Maximos en MINUTOS. Paso de 0.1 min (6 segundos)
OpcionMenu menuTiempos[] = {
    {"Calef ON", &TIEMPO_ENCENDIDO_CALEFACCION, T_TIEMPO_MS, 0.1, 60.0, 0.1}, 
    {"Calef OFF", &TIEMPO_APAGADO_CALEFACCION, T_TIEMPO_MS, 0.1, 60.0, 0.1},
    {"Hum ON Min", &TIEMPO_MIN_ENCENDIDO_HUM, T_TIEMPO_MS, 0.1, 30.0, 0.1},
    {"Hum OFF Min", &TIEMPO_MIN_DESCANSO_HUM, T_TIEMPO_MS, 0.1, 60.0, 0.1}
};

// --- SUBMENÚ: VENTILACIÓN ---
// Paso de Potencia (PWM) ajustado a 1
OpcionMenu menuVent[] = {
    {"Ext Basal T", &T_EXT_BASAL, T_TIEMPO_MS, 0.1, 120.0, 0.1},
    {"Ext Rafaga T", &T_EXT_RAFAGA, T_TIEMPO_MS, 0.1, 30.0, 0.1},
    {"Ext Descanso", &T_EXT_DESCANSO, T_TIEMPO_MS, 0.0, 120.0, 0.1},
    {"Potencia Basal", &PWM_EXT_BASAL, T_INT, 0, 255, 1}, // Paso de 1 en 1
    {"Int ON T", &T_VENT_INT_ON, T_TIEMPO_MS, 0.1, 60.0, 0.1},
    {"Int OFF T", &T_VENT_INT_OFF, T_TIEMPO_MS, 0.1, 60.0, 0.1}
};

// --- ÍNDICES Y ESTADOS ---
int categoriaSeleccionada = 0;
int itemSeleccionado = 0;
bool enSubmenu = false;
bool enEdicion = false;

// Nombres de categorías principales
const char* categorias[] = {"1. SALIR", "2. CLIMA", "3. LUZ/DIA", "4. TIEMPOS", "5. VENTILACION"};
const int numCategorias = 5;

// Helpers
int getNumItemsSubmenu() {
    switch(categoriaSeleccionada) {
        case 1: return sizeof(menuClima)/sizeof(menuClima[0]);
        case 2: return sizeof(menuLuz)/sizeof(menuLuz[0]);
        case 3: return sizeof(menuTiempos)/sizeof(menuTiempos[0]);
        case 4: return sizeof(menuVent)/sizeof(menuVent[0]);
        default: return 0;
    }
}

OpcionMenu* getItemActual() {
    switch(categoriaSeleccionada) {
        case 1: return &menuClima[itemSeleccionado];
        case 2: return &menuLuz[itemSeleccionado];
        case 3: return &menuTiempos[itemSeleccionado];
        case 4: return &menuVent[itemSeleccionado];
        default: return nullptr;
    }
}

// =================================================================
//  FUNCIONES DE DIBUJO
// =================================================================

void dibujarEncabezado(const char* titulo) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE); 
    display.fillRect(0, 0, 128, 10, SSD1306_WHITE); 
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
    display.setCursor(2, 1);
    display.print(titulo);
    display.setTextColor(SSD1306_WHITE); 
}

void dibujarMenuPrincipal() {
    dibujarEncabezado("MENU PRINCIPAL");
    display.setTextSize(1);
    
    for (int i = 0; i < numCategorias; i++) {
        int y = 12 + (i * 10);
        display.setCursor(0, y);
        if (i == categoriaSeleccionada) display.print(F("> "));
        else display.print(F("  "));
        display.println(categorias[i]);
    }
    display.display();
}

void dibujarSubmenu() {
    dibujarEncabezado(categorias[categoriaSeleccionada]);
    display.setTextSize(1);

    int totalItems = getNumItemsSubmenu();
    int inicio = 0;
    if (itemSeleccionado > 3) inicio = itemSeleccionado - 3; 

    for (int i = 0; i < 5; i++) {
        int idx = inicio + i;
        if (idx >= totalItems) break;
        
        // Puntero temporal al array correcto
        OpcionMenu* lista = nullptr;
        if(categoriaSeleccionada==1) lista = menuClima;
        else if(categoriaSeleccionada==2) lista = menuLuz;
        else if(categoriaSeleccionada==3) lista = menuTiempos;
        else if(categoriaSeleccionada==4) lista = menuVent;

        int y = 12 + (i * 10);
        display.setCursor(0, y);
        if (idx == itemSeleccionado) display.print(F("> "));
        else display.print(F("  "));
        
        display.print(lista[idx].nombre);
        display.print(F(": "));
        
        // Mostrar valor preliminar
        if (lista[idx].tipo == T_FLOAT) {
            display.print(*(float*)lista[idx].variable, 1);
        }
        else if (lista[idx].tipo == T_INT) {
            display.print(*(int*)lista[idx].variable);
        }
        else if (lista[idx].tipo == T_TIEMPO_MS) {
            // Mostrar con 1 decimal (ej. 2.5 min)
            float minutos = *(unsigned long*)lista[idx].variable / 60000.0;
            display.print(minutos, 1);
        }
        else if (lista[idx].tipo == T_HORA_MIN) {
            int val = *(int*)lista[idx].variable;
            if(val/60 < 10) display.print("0");
            display.print(val/60);
            display.print(":");
            if(val%60 < 10) display.print("0");
            display.print(val%60);
        }
    }
    display.display();
}

void dibujarEdicion() {
    OpcionMenu* item = getItemActual();
    display.clearDisplay();
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(F("EDITANDO:"));
    display.setCursor(0, 10);
    display.print(item->nombre);
    display.drawLine(0, 20, 128, 20, SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(10, 35);

    if (item->tipo == T_FLOAT) {
        display.print(*(float*)item->variable, 1);
    } 
    else if (item->tipo == T_INT) {
        display.print(*(int*)item->variable);
    }
    else if (item->tipo == T_TIEMPO_MS) {
        // Mostrar valor en minutos con decimales
        float minutos = *(unsigned long*)item->variable / 60000.0;
        display.print(minutos, 1);
        display.setTextSize(1); display.print(" min");
    }
    else if (item->tipo == T_HORA_MIN) {
        int val = *(int*)item->variable;
        if(val/60 < 10) display.print("0");
        display.print(val/60);
        display.print(":");
        if(val%60 < 10) display.print("0");
        display.print(val%60);
    }
    
    display.display();
}

// =================================================================
//  LÓGICA DEL MENÚ
// =================================================================

void setupMenu() {
    pinMode(ENC_CLK, INPUT);
    pinMode(ENC_DT, INPUT);
    pinMode(ENC_SW, INPUT_PULLUP);
    estadoCLK_Ultimo = digitalRead(ENC_CLK);
}

void administrarMenuPantalla() {
    Serial.println(F("MENU: Entrando..."));
    
    digitalWrite(CALEFACTORA_PIN, RELAY_APAGADO);
    digitalWrite(HUMIDIFICADOR_PIN, RELAY_APAGADO);
    estatusResistencia = false;

    ultimoMovimientoMenu = millis();
    categoriaSeleccionada = 0;
    itemSeleccionado = 0;
    enSubmenu = false;
    enEdicion = false;

    while (menuActivo) {
        #ifdef WDTO_8S
        wdt_reset(); 
        #endif

        if (millis() - ultimoMovimientoMenu > 30000) {
            menuActivo = false; break;
        }

        // --- ENCODER ROTACIÓN ---
        estadoCLK_Actual = digitalRead(ENC_CLK);
        if (estadoCLK_Actual != estadoCLK_Ultimo && estadoCLK_Actual == 1) {
            ultimoMovimientoMenu = millis();
            int dir = (digitalRead(ENC_DT) != estadoCLK_Actual) ? 1 : -1;

            if (!enSubmenu) {
                categoriaSeleccionada += dir;
                if (categoriaSeleccionada < 0) categoriaSeleccionada = numCategorias - 1;
                if (categoriaSeleccionada >= numCategorias) categoriaSeleccionada = 0;
            } 
            else if (!enEdicion) {
                itemSeleccionado += dir;
                int maxItems = getNumItemsSubmenu();
                if (itemSeleccionado < 0) itemSeleccionado = maxItems - 1;
                if (itemSeleccionado >= maxItems) itemSeleccionado = 0;
            }
            else {
                // --- MODO EDICIÓN ---
                OpcionMenu* item = getItemActual();
                
                if (item->tipo == T_FLOAT) {
                    float* val = (float*)item->variable;
                    *val += (dir * item->paso);
                    if (*val < item->min) *val = item->min;
                    if (*val > item->max) *val = item->max;
                }
                else if (item->tipo == T_INT) {
                    int* val = (int*)item->variable;
                    *val += (dir * (int)item->paso);
                    if (*val < (int)item->min) *val = (int)item->min;
                    if (*val > (int)item->max) *val = (int)item->max;
                }
                else if (item->tipo == T_TIEMPO_MS) {
                    unsigned long* val = (unsigned long*)item->variable;
                    
                    // Convertimos el paso (ej 0.1 min) a milisegundos (6000 ms)
                    long pasoMS = item->paso * 60000; 
                    
                    // Como es unsigned long, hacemos casting a long para evitar overflow negativo
                    long nuevoValor = (long)*val + (dir * pasoMS);
                    
                    // Verificamos limites (convertidos a MS)
                    long minMS = item->min * 60000;
                    long maxMS = item->max * 60000;
                    
                    if (nuevoValor < minMS) nuevoValor = minMS;
                    if (nuevoValor > maxMS) nuevoValor = maxMS;
                    
                    *val = (unsigned long)nuevoValor;
                }
                else if (item->tipo == T_HORA_MIN) {
                    int* val = (int*)item->variable;
                    *val += (dir * (int)item->paso);
                    if (*val < 0) *val = 1425; // Loop vuelta atras
                    if (*val > 1439) *val = 0; // Loop vuelta adelante
                }
            }
        }
        estadoCLK_Ultimo = estadoCLK_Actual;

        // --- ENCODER BOTÓN ---
        if (digitalRead(ENC_SW) == LOW) {
            delay(50);
            if (digitalRead(ENC_SW) == LOW) {
                ultimoMovimientoMenu = millis();
                while(digitalRead(ENC_SW) == LOW); 

                if (!enSubmenu) {
                    if (categoriaSeleccionada == 0) menuActivo = false; 
                    else { enSubmenu = true; itemSeleccionado = 0; }
                }
                else if (!enEdicion) {
                    enEdicion = true;
                }
                else {
                    enEdicion = false;
                }
            }
        }
        
        // --- DIBUJADO ---
        if (!enSubmenu) dibujarMenuPrincipal();
        else if (!enEdicion) dibujarSubmenu();
        else dibujarEdicion();
    }
    
    display.clearDisplay();
    display.display();
}

void verificarEntradaMenu() {
    if (digitalRead(ENC_SW) == LOW) {
        if (tiempoBotonPresionado == 0) tiempoBotonPresionado = millis();
        else if (millis() - tiempoBotonPresionado > 2000) { 
            menuActivo = true;
            tiempoBotonPresionado = 0;
            while(digitalRead(ENC_SW) == LOW); 
        }
    } else {
        tiempoBotonPresionado = 0;
    }
}

#endif // CONTROL_MENU__H