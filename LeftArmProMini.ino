#include <FIFO_Buff.h>
#include <Wire.h>

/*
 * Kyle Michaels
 * 4 Mar 2019
 * Written for ProMini 5V, Arduino 1.8.9
 * Sketch to pass current pin status to the i2c bus
 */

FIFO_Buff<32> i2cBuff; //create a buffer object to hold the write cache

bool btnState[30];
bool sync = true;

//array of momentary pins
int mom[]= {};

//array of latching pins
int lat[] = {};

void setup()
{
  //start Serial
  Serial.begin(9600);

  //create i2c bus
  Wire.begin(0x21);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop()
{

}

void requestEvent()
{
  if(sync)
  {
    Serial.println("Sending sync data");
    sync=false; //clear the flag
  }
  else
  {
    Serial.println("Sending updates:");

    size_t len = i2cBuff.length();
    uint8_t datum[len];

    for (int i = 0; i < len; i++)
    {
      datum[i]=i2cBuff.read();
    }

    Serial.write(datum, len);
    Wire.write(datum, len); //dump the message buffer into the i2c bus.
  }
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
    case 0x99: //request for all pin updates toss the flag
      Serial.println("sync request");
      sync = true;
      break;
  }

}

bool momentary(int pin)
{
  return false;
}

bool latching(int pin)
{
  return false;
}
