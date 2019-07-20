#ifndef __ANIMATION_FACTORY_H__
#define __ANIMATION_FACTORY_H__

#include <animations/i_animation.h>
#include <hsv.h>

#include <list>
#include <map>
#include <SPIFFS.h>


#define MAX_SUPPORTED_PIXELS 600 

class AnimationFactory {

public:
  static std::list<IAnimation *> *AnimationsListFromJson(const char *jsonStr);
  static IAnimation *CreateAnimation(const JsonObject &animationAsJsonObj);
  static int InitObjectsConfig(HSV ledsArr[], JsonDocument &doc, File &f);

public:
  static const char *objectsMapErrorString;

private:
  static const char *InitObjectsMap(HSV ledsArr[], int totalPixels, const JsonObject &objectsMap);

private:
  typedef std::map<std::string, std::vector<HSV *> > LedObjectMap;
  static LedObjectMap object_map;

};

#endif // __ANIMATION_FACTORY_H__
