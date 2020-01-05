//初始化 响3次
//未检测到 响一次
//检测到连续响
//静置检测角度60
#include <Servo.h>
#include<math.h>
#define   forwardSpeed 1000 //顺时针
#define   inversionSpeed 2000
#define step 300

Servo servo360;
Servo servo180;

int port1 = A0;//连接火焰传感器
int value1 = 0;//测量的模拟数值
int port2 = A1;
int value2 = 0;
int port3 = A2;
int value3 = 0;
int port4 = A3;
int value4 = 0;
int port5 = A4;
int value5 = 0;
int port6 = A5;
int value6 = 0;
int port7 = A6;
int value7 = 0;
int port8 = A7;
int value8 = 0;
int port9 = A8;//纵向数值
int value9 = 0;
int fireValue[8] = {1024};
int fireValue180[14] = {1024}; //每十度一检测
int nowLocation = 1;
int sumStep = 0;//累计步长
void setup() {
  pinMode(4, OUTPUT); //激光
  pinMode(3, OUTPUT); //蜂鸣器
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  delay(100);
  Serial.println("正在启动...");
  initServo();
  delay(200);
  bling(3);//初始化完成
  Serial.println("开始监测...");
}

void loop() {
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);

  readValues();//读取火焰传感器数值
  int minValue = 1024;
  int sensorNum = -1;
  for (int i = 0; i < 8; i++)
  {
    if (fireValue[i] < minValue)
    {
      minValue = fireValue[i];
      sensorNum = i + 1;
    }
  }

  if (minValue < 500)//500为响应阈值
  {
    //  printInfo(sensorNum, minValue);//串口打印调试
    find360Location(sensorNum);//确定舵机相对位置
    //printAll();
  }

}//loop()

void find360Location(int fireNum)
{
  //获取舵机相对路程差
  //舵机当前位置转到目标位置的路程
  //初始位置为1
  int go = nowLocation - fireNum;

  nowLocation = fireNum;//更新当前位置
  changeLocation(go);//旋转360度舵机到指定位置
}
void changeLocation(int length)
{
  Serial.print("步长");
  Serial.print(length);
  Serial.print("  累计步长");
  Serial.print(sumStep);
  //控制舵机转向和距离
  if (length < 0)//顺时针旋转
  {
    Serial.print("  顺时针 ");
    length = abs(length);

    if (sumStep + length < 8  )
    {
      servoControl360(forwardSpeed, length * step);

      sumStep = sumStep + length;
      Serial.print(length);
      Serial.print("  当前位置 ");
      Serial.println(nowLocation);
    }
    else
    {
      servoControl360(inversionSpeed, (8 - length) * step);
      sumStep = sumStep - length;
      Serial.print("  else ");
      Serial.print(8 - length);
      Serial.print("  当前位置 ");
      Serial.println(nowLocation);
    }
  }
  else if (length > 0)//逆时针旋转
  {
    length = abs(length);
    Serial.print("  逆时针 ");

    if (sumStep + length > 0  )
    {
      servoControl360(inversionSpeed, length * step);
      sumStep = sumStep - length;
      Serial.print(length);
      Serial.print("  当前位置 ");
      Serial.println(nowLocation);
    }
    else
    {
      servoControl360(forwardSpeed, (8 - length) * step);
      sumStep = sumStep + length;
      Serial.print("  else ");
      Serial.print(8 - length);
      Serial.print("  当前位置 ");
      Serial.println(nowLocation);
    }
  }
}
void servoControl360(int thisSpeed, int thisTime)
{
  //360度舵机旋转到指定位置
  //控制舵机旋转距离
  delay(10);
  servo360.attach(6);
  delay(10);
  servo360.writeMicroseconds(thisSpeed);
  delay(thisTime);
  servo360.detach();
  delay(300);
  detect180(); //纵向检测火焰位置
}
void initServo()
{
  delay(10);
  servo360.attach(6);
  delay(10);
  servo360.writeMicroseconds(1000);
  delay(500);
  servo360.detach();
  delay(300);
  servo360.attach(6);
  delay(10);
  servo360.writeMicroseconds(2000);
  delay(500);
  servo360.detach();
  delay(300);
  servo180.attach(5); 
  delay(1000);
  servo180.write(20);
  delay(500);
  servo180.write(60);


}
void detect180()
{
  //纵向检测火焰位置 检测扇区 140度
  for (int i = 1; i <= 14; i++)
  {
    servo180.write(10 * i);
    readValues180(i - 1);
    delay(200);//检测速度
  }
  int minValue180 = 1024;
  int sensorAngle = 180;
  for (int i = 0; i < 14; i++)
  {
    if (fireValue180[i] < minValue180)
    {
      minValue180 = fireValue180[i];
      sensorAngle = (i + 1) * 10;
    }
  }

  //找到纵向坐标 小于阈值
  if (minValue180 < 200)
  {
    //找到火焰位置
    bling(2);
    servo180.write(sensorAngle);
    digitalWrite(4, HIGH);//激光点亮
    digitalWrite(3, LOW);//蜂鸣器响起
    bling(3);
    digitalWrite(4, HIGH);//激光点亮

    blingWithoutLight(6);

    delay(2000);
  }
  else
  {
    //没有找到火焰位置
    servo180.write(60);
    bling(1);
  }
}
void bling(int time)
{
  for (int i = 0; i < time; i++)
  {
    digitalWrite(4, HIGH);//激光点亮
    digitalWrite(3, LOW);//蜂鸣器响起
    delay(50);
    digitalWrite(4, LOW);//激光关闭
    digitalWrite(3, HIGH);//蜂鸣器关闭
    delay(50);
  }

}
void blingWithoutLight(int time)
{
  for (int i = 0; i < time; i++)
  {
    digitalWrite(4, HIGH);//激光点亮
    digitalWrite(3, LOW);//蜂鸣器响起
    delay(50);
    digitalWrite(3, HIGH);//蜂鸣器关闭
    delay(50);
  }
}
void printInfo(int num2, int value2)
{
  Serial.print("num: ");
  Serial.print(num2);
  Serial.print("    value: ");
  Serial.println(value2);
}
void readValues180(int readTime)
{
  //readTime 0-14 存储15次数值
  value9 = analogRead(port9);
  fireValue180[readTime] = value9;

}
void readValues()
{
  delay(500);
  value1 = analogRead(port1);
  fireValue[0] = value1;
  delay(10);
  value2 = analogRead(port2);
  fireValue[1] = value2;
  delay(10);
  value3 = analogRead(port3);
  fireValue[2] = value3;
  delay(10);
  value4 = analogRead(port4);
  fireValue[3] = value4;
  delay(10);
  value5 = analogRead(port5);
  fireValue[4] = value5;
  delay(10);
  value6 = analogRead(port6);
  fireValue[5] = value6;
  delay(10);
  value7 = analogRead(port7);
  fireValue[6] = value7;
  delay(10);
  value8 = analogRead(port8);
  fireValue[7] = value8;
  delay(10);
}
void printAll()
{
  Serial.print("A1: ");
  Serial.print(value1);
  Serial.print("  A2: ");
  Serial.print(value2);
  Serial.print("  A3: ");
  Serial.print(value3);
  Serial.print("  A4: ");
  Serial.print(value4);
  Serial.print("  A5: ");
  Serial.print(value5);
  Serial.print("  A6: ");
  Serial.print(value6);
  Serial.print("  A7: ");
  Serial.print(value7);
  Serial.print("  A8: ");
  Serial.println(value8);
}
