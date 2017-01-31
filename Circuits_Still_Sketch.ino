

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

int val;
int tempPin = 1;
int pump = 3;           // the PWM pin the pump is attached to
int pump_speed = 100;    // how bright the pump is
int pump_inc = 25.5;    // how many points to fade the pump by  2.55
int switch01 = 2;
int HOTPLUG = 7 ;  // Pin to Control the HOT plug Relay
int COLDPLUG = 4 ;  // Pin to Control the Cold plug Relay
int M_REQTEMP = 15;    //  begining required temp for Mash mode (from 0 to 254) 
int S_REQTEMP = 60;    // begining required temp for Still mode (from 0 to 254)    
int buttonPlus=6;    
int buttonMinus=5;   
int counter = 0;


void setup()
{
lcd.begin(16, 2);  
pinMode(pump, OUTPUT); 
// make the switch01's pin an input:
pinMode(switch01, INPUT); 
Serial.begin(9600);  
pinMode(HOTPLUG, OUTPUT);	// Assign PIN is OUTPUT
pinMode(COLDPLUG, OUTPUT);	// Assign PIN is OUTPUT
}


void loop()
{

int buttonState = digitalRead(switch01); 
 
  if (digitalRead(buttonPlus) == HIGH) { 
   M_REQTEMP   +=   2;                           //Check the statue of the up button and increase the required Temperature by X for Mash mode
  }   
  if (digitalRead(buttonMinus) == HIGH) { 
   M_REQTEMP -=  2;                              //Check the statue of the up button and decrease the required Temperature by X for Mash mode
  }    
  M_REQTEMP = constrain(M_REQTEMP, 0, 150);      //Set upper and lower limit to required Temperature for Mash mode
 
   if (digitalRead(buttonPlus) == HIGH) { 
   S_REQTEMP   +=   1;                          //Check the statue of the up button and increase the required Temperature by X for Still mode
  }   
  if (digitalRead(buttonMinus) == HIGH) { 
   S_REQTEMP -=  1;               //Check the statue of the up button and decrease the required Temperature by X for Still mode
  }    
  S_REQTEMP = constrain(S_REQTEMP, 0, 150);   //Set upper and lower limit to required Temperature for Still mode
  
  
if (buttonState == 1 )   ///START OF STILL CODE
  {
   Serial.println();
   Serial.print("Running Mode = ");
    Serial.print("STILL");
  
lcd.setCursor(0,0);
lcd.print("Mode = ");  
lcd.print("STILL");    
  
digitalWrite(HOTPLUG,HIGH); 
digitalWrite(COLDPLUG,HIGH); 
  
  
val = analogRead(tempPin);
float mv = ( val/1024.0)*5000;
float cel = mv/10;
float farh = (cel*9)/5 + 32;
  
/// SET upper and lower limit to be + or - X for REQTEMP
  
float S_plusREQTEMP = (S_REQTEMP + 2); 
float S_minusREQTEMP = (S_REQTEMP - 2);   

Serial.println(); 
Serial.print("REQTEMP = ");
Serial.print(S_REQTEMP);  
Serial.println(); 
Serial.print("+ REQTEMP = ");
Serial.print(S_plusREQTEMP);  
Serial.println();   
Serial.print("- REQTEMP = ");
Serial.print(S_minusREQTEMP);  
  
Serial.println(); 
Serial.print("TEMPRATURE = ");
Serial.print(cel);
Serial.print("*C");
  
Serial.println();  
float percent = (pump_speed / 2.55);
Serial.print("speed = ");
Serial.print(percent,0);  
Serial.print("%");
Serial.println();  
 
lcd.setCursor(0,1);
lcd.print("T= "); 
lcd.print(cel);  
//lcd.print("*C");      
lcd.print(" S= "); 
lcd.print(percent,0);   
lcd.print("%");
  
   

 if (cel > S_minusREQTEMP and cel < S_plusREQTEMP)     
{
  analogWrite(pump, pump_speed); 
}
  
 else if (cel < S_minusREQTEMP) 
{
  analogWrite(pump, pump_speed);
  
    pump_speed = pump_speed - pump_inc;
 if (pump_speed <= 0)  { pump_speed = 0;}; // 'sets Lower limit   
    
}  
 else if (cel > S_plusREQTEMP) 
 {

   analogWrite(pump, pump_speed);
   
     pump_speed = pump_speed + pump_inc;
   if (pump_speed >= 255)  { pump_speed = 255;}; // 'sets upper limit
 
 }  
   
  } //// !!!!!! END OF STILL CODE !!!!!!!!!!!!!!!!
  
  
  
  else
  {
    
   val = analogRead(tempPin);
float mv = ( val/1024.0)*5000;
float cel = mv/10;
float farh = (cel*9)/5 + 32;
   
analogWrite(pump, 0);
Serial.println();
Serial.print("Running Mode = ");
Serial.print("MASH");
Serial.println();
    
float M_plusREQTEMP = (M_REQTEMP + 5); 
float M_minusREQTEMP = (M_REQTEMP - 5);   

Serial.println(); 
Serial.print("REQTEMP = ");
Serial.print(M_REQTEMP);  
Serial.println(); 
Serial.print("+ REQTEMP = ");
Serial.print(M_plusREQTEMP);  
Serial.println();   
Serial.print("- REQTEMP = ");
Serial.print(M_minusREQTEMP);      
    
    
lcd.setCursor(0,0);
lcd.print("Mode = ");  
lcd.print("MASH");    
  
Serial.println(); 
Serial.print("TEMPRATURE = ");
Serial.print(cel);
Serial.print("*C");    
lcd.setCursor(0,1);
lcd.print("T= "); 
lcd.print(cel);  
    
 if (cel < M_minusREQTEMP){ digitalWrite(HOTPLUG,LOW);
                              digitalWrite(COLDPLUG,HIGH);
                              lcd.print(" H = ON ");
                              Serial.println();
                              Serial.print(" H = ON ");
                            }  
    
if (cel > M_minusREQTEMP and cel < M_plusREQTEMP) { digitalWrite(HOTPLUG,HIGH);
                             digitalWrite(COLDPLUG,HIGH);
                             lcd.print(" OFF ");
                             Serial.println(); 
                             Serial.print(" OFF "); 
                            } 

if (cel > M_plusREQTEMP){ digitalWrite(COLDPLUG,LOW);
     						   digitalWrite(HOTPLUG,HIGH);
                               lcd.print(" C = ON ");
   							   Serial.println(); 
         					   Serial.print(" C = ON ");
    					      }
 
counter = (counter + 1);
Serial.println();  
    if (counter == 60) {counter = 0;
                      Serial.print("TEMPRATURE ON SD CARD= ");
                      Serial.print(cel);
                      }
   
  } //// !!!!!! END OF STILL CODE !!!!!!!!!!!!!!!!
  
  delay(1000); 
  lcd.clear(); 
}
