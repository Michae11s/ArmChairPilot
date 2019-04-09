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
//#define ky Keyboard

//define pins
#define PINS         55 //total Num of Pins

//two way latching
#define HSD       4  //UP=CLOSE
#define TV        7  //UP=OPEN
#define ANTI_COL  8  //UP=OPEN
#define TAXI      9  //UP=OPEN
#define HOOK      10 //UP=CLOSE
#define NAV2      6  //UP=CLOSE

//push buttons
#define HUD1      36
#define HUD2      37
#define HUD3      39
#define HUD4      41
#define HUD5      38
#define AUX1      53
#define AUX2      50
#define AUX3      48
#define AUX4      46
#define AUX5      52
#define AUX6      51
#define AUX7      44
#define AUX8      47
#define AUX9      49
#define AUX10     45

//3 way momentary
#define R_MOMUP   32
#define R_MOMDN   33
#define L_MOMUP   34
#define L_MOMDN   35
#define FORM_UP   31
#define FORM_DN   40
#define XTRA_UP   23
#define XTRA_DN   22

//3 way toggles
#define NAV_UP    28
#define NAV_DN    29
#define L_TOG_UP  26
#define L_TOG_DN  27
#define R_TOG_UP  24
#define R_TOG_DN  25

//ejection
#define EJECT     43


//define keys
#define LCTRL  0x80
#define LSHFT  0x81
#define LALT   0x82
#define RCTRL  0x84
#define RALT   0x86
#define RSHFT  0x85
#define KEY_DELAY 150

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

//nav vars
#define BRIGHT 0
#define OFF    1
#define DIM    2
uint8_t navl = OFF;

//R/L toggle vars
#define DOWN   2
uint8_t rtog = OFF;
uint8_t ltog = OFF;

//flaps vars
#define UP     0
#define HALF   1
#define FULL   2
uint8_t Flaps = HALF;

//map
#define FIRST  0
#define THIRD  1
#define F10    2
uint8_t view = 0;

debounce<PINS> db;
Buzzer<100> i2cCheck;

void setup()
{
   //start i2c
   Wire.begin();

   //start Serial
   // Serial.begin(9600);
   // Serial.println("Serial Begin");

   //start ky emulation
   Keyboard.begin();

   //Setup pull up pins

   //initalize btnState
   for (int i = 4; i < PINS; i++)
   {
      if(i != 20 && i != 21)
      {
         pinMode(i, INPUT_PULLUP);
         db.init(i);
      }
   }

   //i2c sync
   //sync(LEFTARM);

   // Keyboard.write('S');
   // Keyboard.write('T');
   // Keyboard.write('A');
   // Keyboard.write('R');
   // Keyboard.write('T');
}

