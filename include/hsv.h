#ifndef __HSV_H__
#define __HSV_H__

#include <ArduinoJson.h>

class HSV
{
public:
  HSV() : hue(0.0f), sat(0.0f), val(0.0f) {}

  HSV(float hue, float sat, float val) : hue(hue), sat(sat), val(val) {}

  HSV(JsonArray colorHsvJson)
  {
    size_t numOfParams = colorHsvJson.size();
    hue = colorHsvJson[0];
    sat = numOfParams > 1 ? colorHsvJson[1] : 1.0;
    val = numOfParams > 2 ? colorHsvJson[2] : 1.0;
  }

public:
  float hue;
  float sat;
  float val;
};

#endif // __HSV_H__
