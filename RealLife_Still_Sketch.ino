#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h> // F Malpartida's NewLiquidCrystal library
#include <OneWire.h>
#include <DallasTemperature.h>

#define I2C_ADDR 0x3F // Define I2C Address for controller
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
#define BACKLIGHT 3
#define ONE_WIRE_BUS 2
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
const int chipSelect = 10; //cs or the save select pin from the sd shield is connected to 10.
RTC_DS1307 RTC;

OneWire oneWire(ONE_WIRE_BUS);
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
DallasTemperature cel(&oneWire);
File dataFile;
DateTime now;

//int val;             // initiate value variable (needed to LM35 temp prob) 
//int tempPin = 1;     // initiate Temp prob of A1 Pin (needed to LM35 temp prob)
int pump = 3;           // the PWM pin the pump is attached to
int pump_speed = 100;    // how fast the pump is going, used in showing the speed as a %
int pump_inc = 25.5;    // speed increment to fade or increase the pump by speed by eg 2.55 = 1% and 25.5 = 10%
int switch01 = 8;     //mode switch 
int HOTPLUG = 7 ;  // Pin to Control the HOT plug Relay
int COLDPLUG = 4 ;  // Pin to Control the Cold plug Relay
int M_REQTEMP = 15;    //  begining required temp for Mash mode (from 0 to 254) 
int S_REQTEMP = 60;    // begining required temp for Still mode (from 0 to 254)
int buttonPlus=6;     //  up button for Required Temp  
int buttonMinus=5;   //  down button for Required Temp
int counter = 0;     // Cycle counter for writing to SD card 


void setup(){

//setup clock
  Wire.begin();
  RTC.begin();
//check or the Real Time Clock is on
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compipump
    // uncomment it & upload to set the time, date and start run the RTC!
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
//setup SD card
   Serial.print("Initializing SD card...");

  // see if the SD card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card faipump, or not present");
    // don't do anything more: NOTE SD CARD MUST BE INSTALL OR THE PROGRAM WILL NOT CONTINUE  
    return;
  }
Serial.println("card initialized.");

  //write down the date (year / month / day         prints only the start, so if the logger runs for sevenal days you only findt the start back at the begin.
    now = RTC.now();
    dataFile = SD.open("datalog.txt", FILE_WRITE);
    dataFile.print("Start logging on: ");
    dataFile.print(now.year(),DEC);
    dataFile.print('/');
    dataFile.print(now.month(),DEC);
    dataFile.print('/');
    dataFile.print(now.day(),DEC);
    dataFile.println(" ");
    dataFile.println("Celsius              Time");
    

  
lcd.begin (20,4); // initialize the lcd 
// Switch on the backlight
lcd.setBacklightPin(BACKLIGHT,POSITIVE);
lcd.setBacklight(HIGH);
pinMode(pump, OUTPUT); 
// make the switch01's pin an input:
pinMode(switch01, INPUT); 
Serial.begin(9600);  //Start Serial logging 
pinMode(HOTPLUG, OUTPUT);  // Assign PIN is OUTPUT
pinMode(COLDPLUG, OUTPUT);  // Assign PIN is OUTPUT
cel.begin(); //beging onewire prob
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

  now = RTC.now(); //Read current time from clock
   dataFile = SD.open("datalog.txt", FILE_WRITE);   //open file to log data in.
    
  // if the file is available, write to it:
  // log the temperature and time.
  if (dataFile) {
    dataFile.print(" ");
    dataFile.print(now.hour(),DEC);
    dataFile.print(":");
    dataFile.print(now.minute(),DEC);
    dataFile.print(":");
    dataFile.println(now.second(),DEC);
    // print to the serial port too:
    //Serial.println("data stored");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    
  }

