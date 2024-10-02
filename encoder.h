#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "controller.h"
//pines encoder 1 --------------------------------------------------------------------------------------------------
const int encoderCLK_2 = A1;  
const int encoderDT_2 = A2;   
const int encoderSW_2 = A3;   
const int encoderCLK_1 = 12;  
const int encoderDT_1 = 13;   
const int encoderSW_1 = A0;   

int currentStateCLK_1;
int lastStateCLK_1;
bool buttonPressed_1 = false;

int currentStateCLK_2;
int lastStateCLK_2;
bool buttonPressed_2 = false;

void pinoutEncoder(){ // Configuración de pines encoder
  pinMode(encoderCLK_1, INPUT);
  pinMode(encoderDT_1, INPUT);
  pinMode(encoderSW_1, INPUT_PULLUP);
  lastStateCLK_1 = digitalRead(encoderCLK_1);     // Leer el estado inicial del CLK
  
  pinMode(encoderCLK_2, INPUT);
  pinMode(encoderDT_2, INPUT);
  pinMode(encoderSW_2, INPUT_PULLUP);
  lastStateCLK_2 = digitalRead(encoderCLK_2); // Leer el estado inicial del CLK
}

void encoder_1() {
  currentStateCLK_1 = digitalRead(encoderCLK_1); // Leer el estado del CLK
  if (currentStateCLK_1 != lastStateCLK_1 && currentStateCLK_1 == LOW) { // Si el estado ha cambiado
    if (digitalRead(encoderDT_1) != currentStateCLK_1) {  // Si DT es diferente a CLK significa que el encoder está girando en sentido horario
      if (v_ref < 30) {  // Limita el incremento a 30
        v_ref += 0.1;
        Serial.print("Giro aumento");
      }
    } else {
      if (v_ref > 0) {  // Limita la disminución a 0
        v_ref -= 0.1;
        Serial.print("Giro decremento");
      }
    }
    Serial.print("Valor: ");
    Serial.println(v_ref);
  }
  lastStateCLK_1 = currentStateCLK_1;
  
  if (digitalRead(encoderSW_1) == LOW) {   // Leer el estado del botón
    if (!buttonPressed_1) {
      buttonPressed_1 = true;
      Serial.println("Boton Pulsado");
    }
  } else {
    if (buttonPressed_1) {
      buttonPressed_1 = false;
      Serial.println("Boton Liberado");
    }
  }
}

void encoder_2() {
  currentStateCLK_2 = digitalRead(encoderCLK_2); // Leer el estado del CLK
  if (currentStateCLK_2 != lastStateCLK_2 && currentStateCLK_2 == LOW) { // Si el estado ha cambiado
    if (digitalRead(encoderDT_2) != currentStateCLK_2) {  // Si DT es diferente a CLK significa que el encoder está girando en sentido horario
      if (i_max < 3) {  // Limita el incremento a 3
        i_max += 0.1;
        Serial.print("Giro aumento");
      }
    } else {
      if (i_max > 0) {  // Limita la disminución a 0
        i_max -= 0.1;
        Serial.print("Giro decremento");
      }
    }
    Serial.print("Valor i_encoder: ");
    Serial.println(i_max);
  }
  lastStateCLK_2 = currentStateCLK_2;
  
  if (digitalRead(encoderSW_2) == LOW) { // Leer el estado del botón
    if (!buttonPressed_2) {
      buttonPressed_2 = true;
      Serial.println("Botón 2 Pulsado");
    }
  } else {
    if (buttonPressed_2) {
      buttonPressed_2 = false;
      Serial.println("Botón 2 Liberado");
    }
  }
}

#endif