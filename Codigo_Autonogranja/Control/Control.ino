#include "max6675.h"

// Constantes y variables del PID 1
float A_1=0, B_1=0, C_1=0, Ts_1=0.25, kp_1=228.874818, ti_1=1.249894, td_1=0.0624735;
float ek_1=0, ek_11=0, ek_21=0, Ack_1=0, Ack_11=0;
float referencia_1=100, sensor_1=0, sensor_11=0;
int senalpwm_1=19;
float w_1=0,fn_1=0,wn_1=0.00168,pi=3.1416;
char valor_ref = '0';
int datos[3];
float sensor_f_1=0, sensor_f_11=0, a_1=0, b_1=0;

// Constantes y variables del PID 2
float A_2=0, B_2=0, C_2=0, Ts_2=0.25, kp_2=0.54, ti_2=0.2508355, td_2=0.062708875;
float ek_2=0, ek_12=0, ek_22=0, Ack_2=0, Ack_12=0;
float w_2=0,fn_2=0,wn_2=0.1;
float referencia_2=100, sensor_2=0, sensor_12=0;
int actuador_2=26;
int briego=25;
float sensor_f_2=0, sensor_f_12=0, a_2=0, b_2=0;

// Pines compartidos
int mortor1=32, mortor2=33;
int thermoDO=4, thermoCS=5, thermoCLK=18;
const int Trigger=14, Echo=27;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
float filtro=0; // Variable declarada
String ref1;
String valor_recibido; // Valor inicial
float valor_referencia = 30 ;
// Función para el núcleo 0 (PID 1)
void loop0(void* pvParameters) {
  while(1) {
    // Código del PID 1
     fn_1 = wn_1/(2*pi);
  // w
    w_1 = 2*pi*fn_1;
    sensor_1 = thermocouple.readCelsius();
    sensor_f_1 = pow(2.71828, -w_1*Ts_1)*sensor_f_11 + (1-pow(2.71828, -w_1*Ts_1))*sensor_11;
    a_1 = pow(2.71828, -w_1*Ts_1);
    b_1 = 1-pow(2.71828, -w_1*Ts_1);
    referencia_1 = valor_referencia;
    
    ek_1 = referencia_1 - sensor_1;
    Ack_1 = Ack_11 + A_1*ek_1 + B_1*ek_11 + C_1*ek_21;
    if (Ack_1 >= 200) Ack_1 = 200;
    if (Ack_1 < 0) Ack_1 = 0;
    analogWrite(senalpwm_1, Ack_1);
    Ack_11 = Ack_1;
    sensor_11 = sensor_1;
    sensor_f_11 = sensor_f_1;
    ek_11 = ek_1;
    ek_21 = ek_11;
    datos[0] = Ack_2;
    datos[1] = sensor_2;
    datos[2] = Ack_1;
    datos[3] = sensor_1;
    Serial.println(String(datos[0])+","+String(datos[1])+","+String(datos[2])+","+String(datos[3]));
    
    delay(1000);
    vTaskDelay(100);
  }
}

// Función para el núcleo 1 (PID 2)
void loop1(void* pvParameters) {
  while(1) {
    // Código del PID 2
    long t, d;
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trigger, LOW);
    t = pulseIn(Echo, HIGH);
    d = t/58.8235294;
    if (d >= 11) d = 11;
    if (d < 2) d = 2;
    filtro = (((11-d)*100)/9);
    fn_2 = wn_2/(2*pi);
  // w
    w_2 = 2*pi*fn_2;
    sensor_2 = filtro;
    sensor_f_2 = pow(2.72, -w_2*Ts_2)*sensor_f_12 + (1-pow(2.72, -w_2*Ts_2))*sensor_12;
    a_2 = pow(2.72, -w_2*Ts_2);
    b_2 = 1-pow(2.72, -w_2*Ts_2);
    referencia_2 = 50;
    ek_2 = referencia_2 - sensor_2;
    Ack_2 = Ack_12 + A_2*ek_2 + B_2*ek_12 + C_2*ek_22;
    if (Ack_2 >= 255) Ack_2 = 255;
    if (Ack_2 < 0) Ack_2 = 0;
    analogWrite(actuador_2, Ack_2);
    Ack_12 = Ack_2;
    sensor_12 = sensor_2;
    sensor_f_12 = sensor_f_2;
    ek_12 = ek_2;
    ek_22 = ek_12;
    
    delay(1000);

    vTaskDelay(100);
  }
}
void loop_serial(void* pvParameters) {
  while(1) {
    if (Serial.available() > 0) {
      valor_recibido = Serial.readStringUntil('\n');
      if (valor_recibido == "R") {
        analogWrite(briego,255);
        delay(5000);
        valor_recibido = "0";
        analogWrite(briego,0);
      }
      else {
        ref1=valor_recibido;
        Serial.println(ref1);
        int commaindex= ref1.indexOf(',');
        String var1= ref1.substring(0,commaindex);
        String var2= ref1.substring(commaindex+1);

        valor_referencia= var2.toFloat();

      }
       
      }
    }
    delay(100); // Pequeña pausa para no saturar el procesador
  }
    


void setup() {
  // Cálculo de constantes
  A_1 = kp_1 + (kp_1*Ts_1/(2*ti_1)) + (kp_1*td_1/Ts_1);
  B_1 = -kp_1 + (kp_1*Ts_1/(2*ti_1)) - (2*kp_1*td_1/Ts_1);
  C_1 = kp_1*td_1/Ts_1;
  A_2 = kp_2 + (kp_2*Ts_2/(2*ti_2)) + (kp_2*td_2/Ts_2);
  B_2 = -kp_2 + (kp_2*Ts_2/(2*ti_2)) - (2*kp_2*td_2/Ts_2);
  C_2 = kp_2*td_2/Ts_2;

  // Configuración de pines
  pinMode(senalpwm_1, OUTPUT);
  pinMode(actuador_2, OUTPUT);
  pinMode(briego, OUTPUT);
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(mortor1, OUTPUT);
  pinMode(mortor2, OUTPUT);
  digitalWrite(mortor1, HIGH);
  digitalWrite(mortor2, LOW);

  // Monitor serie
  Serial.begin(115200);

  // Crear tareas para los núcleos
  xTaskCreatePinnedToCore(loop0, "Tarea0", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop1, "Tarea1", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(loop_serial, "TareaSerial", 10000, NULL, 1, NULL, 1);
}

void loop() {
  
}
