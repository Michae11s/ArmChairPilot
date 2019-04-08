#include <Keyboard.h>
#include <Wire.h>
#include <Debounce.h>
#include <Buzzer.h>

/*
 * Code to utilize a switch board to send keypresses to control DCS
 * 4 Mar 2019
 * Due, Arduino 1.8.9
 * Written By Michae1s
 */

//laziness
#define ky Keyboard

//create buttons
#define GroundPower 52
#define AirSupply   48
#define WheelChock  44
#define Rearm       40

//define keys
#define LCTRL  0x80
#define LSHFT  0x81
#define LALT   0x82
#define KEY_DELAY 50

//I2C defines
#define LEFTARM   0x21
#define SYNC      0x99
#define D_MSK  0b11100000
#define P_MSK  0b00011110
#define O_MSK  0b00000001

#define db_delay    45

bool GPon = false;
bool ASon = false;
bool WCon = true;

//flaps vars
#define UP     0
#define HALF   1
#define FULL   2
uint8_t Flaps = HALF;

debounce<65> db;
Buzzer<15000> rset;

void setup()
{
   //start i2c
   Wire.begin();

   //start ky emulation
   ky.begin();

   //Setup pull up pins
   pinMode(GroundPower, INPUT_PULLUP);
   pinMode(AirSupply, INPUT_PULLUP);
   pinMode(WheelChock, INPUT_PULLUP);
   pinMode(Rearm, INPUT_PULLUP);

   //initalize btnState
   for (int i = 0; i < 60; i++)
      db.change(i);

   //i2c sync
   sync(LEFTARM);

   // Serial.begin(9600);
   // Serial.println("start");
}

void loop()
{
   //GroundPower
   if(db.change(GroundPower) == CLOSE)
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
   if(db.change(AirSupply) == CLOSE)
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
   if(db.change(WheelChock) == CLOSE)
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
   if(db.change(Rearm) == CLOSE)
   {
     send(LALT, 0x27); //LALT + '
   }


   Wire.requestFrom(LEFTARM, 16);
   i2c_decode();
   delay(50);
}

void sync(int address)
{
   Wire.beginTransmission(address);
   Wire.write(SYNC);
   Wire.endTransmission();
   Wire.requestFrom(address, 16);
   i2c_decode();
}

void i2c_decode()
{
   uint8_t byt;
   uint8_t device;
   uint8_t pin;
   uint8_t status;
   while(Wire.available())
   {
      byt=Wire.read();
      device=(byt & D_MSK) >> 5;
      pin=(byt & P_MSK) >> 1;
      status=byt & O_MSK;
      switch(device)
      {
      case (LEFTARM & 0x0F):
         switch(pin)
         {
         case 9: //APU start
            if(status == CLOSE)
               send(LCTRL, 'S');
            break;
         case 10: //MISC START
            if(status == CLOSE)
               send(LALT, 'S');
            break;
         case 14: //L Crank
            if(status == CLOSE)
               send(LCTRL, 'O');
            break;
         case 15: //R Crank
            if(status == CLOSE)
               send(LALT, 'O');
            break;
         case 11: //L FUEL
            if(status == CLOSE)
               send(LCTRL, 'H');
            else
               send(LCTRL, LSHFT, 'H');
            break;
         case 12: //R FUEL
            if(status == CLOSE)
               send(LALT, 'H');
            else
               send(LALT, LSHFT, 'H');
            break;
         case 4: //Master Arm
            if(status == CLOSE)
               send(LALT, 'A');
            else
               send(LCTRL, 'A');
            break;
         case 3: //ECM JETT
            if(status == CLOSE)
               send(LCTRL, 'J');
            break;
         case 2: //EJETT
            if(status == CLOSE)
               send(LCTRL, LSHFT, 'J');
            break;
         case 5: //Gear Lever
            if(status == CLOSE) //gear leverdb.DOWN
               send(LCTRL, 'G');
            else //gear lever up
               send(LSHFT, 'G');
            break;
         case 6: //flaps Upper half
            if(status) //Flaps UP
            {
               send(LSHFT, 'F');
               Flaps = UP;
            }
            else if(Flaps != FULL) //if flaps are full don't do anything
            {
               send(LALT, 'F');
               Flaps = HALF;
            }
            break;
         case 7: //flaps lower half
            if(status) //Flaps Full
            {
               send(LCTRL, 'F');
               Flaps = FULL;
            }
            else if(Flaps != UP) //if flaps are up don't do anything
            {
               send(LALT, 'F');
               Flaps = HALF;
            }
            break;
         case 8: //Jett
            if(status)
               send(LSHFT, 'J');
            break;
         }
         break;
      }
   }
}

void send(char key)
{
   Keyboard.press(key);
   delay(KEY_DELAY);
   Keyboard.releaseAll();
}

void send(char Mod, char key)
{
   Keyboard.press(Mod);
   Keyboard.press(key);
   delay(KEY_DELAY);
   Keyboard.releaseAll();
}

void send(char Mod,char Modd, char key)
{
   Keyboard.press(Mod);
   Keyboard.press(Modd);
   Keyboard.press(key);
   delay(KEY_DELAY);
   Keyboard.releaseAll();
}
