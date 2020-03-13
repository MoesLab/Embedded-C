/////////////////////////////////////////////////////////////////////////////////
// Electric Roller Booster V2.0 2012
// The brain of ERB it will receive controll commands from XBee remote controller
// and controls a pair of boots. 
// for configuration and feedbacks, it uses bluetooth module to communicate with phone 
////////////////////////////////////////////////////////////////////////////////


#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <Servo.h> 
#include <EEPROM.h>
/******************************************************************PINS********/
const int Motor_L_PN = 4;         // Motor PWM control Left
const int Motor_R_PN = 5;         // Motor PWM control Right
const int Brake_L_PN = 6;         // Brake PWM control Left
const int Brake_R_PN = 7;         // Brake PWM control Right
const int BrakeSW_L_PN = 48;      // Brake on/off control Left
const int BrakeSW_R_PN = 49;      // Brake on/off control Right

const int Servo_L_PN = 8;            // Servo PWM control Left
const int Servo_R_PN = 9;            // Servo PWM control Right
const int Buzz_PN =  52;             // the number of the LED pin
const int Res_Relay_PN = 22;         // the number of the pushbutton pin
const int Main_Relay_PN = 23;        // the number of the pushbutton pin
const int TLED_RED_PN = 10;          // Tail Light RED pin
const int TLED_GRN_PN = 12;          // Tail Light RED pin
const int TLED_BLU_PN = 11;          // Tail Light RED pin
const int HeadLight_PN = 13;         // Tail Light RED pin
const int Tick_L_PN = 20;         // Tick counter pin intrrupt Left
const int Tick_R_PN = 21;         // Tick counter pin intrrupt Right

const int Volt_sens1_PN=A1;       // Analog sensor 1 main battery voltage pin
const int Amp_sens1_PN=A2;        // Analog sensor 1 main battery current pin
const int Meter_L_PN=A5;          // Analog Meter form BLDC left pin
const int Meter_R_PN=A6;          // Analog Meter form BLDC right pin
/**************************************************************VARIABLES**********************/
 int potpin =1;
 int potpin2 =5;
 int i=0;
 int j=0;
 int k=0;
 char buff[]="";
 char Zch=0,Bch=0;
 unsigned long millis_old;
 unsigned long millis_new;

volatile byte Tick_L,Tick_R;     // tick pulses that come from drivers

int Motor_R=0;           // speed of Right motor
int Motor_R2=0;          // speed of Right motor
int Motor_L=0;           // speed of Left motor
int Motor_L2=0;          // speed of Left motor

int Brake_R=0;           // break RIGHT level
int Brake_R2=0;          // break RIGHT level
int Brake_L=0;           // break LEFT level
int Brake_L2=0;          // break LEFT level

int Volt_sens1=0;        // Analog sensor 1 main battery voltage
int Amp_sens1=0;         // Analog sensor 1 main battery current
int Meter_L=0;           // Analog Meter valu left
int Meter_R=0;           // Analog Meter valu right

int SRV_L_POS=0;                  // the position of servo 0-179 LEFT
int SRV_R_POS=0;                  // the position of servo 0-179 RIGHT
int SRV_L=0;                      // servo left position gear>  0:NUTRAL 1:ENGAGE 2:BREAK
int SRV_R=0;                      // servo right position gear> 0:NUTRAL 1:ENGAGE 2:BREAK
int SRV_L_GEAR[3]={90,90,90};     // the values for all stages NU,EN,BR  left
int SRV_R_GEAR[3]={90,90,90};     // the values for all stages NU,EN,BR  Right


int LED_HED=0;          // the value of Headlight 0-255
int LED_BLU=0;           // the value of blue LED 0-255
int LED_RED=0;           // the value of red LED 0-255
int LED_GRN=0;           // the value of Green LED 0-255


int Stick_H=0;           // 0-255 mem for RC Horizontal pot
int Stick_V=0;           // 0-255 mem for RC vertical pot
int Stick_S=0;           // RC Stick select  button 0 /1
int Red_Button=0;        // RC Red select  button 0 /1

int L_speed=0;           // Speed of Left boot
int R_speed=0;           // Speed of Right boot
int Z_Value=0;           // value part of Zigbee command
int B_Value=0;           // value part of Bluetooth command

