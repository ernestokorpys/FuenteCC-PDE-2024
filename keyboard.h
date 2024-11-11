#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <Key.h>
#include <Keypad.h>
#include <Arduino.h>
#define MCP4661_ADDRESS 0x28    //Potenciometro
int screen = 0;         // Variable que indica la pantalla actual
int menuSelection = 1;  // Variable que indica la selección actual del menú y ayuda a moverse
float i_teclado_aux;
int tiempo = 5;
char tensString[5] = "    ";
char corrienteString[5] = "    ";  // Almacenar los valores como una cadena de caracteres de 4 dígitos
bool edit_values = false;
bool encoders = false;
int modo = 0;  //0.Sin Modo; 1. Tensión; 2. i_teclado; 3. Rampa Define el modo de funcionamiento
const byte ROWS = 4;
const byte COLUMNS = 4;
char keys[ROWS][COLUMNS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 11, 10, 9, 8 };
byte columnPins[COLUMNS] = { 7, 6, 5, 4 };
Keypad teclado = Keypad(makeKeymap(keys), rowPins, columnPins, ROWS, COLUMNS);
char key;
int listValue[8] = { 0 };  // Lista para almacenar los valores de tensión y corriente
int listRamp[6] = { 0 };   //Almacena valores tension y tiempo
int index = 0;             //Posicion en la lista
float v_final;
float i_proteccion = 3.0;       // Variable para almacenar el valor de corriente máxima de protección
int corrienteMaxString[4] = {0}; // Array para almacenar cada dígito de corriente máxima
int indexCorrienteMax = 0;       // Índice para ingresar los dígitos de corriente máxima
extern volatile bool sobrecorriente;

void Keyboard(int key);
bool updateFloatValues();
void updateValues(int key);
void updateRampValues(int key);
bool updateRampFloatValues();
bool updateCurrentProtection();
int mapCurrentToPotValue(float desiredCurrent);
void setPotentiometer(byte channel, byte value);

int Menu_Teclado() {
  key = teclado.getKey();
  if (key) {
    Serial.println(key);
    Keyboard(key);
    if (key == '#' && screen != 0) {
      sobrecorriente = false;
      digitalWrite(2, HIGH);
      switch (menuSelection) {
        case 1:  //Limita tension y corriente.
          return modo = 1;
          break;
        case 2:  //Modo corriente
          return modo = 2;
          break;
        case 3:  //Hace rampa
          rampa_encendida = true;
          return modo = 3;
          break;
      }
    }
    return modo;
  }
}

void Keyboard(int key) {
  if (edit_values == true) {
    if (key >= '0' && key <= '9') {
      if (menuSelection == 6) { // Corriente máxima
        if (indexCorrienteMax < 4) {
          corrienteMaxString[indexCorrienteMax++] = key - '0';
        }
        if (indexCorrienteMax == 4) { // Si se han ingresado los 4 valores de corriente máxima
          indexCorrienteMax = 0;
          edit_values = false;
          if (updateCurrentProtection()) {
            Serial.print("Actualizado");
          } else Serial.print("Fuera de rango");

        }
      }

      else if (menuSelection != 3) {
        updateValues(key);
        if (index == 8) {       // Si se han ingresado todos los valores
          edit_values = false;  // Finalizar la edición de valores
          if (updateFloatValues()) {
            switch (menuSelection) {
              case 1:
                screen = 1;
                break;
              case 2:
                screen = 2;
                break;
            }
          } else {
            screen = 4;
          }  // Actualizar los valores de tensión y corriente
        }
      } else {
        updateRampValues(key);
        if (index == 6) {       // Si se han ingresado todos los valores
          edit_values = false;  // Finalizar la edición de valores
          if (updateRampFloatValues()) {
            screen = 3;
          } else {
            screen = 4;
          }
        }
        //Funcion para cargar el valor de tension y el tiempo en seg
      }
    }
  }

  if (key == 'A' && screen == 0 && edit_values == false) { //seteo valor maximo de corriente.
    edit_values = true;
    screen = 6;
    menuSelection = 6;
  }
  else if (key == 'A' && screen != 0) {
    edit_values = true;
    screen = 5;  // Cambiar a la pantalla de visualización de valores
    switch (menuSelection) {
      case 1:
        index = 0;
        break;
      case 2:
        index = 4;
        break;
      case 3:
        index = 0;
    }
    memset(tensString, ' ', sizeof(tensString));            // Limpiar el arreglo de la tensión
    memset(corrienteString, ' ', sizeof(corrienteString));  // Limpiar el arreglo de la corriente
    memset(listValue, 0, sizeof(listValue));                // Reiniciar la lista de valores
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
    if (screen == 0) {
      modo = 0;
      ui = 0;
      ui_m1 = 0;
      reset_variables();
    }
    screen = 0;
  }
  if (key == '*') {
    if (encoders == true) {
      encoders = false;
    } else {
      encoders = true;
    }
  }
}

