#include <Keyboard.h>
#include <Wire.h>

/*
 * Code to utilize a switch board to send keypresses to control DCS
 * 4 Mar 2019
 * Due, Arduino 1.8.9
 * Written By Michae1s
 */

//create buttons
#define GroundPower 52
#define AirSupply   48
#define WheelChock  44
#define Rearm       40


#define db_delay    45

bool btnState[60]; //true btn up, false btn down

bool GPon = false;
bool ASon = false;
bool WCon = true;

void setup()
{
   //start i2c
   Wire.begin(0x20);
   Wire.

   //Setup pull up pins
   pinMode(GroundPower, INPUT_PULLUP);
   pinMode(AirSupply, INPUT_PULLUP);
   pinMode(WheelChock, INPUT_PULLUP);
   pinMode(Rearm, INPUT_PULLUP);

   //initalize btnState
   for (int i = 0; i < 60; i++){
    btnState[i]=true;
   }

   Keyboard.begin();
   //Serial.begin(9600);
   //Serial.println("start");
}

void loop()
{
  //GroundPower
  if(pressed(GroundPower))
  {
    if(!GPon)
    {
      Keyboard.write(0x5C); //write \\ coms menu
      delay(100);
      Keyboard.write(0xC9); //write F8 ground crew
      delay(100);
      Keyboard.write(0xC3); //write F2 ground power
      delay(100);
      Keyboard.write(0xC2); //write F1 connect
      GPon=true;
    }
    else
    {
      Keyboard.write(0x5C); //write \\ coms menu
      delay(100);
      Keyboard.write(0xC9); //write F8 ground crew
      delay(100);
      Keyboard.write(0xC3); //write F2 ground power
      delay(100);
      Keyboard.write(0xC3); //write F2 disconnect
      GPon=false;
    }
  }

  //AirSupply
  if(pressed(AirSupply))
  {
    if(!ASon)
    {
      Keyboard.write(0x5C); //write \\ coms menu
      delay(100);
      Keyboard.write(0xC9); //write F8 ground crew
      delay(100);
      Keyboard.write(0xC6); //write F5 AirSupply
      delay(100);
      Keyboard.write(0xC2); //write F1 connect
      ASon=true;
    }
    else
    {
      Keyboard.write(0x5C); //write \\ coms menu
      delay(100);
      Keyboard.write(0xC9); //write F8 ground crew
      delay(100);
      Keyboard.write(0xC6); //write F5 AirSupply
      delay(100);
      Keyboard.write(0xC3); //write F2 disconnect
      ASon=false;
    }
  }

  //wheel chocks
  if(pressed(WheelChock))
  {
    if(!WCon)
    {
      Keyboard.write(0x5C); //write \\ coms menu
      delay(100);
      Keyboard.write(0xC9); //write F8 ground crew
      delay(100);
      Keyboard.write(0xC5); //write F4 WheelChocks
      delay(100);
      Keyboard.write(0xC2); //write F1 place
      WCon=true;
    }
    else
    {
      Keyboard.write(0x5C); //write \\ coms menu
      delay(100);
      Keyboard.write(0xC9); //write F8 ground crew
      delay(100);
      Keyboard.write(0xC5); //write F4 WheelChocks
      delay(100);
      Keyboard.write(0xC3); //write F2 remove
      WCon=false;
    }
  }

  //Rearm
  if(pressed(Rearm))
  {
    Keyboard.press(0x82); //LAlt
    Keyboard.press(0x27); //'
    delay(50);
    Keyboard.releaseAll();
  }
}

bool pressed(int pin)
{
  if(!digitalRead(pin))//is button down
  {
    if(btnState[pin]){ //button was up
      delay(db_delay);
      btnState[pin]=false;
      return true;
    }
    else return false;//btn was down do nothing
  }
  else //button is up
  {
    if(btnState[pin]){//btn was up do nothing
      return false;
    }
    else //btn was down lets debounce
    {
      delay(db_delay);
      btnState[pin]=true;
      return false;
    }
  }
}