void loop()
{
   switch(db.change(EJECT))
   {
   case OPEN:
      Keyboard.press(LCTRL);
      delay(50);
      Keyboard.press('e');//1
      delay(50);
      Keyboard.release('e');
      delay(50);
      Keyboard.press('e');//2
      delay(50);
      Keyboard.release('e');
      delay(50);
      Keyboard.press('e');//3
      delay(50);
      Keyboard.release('e');
      delay(50);
      Keyboard.releaseAll();

   }

   switch(db.change(AUX1)) //GroundPower on/off
   {
   case CLOSE:
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
      break;
   }

   switch(db.change(AUX2)) //AirSupply on/off
   {
   case CLOSE:
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

   switch(db.change(AUX3)) //WheelChocks on/off
   {
   case CLOSE:
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

   switch(db.change(AUX4)) //open rearming panel
   {
   case CLOSE:
      send(LALT, 0x27); //LALT + '
      break;
   }

   switch(db.change(AUX5)) //control the view
   {
   case CLOSE:
      switch(view)
      {
      case FIRST:
         Keyboard.write(0xC3); //F2
         view=THIRD;
         break;
      case THIRD:
         Keyboard.write(0xCB); //F10
         view=F10;
         break;
      case F10:
         Keyboard.write(0xC2); //F1
         view=FIRST;
         break;
      }
   }

   switch(db.change(AUX6))
   {
   case CLOSE:
      send(RCTRL, RALT, '6');
      break;
   }

   switch(db.change(AUX7))
   {
   case CLOSE:
      send(RCTRL, RALT, '7');
      break;
   }

   switch(db.change(AUX8))
   {
   case CLOSE:
      send(RCTRL, RALT, '8');
      break;
   }

   switch(db.change(AUX9))
   {
   case CLOSE:
      send(RCTRL, RALT, '9');
      break;
   }

   switch(db.change(AUX10))
   {
   case CLOSE:
      send(RCTRL, RALT, '0');
      break;
   }

   switch(db.change(HOOK))
   {
   case CLOSE:
      send(LALT, 'h');
      break;
   case OPEN:
      send(LCTRL, 'h');
      break;
   }

   switch(db.change(HUD1))
   {
   case CLOSE:
      send(LSHFT, '1');
      break;
   }

   switch(db.change(HUD2))
   {
   case CLOSE:
      send(LSHFT, '2');
      break;
   }

   switch(db.change(HUD3))
   {
   case CLOSE:
      send(LSHFT, '3');
      break;
   }

   switch(db.change(HUD4))
   {
   case CLOSE:
      send(LSHFT, '4');
      break;
   }

   switch(db.change(HUD5))
   {
   case CLOSE:
      send(LSHFT, '5');
      break;
   }

   switch(db.change(TV))
   {
   case CLOSE:
      send(LCTRL, 't');
      break;
   case OPEN:
      send(LALT, 't');
      break;
   }

   switch(db.change(HSD))
   {
   case CLOSE:
      send(LCTRL, 'd');
      break;
   case OPEN:
      send(LALT, 'd');
      break;
   }

   switch(db.change(ANTI_COL))
   {
   case CLOSE:
      send(LCTRL, 'l');
      break;
   case OPEN:
      send(LALT, 'l');
      break;
   }

   switch(db.change(TAXI))
   {
   case CLOSE:
      send(RCTRL, 'l');
      break;
   case OPEN:
      send(RALT, 'l');
      break;
   }

   switch(db.change(NAV2))
   {
   case CLOSE:
      send(LCTRL, 'n');
      break;
   case OPEN:
      send(LALT, 'n');
      break;
   }

   switch(db.change(NAV_UP))
   {
   case CLOSE:
      send(RCTRL, 'n');
      navl=BRIGHT;
      break;
   case OPEN:
      if(navl != DIM)
      {
         send(RCTRL, RALT, 'n');
         navl=OFF;
      }
      break;
   }

   switch(db.change(NAV_DN))
   {
   case CLOSE:
      send(RALT, 'n');
      navl=DIM;
      break;
   case OPEN:
      if(navl != BRIGHT)
      {
         send(RCTRL, RALT, 'n');
         navl=OFF;
      }
      break;
   }

   switch(db.change(R_TOG_UP))
   {
   case CLOSE:
      send(RCTRL, 'i');
      rtog=UP;
      break;
   case OPEN:
      if(rtog != DOWN)
      {
         send(RCTRL, RALT, 'i');
         rtog=OFF;
      }
      break;
   }

   switch(db.change(R_TOG_DN))
   {
   case CLOSE:
      send(RALT, 'i');
      rtog=DOWN;
      break;
   case OPEN:
      if(rtog != UP)
      {
         send(RCTRL, RALT, 'i');
         rtog=OFF;
      }
      break;
   }

   switch(db.change(L_TOG_UP))
   {
   case CLOSE:
      send(LCTRL, 'i');
      ltog=UP;
      break;
   case OPEN:
      if(ltog != DOWN)
      {
         send(LCTRL, LALT, 'i');
         ltog=OFF;
      }
      break;
   }

   switch(db.change(L_TOG_DN))
   {
   case CLOSE:
      send(LALT, 'i');
      ltog=DOWN;
      break;
   case OPEN:
      if(ltog != UP)
      {
         send(LCTRL, LALT, 'i');
         ltog=OFF;
      }
      break;
   }

   switch(db.change(FORM_DN))
   {
   case CLOSE:
      send(LCTRL, 'r');
      break;
   }

   switch(db.change(FORM_UP))
   {
   case CLOSE:
      send(LALT, 'r');
      break;
   }

   switch(db.change(L_MOMDN))
   {
   case CLOSE:
      //send(LCTRL, 'm');
      Keyboard.press(LCTRL);
      Keyboard.press('m');
      break;
   case OPEN:
      Keyboard.releaseAll();
      break;
   }

   switch(db.change(L_MOMUP))
   {
   case CLOSE:
      //send(LALT, 'm');
      Keyboard.press(LALT);
      Keyboard.press('m');
      break;
   case OPEN:
      Keyboard.releaseAll();
      break;
   }

   switch(db.change(R_MOMDN))
   {
   case CLOSE:
      send(RCTRL, 'm');
      break;
   }

   switch(db.change(R_MOMUP))
   {
   case CLOSE:
      send(RALT, 'm');
      break;
   }

   switch(db.change(XTRA_DN))
   {
   case CLOSE:
      send(LCTRL, LALT, 'm');
      break;
   }

   switch(db.change(XTRA_UP))
   {
   case CLOSE:
      send(RCTRL, RALT, 'm');
      break;
   }

   if(i2cCheck.Alrm())
   {
      Wire.requestFrom(LEFTARM, 16);
      i2c_decode();
   }

   // Keyboard.write('T');
   // Keyboard.write('H');
   // Keyboard.write('R');
   // Keyboard.write('E');
   // Keyboard.write('W');
}

void sync(int address)
{
   // Serial.println("sending sync command")
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
         // Serial.println("byte revieced from left arm");
         // Serial.println(byt, HEX);
         switch(pin)
         {
         case 9: //APU start
            if(status == CLOSE)
               send(LCTRL, 's');
            break;
         case 10: //MISC START
            if(status == CLOSE)
            {
               char key;
               for(char i = 0x01; i <= 0x09; i++)
               {
                  key=0x30+i;
                  send(LALT, key);
                  delay(300);
               }
            }
            break;
         case 14: //L Crank
            if(status == CLOSE)
               send(LCTRL, 'o');
            break;
         case 15: //R Crank
            if(status == CLOSE)
               send(LALT, 'o');
            break;
         case 11: //L FUEL
            if(status == CLOSE)
               send(LCTRL, 'u');
            else
               send(LCTRL, 'u');
            break;
         case 12: //R FUEL
            if(status == CLOSE)
               send(RCTRL, 'u');
            else
               send(RCTRL, 'u');
            break;
         case 4: //Master Arm
            if(status == CLOSE)
               send(LALT, 'a');
            else
               send(LCTRL, 'a');
            break;
         case 3: //ECM JETT
            if(status == CLOSE)
               send(LCTRL, 'j');
            break;
         case 2: //EJETT
            if(status == CLOSE)
               send(LALT, 'j');
            break;
         case 5: //Gear Lever
            if(status == CLOSE) //gear leverdb.DOWN
               send(LCTRL, 'g');
            else //gear lever up
               send(LSHFT, 'g');
            break;
         case 6: //flaps Upper half
            if(status == CLOSE) //Flaps UP
            {
               send(LSHFT, 'f');
               Flaps = UP;
            }
            else if(Flaps != FULL) //if flaps are full don't do anything
            {
               send(LALT, 'f');
               Flaps = HALF;
            }
            break;
         case 7: //flaps lower half
            if(status == CLOSE) //Flaps Full
            {
               send(LCTRL, 'f');
               Flaps = FULL;
            }
            else if(Flaps != UP) //if flaps are up don't do anything
            {
               send(LALT, 'f');
               Flaps = HALF;
            }
            break;
         case 8: //Jett
            if(status == CLOSE)
               send(RCTRL, 'j');
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