boolean Stick_S_Togg=false;   // keep track of Stick Select buttom false:off true: on FLAG
boolean Red_B_Togg=false;     // keep track of Red buttom false:off true: on FLAG

String B_STR_FB="";           // array for DATA sending to Bluetooth as feedback
String B_STR_IN="";           // array for DATA Comming trough Bluetooth as command

String B_str="";       //temp Bluetooth strings
String B_CMD="";
String B_VAL="";

String Z_str="";       //temp Zbee strings
String Z_CMD="";
String Z_VAL="";

Servo Left_servo,Right_servo;  // create servo object to control a servo 

/********************************************************** SETUP() *************************************/
void setup()
{
  pinMode(Motor_L_PN, OUTPUT); 
  pinMode(Motor_R_PN, OUTPUT); 
  pinMode(Brake_L_PN, OUTPUT); 
  pinMode(Brake_R_PN, OUTPUT); 
  pinMode(BrakeSW_L_PN, OUTPUT); 
  pinMode(BrakeSW_R_PN, OUTPUT); 

  pinMode(Buzz_PN, OUTPUT); 
  pinMode(Res_Relay_PN, OUTPUT);   
  pinMode(Main_Relay_PN, OUTPUT);
  pinMode(TLED_RED_PN, OUTPUT);
  pinMode(TLED_BLU_PN, OUTPUT);
  pinMode(TLED_GRN_PN, OUTPUT);
  pinMode(HeadLight_PN, OUTPUT);
  pinMode(Buzz_PN, OUTPUT);

  //Serial.begin(115200);    //USB
  Serial1.begin(115200);   //Xbee
  Serial2.begin(115200);  // Blue tooth
  
  START();
  
 //reading from EEPROM for servos position
 
  UPDATER();
  
  delay(1000);
   
}
/****************************************************************** LOOP() *******************/
void loop()
{
/******************************** Read in ***************************/

 
/********************************* write out *************************/
if (Serial1.available())         //-------------------Zigbee
  {
    Zch = Serial1.read();
    if(Zch!='\n') Z_str+=Zch;    // append on chr to string
    else
     {  
       if(Z_str.length()!=0)
        { 
          // do something with the string
          Z_CMD = Z_str.substring(0,3);        //extract the command part first 3 chs
          Z_VAL = Z_str.substring(3);          //extract the value part last 3 chs
          AGENT(Z_CMD,Z_VAL);                  // run the command
          Z_str="";
        }
     }   
  }

if (Serial2.available())         //-------------------Bluetooth
  {
    Bch = Serial2.read();
    if(Bch!='\n') B_str+=Bch;    // append on chr to string
    else
     {  
       if(B_str.length()!=0)
        {
          // do something with the string
         B_CMD = B_str.substring(0,3);        //extract the command part first 3 chs
         B_VAL = B_str.substring(3);          //extract the value part last 3 chs
         AGENT(B_CMD,B_VAL);                  // run the command
         B_str="";
        }
     } 
  }// end if serial available Bluetooth   

 UPDATER();  // update all outputs respectedly 

} // end loop 




