#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Key.h>
#include <Keypad.h>
#include "display.h"
#include "controller.h"
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
void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinoutEncoder();
  initDisplay();
  // Inicialización de ADC y DAC
  ads.begin(ADS1115_ADDRESS);
  dac.begin(MCP4725_ADDRESS);
  ads.setDataRate(RATE_ADS1115_860SPS);   // Configura la velocidad de muestreo a 250 SPS
  pinMode(2, OUTPUT); // Pin D2 Output Rele
  digitalWrite(2, HIGH);
  reset_variables();
  constantesControlador();
  setPotentiometer(0,50);  // Canal 0, valor 128 (mitad del rango)
  proteccionDeCarga();
}

void loop() {
  //referenceValues();
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  v_act = (adc0 * (5.0 / ADC_RESOLUTION))*H_v;  // Convierte el valor del ADC0 a voltaje
  i_act = (adc1 * (5.0 / ADC_RESOLUTION))*H_i;  // Convierte el valor del ADC1 a voltaje

  // proteccionDeCarga();
  // Serial.println(v_ref, 6);  // Imprime i_act con 6 decimales de precisión
  algoritmo_control(v_act, i_act);
  //Actualizar_Pantalla(v_act, i_act);
  //encoder_1();
  //encoder_2();
  aux = (ui * DAC_RESOLUTION) / 5.0;  // Ajusta el voltaje a la resolución del DAC
  dacValue = aux;
  dac.setVoltage(dacValue, false);  // Enviar valor al DAC
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
void referenceValues(){
  modo_ref = Menu_Teclado();
  switch (modo_ref){
    case 0: //No hace nada
      // Desconecta rele forzosamente.
      break;   
    case 1:
      v_ref= t_teclado;
      i_max= i_teclado;
     break;
    case 5:
      v_ref= v_encoder;
      i_max = i_encoder;
      break;
    case 2:
      i_max = i_teclado;
      break;
    case 6:
      i_max = i_encoder;
      break;
    case 3:
      v_ref = t_teclado;
      break;
    case 7:
      v_ref= v_encoder;
      break;
  }  
}


