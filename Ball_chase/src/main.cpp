#include <Arduino.h>
#include <Wire.h>

//ボールセンサーのピン定義
#define B_0  2  
#define B_2  4  
#define B_4  6  
#define B_6  8  
#define B_8  22 
#define B_10 24 
#define B_12 26 
#define B_14 28 

//関数のプロトタイプ宣言
void Motor(int, int);
int setMotorSpeed(int, int, int);

int ball_pin[16] = {B_0,B_2,B_4,B_6,B_8,B_10,B_12,B_14};

int Motor_1, Motor_2, Motor_3, Motor_4, Motor_5, Motor_6;
float ball_ang;

char motor_pow[6] = {0,1,2,3,4,5}; //モーター出力用


void setup() {
    
  pinMode(B_0, INPUT);    //入力
  pinMode(B_2, INPUT);
  pinMode(B_4, INPUT);
  pinMode(B_6, INPUT);
  pinMode(B_8, INPUT);
  pinMode(B_10, INPUT);
  pinMode(B_12, INPUT);
  pinMode(B_14, INPUT);
    
  Wire.begin(); // join i2c bus
  Serial.begin(9600); //シリアル通信の初期化

}

void loop(){

  int Motor1 = 0;
  int Motor2 = 0;
  int Motor3 = 0;
  int Motor4 = 0;

  //IRセンサからロボットを中心に見た時のボールの角度を出す
  int b_val[16];
  int ball_best = 0;
  int ball_best_num;
  for(int i = 0; i < 16; i++){
      b_val[i] = 0;
  }
  for(int i = 0; i < 100; i++){
      for(int i = 0; i < 8; i++){
          if(digitalRead(ball_pin[i]) == 0){
              b_val[i]++;
          }
      }
  }
  for(int i = 0; i < 8; i++){
      if(ball_best < b_val[i]){
          ball_best = b_val[i];
          ball_best_num = i;
      }
  }
  ball_ang = ball_best_num * 45;

  //それぞれのモーターのスピードの大きさを出す
  Motor1 = setMotorSpeed(ball_ang, 40, 45);
  Motor2 = setMotorSpeed(ball_ang, 40, 315);
  Motor3 = setMotorSpeed(ball_ang, 40, 135);
  Motor4 = setMotorSpeed(ball_ang, 40, 225);

  int Motor_value1,Motor_value2,Motor_value3,Motor_value4;

  //どれか一つが半田ミス
  Motor_value1 = Motor1 *  1;  
  Motor_value2 = Motor2 * -1;
  Motor_value3 = Motor3 * -1;
  Motor_value4 = Motor4 * -1;

  //モーターの番号と大きさ
  Motor(0, Motor_value1);
  delay(5);
  Motor(1, Motor_value2);
  delay(5);
  Motor(2, Motor_value3);
  delay(5);
  Motor(3, Motor_value4);
  delay(5);
  //ダミーデータを送らないとフリーズorハングアウト
  Motor(4, 0);
  delay(5);
  Motor(5, 0);
  delay(5);


  Wire.beginTransmission(0x14>>1);
  Wire.write(motor_pow[0]); // 1ch
  Wire.write(motor_pow[1]); // 2ch
  Wire.write(motor_pow[2]); // 3ch
  Wire.write(motor_pow[3]); // 4ch
  Wire.write(motor_pow[4]); // 5ch
  Wire.write(motor_pow[5]); // 6ch
  Wire.endTransmission(/*falseを入れると二回目のループに入れる*/);
  //ディレイをかけないとハングアウト
  delay(20);

}

//関数

//それぞれのモーターの大きさを出す
int setMotorSpeed(int gAngle, int Speed, int mAngle) {
    int motorSpeed = sin(radians(gAngle - mAngle)) * Speed;
    return motorSpeed;
}

//ダイセン6chモータードライバー
//モーターnumは0～5
//入力valは-100~100 0でストップ 999でブレーキ
void Motor(int num, int val){
int mot;

  if(val < 0 && val >= -100){
    val = abs(val);
    motor_pow[num] = val + 128;
  }else if(val >= 0 && val <= 100){
    motor_pow[num] = val; 
  }else if(val == 999){
    motor_pow[num] = 128; 
  }else{
    return;
  }
}