#include <Wire.h>
#include <Debounce.h>

/*
 * Code to test the i2c bus
 * Due, Arduino 1.8.9
 * Written By Michae1s
 */

//I2C defines
#define LEFTARM   0x21
#define SYNC      0x99
#define D_MSK  0b11100000
#define P_MSK  0b00011110
#define O_MSK  0b00000001

#define PINS 60

debounce<PINS> db;
uint8_t status;

void setup()
{
   Serial.begin(9600);
   Serial.println("Serial Begin\n");

   for(int i = 3; i <= PINS; i++)
   {
      pinMode(i, INPUT_PULLUP);
      db.change(i);
   }
}

void loop()
{
   for(int i=3; i < PINS; i++)
   {
      status=db.change(i);
      if(status != NOCNG)
      {
         Serial.print("Pin: ");
         Serial.print(i);
         Serial.print(" Changed to : ");
         if (status == CLOSE)
            Serial.println("CLOSE");
         else
            Serial.println("OPEN");
      }
   }
}
