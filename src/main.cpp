#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include <Wire.h>
#include <SPI.h>

#include <PINS_.h>
#include <VARS_.h>

#include <SENSORES_.h>
#include <CONTROL_LUCES_.h>
#include <CONTROL_VENTILACION_.h>

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
    dhtSuperior.begin();
    dhtInferior.begin();
    dhtPuerta.begin();
    reloj.begin();
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

    // Configuración de pines
    pinMode(LEDS_ROJOS_PIN, OUTPUT);
    pinMode(CALEFACTORA_PIN, OUTPUT);
    pinMode(HUMIDIFICADOR_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(VENTILADOR_PIN, OUTPUT);
    pinMode(VENTINTER_PIN, OUTPUT);
    pinMode(LUCES_BLANCAS_PIN, OUTPUT);
    
    pinMode(BTN_ENTER_PIN, INPUT_PULLUP);
    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);


}

void loop() {
    //leerTemperaturaHumedad();
    //controlLuces();
    //controlVentilacion();
}