#include <FIFO_Buff.h>
#include <Wire.h>
#include <Debounce.h>

/*
 * Kyle Michaels
 * 4 Mar 2019
 * Written for ProMini 5V, Arduino 1.8.9
 * Sketch to pass current pin status to the i2c bus
 */

//I2C Addressing
#define ADDR   0x21
#define SYNC   0x99
#define D_MSK  0b11100000
#define P_MSK  0b00011110
#define O_MSK  0b00000001

FIFO_Buff<32> i2cBuff; //create a buffer object to hold the write cache

bool sync = true;

debounce<16> db;

void setup()
{
   //start Serial
   Serial.begin(9600);
   Serial.println("Serial begin");

   //create i2c bus
   Wire.begin(0x21);
   Wire.onRequest(requestEvent);
   Wire.onReceive(receiveEvent);

  //Start pins in digital pull up
   for (int i = 2; i <= 15; i++)
   {
      pinMode(i, INPUT_PULLUP);
      dp.change(i);
   }
}

void loop()
{
   //check all the button statuses
   for(int i = 2; i <= 15; i++)
   {
      uint8_t change = db.change(i);
      if(change != NOCNG)
      {
         Serial.print("Pin updated: ");
         Serial.print(i);
         Serial.print(" to: ");
         etch(change);

         //build the byte
         uint8_t device = ((ADDR & 0x0F) << 5) & D_MSK;
         uint8_t pin = (i << 1) & P_MSK;
         uint8_t packet = device | pin | change;

         if(i2cBuff.write(packet))
            Serial.println("Buffer Full");
         Serial.print("byte added to buffer: ");
         Serial.println(change, HEX);
         Serial.println();
      }
   }
}

void requestEvent()
{
   if(sync) //going to sync, clear the buffer, fill the buffer with current state
   {
      Serial.println("Sending sync data");
      i2cBuff.flush();
      for(int i = PINMIN; i <= PINMAX; i++)
      {
         uint8_t status = db.status(i);
         Serial.print("Pin: ");
         Serial.print(i);
         Serial.print(" is: ");
         etch(status);

         //build the byte
         uint8_t device = ((ADDR & 0x0F) << 5) & D_MSK;
         uint8_t pin = (i << 1) & P_MSK;
         uint8_t packet = device | pin | status;

         if(i2cBuff.write(packet))
            Serial.println("Buffer Full");
         Serial.print("byte added to buffer: ");
         Serial.println(packet, HEX);
         Serial.println();
      }
      sync=false; //clear the flag for the next run
   }

   Serial.println("Sending updates:");

   //have to load the buffer into an array
   size_t len = i2cBuff.length();
   uint8_t datum[len];

   for (int i = 0; i < len; i++)
      datum[i]=i2cBuff.read();

   Serial.write(datum, len);
   Serial.println('\n');

   Wire.write(datum, len); //dump the message buffer into the i2c bus.
}

void receiveEvent(int Many)
{
  Serial.println("recieving on i2c bus");
  byte cache = Wire.read();
  while (Wire.available())
  {
    Serial.println("more that one byte was transmitted!" && '\n' && "Clearing buffer");
    Wire.read();
  }

  switch (cache)
  {
    case SYNC: //request for all pin updates toss the flag
      Serial.println("sync request");
      sync = true;
      break;
  }

}

void etch(uint8_t status) //don't judge me I'm bad at names
{
   if (status == CLOSE)
      Serial.println("CLOSE");
   else
      Serial.println("OPEN");
}
