#ifndef __HSV_H__
#define __HSV_H__

#include <ArduinoJson.h>

class HSV
{
public:

  HSV() :
    hue(0.0f), sat(0.0f), val(0.0f) {}
  HSV(float hue, float sat, float val) :
    hue(hue), sat(sat), val(val) {}
  HSV(JsonObject colorhsv_json):
    hue(colorhsv_json["hue"]), sat(colorhsv_json["sat"]), val(colorhsv_json["val"]) {}

  public:
    float hue;
    float sat;
    float val;
};

#endif // __HSV_H__
