#include <Arduino.h>
#include <Servo.h>
#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>


//Define States
#define Stop 1
#define Forward 2
#define Left 3
#define Right 4
#define Mark 5
#define Drop1 6
#define Drop2 7
#define Drop3 8
#define Door 9

// Define IR Sensor Pins
int IRR1=32;     //Right Sensor 1
int IRR2=33;     //Right Sensor 2
int IRL1=34;    //Left Sensor 1
int IRL2=35;    //Left Sensor 2
int MS1=30;    //Mark Sensor 1
int MS2=31;    //Mark Sensor 2

//Define Obstacle Sensor Pins
int OBS1=38; //Obs Sensor 1 
int OBS2=39; //Obs Sensor 2


// Define Motor Pins
  int FR1= A8; //Front Right Motor 1
int FR2= A9; //Front Right Motor 2
int FRE=7; //Front Right Enable

int RR1=A10; //Rear Right Motor 1
int RR2=A11; //Rear Right Motor 2
int RRE=6; //Rear Right Enable

int FL1=A12; //Front Left Motor 1
int FL2=A13; //Front Left Motor 2
int FLE=5; //Front Left Enable

int RL1=A14; //Rear Left Motor 1
int RL2=A15; //Rear Left Motor 2
int RLE=4; //Rear Left Enable


// Define Servo Pins
int S01=8; //Servo 1
int S02=9; //Servo 2
int S03=10; //Servo 3

Servo S1;
Servo S2;
Servo S3;

//Define RF Pins
RF24 radio(22,23); // CE, CSN
const uint64_t address = 0xE8E8F0F0E1LL;
bool radioNumber = 0;

//Define Variables
int IRR1_state = 0;
int IRR2_state = 0;
int IRL1_state = 0;
int IRL2_state = 0; 
int MS1_state = 0;
int MS2_state = 0;
int OBS1_state = 0;
int OBS2_state = 0;
int MarkCount = 0;
int State = 1;
int Speed = 200;

int LSpeed = 200;
int RSpeed = 200;

int stopdelay = 10000;
int dropdelay = 5000;
int doordelay = 6000;
int servodelay = 5000;
int currentLspeed = 0;
int currentRspeed = 0;
void setup ()
{

//Define Input Pin Modes
pinMode (IRR1, INPUT);
pinMode (IRR2, INPUT);
pinMode (IRL1, INPUT);
pinMode (IRL2, INPUT);
pinMode (MS1, INPUT);
pinMode (MS2, INPUT);
pinMode (OBS1, INPUT);
pinMode (OBS2, INPUT);



//Define Output Pin Modes
pinMode (FR1, OUTPUT);
pinMode (FR2, OUTPUT);
pinMode (FRE, OUTPUT);
pinMode (RR1, OUTPUT);
pinMode (RR2, OUTPUT);
pinMode (RRE, OUTPUT);
pinMode (FL1, OUTPUT);
pinMode (FL2, OUTPUT);
pinMode (FLE, OUTPUT);
pinMode (RL1, OUTPUT);
pinMode (RL2, OUTPUT);
pinMode (RLE, OUTPUT);

S1.attach(S01);
S2.attach(S02);
S3.attach(S03);
Serial.begin(9600);

//Define RF module setup
  radio.begin();
  radio.setRetries(15, 15);
  Serial.println("lets begin");
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}


void loop ()
{
IRR1_state = digitalRead(IRR1);
IRR2_state = digitalRead(IRR2);
IRL1_state = digitalRead(IRL1);
IRL2_state = digitalRead(IRL2);
MS1_state = digitalRead(MS1);
MS2_state = digitalRead(MS2);
OBS1_state = digitalRead(OBS1);
OBS2_state = digitalRead(OBS2);
//OBS1_state = 1; //This was used when obstacle detection was not used 
//OBS2_state = 1;

//delay(1000);
S1.write(0);
S2.write(0);
S3.write(0);


Serial.print("State: "); Serial.print(State);
Serial.print("  IR: "); Serial.print(IRL1_state);Serial.print(IRL2_state);Serial.print(IRR1_state); Serial.print(IRR2_state);
Serial.print("  Mark Detect: "); Serial.print(MS1_state);Serial.print(MS2_state);
Serial.print("  Obj Detect: "); Serial.print(OBS1_state);Serial.print(OBS2_state);
Serial.print("  Mark Count: "); Serial.println(MarkCount);
//Serial.print("  Speed: "); Serial.print(currentLspeed);Serial.println(currentRspeed);
   
switch (State)
{
  case Stop:
      digitalWrite(FR1, LOW);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, LOW);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, LOW);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, LOW);
      S1.write(0);
      S2.write(0);
      S3.write(0);
      
     delay(stopdelay);
     State = Forward;
     break;

  case Forward:
      currentLspeed = Speed;
      currentRspeed = Speed;
      analogWrite(FRE, Speed);//Front Right Enable
      analogWrite(RRE, Speed);//Rear Right Enable
      analogWrite(FLE, Speed);//Front Left Enable
      analogWrite(RLE, Speed);//Rear Left Enable
      digitalWrite(FR1, HIGH);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, HIGH);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, HIGH);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, HIGH);
      digitalWrite(RL2, LOW);
      S1.write(0);
      S2.write(0);
      S3.write(0);
      
 if((IRL1_state==1 || IRL2_state==1) && IRR1_state==0 && IRR2_state==0 && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )     
      {
        State=Left;    
        break;  
      }
 if(IRL1_state==0 && IRL2_state==0 && (IRR1_state==1 || IRR2_state==1) && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )     
      {
        State=Right; 
        break;
      }
 if (OBS1_state==0 || OBS2_state==0 )     
      {
        State=Stop;     
        break; 
      }
 if (MS1_state==1 || MS2_state==1)     
      {
        State=Mark; 
        break;
      }  
 if (IRL1_state==0 && IRL2_state==0 && IRR1_state==0 && IRR2_state==0 && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )  
      {
        State=Forward;
        break;  
      }
      break;

  case Left:
      currentLspeed = LSpeed;
      currentRspeed = RSpeed;
      analogWrite(FRE, RSpeed);//Front Right Enable
      analogWrite(RRE, RSpeed);//Rear Right Enable
      analogWrite(FLE, LSpeed);//Front Left Enable
      analogWrite(RLE, LSpeed);//Rear Left Enable
      digitalWrite(FR1, HIGH);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, HIGH);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, LOW);
      digitalWrite(FL2, HIGH);
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, HIGH);
      S1.write(0);
      S2.write(0);
      S3.write(0);

 if (IRL1_state==0 && IRL2_state==0 && IRR1_state==0 && IRR2_state==0 && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )  
      {
        State=Forward;
        break;  
      }
