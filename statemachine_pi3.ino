#include <movingAvg.h>
#include <Servo.h>

Servo servo_eixo_x;
Servo servo_eixo_y;

const int servo_eixo_x_PIN = 9;
const int servo_eixo_y_PIN = 10;
const int ldr_esquerda = A0;
const int ldr_direita = A1;
const int ldr_cima = A2;
const int ldr_baixo = A3;

int ldrs[] = {ldr_esquerda, ldr_direita, ldr_cima, ldr_baixo};
int ldrs_values[sizeof(ldrs)];
int angulo_eixo_x;
int angulo_eixo_y;
int state;
unsigned long previousMillis = 0;
const long interval = 15;

#define WAIT 0
#define ANALYZE 1

#define ESQUERDA 0
#define DIREITA 1
#define CIMA 2
#define BAIXO 3

#define THRESHOLD 200

movingAvg media_ldr_esquerda(10);  
movingAvg media_ldr_direita(10);
movingAvg media_ldr_cima(10);  
movingAvg media_ldr_baixo(10);
movingAvg medias[] = { media_ldr_esquerda, media_ldr_direita, media_ldr_cima, media_ldr_baixo };
 
void setup() 
{  
  Serial.begin(9600);
  state = 0;
  angulo_eixo_x = 90;
  angulo_eixo_y = 90;
  
  servo_eixo_x.attach(servo_eixo_x_PIN);
  servo_eixo_x.write(angulo_eixo_x);
  
  servo_eixo_y.attach(servo_eixo_y_PIN);
  servo_eixo_y.write(angulo_eixo_y);

  for(int i = 0; i < sizeof(ldrs); i++){
    pinMode(ldrs[i], INPUT);
    ldrs_values[i] = 0;
    medias[i].begin();
  }
}

void loop() {
  unsigned long currentMillis = millis();

  for(int i = 0; i < sizeof(ldrs); i++){
    ldrs_values[i] = medias[i].reading(analogRead(ldrs[i]));
  }  
  
  if(abs_compare_ldrs(ESQUERDA, DIREITA) || abs_compare_ldrs(CIMA, BAIXO))
  {
    switch(state)
    {
      case WAIT:
        if (currentMillis - previousMillis >= interval)
        {
           previousMillis = currentMillis;
           state = ANALYZE;
        }
        break;
    
      case ANALYZE:
          if (compare_ldrs(DIREITA, ESQUERDA))
          {
            mover_direita();
          }
    
          if(compare_ldrs(ESQUERDA, DIREITA))
          {
            mover_esquerda();
          }
    
          if(compare_ldrs(CIMA, BAIXO))
          {
            mover_cima();
          }
    
          if(compare_ldrs(BAIXO, CIMA))
          {
            mover_baixo();
          }
          
          state = WAIT;
          break;
        
      default:
       state = WAIT;
       break;
    }
  }
}

void mover_esquerda(){
  if(angulo_eixo_x > 0){
    servo_eixo_x.write(--angulo_eixo_x);
  }
}

void mover_direita(){
  if(angulo_eixo_x < 180){
    servo_eixo_x.write(++angulo_eixo_x);
  }
}

void mover_cima(){
  if(angulo_eixo_y < 180){
    servo_eixo_y.write(++angulo_eixo_y);
  }
}

void mover_baixo(){
  if(angulo_eixo_y > 0){
    servo_eixo_y.write(--angulo_eixo_y);
  }
}

bool abs_compare_ldrs(int first_index, int second_index){
  return abs(ldrs_values[first_index] - ldrs_values[second_index]) > THRESHOLD;
}
bool compare_ldrs(int first_index, int second_index){
  return ldrs_values[first_index] - ldrs_values[second_index] > THRESHOLD;
}
 
