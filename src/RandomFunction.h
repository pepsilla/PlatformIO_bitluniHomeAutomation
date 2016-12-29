/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it ba sharing http://youtube.com/bitlunislab
Random function added by @pepsilla
*/

#include "LedFunction.h"

class RandomFunction: public LedFunction
{
  public:
    uint8_t serverParms[3] = {0, 0, 0};
    int counter=0;
    int delayValue=0;
  RandomFunction()
  {
  }

  virtual bool init(ESP8266WebServer &server)
  {
    delayValue=getArgValue(server, "delay");
    if(delayValue<0)delayValue = 0;
    if(!loadRGBValues(server, serverParms))
      return false;
  }

  virtual void render()
  {
  //  static uint8_t counter = 0;
    if(counter==delayValue){
      uint8_t trirgb[3] = {0, 0, 0};
      for(int i = 0; i < state->count; i++)
      {
      randomSeed(micros());
      trirgb[0]=random(255);
      trirgb[1]=random(255);
      trirgb[2]=random(255);
      state->setRgb(i, trirgb[0], trirgb[1], trirgb[2]);
      }
      counter = 0;
    }
    else counter++;
  }
};
