#ifndef CONTROLLER_H
#define CONTROLLER_H
//--------------------------------------------------------------------------------------------------------------------------------
//    Constantes de control
//--------------------------------------------------------------------------------------------------------------------------------
bool estado=true; //0 1_Vacio 2_Carga
float ki1, ki2, ki3, kv1, kv2, kv3;
float  ei, ei_m1, ei_m2, ei_m3, ui , ui_m1,  ev , ev_m1 , ev_m2 , ev_m3, uv, uv_m1;
float v_ref=15;
float H_v = 35 / 5;
float H_i = 0.6;
float i_max=1.5;
int caso;
unsigned long lastExecutionTime = 0; // Variable para almacenar el tiempo de la última ejecución
const unsigned long executionInterval = 100; // Intervalo de 5 segundos en milisegundos

float valores[6][12] = {
        //sin carga                                     con carga
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //5 ajustar para sin carga
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //10
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //15
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //20
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,      0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001}, //25 //Espectacular estas constantes
        {0.5, 0.45, 0.01, 0.01, -0.0095, 0.00001,     0.1, 0.01, 0.001, 0.011, -0.0102, 0.00001}  //>25
};
float i_min_range[6]={
  0.06,   0.06,   0.06,   0.06,   0.06,   0.06,
  };
float i_min; bool flag;
float sobrepaso[6]={
  1.2, 1.2,  1.3,  1.14, 1.15, 1.15
};
void reset_variables();
void control_sin_carga(float v_act, float i_act);
void lazo_control(float v_act, float i_act);

void algoritmo_control(float v_act, float i_act){
  if (millis() - lastExecutionTime >= executionInterval) {
    if (i_act > i_min && flag==true) { // || i_act >= 0.05
      estado = false; //Lazo con carga
      flag = false;
      reset_variables();
      lastExecutionTime = millis(); // Actualizar el tiempo de la última ejecución
    }
    if (v_act >= v_ref * 1.2 && i_act <= i_min) { 
      estado = true; //Lazo sin carga
      flag = true;
      //reset_variables();
      lastExecutionTime = millis(); // Actualizar el tiempo de la última ejecución
    }
    if(v_act <= v_ref * 0.2 && flag==true){ 
      estado = false; //Lazo con carga
      flag = false;
      reset_variables();
      lastExecutionTime = millis(); // Actualizar el tiempo de la última ejecución
    }
  }
  if (estado){
    control_sin_carga(v_act, i_act);
    //Serial.println("Lazo sin carga");
  } else {
    lazo_control(v_act, i_act);
    //Serial.println("Lazo con carga");
  }
}

void reset_variables() {
  Serial.println("Reset");
  H_i = 0.6;
  ei = 0; ei_m1 = 0; ei_m2 = 0; ei_m3 = 0;
  ui = 0; ui_m1 = 0;
  H_v = 35 / 5;
  ev = 0; ev_m1 = 0; ev_m2 = 0; ev_m3 = 0;
  uv = 0; uv_m1 = 0;
}

void control_sin_carga(float v_act, float i_act) {
  //Lazo de tensión:
  ev = v_ref - v_act;
  uv = kv1 * ev + kv2 * ev_m1 + kv3 * ev_m2 + uv_m1;
  if (uv < 0.3) {
    uv = 0.3 - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  if (uv > 1.5) {
    uv = 1.5 - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  ev_m3 = ev_m2;
  ev_m2 = ev_m1;
  ev_m1 = ev;
  uv_m1 = uv;
  ei=0, ei_m1=0, ei_m2=0, ei_m3=0;
  ui=uv;
  ui_m1=ui;
}

void lazo_control(float v_act, float i_act) {
  //Lazo de tensión:
  ev = v_ref - v_act;
  uv = kv1 * ev + kv2 * ev_m1 + kv3 * ev_m2 + uv_m1;
  if (uv < 0) {
    uv = 0 - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  if (uv > i_max) {
    uv = i_max - kv1 * ev - kv2 * ev_m1 - kv3 * ev_m2;
  }
  ev_m3 = ev_m2;
  ev_m2 = ev_m1;
  ev_m1 = ev;
  uv_m1 = uv;
  //Lazo de corriente:
  ei = uv - i_act;
  ui = ki1 * ei + ki2 * ei_m1 + ki3 * ei_m2 + ui_m1;
  if (ui > 1.5) {
    ui = 1.5 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  if (ui < 0.3) {
    ui = 0.3 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
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
  i_min=i_min_range[caso];
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