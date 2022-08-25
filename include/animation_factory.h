#ifndef __ANIMATION_FACTORY_H__
#define __ANIMATION_FACTORY_H__

#include <animations/i_animation.h>
#include <hsv.h>

#include <list>
#include <map>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#ifdef NUM_LEDS
#define MAX_SUPPORTED_PIXELS NUM_LEDS
#else
#define MAX_SUPPORTED_PIXELS 1500
#endif // NUM_LEDS

class AnimationFactory
{
public:
  static AnimationsList *AnimationsListFromJson(JsonDocument &doc);
  static void CreateAnimationAndAppend(JsonObject anJsonConfig, const char *pixelsName, AnimationsList *listToAppend);
  static IAnimation *CreateAnimation(const JsonObject &animationAsJsonObj, const char *pixelsName);
  static int InitObjectsConfig(HSV ledsArr[], JsonDocument &doc, File &f);

public:
  static const char *objectsMapErrorString;

private:
  static const char *InitObjectsMap(HSV ledsArr[], int totalPixels, const JsonObject &objectsMap);

private:
  typedef std::map<std::string, const std::vector<HSV *> *> LedObjectMap;
  static LedObjectMap object_map;
};

#endif // __ANIMATION_FACTORY_H__
