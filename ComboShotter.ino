/********************************************************************************************************************************
 Name:PS3 Combo Shotter x8 2 columns and 4 rows for Arduino
 Author: MoesLab 
 
 There is definity room to improve if you are serious about speed this example has been optimize for SoulCalibur 4 combo list

********************************************************************************************************************************/

#include <stdlib.h> 
#include <stdio.h>
#include <Keypad.h>

const byte ROWS = 2; //two rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {{'1','2','3','4'},{'5','6','7','8'}};

// (U) up,(D) down,(L) left,(R) right----- (S) square,(T) triangle,(C) circle,(F) fork  >>> HIGH status
// (u) up,(d) down,(l) left,(r) right----- (s) square,(t) triangle,(c) circle,(f) fork  >>> LOW status
// (1,2,3,4,5,6,7,8,9) x 10 gives milisec timeing
///////////////////////////////////////////////////////////////////////////////////////////////
char combo[8][60]= 
{
// Here you hard code the combo[][] command list for 2 x 4 buttons once been press the combo[][] will be send to func_combo()
// Example
// "S8s4S8s4T8t4 "  SQUARE will be HIGH for 8 x 10 = 80 milisec then SQUARE will be LOW for 4 x 10 milisec again 
//                  SQUARE will be HIGH for 8 x 10 = 80 milisec then SQUARE will be LOW for 4 x 10 milisec then
//                  TRIANGLE will be HIGH for 8 x 10 = 80 milisec then TRIANGLE will be LOW for 4 x 10 milisec and so on...
 "S9L9l2Rs2T9rt9S9s9S9s9S9s "
,"S5L5l1Rs1T5rt5S5s5S5s5S5s "
,"S9s2L9l2RT9rt9RS9s9S9s9S9rs "
,"S5s1L5l1RT5rt5RS5s5S5s5S5rs "
,"TC99tc9.RT9rt9.L9l5LD9ld5D9d5DR9dr5RT9 "
,"TC99tc9.RT9rt9.L9l2LD9ld2D9d2DR9dr2RT9 "
,"TC99tc9.RT9rt9.L6l4LD6ld4D6d4DR6dr4RT6 "
,"TC99tc9.RT3rt3.L3l1LD3ld1D3d1DR3dr1RT3 "

};
//////////////////////////////////////////////////////////////////////////////////////////////

byte rowPins[ROWS] = {9,8 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 5, 4, 3}; //connect to the column pinouts of the keypad

const int buttonPin = 2;           // the number of the pushbutton pin
const int buzzPin = 7;             // the number of the BUZZER pin

const int upPin = 10;              // the number of the gamepad pins
const int downPin = 11;            
const int leftPin = 12;            
const int rightPin = 13;       
const int squarePin = A0;        
const int trianglePin = A1;       
const int circlePin = A2;       
const int forkPin = A3;        

Keypad cusomKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int buttonState = 0;          // variable for reading the pushbutton status

void setup()
{
  pinMode(buttonPin, INPUT);     
  pinMode(buzzPin, OUTPUT); 
  pinMode(upPin, OUTPUT); 
  pinMode(downPin, OUTPUT); 
  pinMode(leftPin, OUTPUT); 
  pinMode(rightPin, OUTPUT); 
  pinMode(squarePin, OUTPUT); 
  pinMode(trianglePin, OUTPUT); 
  pinMode(circlePin, OUTPUT); 
  pinMode(forkPin, OUTPUT); 

  Serial.begin(9600);
}

void func_Combo(char *str)
{
 int i=0;
 for(i=0;str[i]!=' ';i++)
  {
   if(str[i]=='1' || str[i]=='2' || str[i]=='3' || str[i]=='4' || str[i]=='5' || str[i]=='6' || 
      str[i]=='7' || str[i]=='8' || str[i]=='9')  
    {
      delay((int(str[i])-48)*10);
    }
   else
    switch(str[i])
    {
     case 'U' : digitalWrite(upPin,HIGH); break; 
     case 'D' : digitalWrite(downPin,HIGH); break; 
     case 'L' : digitalWrite(leftPin,HIGH); break; 
     case 'R' : digitalWrite(rightPin,HIGH); break; 
     case 'S' : digitalWrite(squarePin,HIGH); break; 
     case 'T' : digitalWrite(trianglePin,HIGH); break; 
     case 'C' : digitalWrite(circlePin,HIGH); break; 
     case 'F' : digitalWrite(forkPin,HIGH); break; 
    
     case 'u' : digitalWrite(upPin,LOW); break; 
     case 'd' : digitalWrite(downPin,LOW); break; 
     case 'l' : digitalWrite(leftPin,LOW); break; 
     case 'r' : digitalWrite(rightPin,LOW); break; 
     case 's' : digitalWrite(squarePin,LOW); break; 
     case 't' : digitalWrite(trianglePin,LOW); break; 
     case 'c' : digitalWrite(circlePin,LOW); break; 
     case 'f' : digitalWrite(forkPin,LOW); break; 
    }
  }
  digitalWrite(upPin,LOW); 
  digitalWrite(downPin,LOW); 
  digitalWrite(leftPin,LOW);
  digitalWrite(rightPin,LOW);
  digitalWrite(squarePin,LOW);
  digitalWrite(trianglePin,LOW); 
  digitalWrite(circlePin,LOW); 
  digitalWrite(forkPin,LOW);
}
  
void loop()
{
  char customKey = cusomKeypad.getKey();
  buttonState = digitalRead(buttonPin);

  
  if (customKey != NO_KEY)
  {
    switch(customKey)
    {
      case '1': func_Combo(combo[0]); break;
      case '2': func_Combo(combo[1]); break;
      case '3': func_Combo(combo[2]); break;
      case '4': func_Combo(combo[3]); break;
      case '5': func_Combo(combo[4]); break;
      case '6': func_Combo(combo[5]); break;
      case '7': func_Combo(combo[6]); break;
      case '8': func_Combo(combo[7]); break;
    }  
  }
}
