#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Key.h>
#include <Keypad.h>
#include "display.h"
#include "controller.h"
#include "encoder.h"
#include "keyboard.h"
// Direcciones de los dispositivos I2C
#define ADS1115_ADDRESS 0x48
#define MCP4725_ADDRESS 0x60
#define DAC_RESOLUTION 4095.0
#define ADC_RESOLUTION 26666.0  // Valor Practico diferente a valor real que es 2^16
#define MCP4661_ADDRESS 0x28    // Potenciometro

Adafruit_ADS1115 ads;
Adafruit_MCP4725 dac;
//Definición de pines
#define pinCarga 2
#define int_cc 3
// Variables para almacenar valores del ADC
int16_t adc0, adc1;
float v_act = 0;
float i_act = 0;
float aux;
int dacValue;
unsigned long tiempoDesconexion = 0; // Para almacenar el tiempo de desconexión
bool cargando = true;               // Estado del pin de carga
// Para actualización de la pantalla
unsigned long previousMillis = 0;  // Almacena el último momento en que se actualizó la pantalla
#define interval 1000              // Intervalo de actualización de pantalla (en milisegundos, 1000 ms = 1 segundo)
//Interrupción de protección por sobrecorriente
volatile bool sobrecorriente = false;
void ISR_apagar_todo() {
  sobrecorriente = true;
  digitalWrite(pinCarga, LOW);
}
bool sobretension = false;        //Bandera de protección por sobretensión.
unsigned long startTime = 0;  // Tiempo de inicio para la medición
bool isBelowThreshold = false;  // Estado si v_act está por debajo del umbral
//------INICIALIZACIÓN----------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinoutEncoder();
  initDisplay();
  ads.begin(ADS1115_ADDRESS); // Inicialización de ADC y DAC
  dac.begin(MCP4725_ADDRESS);
  ads.setDataRate(RATE_ADS1115_860SPS);  // Configura la velocidad de muestreo a 860 SPS
  setPotentiometer(0, 1);               // Canal 0, valor 128 (mitad del rango)  1= 5v 256 =0v
  //Inicialización de Pines
  pinMode(pinCarga, OUTPUT);  // Pin D2 Salida al Rele
  pinMode(int_cc, INPUT);     // Pin D3 Entrada del comparador de ventana
  attachInterrupt(digitalPinToInterrupt(int_cc), ISR_apagar_todo, FALLING);
  //Valores iniciales por defecto
  reset_variables();
  v_ref = 15;
  i_max = 1;
  escribir_DAC(0);  
}
//-------BUCLE PRINCIPAL-----------------------------------------------------------
void loop() {
  Menu_Teclado();
  if (encoders) {
    encoder_1();
    encoder_2();
  }
  switch (modo) {
    case 0:  //Modo Apagado
      lecturaValores();
      digitalWrite(2, LOW); 
      escribir_DAC(0);
      break;

    case 1: //Modo tensión
      lecturaValores();
      proteccion_tension();
      algoritmo_control_tension(v_act, i_act);
      escribir_DAC(ui);
      break;

    case 2: //Modo corriente
      lecturaValores();
      algoritmo_control_corriente(v_act, i_act);
      escribir_DAC(ui);
      break;
    case 3:  //Modo Rampa
      lecturaValores();
      proteccion_tension();
      algoritmo_control_rampa(v_act, i_act, v_final, tiempo / 1.6); //Tension. Segundos
      escribir_DAC(ui);
    default:
      break;
  }
  ejecutarPantallaCadaSegundo(v_act, i_act);
}

void lecturaValores() {
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  v_act = (adc0 * (5.0 / ADC_RESOLUTION)) * H_v;  // Convierte el valor del ADC0 a voltaje
  i_act = (adc1 * (5.0 / ADC_RESOLUTION)) * H_i;  // Convierte el valor del ADC1 a voltaje
}

void escribir_DAC(float valor) {
  aux = (valor * DAC_RESOLUTION) / 5.0;  // Ajusta el voltaje a la resolución del DAC
  dacValue = aux;
  dac.setVoltage(dacValue, false);  // Enviar valor al DAC
}

void ejecutarPantallaCadaSegundo(float v_act, float i_act) {
  unsigned long currentMillis = millis();  // Obtiene el tiempo actual
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;     // Actualiza el tiempo para la próxima ejecución
    Actualizar_Pantalla(v_act, i_act);  // Llama a la función de actualización de pantalla
  }
}

void proteccion_tension(){
   if (v_act < v_ref*1.1) {
    if (!isBelowThreshold) {
      startTime = millis();  // Si es la primera vez que está por debajo, guarda el tiempo actual
      isBelowThreshold = true;
    } else {
      if (millis() - startTime >= 2000) { // Han pasado 2 segundos con v_act por debajo del valor de referencia
        digitalWrite(pinCarga, HIGH);
      }
    }
  } else {    // Si v_act está por encima del valor de referencia
    isBelowThreshold = false;  // Reinicia el estado
    digitalWrite(pinCarga, LOW);
  }
}