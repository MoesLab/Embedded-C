///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   Project Name: BeachBomBox 
//         Author: Moe Azizi
//Date & revision: May 2012 V1.0
//           Note: A simple firmware application for my waterproof stereo system that uses MSGEQ7 to filter audio frequency
// and play colour spectrum on its RGB LEDs 
// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//the I/O Skelton of BeachBomBox 
#include "SPI.h"
#include "WS2801.h"
#include "MsTimer2.h"
#include "OneButton.h"

const int KP_Menu_Pin = 2;  //Keypad Botton MENU     
const int Mic_Pin = 3;      //TSSR Mic Function
const int BF_RST_Pin = 4;   //Band Filter Reset    
const int BF_STB_Pin = 5;   //Band Filter Strobe     
const int CKI_R_Pin = 6;    //Light Right CKI    
const int SDI_L_Pin = 7;    //Light Left SDI    
const int SDI_R_Pin = 8;    //Light Right SDI    
const int CKI_L_Pin = 9;    //Light Left CKI      
const int DS_DQ_Pin = 10;   //Digital Vol Data     
const int DS_RST_Pin = 11;  //Digital Vol Reset   
const int DS_CLK_Pin = 12;  //Digital Vol Clock  
const int IR_Pin = 13;      //IR Receiver
const int KP_Minus_Pin = A3;//Keypad Botton Minus     
const int KP_Plus_Pin = A2; //Keypad Botton Plus    
const int KP_DN_Pin = A1;   //Keypad Botton Down  
const int KP_UP_Pin = A0;   //Keypad Botton Up  
const int SDA_Pin = A6;     //I2C communication
const int SCL_Pin = A7;     //I2C communication
const int BF_L_Pin = A4;    //Band Filter Analog In Left
const int BF_R_Pin = A5;    //Band Filter Analog In Right

int spectrumValue[7];       // 7 frequency bands 63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25kHz and 16kHz
byte StripColor[3]={0,0,0};
int WiperIndex=0;
int Light_Mode=0;
 
// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
int filterValue = 130;
unsigned short Vol_Val=150; // Volume Value

// 2 strips on the left and right (stereo system)
_WS2801 strip_R = Adafruit_WS2801(6, SDI_R_Pin, CKI_R_Pin);/////////////////////////////////////length
_WS2801 strip_L = Adafruit_WS2801(6, SDI_L_Pin, CKI_L_Pin);/////////////////////////////////////length

// Menu Bottons to control volume and other functions
OneButton KP_Menu(KP_Menu_Pin, false);
OneButton KP_Minus(KP_Minus_Pin, true);
OneButton KP_Plus(KP_Plus_Pin, true);

