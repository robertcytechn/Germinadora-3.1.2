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
}

void loop() {
    // sigo vivo
    wdt_reset();
    TIEMPO_ACTUAL_MS = millis();

    // usamos TIEMPO_REACCION_GLOBAL para evitar que la lógica principal se ejecute 
    // inmediatamente al inicio, permitiendo que los sensores se estabilicen.
    if (TIEMPO_ACTUAL_MS < TIEMPO_REACCION_GLOBAL) {
        return;
    }
    
    RELOJ_GLOBAL = reloj.now();
    leerTemperaturaHumedad();
    controlLuces();
    controlVentilacion();
    controlCalefaccion();
    controlHumedad();
    mostrarPantalla();
}