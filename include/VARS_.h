#ifndef VARS__H
#define VARS__H
// =================================================================
//  DEFINICIÓN DE VARIABLES GLOBALES
// =================================================================

// Configuración de temperatura
float tempDia = 25.0;
float tempNoche = 15.0;
float tempHisteresis = 1.0;
float tempMaxSeguridad = 28.0;
float tempMinSeguridad = 10.0;

// Configuración de humedad
float humObjetivo = 75.0;
float humHisteresis = 5.0;
float humMaxSeguridad = 95.0;
float humMinSeguridad = 55.0;



// Tiempos
const unsigned long TIEMPO_LECTURA_DHT = 5000;
const unsigned long TIEMPO_ACTUALIZACION_PANTALLA = 1000;

const unsigned long T_VENT_INT_ON = 3 * 60000UL;
const unsigned long T_VENT_INT_OFF = 12 * 60000UL;

const unsigned long T_EXT_BASAL = 30 * 60000UL;    // 30 minutos de ventilación externa basal
const unsigned long T_EXT_RAFAGA = 5 * 60000UL;   // 5 minutos de ventilación externa en ráfaga
const unsigned long T_EXT_DESCANSO = 25 * 60000UL; // 25 minutos de descanso entre ciclos de ventilación externa

// PWM
const int PWM_EXT_BASAL = 25;
const int PWM_EXT_RAFAGA = 255;
const int PWM_EXT_OFF = 0;

const int PWM_INT_VENT_MAX = 255;
const int PWM_INT_VENT_MED = 128;
const int PWM_INT_VENT_MIN = 0;


// Variables de estado
float temperaturas[3];
float humedades[3];
float tempPromedio = 0;
float humPromedio = 0;
float temperaturaMax = 0;
float humedadMax = 0;


int potenciaVentiladorexterno = 0;
bool estatusResistencia = false;

// Temporizadores
unsigned long ultimaVentInterno = 0;

unsigned long inicioCicloExt = 0;
int estadoVentExt = 0;

unsigned long inicioCalefaccion = 0;
const unsigned long MAX_TIEMPO_CALEFACCION = 5 * 60000UL;
const unsigned long TIEMPO_MINIMO_APAGADO_CALEFACCION = 10 * 60000UL;

// Alarmas
bool alarmaActiva = false;
int codigoAlarma = 0;

#endif // VARS__H