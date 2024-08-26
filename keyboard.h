#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <Key.h>
#include <Keypad.h>
#include <Arduino.h>

int SCREEN = 0; // Variable que indica la pantalla actual
int menuSelection = 1; // Variable que indica la selección actual del menú y ayuda a moverse
const float MAX_TENSION = 30.00;
const float MAX_CORRIENTE = 3.00;
float Tension = 0.00; float Corriente = 0.000;
float Tension2, Corriente2;
int Tiempo2;
int Tiempo = 0;
int TiempoTranscurrido = 0;
char tensString[5] = "    ";  char corrienteString[5] = "    "; // Almacenar los valores como una cadena de caracteres de 4 dígitos
bool edit_values = false; bool encoders = false;
int Mode=0; //0.Sin Modo; 1. Tensión; 2. Corriente; 3. Rampa Define el modo de funcionamiento
const byte ROWS = 4; const byte COLUMNS = 4;
char keys[ROWS][COLUMNS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8}; byte columnPins[COLUMNS] = {7, 6, 5,4}; Keypad teclado = Keypad(makeKeymap(keys), rowPins, columnPins, ROWS, COLUMNS);
char key;
int listValue[8] = {0}; // Lista para almacenar los valores de tensión y corriente
int listRamp[6] = {0}; //Almacena valores tension y tiempo 
int index=0; //Posicion en la lista
void Keyboard(int key); 
bool updateFloatValues(); void updateValues(int key); 
void updateRampValues(int key); bool updateRampFloatValues();

int Menu_Teclado(){
  key = teclado.getKey();
  if (key) {
    Serial.println(key);
    Keyboard(key);
    if (key == '#' && SCREEN!=0) {
    switch (menuSelection){
      case 0: 
        return Mode=0;
        //No hace nada 
        break;
      case 1:         //Limita tension y corriente.
        if(encoders==true){
          return Mode=5;
        } else{
          return Mode=1;
        }
        break;
      case 2:
        if(encoders==true){
          return Mode=6;
        } else{
          return Mode=2;
        }
        break;
      case 3: //Hace rampa
        if(encoders==true){
          return Mode=7;
        } else{
          return Mode=3;
        }
        break;
      }
  }
  return Mode;
  }
}

void Keyboard(int key) {
  if (edit_values == true) {
    if (key >= '0' && key <= '9') {
      if(menuSelection!=3){
      updateValues(key);
      if (index == 8) { // Si se han ingresado todos los valores
        edit_values = false; // Finalizar la edición de valores
        if(updateFloatValues()){
          switch (menuSelection){
            case 1:
              SCREEN=1;
              break;
            case 2:
              SCREEN=2;
              break;
          }
        }else{
          SCREEN=4;
        } // Actualizar los valores de tensión y corriente
      }
      } else {
        updateRampValues(key);
         if (index == 6) { // Si se han ingresado todos los valores
          edit_values = false; // Finalizar la edición de valores
          if(updateRampFloatValues()){
            SCREEN=3;
          } else{
            SCREEN=4;
          }
        }
        //Funcion para cargar el valor de tension y el tiempo en seg
      }
    }
  }
  if (key == 'A' && SCREEN != 0) {
    edit_values = true;
    SCREEN = 5; // Cambiar a la pantalla de visualización de valores
    switch (menuSelection) {
      case 1: 
      index=0;
      break;
      case 2:
      index=4;
      break;
      case 3:
      index=0;
    }
    memset(tensString, ' ', sizeof(tensString)); // Limpiar el arreglo de la tensión
    memset(corrienteString, ' ', sizeof(corrienteString)); // Limpiar el arreglo de la corriente
    memset(listValue, 0, sizeof(listValue)); // Reiniciar la lista de valores
  }
  if (key == 'B' && SCREEN == 0) {
    menuSelection++;
    if (menuSelection > 3) {
      menuSelection = 1;
    }
  }
  if (key == 'C') {
    SCREEN = menuSelection;
  }
  if (key == 'D') {
    if (SCREEN==0){
      Mode=0;
    }
    SCREEN = 0;
  }
  if (key == '*'){
    if(encoders==true){
      encoders=false;
    } else{
      encoders=true;
    }
  }
}

void updateValues(int key) {
  if (index < 4) { // Si todavía estamos ingresando los valores de tensión
    tensString[index] = key; // Almacenar el valor en el arreglo de la tensión
  } else { // Si ya hemos terminado de ingresar los valores de tensión
    corrienteString[index - 4] = key; // Almacenar el valor en el arreglo de la corriente
  }
  listValue[index] = key - '0'; // Almacenar el valor en la lista
  index++;
}

bool updateFloatValues() {
  // Calcular los valores de tensión y corriente a partir de los valores ingresados
  Tension2 = (listValue[0] * 10 + listValue[1]) + (listValue[2] * 0.1) + (listValue[3] * 0.01);
  Corriente2 = (listValue[4] + (listValue[5] * 0.1) + (listValue[6] * 0.01) + (listValue[7] * 0.001));
  if (Tension2<=MAX_TENSION && Corriente2<=MAX_CORRIENTE){
    Tension=Tension2;
    Corriente=Corriente2;
    return true;
  }
  return false;
}

void updateRampValues(int key) {
  if (index < 4) { // Si todavía estamos ingresando los valores de tensión
    tensString[index] = key; // Almacenar el valor en el arreglo de la tensión
    listRamp[index] = key - '0'; // Almacenar el valor en la lista de rampa
  } else if (index >= 4 && index < 6) { // Si estamos ingresando los valores de tiempo
    corrienteString[index - 4] = key; // Almacenar el valor en el arreglo de tiempo
    listRamp[index] = key - '0'; // Almacenar el valor en la lista de rampa
  }
  index++;
}

bool updateRampFloatValues() {
  // Calcular los valores de tensión y tiempo a partir de los valores ingresados en listRamp
  Tension2 = (listRamp[0] * 10 + listRamp[1]) + (listRamp[2] * 0.1) + (listRamp[3] * 0.01);
  Tiempo2 = (listRamp[4] * 10 + listRamp[5]);
  
  // Verificar si los valores están en rango
  if (Tension2 <= MAX_TENSION && Tiempo2 >= 0) {
    Tension = Tension2;
    Tiempo = Tiempo2;
    Serial.print("TiempoCargado: ");
    Serial.println(Tiempo);
    return true; // Los valores están en rango y se cargaron a las variables
  }
  return false; // Al menos uno de los valores está fuera de rango
}

#endif