if ((IRL1_state==0 || IRL2_state==0) && (IRR1_state==1 || IRR2_state==1) && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )           
      {
        State=Right;      
      }
if (MS1_state==1 || MS2_state==1)     
      {
        State=Mark;      
        break;
      }
if (OBS1_state==0 || OBS2_state==0 )         
      {
        State=Stop; 
        break;
      }
 if ((IRL1_state==1 || IRL2_state==1) && IRR1_state==0 && IRR2_state==0 && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )     
      {
        State=Left;      
        break;
      }
      break;

  


    case Right:
      currentLspeed = LSpeed;
      currentRspeed = RSpeed;
      analogWrite(FRE, RSpeed);//Front Right Enable
      analogWrite(RRE, RSpeed);//Rear Right Enable
      analogWrite(FLE, LSpeed);//Front Left Enable
      analogWrite(RLE, LSpeed);//Rear Left Enable
      digitalWrite(FR1, LOW);
      digitalWrite(FR2, HIGH);
      digitalWrite(RR1, LOW);
      digitalWrite(RR2, HIGH);
      digitalWrite(FL1, HIGH);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, HIGH);
      digitalWrite(RL2, LOW);
      S1.write(0);
      S2.write(0);
      S3.write(0);

 if (IRL1_state==0 && IRL2_state==0 && IRR1_state==0 && IRR2_state==0 && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )  
      {
        State=Forward;
        break;  
      }
 if ((IRL1_state==1 || IRL2_state==1) && (IRR1_state==0 || IRR2_state==0) && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )     
      {
        State=Left;      
        break;
      }
if(MS1_state==1 || MS2_state==1)           {
        State=Mark; 
        break;
      }
if(OBS1_state==0 || OBS2_state==0 )     
      {
        State=Stop; 
        break;
      }
 if ((IRL1_state==0 || IRL2_state==0) && (IRR1_state==1 || IRR2_state==1) && MS1_state==0 && MS2_state==0 && OBS1_state==1 && OBS2_state==1 )           
      {
        State=Right;    
        break;  
      }
      
      break;

  case Mark:
    MarkCount=MarkCount+1;

    if (MarkCount==1)
      {
        State=Stop;
      }
   if (MarkCount==2)
      {
        State=Drop1;
      }
   if (MarkCount==3)
      {
        State=Door;
      }
   if (MarkCount==4)
      {
        State=Door;
      }
   if (MarkCount==5)
      {
        State=Drop2;
      }
    if (MarkCount==6)
      {
        State=Drop3;
      }
      break;

  case Drop1:
      digitalWrite(FR1, LOW);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, LOW);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, LOW);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, LOW);
      S1.write(90);
      delay(servodelay);
      S1.write(0);
      S2.write(0);
      S3.write(0);
      delay(dropdelay);
     
     State = Forward;
     break;

  case Drop2:
      digitalWrite(FR1, LOW);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, LOW);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, LOW);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, LOW);
      S1.write(0);
      S2.write(90);
      delay(servodelay);
      S2.write(0);
      S3.write(0);
      delay(dropdelay);
     
     State = Forward;
     break;

  case Drop3:
      digitalWrite(FR1, LOW);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, LOW);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, LOW);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, LOW);
      S1.write(0);
      S2.write(0);
      S3.write(90);
      delay(servodelay);
      S3.write(0);
      delay(dropdelay);
     
     State = Stop;
     break;

case Door:
      digitalWrite(FR1, LOW);
      digitalWrite(FR2, LOW);
      digitalWrite(RR1, LOW);
      digitalWrite(RR2, LOW);
      digitalWrite(FL1, LOW);
      digitalWrite(FL2, LOW);
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, LOW);     
  
    Serial.println("sending:");
    const char text[] = "RC Robo";
    radio.write(&text, sizeof(text));
  
  
      delay(doordelay);    
      
      State = Forward;
      break;
}



}
