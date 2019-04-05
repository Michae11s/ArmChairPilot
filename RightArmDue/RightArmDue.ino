#include <Keyboard.h>
#include <Wire.h>

/*
 * Code to utilize a switch board to send keypresses to control DCS
 * 4 Mar 2019
 * Due, Arduino 1.8.9
 * Written By Michae1s
 */

//shorthand cuz I'm lazy
#define ky Keyboard
#define rA  releaseAll

//create buttons
#define GroundPower 52
#define AirSupply   48
#define WheelChock  44
#define Rearm       40

//define keys
#define LCTRL  0x80
#define LSHFT  0x81
#define LALT   0x82

//I2C defines
#define ADDR      0x20
#define LEFTARM   0x21
#define SYNC      0x99
#define D_MSK  0b11100000
#define P_MSK  0b00011110
#define O_MSK  0b00000001

#define db_delay    45

bool btnState[60]; //true btn up, false btn down

bool GPon = false;
bool ASon = false;
bool WCon = true;

#define UP     0
#define HALF   1
#define FULL   2
uint8_t Flaps = HALF;

void setup()
{
   //start i2c
   Wire.begin(0x20);

   //start ky emulation
   ky.begin();

   //Setup pull up pins
   pinMode(GroundPower, INPUT_PULLUP);
   pinMode(AirSupply, INPUT_PULLUP);
   pinMode(WheelChock, INPUT_PULLUP);
   pinMode(Rearm, INPUT_PULLUP);

   //initalize btnState
   for (int i = 0; i < 60; i++)
      btnState[i]=true;

   //i2c sync
   sync(LEFTARM);

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
      ky.write(0x5C); //write \\ coms menu
      delay(100);
      ky.write(0xC9); //write F8 ground crew
      delay(100);
      ky.write(0xC3); //write F2 ground power
      delay(100);
      ky.write(0xC2); //write F1 connect
      GPon=true;
    }
    else
    {
      ky.write(0x5C); //write \\ coms menu
      delay(100);
      ky.write(0xC9); //write F8 ground crew
      delay(100);
      ky.write(0xC3); //write F2 ground power
      delay(100);
      ky.write(0xC3); //write F2 disconnect
      GPon=false;
    }
  }

  //AirSupply
  if(pressed(AirSupply))
  {
    if(!ASon)
    {
      ky.write(0x5C); //write \\ coms menu
      delay(100);
      ky.write(0xC9); //write F8 ground crew
      delay(100);
      ky.write(0xC6); //write F5 AirSupply
      delay(100);
      ky.write(0xC2); //write F1 connect
      ASon=true;
    }
    else
    {
      ky.write(0x5C); //write \\ coms menu
      delay(100);
      ky.write(0xC9); //write F8 ground crew
      delay(100);
      ky.write(0xC6); //write F5 AirSupply
      delay(100);
      ky.write(0xC3); //write F2 disconnect
      ASon=false;
    }
  }

  //wheel chocks
  if(pressed(WheelChock))
  {
    if(!WCon)
    {
      ky.write(0x5C); //write \\ coms menu
      delay(100);
      ky.write(0xC9); //write F8 ground crew
      delay(100);
      ky.write(0xC5); //write F4 WheelChocks
      delay(100);
      ky.write(0xC2); //write F1 place
      WCon=true;
    }
    else
    {
      ky.write(0x5C); //write \\ coms menu
      delay(100);
      ky.write(0xC9); //write F8 ground crew
      delay(100);
      ky.write(0xC5); //write F4 WheelChocks
      delay(100);
      ky.write(0xC3); //write F2 remove
      WCon=false;
    }
  }

  //Rearm
  if(pressed(Rearm))
  {
    ky.press(0x82); //LAlt
    ky.press(0x27); //'
    kdy;
    ky.releaseAll();
  }

  Wire.requestFrom(LEFTARM, 32);
  i2c_decode();
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

void sync(int address)
{
   Wire.beginTransmission(address);
   Wire.write(SYNC);
   Wire.endTransmission();
   Wire.requestFrom(address, 32);
   i2c_decode();
}

void i2c_decode()
{
   uint8_t byt;
   uint8_t device;
   uint8_t pin;
   uint8_t state;
   while(Wire.available())
   {
      byt=Wire.read();
      device=(byt & D_MSK) >> 5;
      pin=(byt & P_MSK) >> 1;
      state=byt & O_MSK;
      switch(device)
      {
      case 0x01:
         switch(pin)
         {
         case 5: //Gear Lever
            if(state) //gear lever down
            {
               ky.press(LCTRL);
               ky.press('G');
               kdy();
               ky.releaseAll();
            }
            else //gear lever up
            {
               ky.press(LSHFT);
               ky.press('G');
               kdy();
               ky.releaseAll();
            }
            break;
         case 6: //flaps Upper half
            if(state) //Flaps UP
            {
               ky.press(LSHFT);
               ky.press('F');
               kdy();
               ky.rA();
               Flaps = UP;
            }
            else if(Flaps != FULL) //if flaps are full don't do anything
            {
               ky.press(LALT);
               ky.press('F');
               kdy();
               ky.rA();
               Flaps = HALF;
            }
            break;
         case 7: //flaps lower half
            if(state) //Flaps Full
            {
               ky.press(LCTRL);
               ky.press('F');
               kdy();
               ky.rA();
               Flaps = FULL;
            }
            else if(Flaps != UP) //if flaps are full don't do anything
            {
               ky.press(LALT);
               ky.press('F');
               kdy();
               ky.rA();
               Flaps = HALF;
            }
            break;
         case 8: //Jett
            if(state)
            {
               Keyboard.write('F');
            }
            break;
         }
         break;
      }
   }
}

void kdy()
{
   delay(50);
}
