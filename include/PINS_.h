#ifndef PINS__H
#define PINS__H

// =================================================================
//  DEFINICIÓN DE PINES
// =================================================================


// [] = [nombre del dispositivo] [numero del pin]
// === SENSORES ===
#define DHT_SUPERIOR_PIN 2              //[dht11] superior
#define DHT_INFERIOR_PIN 3              //[dht11] inferior
#define DHT_PUERTA_PIN 4                //[dht11] puerta

// === ACTUADORES PWM ===
#define VENTILADOR_PIN 5                // pin del ventilador externo (PWM)
#define LUCES_PIN 6                     // pin de las luces blancas para simular la luz del sol (PWM)
#define VENTINTER_PIN 8                 // pin de la ventilación interna (PWM)

// === ACTUADORES DIGITALES ===
#define CALEFACTORA_PIN 7               // pin de la resistencia de calefacción
#define LEDS_ROJOS_PIN 9                // pin de los LEDs rojos (espectro fotosíntesis)
#define HUMIDIFICADOR_PIN 10            // pin del nebulizador/humidificador ultrasónico
#define BUZZER_PIN 11                   // pin del buzzer para alarmas


// === COMUNICACIÓN I2C ===
#define RTC_ADDR 0x68                   // dirección i2c del rtc DS1307
#define OLED_ADDR 0x3C                  // dirección i2c del oled SSD1306
#define OLED_WIDTH 128                  // ancho del oled en píxeles
#define OLED_HEIGHT 64                  // alto del oled en píxeles


// === PUSCH BUTTONS (Menú de Navegación) ===
#define BTN_ENTER_PIN 25                // pin del botón Enter del menú
#define BTN_UP_PIN 26                   // pin del botón Arriba del menú
#define BTN_DOWN_PIN 27                 // pin del botón Abajo del menú


#endif // PINS__H