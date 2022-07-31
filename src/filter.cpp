#include "filter.h"
#include "math.h"


float filter(float sample, float alfa){
  static float old_y=0;
  static bool first=true;
  float y;

  if(first){
    old_y=sample;
    y=sample; first = false;
    return y;
  }
  else{
    y = alfa * sample + (1.0 - alfa) * old_y;
    old_y = y;
    return y;
  }
}
