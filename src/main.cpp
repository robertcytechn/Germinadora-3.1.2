#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>
#include <avr/wdt.h> // Necesario para wdt_enable

#include <PINS_.h>
#include <VARS_.h>

#include <SENSORES_.h>
#include <CONTROL_LUCES_.h>
#include <CONTROL_VENTILACION_.h>
#include <CONTROL_TEMPERATURA_.h>
#include <MOSTRAR_PANTALLA_.h>
#include <CONTROL_HUMEDAD_.h>
#include <CONTROL_MENU_.h>

// =================================================================
//  OBJETOS GLOBALES
// =================================================================
RTC_DS1307 reloj;
DHT dhtSuperior(DHT_SUPERIOR_PIN, DHT11);
DHT dhtInferior(DHT_INFERIOR_PIN, DHT11);
DHT dhtPuerta(DHT_PUERTA_PIN, DHT11);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_ADDR);




void setup() {
    Serial.begin(9600);
    Serial.println("\n\n=====================================");
    Serial.println("Iniciando sistema Germinadora 3.1.2");
    Serial.println("=====================================");

    Wire.begin();
    Wire.setWireTimeout(25000, true);

    // Inicializar sensores y reloj RTC
    Serial.println("Inicializando sensores y reloj...");
    dhtSuperior.begin();
    dhtInferior.begin();
    dhtPuerta.begin();
    if (!reloj.begin()) {
        Serial.println("ERROR: No se pudo encontrar el reloj RTC. Verifique la conexión.");
        while (1);
    }
    // Ajustar la hora del reloj RTC al momento de la compilación - comentar después de la primera vez y volver a cargar el firmaware
    // reloj.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Inicializar pantalla OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
        Serial.println(F("ERROR: Fallo al inicializar la pantalla OLED."));
    }

    // Configuración de pines
    pinMode(LEDS_ROJOS_PIN, OUTPUT);
    digitalWrite(LEDS_ROJOS_PIN, HIGH);           // apagamos rele con logica inversa
    pinMode(CALEFACTORA_PIN, OUTPUT);
    digitalWrite(CALEFACTORA_PIN, HIGH);           // apagamos rele con logica inversa
    pinMode(VENTILADOR_PIN, OUTPUT);
    pinMode(VENTINTER_PIN, OUTPUT);
    pinMode(LUCES_BLANCAS_PIN, OUTPUT);
    pinMode(HUMIDIFICADOR_PIN, OUTPUT);
    digitalWrite(HUMIDIFICADOR_PIN, RELAY_APAGADO); // Aseguramos que arranque "sin presionar"


    Serial.println("Inicializacion completada.");

    //watchdog por si acaso se cuelga el sistema - opcional
    wdt_enable(WDTO_8S); // habilitar el watchdog con un tiempo de 8 segundos

    if (modoAntiHongos){
        Serial.println("MODO ANTI-HONGOS ACTIVADO");
        // modificamos variables globales para modo anti-hongos solo si está activado el modo anti-hongos
        tempDia = 30.0; // ajustamos temperatura diurna a 30 grados
        tempNoche = 30.0; // ajustamos temperatura nocturna a 30 grados
        tempMaxSeguridad = 35.0; // ajustamos temperatura máxima de seguridad a 35 grados
    }
}

void loop() {
    wdt_reset(); // 1. Alimentar al perro guardián (Estoy vivo)
    TIEMPO_ACTUAL_MS = millis();

    // 2. EL PORTERO DEL MENÚ
    // Verifica si el botón del encoder se está presionando
    verificarEntradaMenu();

    if (menuActivo) {
        // ==========================================
        // MODO 1: MENÚ DE CONFIGURACIÓN (Bloqueante)
        // ==========================================
        // Si entramos aquí, el resto del código se DETIENE.
        // La función administra la pantalla y los cambios de variables.
        
        administrarMenuPantalla(); 
        
        // Al salir del menú:
        display.clearDisplay(); // Limpiamos residuos visuales
        RELOJ_GLOBAL = reloj.now(); // Actualizamos hora inmediatamente
    } 
    else {
        // ==========================================
        // MODO 2: CONTROL AUTOMÁTICO (Normal)
        // ==========================================
        
        // Protección de arranque: Esperar unos ms para que sensores estabilicen
        if (TIEMPO_ACTUAL_MS < TIEMPO_REACCION_GLOBAL) {
            return;
        }
        
        // A. Leer Hora (Una vez por ciclo)
        RELOJ_GLOBAL = reloj.now();
        
        // B. Ejecutar Módulos de Control
        leerTemperaturaHumedad();
        controlLuces();
        controlVentilacion();
        controlCalefaccion();
        controlHumedad();
        
        // C. Interfaz de Usuario
        mostrarPantalla();
    }
}