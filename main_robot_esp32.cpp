#include "BluetoothSerial.h"
BluetoothSerial SerialBT;


int IN1 = 27;
int IN2 = 26;
int IN3 = 25;
int IN4 = 33;

int ENA = 32;
int ENB = 14;

int ENC_IZQ = 35;
int ENC_DER = 34;

const int PWM_CHANNEL_0 = 0;
const int PWM_CHANNEL_1 = 1;

int contador_left = 0;
int contador_right = 0;
int contador_cuarenta_cinco = 0;
int contador_noventa = 0;
bool esperado = false;
int estado_anterior_left = LOW;
int estado_anterior_right = LOW;
int VEL_left = 60;
int VEL_right = 60;  // <--- para calcular distancias y giros solo esta velocidad
int recorrido = 1;


bool forward_right = true;
bool forward_left = true;
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(ENC_DER, INPUT);
  pinMode(ENC_IZQ, INPUT);

  Serial.begin(115200);

  SerialBT.begin("Chipilos"); //Bluetooth device name

  Serial.println("The device started, now you can pair it with bluetooth!");

  const int PWM_FREQ = 500;
  const int PWM_RESOLUTION = 8;

  ledcSetup(PWM_CHANNEL_0, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_1, PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(ENA, PWM_CHANNEL_0);
  ledcAttachPin(ENB, PWM_CHANNEL_1);

  estado_anterior_left = LOW; //////////////////////////////////////////////
  estado_anterior_right = LOW;  //////////////////////////////////////////////
  
  delay(3000);
}

void MotorL(boolean adelante, int pvm) {
  if (adelante) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  ledcWrite(PWM_CHANNEL_0, pvm);
}

void MotorR(boolean adelante, int pvm) {
  if (adelante) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  ledcWrite(PWM_CHANNEL_1, pvm);
}

void avanzar(int holes, bool izq, bool der){
  int pwm_right = 0;
  int pwm_left = 0;
  bool iniciado_right = false;
  bool iniciado_left = false;
  int contador_right = 0;
  int contador_left = 0;
  int lowv_right = 45;
  int lowv_left = 39;
  while(contador_right < holes || contador_left < holes){
    if(iniciado_right && iniciado_left){
      pwm_right = lowv_right;
      pwm_left = lowv_left;
    }else if(iniciado_right && !iniciado_left){
      pwm_right = lowv_right;
      pwm_left = VEL_left;
    }else if(!iniciado_right && iniciado_left){
      pwm_right = VEL_right;
      pwm_left = lowv_left;
    }else{
      pwm_right = VEL_right;
      pwm_left = VEL_left;
    }

    int estado_actual_left = digitalRead(ENC_IZQ);
    int estado_actual_right = digitalRead(ENC_DER);

    if(contador_left < holes){
      MotorL(izq, pwm_left);
      if(estado_actual_left == HIGH && estado_anterior_left == LOW){
        contador_left++;
        iniciado_left = true;
        Serial.println("En el izquierdo");
      }
    }else{
      MotorL(izq, 0);
    }
    
    if(contador_right < holes){
      MotorR(der, pwm_right);
      if(estado_actual_right == HIGH && estado_anterior_right == LOW){
        contador_right++;
        iniciado_right = true;
      }
    }else{
      MotorR(der, 0);
    }

    if(contador_right < contador_left){
      MotorR(der, VEL_right);
    }else if(contador_right > contador_left){
      MotorL(izq, VEL_left);
    }
    
    estado_anterior_left = estado_actual_left;
    estado_anterior_right = estado_actual_right;
  }
}

void detener(int time){
  int pwm_right = 0;
  int pwm_left = 0;
  MotorR(true, pwm_right);
  MotorL(true, pwm_left);
  delay(time);
}



void loop() {
  char lecture = 'q';
  if ( Serial.available())
        SerialBT.write(Serial.read());
  if (SerialBT.available())
    //  Serial.write(SerialBT.read());
    lecture = SerialBT.read();
    Serial.println(lecture);
     if(lecture == 'a'){
       avanzar(5, true, true);
     }else if(lecture =='d'){
       avanzar(1, false, true);
     }else if(lecture == 'i'){
       avanzar(1, true, false);
     }else if(lecture == 'r'){
       avanzar(5, false, false);
     }else{
       MotorL(true, 0);
       MotorR(true, 0);
     }
  delay(20);

  
  // avanzar(20, true, true);
  // detener(5000);
  // avanzar(10, true, false);
  // detener(5000);  
}
