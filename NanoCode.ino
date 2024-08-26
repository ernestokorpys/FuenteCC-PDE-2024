#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Key.h>
#include <Keypad.h>
#include "display.h"

Adafruit_ADS1115 ads;
Adafruit_MCP4725 dac;

// Variables para almacenar valores del ADC
int16_t adc0, adc1;
float v_ref, i_ref;
int modo_ref;

// Direcciones de los dispositivos I2C
#define ADS1115_ADDRESS 0x48
#define MCP4725_ADDRESS 0x60
#define DAC_RESOLUTION 4095.0
#define ADC_RESOLUTION 26666.0   // Valor Practico diferente a valor real que es 2^16 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinoutEncoder();
  initDisplay();
  // Inicialización de ADC y DAC
  ads.begin(ADS1115_ADDRESS);
  dac.begin(MCP4725_ADDRESS);
}

void loop() {
  modo_ref = Menu_Teclado();
  switch (modo_ref){
    case 0: //No hace nada
      break;   
    case 1:
      v_ref= Tension;
      i_ref= Corriente;
     break;
    case 5:
      v_ref= v_encoder;
      i_ref = i_encoder;
      break;
    case 2:
      i_ref = Corriente;
      break;
    case 6:
      i_ref = i_encoder;
      break;
  }    
  Actualizar_Pantalla( v_ref, i_ref);
  encoder_1();
  encoder_2();

}