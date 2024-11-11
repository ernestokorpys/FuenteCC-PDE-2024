#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "encoder.h"
#include "keyboard.h"
extern float i_act;


// Definiciones de pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Objetos y variables globales
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void Actualizar_Pantalla(float voltage0, float voltage1);
void initDisplay();
void SCREEN0(); 
void SCREEN1(float voltage0, float voltage1); 
void SCREEN2(float voltage1);
void SCREEN3(float voltage0);
void SCREEN4();
void SCREEN5();
void SCREEN6();

void initDisplay() {
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {   // Inicialización de display OLED
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hola Ernesto");
  display.setCursor(0, 10);
  display.println("Bienvenido");
  display.display();
  delay(100);
}

void Actualizar_Pantalla(float voltage0, float voltage1){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  switch (screen) {
    case 0:
      SCREEN0();
      break;
    case 1:
      SCREEN1(voltage0, voltage1);
      break;
    case 2:
      SCREEN2(voltage1);
      break;
    case 3:
      SCREEN3(voltage0);
      break;
    case 4:
      SCREEN4();
      break;
    case 5:
      SCREEN5();
      break;
    case 6:
      SCREEN6();
      break;
  }
  display.display();
}

// Displays mostrando cosas
void SCREEN0() {
  // Mostrar las opciones del menú
  display.setCursor(0, 0);
  if (menuSelection == 1) {
    display.print("* ");
  }
  display.println("1. Tension");
  display.setCursor(0, 10);
  if (menuSelection == 2) {
    display.print("* ");
  }
  display.println("2. Corriente");
  display.setCursor(0, 20);
  if (menuSelection == 3) {
    display.print("* ");
  }
  display.print("3. Rampa  ");
  switch (modo) {
    case 0:
    display.print("OFF ");
    break;
    case 1:
    display.print("T ");
    break;
    case 2:
    display.print("C ");
    break;
    case 3:
    display.print("R ");
    break;
  }
  if (encoders == true) {
    display.print("Enco ");
  }
}
void SCREEN1(float voltage0, float voltage1) { //Pantalla Modo tesnión
  display.setCursor(0, 0);
  display.print("T. R:");
  display.print(voltage0, 3);
  display.print("D: ");
  display.print(v_ref);   
  display.setCursor(0, 10);
  display.print("C. R:");
  display.print(voltage1, 3);
  display.print("D: ");
  display.print(i_max);
  display.setCursor(0, 20);
  display.print("MODO TENSION");
}

void SCREEN2(float voltage1) {//Pantalla Modo corriente
  display.setCursor(0, 0);
  display.print("Real:");
  display.print(i_act, 3);
  display.setCursor(0, 10);
  display.print("Deseada: ");
  display.print(i_max); 
  display.setCursor(0, 20);
  display.print("MODO CORRIENTE");
}

void SCREEN3(float voltage0) { //Pantalla Modo rampa
  display.setCursor(0, 0);
  display.print("T. R:");
  display.print(voltage0, 3);
  display.print("  D: ");
  display.print(v_final); // Mostrar los dígitos ingresados para la tensión
  display.setCursor(0, 10);
  display.print("Tiempo. D:");
  display.print(tiempo);
  display.setCursor(0, 20);
  display.print("MODO RAMPA");
}

void SCREEN4() { //Pantalla Modo valor invalido cargado
  display.setCursor(0, 0);
  display.println("Valores Cargados");
  display.setCursor(0, 10);
  display.println("Invalidos");
  switch (menuSelection) {
      case 1: 
        screen=1;
        break;
      case 2:
        screen=2;
      break;
      case 6:
        screen=0;
        menuSelection = 1;
      break;
    }
}

void SCREEN5() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  switch (menuSelection) {
    case 1:
      display.print("TENSION: ");
      display.print(tensString[0]);   // Primer dígito
      display.print(tensString[1]);   // Segundo dígito
      display.print(".");             // Separador decimal
      display.print(tensString[2]);   // Tercer dígito (decimal)
      display.print(tensString[3]);   // Cuarto dígito (decimal)

      // Mostrar los dígitos ingresados para la corriente
      display.setCursor(0, 10);
      display.print("CORRIENTE: ");
      display.print(corrienteString[0]); // Primer dígito
      display.print(".");    
      display.print(corrienteString[1]); // Segundo dígito
      display.print(corrienteString[2]); // Tercer dígito (decimal)
      display.print(corrienteString[3]); // Cuarto dígito (decimal)

      display.setCursor(0, 20);
      display.print("EDITAR VALORES: ");
      break;
    case 2:
      display.print("Corriente MAX: ");
      for (int i = 0; i < 4; i++) {
        display.print(corrienteString[i]);
      }
      break;
    case 3:
      display.print("TENSION: ");
      display.print(tensString[0]); // Primer dígito
      display.print(tensString[1]); // Segundo dígito
      display.print(".");    
      display.print(tensString[2]); // Tercer dígito (decimal)
      display.print(tensString[3]); // Cuarto dígito (decimal)
      display.setCursor(0, 10);
      display.print("TIEMPO: ");
      display.print(corrienteString[0]); // Cuarto dígito (decimal)
      display.print(corrienteString[1]); // Cuarto dígito (decimal)
      display.print(" s");
      break;
  }
}

void SCREEN6() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Corriente MAX: ");
  for (int i = 0; i < indexCorrienteMax; i++) {
    display.print(corrienteMaxString[i]);
    if (i == 0) display.print("."); // Pone el punto decimal después del primer dígito
  }
  display.setCursor(0, 10);
  display.print("Valor Actual: ");
  display.print(i_proteccion);
  display.display();
}

#endif