/******************************************************************ALL FUNCTIONS***********/
boolean AGENT(String cmd,String val)
{
 
 if(cmd=="ZML" || cmd=="BML")                                                    // if motor value for LEFT
  {
    if(val=="???") { sprintf(buff,"BML%03d",Motor_L); Serial2.println(buff); return true;}
    else { val.toCharArray(buff, 4); Motor_L=atoi(buff); 
           if(Brake_L==0) { Motor_L2=Motor_L; return true; }  
           //else { feed back }
          }
    return false;
  } 
 if(cmd=="ZMR" || cmd=="BMR")                                                     // if motor value for LEFT
  {
    if(val=="???") { sprintf(buff,"BMR%03d",Motor_R); Serial2.println(buff); return true;}
    else { val.toCharArray(buff, 4); Motor_R=atoi(buff); 
           if(Brake_R==0) { Motor_R2=Motor_R; return true; }
          }
    return false;   
  } 
 if(cmd=="ZRB" || cmd=="BRB")                                                     // dynamic break by red button
  {
    if(val=="???") { sprintf(buff,"BRB%03d",Red_Button); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); Red_Button=atoi(buff); 
           if(Red_Button==1) Red_B_Togg!=Red_B_Togg;
         }     
    return false;  
  } 
 if(cmd=="ZPB" || cmd=="BPB")                                                     //if Stick sel buttton presse | released
  {
    if(val=="???") { sprintf(buff,"BPB%03d",Stick_S); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); Stick_S=atoi(buff);   
           if(Stick_S==1) Stick_S_Togg!=Stick_S_Togg;
           
           if(Stick_V>200 && Stick_S_Togg==true)        //if it is on maxim and Stick buttom is pressed
             { SRV_L=1; SRV_R=1; SRV_L_POS=SRV_L_GEAR[SRV_L]; SRV_R_POS=SRV_R_GEAR[SRV_R];
               return true; }                           //Engage 
           else if(Stick_V<50 && Stick_S_Togg==true)    // if it is in min
                  { SRV_L=2; SRV_R=2; SRV_L_POS=SRV_L_GEAR[SRV_L]; SRV_R_POS=SRV_R_GEAR[SRV_R];
               return true; }                           //Brake
           else                                         // if it is in between
             { SRV_L=0; SRV_R=0;  SRV_L_POS=SRV_L_GEAR[SRV_L]; SRV_R_POS=SRV_R_GEAR[SRV_R]; 
               return true; }                           //Nuteral             
         }
    return false;       
  } 
 if(cmd=="ZSV" || cmd=="BSV")                                                      // if Stick Vertical value
  {
    if(val=="???") { sprintf(buff,"BSV%03d",Stick_V); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); Stick_V=atoi(buff); return true; }        
    return false;       
  } 
 if(cmd=="ZSH" || cmd=="BSH")                                                      // if Stick Horizontal value
  {
    if(val=="???") { sprintf(buff,"BSH%03d",Stick_H); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); Stick_H=atoi(buff); return true; }        
    return false;       
  } 
 if(cmd=="ZSL" || cmd=="BSL")                                                      // if servo value for LEFT 0-179
  {
    if(val=="???") { sprintf(buff,"BSL%03d",SRV_L_POS); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); SRV_L_POS=atoi(buff); return true; }         
    return false;       
  } 
 if(cmd=="ZSR" || cmd=="BSR")                                                      // if servo value for RIGHT 0-179
  {
    if(val=="???") { sprintf(buff,"BSR%03d",SRV_R_POS); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); SRV_R_POS=atoi(buff); return true; }         
    return false;       
  } 
 if(cmd=="ZLG" || cmd=="BLG")                                                      // if green LED value
  {
    if(val=="???") { sprintf(buff,"BLG%03d",LED_GRN); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); LED_GRN=atoi(buff); return true; }         
    return false;       
  } 
 if(cmd=="ZLB" || cmd=="BLB")                                                      // if blue LED value
  {
    if(val=="???") { sprintf(buff,"BLB%03d",LED_BLU); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); LED_BLU=atoi(buff); return true; }         
    return false;       
  } 
 if(cmd=="ZLR" || cmd=="BLR")                                                      // if red LED value
  {
    if(val=="???") { sprintf(buff,"BLR%03d",LED_RED); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); LED_RED=atoi(buff); return true; }         
    return false;       
  } 
 if(cmd=="ZLH" || cmd=="BLH")                                                      // if  head light value
  {
    if(val=="???") { sprintf(buff,"BLH%03d",LED_HED); Serial2.println(buff); }
    else { val.toCharArray(buff, 4); LED_HED=atoi(buff); return true; }         
    return false;       
  } 
 return true;  
}

void UPDATER()
{  
  analogWrite(Motor_L_PN,Motor_L);
  analogWrite(Motor_R_PN,Motor_R);
  
  Left_servo.write(SRV_L_POS);
  Right_servo.write(SRV_R_POS);
  
  digitalWrite(BrakeSW_L_PN,(boolean) Red_Button);
  digitalWrite(BrakeSW_R_PN,(boolean) Red_Button);  

  analogWrite(HeadLight_PN,LED_HED);
  analogWrite(TLED_GRN_PN,LED_GRN);
  analogWrite(TLED_BLU_PN,LED_BLU);
  analogWrite(TLED_RED_PN,LED_RED);
 
  
}
void BUZZER(int Length)
{
 digitalWrite(Buzz_PN, HIGH);  
 delay(Length);
 digitalWrite(Buzz_PN, LOW);  
}
void START()
{
 digitalWrite(Res_Relay_PN, HIGH); 
 BUZZER(100);
 digitalWrite(Main_Relay_PN, HIGH);    
}
