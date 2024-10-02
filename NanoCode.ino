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
#define ADC_RESOLUTION 26666.0   // Valor Practico diferente a valor real que es 2^16 
#define MCP4661_ADDRESS 0x28  //Potenciometro
Adafruit_ADS1115 ads;
Adafruit_MCP4725 dac;

// Variables para almacenar valores del ADC
int16_t adc0, adc1;
int modo_ref;
double v_act = 0; 
double i_act = 0;
unsigned long startTime = 0;  // tiempo de inicio para la medición
bool isBelowThreshold = false;  // Estado si v_act está por debajo del umbral
const int pinCarga = 2;  // Pin donde que trabaja sobre el relé
float aux;
int dacValue;
unsigned long previousMillis = 0;   // Almacena el último momento en que se actualizó la pantalla
const long interval = 1000;         // Intervalo de actualización de pantalla (en milisegundos, 1000 ms = 1 segundo)
void setPotentiometer(byte channel, byte value);
void proteccionDeCarga();

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinoutEncoder();
  initDisplay();
  // Inicialización de ADC y DAC
  ads.begin(ADS1115_ADDRESS);
  dac.begin(MCP4725_ADDRESS);
  ads.setDataRate(RATE_ADS1115_860SPS);   // Configura la velocidad de muestreo a 250 SPS
  reset_variables();
  v_ref=15; i_max=1;
  constantesControlador();
  setPotentiometer(0,50);  // Canal 0, valor 128 (mitad del rango)
  pinMode(2, OUTPUT); // Pin D2 Output Rele
  digitalWrite(2, HIGH);
}


void loop() {
  //referenceValues();
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  v_act = (adc0 * (5.0 / ADC_RESOLUTION))*H_v;  // Convierte el valor del ADC0 a voltaje
  i_act = (adc1 * (5.0 / ADC_RESOLUTION))*H_i;  // Convierte el valor del ADC1 a voltaje
  //Serial.println(v_act, 6);
  //Serial.println(i_act, 6);
  proteccionDeCarga();
  //Serial.println(v_ref, 6);  // Imprime i_act con 6 decimales de precisión
  //Serial.println(i_act, 6);  // Imprime i_act con 6 decimales de precisión
  //algoritmo_control(v_act, i_act);
  Actualizar_Pantalla(v_act, i_act);
  Menu_Teclado();
  if(encoders){
    encoder_1();
    encoder_2();
  }
  aux = (ui * DAC_RESOLUTION) / 5.0;  // Ajusta el voltaje a la resolución del DAC
  dacValue = aux;
  dac.setVoltage(dacValue, false);  // Enviar valor al DAC
}

void ejecutarPantallaCadaSegundo(float v_act, float i_act) {
  unsigned long currentMillis = millis();  // Obtiene el tiempo actual
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Actualiza el tiempo para la próxima ejecución
    Actualizar_Pantalla(v_act, i_act);  // Llama a la función de actualización de pantalla
  }
}

void proteccionDeCarga(){
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

void setPotentiometer(byte channel, byte value) {
  Wire.beginTransmission(MCP4661_ADDRESS);
  Wire.write((channel == 0) ? 0x00 : 0x10); // Selecciona el canal (0 o 1)
  Wire.write(value); // Configura el valor del potenciómetro
  Wire.endTransmission();
}



