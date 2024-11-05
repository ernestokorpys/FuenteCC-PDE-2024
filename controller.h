#ifndef CONTROLLER_H
#define CONTROLLER_H
//--------------------------------------------------------------------------------------------------------------------------------
//    Constantes de control
//--------------------------------------------------------------------------------------------------------------------------------

float ki1 = 0.5, ki2 = 0.45, ki3 = 0.0001, kv1 = 0.01, kv2 = 0.01, kv3 = 0.00001;
float ei = 0, ei_m1 = 0, ei_m2 = 0, ui = 0, ui_m1 = 0, ev = 0, ev_m1 = 0, ev_m2 = 0, ev_m3 = 0, uv = 0, uv_m1 = 0;
float H_v = 7;    //Del resultado 35/5
float H_i = 0.6;  //Del resultado 3A/5V
float v_ref = 0;
bool vacio = true;
float i_max = 0.1;
int cont_pasos = 0;
bool rampa_encendida = false;

unsigned long rampa_tiempo_act = 0;
unsigned long rampa_tiempo_ant = 0;
float i_min = 0.06;
bool flag_condicion_vacio = false;
bool estado = true;  //True:Sin Carga   False:Con Carga
void reset_variables();
void control_sin_carga(float v_act, float i_act);
void lazo_control(float v_act, float i_act);

void algoritmo_control_tension(float v_act, float i_act) {

  if (i_act <= 0.150) {
    kv1 = 0.01, kv2 = -0.0095, kv3 = 0.00001;
    vacio = true;
  } else if (i_act > 0.250) {
    vacio = false;
    if (v_act < 10) {
      kv1 = 0.09, kv2 = -0.087, kv3 = 0.0001;
      ki1 = 0.1, ki2 = 0.01, ki3 = 0.0001;
    } else if (v_act < 20) {
      kv1 = 0.08, kv2 = -0.077, kv3 = 0.0001;
      ki1 = 0.1, ki2 = 0.01, ki3 = 0.0001;
    } else if (v_act < 30) {
      kv1 = 0.07, kv2 = -0.067, kv3 = 0.0001;
      ki1 = 0.1, ki2 = 0.01, ki3 = 0.0001;
    }
  }
  if (rampa_encendida) {
    vacio = false;
    kv1 = 0.09, kv2 = -0.087, kv3 = 0.0001;
    ki1 = 0.1, ki2 = 0.01, ki3 = 0.0001;
  }
  /*
    // Zona1 <10V <150mA
    if (v_act < 10 && i_act <= 0.150) {
    kv1 = 0.01, kv2 = -0.0095, kv3 = 0.00001;
    vacio = true;
    }
    // Zona2 <10V >250mA
    else if (v_act < 10 && i_act > 0.250) {
    kv1 = 0.01, kv2 = -0.004, kv3 = 0.0001;
    ki1 = 0.1, ki2 = 0.01, ki3 = 0.0001;
    vacio = false;
    }
    // Zona3 <20V <150mA
    else if (v_act < 20 && i_act <= 0.150) {
    kv1 = 0.01, kv2 = -0.0095, kv3 = 0.00001;
    vacio=true;
    }/*
    // Zona4 >20V >250mA
    else if (v_act < 20 && i_act > 0.250) {
    kv1 = 0.01, kv2 = -0.004, kv3 = 0.0001;
    ki1 = 0.1, ki2 = 0.01, ki3 = 0.0001;
    vacio=false;
    }/*
    // Zona5 <30V <300mA
    else if (v_act < 30 && i_act <= 0.3) {
    kv1 = 0.01, kv2 = 0.01, kv3 = 0.000001;
    ki1 = 0.5, ki2 = 0.45, ki3 = 0.01;
    }
    // Zona6 >30V >300mA
    else if (v_act < 30 && i_act > 0.3) {
    kv1 = 0.01, kv2 = 0.01, kv3 = 0.000001;
    ki1 = 0.5, ki2 = 0.45, ki3 = 0.01;
    }*/
  Serial.println(vacio);
  lazo_control(v_act, i_act);
}
void algoritmo_control_corriente(float v_act, float i_act) {
  // Lazo de corriente:
  ki1 = 0.01;
  ki2 = 0.001;
  ki3 = 0.0001;
  ei = i_max - i_act;
  ui = ki1 * ei + ki2 * ei_m1 + ki3 * ei_m2 + ui_m1;
  // Algoritmo Anti-Windup (Para evitar que se dispare demasiado la tensión sobre la base del transistor)
  if (ui > 1.8) {
    ui = 1.8 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  if (ui < 0.3) {
    ui = 0.3 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
  }
  // Actualización de las variables del control.
  ei_m2 = ei_m1;
  ei_m1 = ei;
  ui_m1 = ui;
}


void algoritmo_control_rampa(float v_act, float i_act, float v_max, float tiempo_rampa) {  // 20ms demora el bucle principal +-
  if (rampa_encendida) {
    int pasos_final = tiempo_rampa * 50;
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
  ui =0;
  ui_m1 = 0;
  ev = 0;
  ev_m1 = 0;
  ev_m2 = 0;
  ev_m3 = 0;
  uv = 0;  //salida lazo tension / entrada lazo corriente
  uv_m1 = 0;
}
/*
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
*/
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
  if (vacio == false) {
    // Lazo de corriente:
    ei = uv - i_act;
    ui = ki1 * ei + ki2 * ei_m1 + ki3 * ei_m2 + ui_m1;
    // Algoritmo Anti-Windup (Para evitar que se dispare demasiado la tensión sobre la base del transistor)
    if (ui > 1.8) {
      ui = 1.8 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
    }
    if (ui < 0.3) {
      ui = 0.3 - ki1 * ei - ki2 * ei_m1 - ki3 * ei_m2;
    }
    // Actualización de las variables del control.
    ei_m2 = ei_m1;
    ei_m1 = ei;
    ui_m1 = ui;
  } else {
    ui = uv;
    ui_m1 = ui;
    ei_m2 = 0;
    ei_m1 = 0;
  }

}
#endif
