/*
Released under Creative Commons Attribution 4.0
by bitluni 2016
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it ba sharing http://youtube.com/bitlunislab
CatRunner by @pepsilla
*/

#include "LedFunction.h"

class CatrunningFunction: public LedFunction
{
  public:
    int myIndex=0;
    bool descending = false;
    uint8_t rgb[3] = {0, 0, 0};
    int delayValue=0;
    int counter=0;
  CatrunningFunction()
  {
  }

  virtual bool init(ESP8266WebServer &server)
  {
    delayValue = getArgValue(server, "delay");
    if(delayValue<0) delayValue=0;
    if(!loadRGBValues(server, rgb))
      return false;
  }

  virtual void render()
  {
    if(counter == delayValue){
      for(int i = 0; i < state->count; i++)
      {
            if(i==myIndex)state->setRgb(myIndex,rgb[0] ,rgb[1],rgb[2]);
            else state->setRgb(i, 0, 0, 0);
      }
      if(!descending) myIndex++;
      else myIndex --;
      if(myIndex == state->count)descending = true;
      if(myIndex == 0) descending = false;
      //Serial.printf("%i,%i",myIndex,counter);
      counter = 0;
    }
    else counter++;
  }
};