cel.requestTemperatures(); // Send the command to get temperatures from DallasTemperature prob
 
  
if (buttonState == 1 )   ///START OF STILL CODE
  {
  
  /* Serial.println();                     //Commented out to save space on running device                
   Serial.print("Running Mode = ");
    Serial.print("MASH");*/ 
  
lcd.setCursor(0,0);              //Set LCD position and print the mode status 
lcd.print("Mode = ");  
lcd.print("STILL");    
  
digitalWrite(HOTPLUG,HIGH);      // Set both of the 240v plugs to off incase moving from made to still
digitalWrite(COLDPLUG,HIGH); 
  
//cel.requestTemperatures(); // Send the command to get temperatures
  
  
float S_plusREQTEMP = (S_REQTEMP + 2);         /// SET upper and lower limit to be + or - X for REQTEMP for Still mode    
float S_minusREQTEMP = (S_REQTEMP - 2);   

/*Serial.println();                         //Commented out to save space on running device
Serial.print("REQTEMP = ");
Serial.print(S_REQTEMP);  
Serial.println(); 
Serial.print("+ REQTEMP = ");
Serial.print(S_plusREQTEMP);  
Serial.println();   
Serial.print("- REQTEMP = ");
Serial.print(S_minusREQTEMP);  */
  
/*Serial.println();                         //Commented out to save space on running device
Serial.print("TEMPRATURE = ");
Serial.print(cel);
Serial.print("*C");*/

Serial.println();                           //Print Temp to serial port
Serial.print("TEMPRATURE = ");
Serial.print(cel.getTempCByIndex(0));
Serial.print("*C");

  
Serial.println();                          //Print pump speed as % to serial port
float percent = (pump_speed / 2.55);
Serial.print("speed = ");
Serial.print(percent,0);  
Serial.print("%");
Serial.println();  
 
lcd.setCursor(0,1);                       //Print Temp and required temp to LCD Screen
lcd.print("T= "); 
lcd.print(cel.getTempCByIndex(0));      
lcd.print(" REQTEMP="); 
lcd.print(S_REQTEMP); 

lcd.setCursor(0,2);                       //Print pump speed as % to LCD Screen
lcd.print("Speed = "); 
lcd.print(percent,0);   
lcd.print("%");
  
dataFile.print("Temp= ");                  //Print Temp to SD card
dataFile.print(cel.getTempCByIndex(0));   

dataFile.print("Speed = ");                //Print pump speed as % to SD card
dataFile.print(percent,0);   
dataFile.print("%");

 if (cel.getTempCByIndex(0) > S_minusREQTEMP and cel.getTempCByIndex(0) < S_plusREQTEMP)   // Pump speed logic - if current Temp is between the lower and upper required temp don't change speed  
{
  analogWrite(pump, pump_speed); 
}
  
 else if (cel.getTempCByIndex(0) < S_minusREQTEMP)                                        // Pump speed logic - if current Temp is less than the required temp then minus the pump speed increment from the current pump speed (but not less than 0)
{
  analogWrite(pump, pump_speed);
  
    pump_speed = pump_speed - pump_inc;
 if (pump_speed <= 0)  { pump_speed = 0;}; // 'sets Lower limit   
    
}  
 else if (cel.getTempCByIndex(0) > S_plusREQTEMP)                       // Pump speed logic - if current Temp is greater than the required temp then add the pump speed increment to the current pump speed (but more than 255)
 {

   analogWrite(pump, pump_speed);
   
     pump_speed = pump_speed + pump_inc;
   if (pump_speed >= 255)  { pump_speed = 255;}; // 'sets upper limit
 
 }  
   
  } //// !!!!!! END OF STILL CODE !!!!!!!!!!!!!!!!
  
  
  
  else
  {
    

analogWrite(pump, 0);           //set the 12v pump to speed 0 incase moving from Still to Mash 

/* Serial.println();                    //Commented out to save space on running device
Serial.print("Running Mode = ");
Serial.print("MASH");
Serial.println();*/ 
    
float M_plusREQTEMP = (M_REQTEMP + 1); 
float M_minusREQTEMP = (M_REQTEMP - 1);   

/*Serial.println();           //Commented out to save space on running device
Serial.print("REQTEMP = ");
Serial.print(M_REQTEMP);  
Serial.println(); 
Serial.print("+ REQTEMP = ");
Serial.print(M_plusREQTEMP);  
Serial.println();   
Serial.print("- REQTEMP = ");
Serial.print(M_minusREQTEMP); */     
    
    
lcd.setCursor(0,0);                    //Set LCD position and print the mode status 
lcd.print("Mode = ");  
lcd.print("MASH");    
  
Serial.println();             //Print Temp to serial port
Serial.print("TEMPRATURE = ");
Serial.print(cel.getTempCByIndex(0));
Serial.print("*C");    

lcd.setCursor(0,1);                       //Print Temp and required temp to LCD Screen
lcd.print("T= "); 
lcd.print(cel.getTempCByIndex(0));  
lcd.print(" REQTEMP="); 
lcd.print(M_REQTEMP); 

    
if (cel.getTempCByIndex(0) < M_minusREQTEMP){ digitalWrite(HOTPLUG,LOW);      //240v relay logic - if current Temp is less than the required temp then turn on heating plug and turn off cooling plug 
                              digitalWrite(COLDPLUG,HIGH);
                              lcd.setCursor(0,2);
                              lcd.print("Heating ON ");                       //Print on LCD the status of the plug
                              Serial.println();
                              Serial.print(" H = ON ");                       //Print on serial port the status of the plug
                            }  
    
if (cel.getTempCByIndex(0) > M_minusREQTEMP and cel.getTempCByIndex(0) < M_plusREQTEMP) { digitalWrite(HOTPLUG,HIGH);   //240v relay logic - if current Temp is between the lower and upper required temp turn off both plugs  
                             digitalWrite(COLDPLUG,HIGH);
                             lcd.setCursor(0,2);
                             lcd.print("ALL OFF ");                           //Print on LCD the status of the plug
                             Serial.println(); 
                             Serial.print(" OFF ");                           //Print on serial port the status of the plug
                            }  

if (cel.getTempCByIndex(0) > M_plusREQTEMP){ digitalWrite(COLDPLUG,LOW);     //240v relay logic - if current Temp is greater than the required temp then on the cooling plug and turn off the heating plug 
                   digitalWrite(HOTPLUG,HIGH);
                   lcd.setCursor(0,2);
                   lcd.print("Cooling ON ");                                 //Print on LCD the status of the plug
                   Serial.println(); 
                     Serial.print(" C = ON ");                               //Print on serial port the status of the plug
                    }
 
counter = (counter + 1);                          //Set counter value to current +1 
Serial.println();  
    if (counter == 60) {counter = 0;
                      Serial.print("TEMPRATURE ON SD CARD2= ");             //If counter equals 60 write current temp to SD Card and reset the counter to 0 (SD Card only writes a temp value ~ once a min ) 
                      Serial.print(cel.getTempCByIndex(0));
                      }
    
  } //// !!!!!! END OF STILL CODE !!!!!!!!!!!!!!!!

  
  delay(1000);             
  //lcd.clear(); 
  dataFile.close();   // Close data file write ready for next run through 
}