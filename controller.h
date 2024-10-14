#ifndef CONTROLLER_H
#define CONTROLLER_H
//--------------------------------------------------------------------------------------------------------------------------------
//    Constantes de control
//--------------------------------------------------------------------------------------------------------------------------------

float ki1, ki2, ki3, kv1, kv2, kv3;
float ei, ei_m1, ei_m2, ei_m3, ui, ui_m1, ev, ev_m1, ev_m2, ev_m3, uv, uv_m1;
float H_v = 35 / 5;
float H_i = 0.6;

float v_ref = 0;
float i_ref = 0.1;
float i_max = 0.1;
int cont_pasos = 0;
bool rampa_encendida = false;

int caso;

unsigned long rampa_tiempo_act = 0;
unsigned long rampa_tiempo_ant = 0;



const float valores[6][12] = {
        //sin carga                                     con carga
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //5 ajustar para sin carga
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //10
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //15
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //20
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,      0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //25 //Espectacular estas constantes
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.1, 0.01, 0.001, 0.011, -0.0102, 0.00001}  //>25
};
/*
float valores[6][12] = {
  //sin carga                                     con carga
  { 0.01, -0.0095, 0.00001, 0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001 },          //5 ajustar para sin carga
  { 0.01, -0.0095, 0.00001, 0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001 },          //10
 { 0.01, -0.0095, 0.00001, 0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001 },      //15
  { 0.01, -0.0095, 0.00001, 0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001 },          //20
  { 0.01, -0.0095, 0.00001, 0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001 },          //25 //Espectacular estas constantes
  { 0.01, -0.0095, 0.00001, 0.1, 0.01, 0.001, 0.011, -0.0102, 0.00001 }         //>25
};
*/
 //{ 0.01, -0.0095, 0.00001, 0.01, 0.001, 0.000, 0.00008, -0.0000795, 0.0001 },  //15
float i_min = 0.06;
bool flag=false;
bool estado = true;  //True:Sin Carga   False:Con Carga
void reset_variables();
void control_sin_carga(float v_act, float i_act);
void lazo_control(float v_act, float i_act);

void algoritmo_control_tension(float v_act, float i_act) {
  // Detección de conexión de carga
  // Analiza la corriente por un lado (si es superior a la minima)
  // Y analiza si la tensión no cayó al 20% del valor de referencia
  if (i_act > i_min && flag == true) {  // || i_act >= 0.05
    flag = false;
    reset_variables();
  }
  if (v_act >= v_ref * 1.2 && i_act <= i_min) {
    flag = true;
    //reset_variables();
  }
  if (v_act <= v_ref * 0.2 && flag == true) {

    flag = false;
    reset_variables();
  }

  if (flag) {
    control_sin_carga(v_act, i_act);
    Serial.println("Lazo sin carga");
  } else {
    lazo_control(v_act, i_act);
    Serial.println("Lazo con carga");
  }
}
void algoritmo_control_corriente(float v_act, float i_act) {
  // Lazo de corriente:
  ki1 = 0.01;
  ki2 = 0.001;
  ki3 = 0.0001;
  ei = i_max - i_act;
  ui = ki1 * ei + ki2 * ei_m1 + ki3 * ei_m2 + ui_m1;
  // Algoritmo Anti-Windup (Para evitar que se dispare demasiado la tensión sobre la base del transistor)
  if (ui > 1.5) {
    ui = 1.5 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  if (ui < 0.3) {
    ui = 0.3 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  // Actualización de las variables del control.
  ei_m3 = ei_m2;
  ei_m2 = ei_m1;
  ei_m1 = ei;
  ui_m1 = ui;
}


void algoritmo_control_rampa(float v_act, float i_act, float v_max, float tiempo_rampa) {  // 20ms demora el bucle principal +-
  if (rampa_encendida) {
    int pasos_final=tiempo_rampa*50;
    rampa_tiempo_act = millis();
    if ((rampa_tiempo_act - rampa_tiempo_ant) >= 20) {
      rampa_tiempo_ant = rampa_tiempo_act;
      cont_pasos++;
      v_ref = cont_pasos * v_max / pasos_final;
      if (cont_pasos >= pasos_final) {
        rampa_encendida = false;
        cont_pasos = 0;
      }
    }
  }
  algoritmo_control_tension(v_act, i_act);
}

void reset_variables() {
  Serial.println("Reset");
  ei = 0;
  ei_m1 = 0;
  ei_m2 = 0;
  ei_m3 = 0;
  //ui =0;
  //ui_m1 = 0;
  ev = 0;
  ev_m1 = 0;
  ev_m2 = 0;
  ev_m3 = 0;
  uv = 0;  //salida lazo tension / entrada lazo corriente
  uv_m1 = 0;
}

void control_sin_carga(float v_act, float i_act) {
  //Se realiza un lazo de control de la tensión unicamente.
  ev = v_ref - v_act;
  uv = kv1 * ev + kv2 * ev_m1 + kv3 * ev_m2 + uv_m1;
  // Algoritmo anti-windup
  if (uv < 0.3) {
    uv = 0.3 - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  if (uv > 1.5) {
    uv = 1.5 - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  //Actualización de las variables de control.
  ev_m3 = ev_m2;
  ev_m2 = ev_m1;
  ev_m1 = ev;
  uv_m1 = uv;
  //Actualizo las acciones de control del lazo de corriente
  ui = uv;
  ui_m1 = ui;
}

void lazo_control(float v_act, float i_act) {
  //Lazo de tensión:
  ev = v_ref - v_act;
  uv = kv1 * ev + kv2 * ev_m1 + kv3 * ev_m2 + uv_m1;
  // Algoritmo Anti-Windup. El valor máximo corresponde a la corriente máxima que se quiere extraer a la fuente.
  if (uv < 0) {
    uv = 0 - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  if (uv > i_max) {
    uv = i_max - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  // Actualizo los estados anteriores
  ev_m3 = ev_m2;
  ev_m2 = ev_m1;
  ev_m1 = ev;
  uv_m1 = uv;
  // Lazo de corriente:
  ei = uv - i_act;
  ui = ki1 * ei + ki2 * ei_m1 + ki3 * ei_m2 + ui_m1;
  // Algoritmo Anti-Windup (Para evitar que se dispare demasiado la tensión sobre la base del transistor)
  if (ui > 1.5) {
    ui = 1.5 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  if (ui < 0.3) {
    ui = 0.3 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  // Actualización de las variables del control.
  ei_m3 = ei_m2;
  ei_m2 = ei_m1;
  ei_m1 = ei;
  ui_m1 = ui;
}

void constantesControlador(){
  if (v_ref <= 5) {
  caso=0;
  } else if (v_ref <= 10) {
  caso=1;
  } else if (v_ref <= 15) {
  caso=2;
  } else if (v_ref <= 20) {
  caso=3;
  } else if (v_ref <= 25) {
  caso=4;
  } else{
    caso=5;
  };
  //i_min=i_min_range[caso];
  if (estado) {
  ki1 = valores[caso][0];
  ki2 = valores[caso][1];
  ki3 = valores[caso][2];
  kv1 = valores[caso][3];
  kv2 = valores[caso][4];
  kv3 = valores[caso][5];
  } else {
  ki1 = valores[caso][6];
  ki2 = valores[caso][7];
  ki3 = valores[caso][8];
  kv1 = valores[caso][9];
  kv2 = valores[caso][10];
  kv3 = valores[caso][11];
  }
}
#endif