bool updateCurrentProtection() {   // Convertir el valor de cadena a un valor flotante de corriente
  float value = corrienteMaxString[0] * 1.0 + corrienteMaxString[1] * 0.1 + corrienteMaxString[2] * 0.01 + corrienteMaxString[3] * 0.001;
  if (value <= 3.0 && value > 0.3) {   // Verificar si el valor está en el rango permitido
    screen = 0;           // Ir a la pantalla de inicio
    i_proteccion = value;
    menuSelection = 1;
    int potValue = mapCurrentToPotValue(i_proteccion);
    setPotentiometer(0, potValue);
    return true;          // Retorna el nuevo valor válido
  } else {
    screen = 4;            // Mostrar mensaje de error
    menuSelection = 6;
    return false;   // Retorna el valor anterior sin cambios
  }
}

void updateValues(int key) {
  if (index < 4) {                     // Si todavía estamos ingresando los valores de tensión
    tensString[index] = key;           // Almacenar el valor en el arreglo de la tensión
  } else {                             // Si ya hemos terminado de ingresar los valores de tensión
    corrienteString[index - 4] = key;  // Almacenar el valor en el arreglo de la corriente
  }
  listValue[index] = key - '0';  // Almacenar el valor en la lista
  index++;
}

bool updateFloatValues() {
  // Calcular los valores de tensión y corriente a partir de los valores ingresados
  float t_teclado_aux = (listValue[0] * 10 + listValue[1]) + (listValue[2] * 0.1) + (listValue[3] * 0.01);
  float i_teclado_aux = (listValue[4] + (listValue[5] * 0.1) + (listValue[6] * 0.01) + (listValue[7] * 0.001));
  if ((t_teclado_aux <= 30 & t_teclado_aux >= 4) && (i_teclado_aux <= 3 & i_teclado_aux >= 0.3)) {
    v_ref = t_teclado_aux;
    i_max = i_teclado_aux;
    return true;
  }
  return false;
}

void updateRampValues(int key) {
  if (index < 4) {                       // Si todavía estamos ingresando los valores de tensión
    tensString[index] = key;             // Almacenar el valor en el arreglo de la tensión
    listRamp[index] = key - '0';         // Almacenar el valor en la lista de rampa
  } else if (index >= 4 && index < 6) {  // Si estamos ingresando los valores de tiempo
    corrienteString[index - 4] = key;    // Almacenar el valor en el arreglo de tiempo
    listRamp[index] = key - '0';         // Almacenar el valor en la lista de rampa
  }
  index++;
}

bool updateRampFloatValues() {
  // Calcular los valores de tensión y tiempo a partir de los valores ingresados en listRamp
  float t_teclado_aux = (listRamp[0] * 10 + listRamp[1]) + (listRamp[2] * 0.1) + (listRamp[3] * 0.01);
  int tiempo2 = (listRamp[4] * 10 + listRamp[5]);

  // Verificar si los valores están en rango
  if (t_teclado_aux <= 30 && tiempo2 >= 0) {
    v_ref = t_teclado_aux;
    v_final = t_teclado_aux;
    tiempo = tiempo2;
    return true;  // Los valores están en rango y se cargaron a las variables
  }
  return false;  // Al menos uno de los valores está fuera de rango
}

int mapCurrentToPotValue(float desiredCurrent) {
  int currentMilliAmps = desiredCurrent * 1000;           // Convertir corriente mA
  int potValue = map(currentMilliAmps, 1, 3000, 255, 1); // Mapear de 1 (0.001 A) a 3000 (3 A) al rango de 255 a 1
  potValue = constrain(potValue, 1, 255);               // Limitar el valor al rango de 1 a 255
  return potValue;
}

void setPotentiometer(byte channel, byte value) {
  Wire.beginTransmission(MCP4661_ADDRESS);
  Wire.write((channel == 0) ? 0x00 : 0x10);  // Selecciona el canal (0 o 1)
  Wire.write(value);                         // Configura el valor del potenciómetro
  Wire.endTransmission();
}


#endif
