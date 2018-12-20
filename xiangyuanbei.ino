
/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio



/*End of auto generated code by Atmel studio */


//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio
const uint8_t input_table[16] = {30, 31, 32, 33, 34, 35, 36, 37, 22, 23, 24, 25, 26, 27, 28, 29,};
const uint8_t left_motor_pin = 13, right_motor_pin = 4;
const uint8_t differential_time = 20;
const uint8_t integral_time = 50;

struct PWMVALUE
{
  int Left = 0;
  int Right = 0;
  int base = 200;//速度基量
  int inc = 0; //速度增量
};

PWMVALUE PWM;

struct PIDVALUE
{
  double kp = 40; //比例
  double ki = 0; //积分
  double kd = 0; //微分
  uint8_t differential_time = 20; //积分时间
  uint8_t integral_time = 50; //积分时间
};

PIDVALUE PID;


int x = 0;
int x_last = 0;

int dx = 0;
int ix = 0;

unsigned long int  _millis = 0;
unsigned long int  present_millis = 0;
unsigned long int last_millis = 0;

void serial_debug();

void serial_debug()
{
  Serial.print("time:");
  Serial.print(millis());
  Serial.print("X:");
  Serial.print(x);
  Serial.print("dX:");
  Serial.print(dx);
  Serial.print("iX:");
  Serial.print(ix);
  Serial.print("PWM_integral_time:");
  Serial.print(PID.integral_time);
  Serial.print("left_motor:");
  Serial.print(PWM.Left);
  Serial.print("right_motor:");
  Serial.println(PWM.Right);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(left_motor_pin, OUTPUT);
  pinMode(right_motor_pin, OUTPUT);
  for (int i = 0; i < 16; i++)
  {
    pinMode(input_table[i], INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //每隔10millis 进入
  present_millis = millis();
  if (present_millis - last_millis >= 10)
  {
    last_millis = present_millis;

    //找黑线
    for (int i = 0; i < 16; i++)
    {
      if (digitalRead(input_table[i]) == HIGH)
      {
        //找到黑线x即误差=>用于比例计算
        x_last = x;
        x = i - 8;
        //计算误差的变化值=>用于微分计算
        dx = x - x_last;
        //对误差的累积值 =>用于积分运算
        ix += x;
        break;
      }
    }
    //PID计算
    PWM.inc = x * PID.kp; //比例环节
    PWM.inc -= dx * PID.kd; //微分环节
    PWM.inc += ix * PID.ki; //积分环节

    PWM.Left = PWM.base + PWM.inc; if (PWM.Left >= 255) {
      PWM.Left = 255;
    } if (PWM.Left <= 0) {
      PWM.Left = 0;
    }
    PWM.Right = PWM.base - PWM.inc; if (PWM.Right >= 255) {
      PWM.Right = 255;
    } if (PWM.Right <= 0) {
      PWM.Right = 0;
    }

    //超过积分时间 ix清零
    PID.integral_time -= 10;
    if (PID.integral_time <= 0)
    {
      ix = 0;
      PID.integral_time = integral_time;
    }
    serial_debug();
  }

  //输出给电机
  analogWrite(left_motor_pin, PWM.Left);
  analogWrite(right_motor_pin, PWM.Right);

  //停车
  if (millis() >= 60000)
  {
    analogWrite(left_motor_pin,0);
    analogWrite(right_motor_pin,0);
  }
}

