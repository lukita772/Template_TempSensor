/*
*   brief: implementaciones para sensor de caudalimetro 
*   author: Agostini, Luca
*/
#include <Arduino.h>
#include <defines.h>

#ifdef SENSOR_ISRAINSENSOR

#include <Adafruit_VL53L0X.h>

// Configs.
#define ST_HIGH 1
#define ST_LOW 0
#define MAX_THR 60
#define MIN_THR 55

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

static uint16_t counter = 0;

uint16_t getCounter()
{
    return counter;
}

void initializeLevelGauge()
{
    Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}

void st_rainGaugeSensor_counter(int data)
{
  static uint8_t vstate = ST_HIGH;
  static uint8_t dbCounter = 0;
  //static int prevData = 0;
  
  switch(vstate)
  {
      case ST_HIGH:
        if( data <= MIN_THR)
        {
          dbCounter ++;

          if(dbCounter > 2)
          {
            vstate = ST_LOW;
            counter ++;
            dbCounter = 0;
            Serial.print("HIGH->LOW\n");
            Serial.println(counter);
          }
        }
        else
        {
          dbCounter = 0;
        }
      break;
      case ST_LOW:
        if( data >= MAX_THR )
        {
           dbCounter ++;

           if(dbCounter > 2)
           {
              vstate = ST_HIGH;
              counter ++;
              dbCounter = 0;
              Serial.print("LOW->HIGH\n");
              Serial.println(counter);
           }
        }
        else
        {
          dbCounter = 0;
        }
      break;
      default:
        vstate = ST_LOW;
        dbCounter = 0;
      break;
   }

   //prevData = data;

}
#endif