///////////////////////////////////////////////////////// initiation 
void setup()
{
  pinMode(KP_Menu_Pin,INPUT);  
  pinMode(Mic_Pin,OUTPUT); 
  pinMode(BF_RST_Pin,OUTPUT);   
  pinMode(BF_STB_Pin,OUTPUT);    
  pinMode(CKI_R_Pin,OUTPUT);   
  pinMode(SDI_L_Pin,OUTPUT);  
  pinMode(SDI_R_Pin,OUTPUT);   
  pinMode(CKI_L_Pin,OUTPUT);       
  pinMode(DS_DQ_Pin,OUTPUT);     
  pinMode(DS_RST_Pin,OUTPUT);   
  pinMode(DS_CLK_Pin,OUTPUT);  
  pinMode(IR_Pin,INPUT); 
  pinMode(KP_Minus_Pin,INPUT);     
  pinMode(KP_Plus_Pin,INPUT);   
  pinMode(KP_DN_Pin,INPUT); 
  pinMode(KP_UP_Pin,INPUT); 
  pinMode(SDA_Pin,OUTPUT); 
  pinMode(SCL_Pin,OUTPUT); 
  pinMode(BF_L_Pin,INPUT); 
  pinMode(BF_R_Pin,INPUT); 

  Serial.begin(9600);
  analogReference(INTERNAL); // 5V

  MsTimer2::set(20, getSpectrum); // ms sampling period 
  MsTimer2::start(); 
  strip_L.begin();
  strip_R.begin();

  // link the doubleclick function to be called on a doubleclick event.   
  KP_Menu.attachClick(KP_Menu_click);
  KP_Menu.attachDoubleClick(KP_Menu_Dclick);
  KP_Menu.attachPress(KP_Menu_press);
  // vol -
  KP_Minus.attachClick(KP_Minus_click);
  KP_Minus.attachDoubleClick(KP_Minus_Dclick);
  KP_Minus.attachPress(KP_Minus_press);
  // vol +
  KP_Plus.attachClick(KP_Plus_click);
  KP_Plus.attachDoubleClick(KP_Plus_Dclick);
  KP_Plus.attachPress(KP_Plus_press);
 
   Set_Vol(Vol_Val);
  // Set startup values for pins
  digitalWrite(BF_RST_Pin, LOW);
  digitalWrite(BF_STB_Pin, HIGH);
  CLS(Color(0,0,0));
}
///////////////////////////////////////////////////////// circulation \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
void loop()
{
  // if there is any click
  KP_Menu.tick();
  KP_Minus.tick();
  KP_Plus.tick();
  if(KP_Plus.ButtState()==6 && Vol_Val<255)
    { Vol_Plus(2); }
  if(KP_Minus.ButtState()==6 && Vol_Val>0)
    { Vol_Minus(3); }

  getSpectrum();
  //if light show is activated
  if(Light_Mode!=0) Wiper();
  delay(10);
}
///////////////////////////////////////////////////////// get Spectrum values
void getSpectrum(void)
{
  if(Light_Mode!=0)
   {
     int sum=0;
     int inx=0;
     int j=0;
  
     // Set reset pin low to enable strobe
     digitalWrite(BF_RST_Pin, HIGH);
     digitalWrite(BF_RST_Pin, LOW);
 
     // Get all 7 spectrum values from the MSGEQ7
     for (int i = 0; i < 7; i++)
      {
        digitalWrite(BF_STB_Pin, LOW);
        delayMicroseconds(30); // Allow output to settle
 
        spectrumValue[i] = analogRead(BF_L_Pin);
        spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);
        spectrumValue[i] = map(spectrumValue[i],filterValue, 1023, 0, 255);
        digitalWrite(BF_STB_Pin, HIGH);
    
        sum+=spectrumValue[i];
        if(spectrumValue[i]>inx && i!=5) 
         { inx=spectrumValue[i]; j=i+64;}

       delayMicroseconds(18);  
      }
      // associate color to the bands
      switch(j)
      {
        case 'A': StripColor[0]=255  ;StripColor[1]=255 ;StripColor[2]=255 ;   //White
         break;
        case 'B': StripColor[0]=0 ;  StripColor[1]=0 ;StripColor[2]=255 ;      //Blue
         break; 
        case 'C': StripColor[0]=255 ;StripColor[1]=0 ;StripColor[2]=0 ;        //Red
         break; 
        case 'D': StripColor[0]=0 ;  StripColor[1]=255 ;StripColor[2]=0 ;      // Green
         break; 
        case 'F': StripColor[0]=255 ;StripColor[1]=255 ;StripColor[2]=0 ;      //Yellow
          break;
        default: StripColor[0]=0 ;   StripColor[1]=0 ;StripColor[2]=0 ;        //Off black
     
     }  
  }
}
///////////////////////////////////////////////////////////// set vol level 
void Set_Vol(unsigned short num1)
{

 unsigned short t, Flag, Mask;
 Mask = 0x80;
 digitalWrite(DS_RST_Pin,HIGH);  // Pull Reset line HIGH to initiate Data transfer
 digitalWrite(DS_DQ_Pin,LOW);    // Stack select bit 0
 digitalWrite(DS_CLK_Pin,HIGH);
 delayMicroseconds(1);
 digitalWrite(DS_CLK_Pin,LOW);

 // Shift in 8-bit wiper position for POT Right
 for (t=0; t<8; t++){
   Flag = num1 & Mask;
   if(Flag==0)  digitalWrite(DS_DQ_Pin,LOW);
   else  digitalWrite(DS_DQ_Pin,HIGH);
   digitalWrite(DS_CLK_Pin,HIGH);
   delayMicroseconds(1);
   digitalWrite(DS_CLK_Pin,LOW);
   Mask = Mask >> 1;
 }
 // Shift in 8-bit wiper position for POT Left
 Mask = 0x80;
 for (t=0; t<8; t++){
   Flag = num1 & Mask;
   if(Flag==0)  digitalWrite(DS_DQ_Pin,LOW);
   else  digitalWrite(DS_DQ_Pin,HIGH);
   digitalWrite(DS_CLK_Pin,HIGH);
   delayMicroseconds(1);
   digitalWrite(DS_CLK_Pin,LOW);
   Mask = Mask >> 1;
 }
 digitalWrite(DS_RST_Pin,LOW); // End Data transfer
 
}
//////////////////////////////////////////// Display color 
void Wiper() {
  int i;
  if(WiperIndex==1) 
   { 
    for (i=0; i < 6; i++)
    {
      strip_L.setPixelColor(i, Color(StripColor[0],StripColor[1],StripColor[2]));
      strip_R.setPixelColor(i, Color(StripColor[0],StripColor[1],StripColor[2]));
    }
    WiperIndex=0;
   }
  else  
   {
    for (i=0; i < 6; i++)
    {
      strip_L.setPixelColor(i, Color(0,0,0));
      strip_R.setPixelColor(i, Color(0,0,0));
    }
    WiperIndex++;
     
   }
    
  strip_L.show();
  strip_R.show();
}
//////////////////////////////////////////// clear color
void CLS(uint32_t c)
{
  int i;
  for (i=0; i < 6; i++)
    {
      strip_L.setPixelColor(i,c);
      strip_R.setPixelColor(i,c);
    }    
  strip_L.show();
  strip_R.show();
}
/////////////////////////////////////////// combining RGB values to a 32 bits word
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}
/////////////////////////////////////////// adding to volume level
void Vol_Plus(int i)
{
 if(Vol_Val<255-i) { Vol_Val+=i; Set_Vol(Vol_Val); }
 else { Vol_Val=255; Set_Vol(Vol_Val); }
 // Serial.println(Vol_Val);
}
/////////////////////////////////////////// subtractng from volume level
void Vol_Minus(int i)
{
 if(Vol_Val>i) { Vol_Val-=i; Set_Vol(Vol_Val); }
 else { Vol_Val=0; Set_Vol(Vol_Val); }
 //  Serial.println(Vol_Val);

}
////////////////////////////////////////// double click for menu
void KP_Menu_Dclick() {
 if(Light_Mode==1) 
  { Light_Mode=0; CLS(Color(0,0,0)); }    
 else Light_Mode++;
// Serial.println(Light_Mode);
} 
////////////////////////////////////////// single click for menu
void KP_Menu_press() {

 //Serial.println("Menu pressed");
} 
////////////////////////////////////////// single click for small decrease on vol level
void KP_Minus_click() {
  Vol_Minus(10);
}
////////////////////////////////////////// double click for larger decrease on vol level
void KP_Minus_Dclick() {
  Vol_Minus(30);
}
////////////////////////////////////////// single click for small increase on vol level
void KP_Plus_click() {
  Vol_Plus(10);
}
////////////////////////////////////////// double click for larger increase on vol level
void KP_Plus_Dclick() {
  Vol_Plus(30);
} 
