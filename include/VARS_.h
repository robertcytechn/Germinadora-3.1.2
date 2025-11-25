// =================================================================
//  DEFINICIÓN DE VARIABLES GLOBALES
// =================================================================

// Configuración de temperatura
float tempDia = 25.0;
float tempNoche = 15.0;
float tempHisteresis = 1.0;
float tempMaxSeguridad = 30.0;
float tempMinSeguridad = 10.0;

// Configuración de humedad
float humObjetivo = 75.0;
float humHisteresis = 5.0;
float humMaxSeguridad = 95.0;
float humMinSeguridad = 35.0;

// Configuración de iluminación
int initDia = 7;
int finDia = 21;
int duracionAmanecer = 90;

// Tiempos
const unsigned long TIEMPO_LECTURA_DHT = 2000;
const unsigned long TIEMPO_ACTUALIZACION_PANTALLA = 1000;

const unsigned long T_VENT_INT_ON = 3 * 60000UL;
const unsigned long T_VENT_INT_OFF = 12 * 60000UL;

const unsigned long T_EXT_BASAL = 30 * 60000UL;
const unsigned long T_EXT_RAFAGA = 5 * 60000UL;
const unsigned long T_EXT_DESCANSO = 25 * 60000UL;

// PWM
const int PWM_EXT_BASAL = 20;
const int PWM_EXT_RAFAGA = 255;
const int PWM_EXT_OFF = 0;

// Variables de estado
float temperaturas[3];
float humedades[3];
float tempPromedio = 0;
float humPromedio = 0;
float temperaturaMax = 0;
float humedadMax = 0;

int potenciadeluces = 0;
int potenciaVentiladorexterno = 0;
bool lucesRojasOn = false;
bool estatusResistencia = false;

// Temporizadores
unsigned long ultimaVentInterno = 0;
bool ventInternoActivo = false;

unsigned long inicioCicloExt = 0;
int estadoVentExt = 0;

unsigned long inicioCalefaccion = 0;
const unsigned long MAX_TIEMPO_CALEFACCION = 5 * 60000UL;

// Alarmas
bool alarmaActiva = false;
int codigoAlarma = 0;