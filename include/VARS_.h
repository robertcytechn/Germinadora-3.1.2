#ifndef VARS__H
#define VARS__H
// =================================================================
//  DEFINICIÓN DE VARIABLES GLOBALES
// =================================================================
DateTime RELOJ_GLOBAL;
const unsigned long TIEMPO_REACCION_GLOBAL = 300; // tiempo de espera inicial para estabilización de sensores en milisegundos (0.3 segundos)
unsigned long TIEMPO_ACTUAL_MS = 0;


// relays con logica inversa
const int RELAY_ENCENDIDO = LOW;
const int RELAY_APAGADO = HIGH;


// --- CONFIGURACIÓN DE UMBRALES DE HUMEDAD ---
float humObjetivo = 70.0;           // (Referencia)
float humTriggerNormal = 50.0;      // < 50% Enciende (si ya descansó)
float humTriggerEmergencia = 30.0;  // < 30% Enciende AHORA (Ignora descanso)
float humMaxSeguridad = 90.0;       // > 90% Apaga forzado (Seguridad)

// --- CONFIGURACIÓN DE TIEMPOS (en milisegundos) ---
const unsigned long TIEMPO_LECTURA_DHT = 2000;              // tiempo entre lecturas de sensores DHT11 (2 segundos)
const unsigned long DURACION_PULSO_BOTON = 250;             // Tiempo del "dedo" (0.25 seg)
const unsigned long TIEMPO_MIN_ENCENDIDO_HUM = 2 * 60000UL; // 2 Minutos OBLIGATORIOS encendido
const unsigned long TIEMPO_MIN_DESCANSO_HUM = 10 * 60000UL; // 10 Minutos OBLIGATORIOS descanso

// --- ESTADOS DE LA MÁQUINA (Lógica Interna) ---
const int ESTADO_HUM_MONITOREO    = 0; // Vigilando (Apagado listo)
const int ESTADO_HUM_PULSANDO_ON  = 1; // "Dedo" presionando para PRENDER
const int ESTADO_HUM_TRABAJANDO   = 2; // Cumpliendo los 2 minutos obligatorios
const int ESTADO_HUM_PULSANDO_OFF = 3; // "Dedo" presionando para APAGAR
const int ESTADO_HUM_DESCANSO     = 4; // Cumpliendo los 10 minutos de enfriamiento

// Variables de control
static int etapaHumidificador = ESTADO_HUM_MONITOREO; 
static unsigned long timerHumidificador = 0; // Cronómetro general del humidificador
// Nota: estatusHumidificador se mantiene en CONTROL_HUMEDAD_.h o aquí según prefieras


// Configuración de iluminación
int initDia = 7 * 60;                           // inicio del cilo del dia en minutos (7:00 AM)
int finDia = 21 * 60;                           // fin del ciclo del dia en minutos (9:00 PM)
int duracionAmanecer = 90;                      // duración del amanecer/atardecer en minutos
int potenciadeluces = 0;                        // potencia actual de las luces blancas
const unsigned long TIEMPO_REACCION_LUCES = 1 * 60000UL;     // tiempo de reaccion de las luces en milisegundos (1 minuto)
unsigned long ultimoCambio = 0;                 // marca de tiempo del último cambio de iluminación



// Configuración de temperatura
float tempDia = 25.0;                                               // temperatura objetivo durante el día
float tempNoche = 15.0;                                             // temperatura objetivo durante la noche
float tempHisteresis = 1.0;                                         // margen de histéresis para evitar cambios frecuentes
float tempMaxSeguridad = 28.0;                                      // temperatura máxima de seguridad
float tempMinSeguridad = 10.0;                                      // temperatura mínima de seguridad  
bool estatusResistencia = false;                                    // estado actual de la resistencia de calefacción
const unsigned long TIEMPO_ENCENDIDO_CALEFACCION = 3 * 60000UL;     // 3 minutos de encendido continuo
const unsigned long TIEMPO_APAGADO_CALEFACCION = 5 * 60000UL;       // 5 minutos de descanso
unsigned long tiempoUltimoCambioCalefaccion = 0;                    // marca de tiempo del último cambio de la calefacción
const unsigned long TIEMPO_REACCION_CALEFACCION = 30000UL;          // tiempo de reaccion de la calefacción en milisegundos (30 segundos)

// configuración de ventilación externa e interna
const unsigned long T_VENT_INT_ON = 3 * 60000UL;                // 3 minutos de ventilación interna encendida
const unsigned long T_VENT_INT_OFF = 12 * 60000UL;              // 12 minutos de ventilación interna apagada
const unsigned long T_EXT_BASAL = 30 * 60000UL;                 // 30 minutos de ventilación externa basal
const unsigned long T_EXT_RAFAGA = 5 * 60000UL;                 // 5 minutos de ventilación externa en ráfaga
const unsigned long T_EXT_DESCANSO = 25 * 60000UL;              // 25 minutos de descanso entre ciclos de ventilación externa
const int PWM_EXT_BASAL = 25;                                   // potencia basal de ventilación externa
const int PWM_EXT_RAFAGA = 255;                                 // potencia máxima de ventilación externa en ráfaga
const int PWM_EXT_OFF = 0;                                      // ventilación externa apagada  
const int PWM_INT_VENT_MAX = 255;                               // ventilación interna al máximo
const int PWM_INT_VENT_MED = 128;                               // ventilación interna a medio nivel
const int PWM_INT_VENT_OFF = 0;                                 // ventilación interna apagada
unsigned long ultimaVentInterno = 0;                            // marca de tiempo del último cambio de ventilación interna
unsigned long inicioCicloExt = 0;                               // marca de tiempo del inicio del ciclo de ventilación externa
int estadoVentExt = 0;                                          // estado actual de la ventilación externa: 0=apagado, 1=basal, 2=ráfaga
bool estadoVentInt = false;                                     // estado actual de la ventilación interna: false=apagada, true=encendida
const unsigned long TIEMPO_REACCION_VENTILACION = 30000UL;      // tiempo de reaccion de la ventilación en milisegundos (30 segundos)
unsigned long ultimoCambioVentilacion = 0;                      // marca de tiempo del último cambio de ventilación


// Tiempos
const unsigned long TIEMPO_ACTUALIZACION_PANTALLA = 1000;         // tiempo de actualización de la pantalla en milisegundos (1 segundo)


// Variables de estado
float temperaturas[3];
float humedades[3];
float tempPromedio = 0;
float humPromedio = 0;
float temperaturaMax = 0;
float humedadMax = 0;

// Alarmas
bool alarmaActiva = false;
int codigoAlarma = 0;

#endif // VARS__H