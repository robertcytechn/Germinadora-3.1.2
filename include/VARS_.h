#ifndef VARS__H
#define VARS__H
// =================================================================
//  DEFINICIÓN DE VARIABLES GLOBALES
// =================================================================

// Configuración de humedad
float humObjetivo = 75.0;                      // humedad objetivo en %
float humHisteresis = 5.0;                     // margen de histéresis para evitar cambios frecuentes
float humMaxSeguridad = 95.0;                  // humedad máxima de seguridad
float humMinSeguridad = 55.0;                  // humedad mínima de seguridad

// Configuración de iluminación
int initDia = 7 * 60;                           // inicio del cilo del dia en minutos (7:00 AM)
int finDia = 21 * 60;                           // fin del ciclo del dia en minutos (9:00 PM)
int duracionAmanecer = 90;                      // duración del amanecer/atardecer en minutos
int potenciadeluces = 0;                        // potencia actual de las luces blancas
unsigned long tiempoReaccion = 1 * 60000UL;     // tiempo de reaccion de las luces en milisegundos (1 minuto)
unsigned long ultimoCambio = 0;                 // marca de tiempo del último cambio de iluminación
// relays con logica inversa
const int RELAY_ENCENDIDO = LOW;
const int RELAY_APAGADO = HIGH;


// Configuración de temperatura
float tempDia = 25.0;                                               // temperatura objetivo durante el día
float tempNoche = 15.0;                                             // temperatura objetivo durante la noche
float tempHisteresis = 1.0;                                         // margen de histéresis para evitar cambios frecuentes
float tempMaxSeguridad = 28.0;                                      // temperatura máxima de seguridad
float tempMinSeguridad = 10.0;                                      // temperatura mínima de seguridad  
bool estatusResistencia = false;                                    // estado actual de la resistencia de calefacción
const unsigned long TIEMPO_ENCENDIDO_CALEFACCION = 3 * 60000UL;     // 3 minutos de encendido continuo
const unsigned long TIEMPO_APAGADO_CALEFACCION = 5 * 60000UL;      // 5 minutos de descanso
unsigned long tiempoUltimoCambioCalefaccion = 0;                    // marca de tiempo del último cambio de la calefacción
unsigned long tiempoReaccionCalefaccion = 3000UL;                 // tiempo de reaccion de la calefacción en milisegundos (3 segundos)

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
unsigned long tiempoReaccionVentilacion = 30000UL;              // tiempo de reaccion de la ventilación en milisegundos (30 segundos)
unsigned long ultimoCambioVentilacion = 0;                      // marca de tiempo del último cambio de ventilación

// configuración de control de humedad
static bool estatusHumidificador = false; 
unsigned long tiempoUltimoCambioHumedad = 0;      
const unsigned long tiempoReaccionHumedad = 2 * 60000UL; // 2 minutos entre cambios normales
const int DURACION_PULSO_BOTON = 500;             
// --- VARIABLES PARA LA RESINCRONIZACIÓN (NUEVO) ---
const float HUM_UMBRAL_DESINCRONIZACION = 93.0;   // Si pasa de esto, asumimos que se quedó pegado encendido
unsigned long ultimoIntentoResync = 0;            // Para no estar pulsando a cada rato si la humedad baja lento
const unsigned long TIEMPO_ESPERA_RESYNC = 5 * 60000UL; // Esperamos 5 min después de un pulso de emergencia antes de intentar otro



// Tiempos
const unsigned long TIEMPO_LECTURA_DHT = 5000;                    // tiempo entre lecturas del sensor DHT en milisegundos (5 segundos)
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