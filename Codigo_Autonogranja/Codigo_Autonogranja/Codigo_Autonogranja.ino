//TaskHandle_t Tarea0;
#include <ESP32_Servo.h>
#include <Stepper.h>

Servo myservo;
Servo myservo2;
Servo myservo3;
Servo myservo4;
const int stepsPerRevolution=2048;
String modo;
String posicion="0";
Stepper myStepper(stepsPerRevolution,19,4,27,25);
int anterior1=90;
int anterior2=0;
int anterior3=0;
int num1,num2,num3,num4;
int pos;

void setup() {
  Serial.begin(115200);
  pinMode(32,OUTPUT);
  digitalWrite(32,0);
  myservo.attach(18,300,2700);
  myservo2.attach(22,300,2700);
  myservo3.attach(23,300,2700);
  myservo4.attach(21,300,2700);
  myStepper.setSpeed(5);
  xTaskCreatePinnedToCore(loop0,"Tarea_0",1000,NULL,1,NULL,0);
  xTaskCreatePinnedToCore(loop1, "Tarea1", 10000, NULL, 1, NULL, 0);
  myservo.write(90);
  myservo2.write(0);
  myservo3.write(0);
}

void loop() {
  while (modo=="POSI"){
    if (posicion!=""){
      int commaIndex1 = posicion.indexOf(','); // Encuentra la primera coma
      int commaIndex2 = posicion.indexOf(',', commaIndex1 + 1); // Encuentra la segunda coma
      int commaIndex3 = posicion.indexOf(',', commaIndex2 + 1); // Encuentra la tercera coma
          
      String var1 = posicion.substring(0, commaIndex1); // Extrae el primer dato
      String var2 = posicion.substring(commaIndex1 + 1,commaIndex2); // Extrae el segundo dato
      String var3 = posicion.substring(commaIndex2 + 1,commaIndex3); // Extrae el tercer dato
      String var4 = posicion.substring(commaIndex3 + 1); // Extrae el cuarto dato
      
      num1 = var1.toInt();
      num2 = var2.toInt();
      num3 = var3.toInt();
      num4 = var4.toInt();

      mov_servo(1,num1);
      mov_servo(2,num2);    
      myStepper.step(num3);
      delay(100);
      mov_servo(3,num4);
      posicion = "";
    }
    else{
      int commaIndex1=0;
      int commaIndex2=0;
      int commaIndex3=0;
      String var1 ="";
      String var2 ="";
      String var3 ="";
      String var4 ="";
    }
  }
  while (modo=="AUTO"){
    delay(1000);
  }
}
void mov_servo(int numero_servo,int grados){
  //Mover servo articular 1
  if (numero_servo==1){
    if (anterior1<grados){
      for (pos=anterior1;pos<=grados;pos+=1){
        myservo.write(pos);
        delay(20);
      }
    }
    if (anterior1>num1){
      for (pos=anterior1;pos>=grados;pos-=1){
        myservo.write(pos);
        delay(20);
      }
    }
    anterior1=grados;
  }
  //Mover servo articular 2
  if (numero_servo==2){
    if (anterior2<grados){
      for (pos=anterior2;pos<=grados;pos+=1){
        myservo2.write(pos);
        delay(20);
      }
    }
    if (anterior2>grados){
      for (pos=anterior2;pos>=grados;pos-=1){
        myservo2.write(pos);
        delay(20);
      }
    }
    anterior2=grados;
  }
  //Mover servo articular 3
  if (numero_servo==3){
    if (anterior3<grados){
      for (int pos=anterior3;pos<=grados;pos+=1){
        myservo3.write(pos);
        delay(10);
      }
    }
    if (anterior3>grados){
      for (int pos=anterior3;pos>=grados;pos-=1){
        myservo3.write(pos);
        delay(10);
      }
    }
    anterior3=grados;
  }
  //Mover servo gripper
  if (numero_servo==4){
    if (grados==150){
      for (int pos=10;pos<=grados;pos+=1){
        myservo4.write(pos);
        delay(10);
      }
    }
    if (grados==10){
      for (int pos=150;pos>=grados;pos-=1){
        myservo4.write(pos);
        delay(10);
      }
    }
  }
}
void loop0(void *parameter){
  while(1==1){
    String dato=Serial.readStringUntil('\n');
    dato.trim();
    if (dato.equals("CE")){
       mov_servo(4,150);
     }
    else if (dato.equals("AB")){
      mov_servo(4,10);
     }
    else if (dato.equals("STOP") or dato.equals("POSI") or dato.equals("AUTO") and not(dato.equals("AB"))and not(dato.equals("CE"))){
       modo=dato;
       }
     else if (modo.equals("POSI") and not(dato.equals(""))and(posicion=="")){
       posicion=dato;
     }
     vTaskDelay(100);
  }
 }
 void loop1(void *parameter){
  while(1==1){
     if (not(modo!="STOP")){
       digitalWrite(32,1);
     }
     else{
        digitalWrite(32,0);
     }
     vTaskDelay(100);
 }
 }
