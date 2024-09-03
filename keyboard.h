#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <Key.h>
#include <Keypad.h>
#include <Arduino.h>

int screen = 0; // Variable que indica la pantalla actual
int menuSelection = 1; // Variable que indica la selección actual del menú y ayuda a moverse
const float MAX_TENSION = 30.00;
const float MAX_CORRIENTE = 3.00;
float t_teclado = 0.00; float i_teclado = 0.000;
float t_teclado_aux, i_teclado_aux;
int tiempo2;
int tiempo = 0;
int TiempoTranscurrido = 0;
char tensString[5] = "    ";  char corrienteString[5] = "    "; // Almacenar los valores como una cadena de caracteres de 4 dígitos
bool edit_values = false; bool encoders = false;
int modo=0; //0.Sin Modo; 1. Tensión; 2. i_teclado; 3. Rampa Define el modo de funcionamiento
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
    if (key == '#' && screen!=0) {
    switch (menuSelection){
      case 0: 
        return modo=0;
        //No hace nada 
        break;
      case 1:         //Limita tension y corriente.
        if(encoders==true){
          return modo=5;
        } else{
          return modo=1;
        }
        break;
      case 2:
        if(encoders==true){
          return modo=6;
        } else{
          return modo=2;
        }
        break;
      case 3: //Hace rampa
        if(encoders==true){
          return modo=7;
        } else{
          return modo=3;
        }
        break;
      }
  }
  return modo;
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
              screen=1;
              break;
            case 2:
              screen=2;
              break;
          }
        }else{
          screen=4;
        } // Actualizar los valores de tensión y corriente
      }
      } else {
        updateRampValues(key);
         if (index == 6) { // Si se han ingresado todos los valores
          edit_values = false; // Finalizar la edición de valores
          if(updateRampFloatValues()){
            screen=3;
          } else{
            screen=4;
          }
        }
        //Funcion para cargar el valor de tension y el tiempo en seg
      }
    }
  }
  if (key == 'A' && screen != 0) {
    edit_values = true;
    screen = 5; // Cambiar a la pantalla de visualización de valores
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
  if (key == 'B' && screen == 0) {
    menuSelection++;
    if (menuSelection > 3) {
      menuSelection = 1;
    }
  }
  if (key == 'C') {
    screen = menuSelection;
  }
  if (key == 'D') {
    if (screen==0){
      modo=0;
    }
    screen = 0;
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
  t_teclado_aux = (listValue[0] * 10 + listValue[1]) + (listValue[2] * 0.1) + (listValue[3] * 0.01);
  i_teclado_aux = (listValue[4] + (listValue[5] * 0.1) + (listValue[6] * 0.01) + (listValue[7] * 0.001));
  if (t_teclado_aux<=MAX_TENSION && i_teclado_aux<=MAX_CORRIENTE){
    t_teclado=t_teclado_aux;
    i_teclado=i_teclado_aux;
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
  t_teclado_aux = (listRamp[0] * 10 + listRamp[1]) + (listRamp[2] * 0.1) + (listRamp[3] * 0.01);
  tiempo2 = (listRamp[4] * 10 + listRamp[5]);
  
  // Verificar si los valores están en rango
  if (t_teclado_aux <= MAX_TENSION && tiempo2 >= 0) {
    t_teclado = t_teclado_aux;
    tiempo = tiempo2;
    Serial.print("TiempoCargado: ");
    Serial.println(tiempo);
    return true; // Los valores están en rango y se cargaron a las variables
  }
  return false; // Al menos uno de los valores está fuera de rango
}

#endif