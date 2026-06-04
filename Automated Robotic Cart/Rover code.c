#include  <SPI.h>
#include <Servo.h>
#include "nRF24L01.h"
#include "RF24.h"

Servo root;
Servo arm_A1;
Servo arm_A2;
Servo arm_B;
Servo wrist_A;
Servo wrist_B;
Servo gripper;

const int signal_LED = 13;
const int motor_LED = 44;
const int arm_LED = 46;

int root_position = 9000;
int arm_A_position = 4000;
int arm_B_position = 2000;
int wrist_A_position = 90;
int wrist_B_position = 9000;
int gripper_position = 90;

const int CE = 2;
const int CS = 3;
int data[10];

int wrist_A_noktalar[30];
int arm_A1_noktalar[30];
int arm_B_noktalar[30];
int root_noktalar[30];

// int kayitsayisi = 0;
// int kaydet_buton;
// int oynat_buton;
 
RF24 radio(CE, CS); // CE - CS
const uint64_t pipe = 0xE8E8F0F0E1LL;

// unsigned long simdikizaman; 
// unsigned long gecmiszaman;  

const int ENA = 4;
const int ENB = 5;
const int INA = 22;
const int INB = 24;
const int INC = 26;
const int IND = 28;

void setup() 
{
  radio.begin(); 
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  
  //Serial.begin(9600); 
  pinMode(10, OUTPUT);
  pinMode(40, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(INC, OUTPUT);
  pinMode(IND, OUTPUT);
  pinMode(signal_LED, OUTPUT);
  pinMode(motor_LED, OUTPUT);
  pinMode(arm_LED, OUTPUT);
  
  root.attach(12);
  arm_A1.attach(11);
  arm_A2.attach(10);
  arm_B.attach(9);
  wrist_A.attach(8);
  wrist_B.attach(7);
  gripper.attach(6);

  root.write(90);
  arm_A1.write(90);
  arm_A2.write(90);
  arm_B.write(90);
  wrist_A.write(90);
  wrist_B.write(90);
  gripper.write(90);

  
}

void loop() 
{
  if(radio.available())
  {
    radio.read(data, sizeof(data));
    digitalWrite(signal_LED, HIGH);  
  }
  else
  {
    digitalWrite(signal_LED, LOW);
    //Serial.println(" No Signal ");
  }

  if(data[6] == 1 )
     {
       motorcontrol();
       digitalWrite(motor_LED, HIGH);
       digitalWrite(arm_LED, LOW);   
     }
     else if(data[6] == 0)
     {
      armcontrol();
      digitalWrite(arm_LED, HIGH);
      digitalWrite(motor_LED, LOW);
     }
    
         
}

void armcontrol() 
{
  if(radio.available())
  {
    radio.read(data, sizeof(data));
    digitalWrite(signal_LED, HIGH);  
  }
  

  root_position = constrain(root_position, 0, 18000);
  if(data[1]>=550)
  {
    root_position++; 
  }
  if(data[1] <= 450)
  {
    root_position--;
  }
  root.write(root_position/100);
  

  arm_A_position = constrain(arm_A_position, 0, 18000);
  if(data[0]>=550)
  {
    arm_A_position++;
  }
  if(data[0] <= 450)
  {
    arm_A_position--;
  }
  arm_A1.write(arm_A_position/100);
  arm_A2.write(180-(arm_A_position/100));

  
  arm_B_position = constrain(arm_B_position, 0, 18000);
  if(data[2]>=550)
  {
    arm_B_position++; 
  }
  if(data[2] <= 450)
  {
    arm_B_position--;
  }
  arm_B.write(180-arm_B_position/100);

  
  wrist_A_position = map(data[5], 0, 1024, 0, 180);
  wrist_A.write(wrist_A_position);
  
  
  wrist_B_position = constrain(wrist_B_position, 0, 18000);
  if(data[3]>=550)
  {
    wrist_B_position++;
  }
  if(data[3] <= 450)
  {
    wrist_B_position--;
  }
  wrist_B.write(wrist_B_position/100);

  gripper_position = map(data[4], 0, 1024, 0, 180);
  gripper.write(gripper_position);
  

  // kaydet_buton = data[8];
  // oynat_buton = data[9];

  /*if(kaydet_buton ==1)
  {
    noktakaydet();
    while(kaydet_buton)
    {
      if(radio.available())
       {
       radio.read(data, sizeof(data));
       digitalWrite(signal_LED, HIGH);  
       kaydet_buton = data[4];
       }
     }
  }
  if(oynat_buton ==1)
  {
    
    while(1)
     {
      otonom();
     }   
  } */

}

void motorcontrol() //Paletlerin kontrolünü sağlayan fonksiyon
{
  if(radio.available())
  {
    radio.read(data, sizeof(data));
    digitalWrite(signal_LED, HIGH);

    if(data[0] > 550)
     {
      analogWrite(ENA, map(data[0], 550, 1023, 0, 255));
      digitalWrite(INB, HIGH);
      digitalWrite(INA, LOW);
     }
     if(data[0] < 450)
     {
      analogWrite(ENA, map(data[0], 450, 0, 0, 255));
      digitalWrite(INB, LOW);
      digitalWrite(INA, HIGH);
     }
     if(data[0]>=450 && data[0]<=550)
     {
      analogWrite(ENA, 0);
      digitalWrite(INA, LOW);
      digitalWrite(INB, LOW);
     }
    if(data[2] > 550)
     {
      analogWrite(ENB, map(data[2], 550, 1023, 0, 255));
      digitalWrite(INC, HIGH);
      digitalWrite(IND, LOW);
     }
    if(data[2] < 450)
     {
      analogWrite(ENB, map(data[2], 450, 0, 0, 255));
      digitalWrite(INC, LOW);
      digitalWrite(IND, HIGH);
     }
      if(data[2]>=450 && data[2]<=550)
     {
      analogWrite(ENB, 0);
      digitalWrite(INC, LOW);
      digitalWrite(IND, LOW);
     }
     //Serial.println("Palet aktif");
   
  }
  else
  {
    digitalWrite(signal_LED, LOW);
    //Serial.println(" No Signal ");
  }
}

/* 
void otonom()
{

    for(int i=0; i<kayitsayisi; i++)
    { 
     wrist_A.write(wrist_A_noktalar[i]/100);
     arm_A1.write(arm_A1_noktalar[i]/100);
     arm_B.write(180 - arm_B_noktalar[i]/100);
     root.write(root_noktalar[i]/100);
     delay(1000);
    } 
 }
void noktakaydet()
{
  if(kayitsayisi < 30)
  {
    wrist_A_noktalar[kayitsayisi] = wrist_A_position;
    arm_A1_noktalar[kayitsayisi] = arm_A_position;
    arm_B_noktalar[kayitsayisi] = arm_B_position;
    root_noktalar[kayitsayisi] = root_position;
    kayitsayisi++;
  }



  Serial.print(wrist_A_noktalar[kayitsayisi-1]);
  Serial.print(" ");
  Serial.print(arm_A1_noktalar[kayitsayisi-1]);
  Serial.print(" ");
  Serial.print(arm_B_noktalar[kayitsayisi-1]);
  Serial.print(" ");
  Serial.print(root_noktalar[kayitsayisi-1]);
  Serial.print(" ");
  Serial.println(kayitsayisi);
}

*/


