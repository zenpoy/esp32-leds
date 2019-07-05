#ifndef __ANIMATION_FACTORY_H__
#define __ANIMATION_FACTORY_H__

#include <animations/i_animation.h>
#include <hsv.h>

#include <list>
#include <map>

class AnimationFactory {

public:
  static std::list<IAnimation *> *AnimationsListFromJson(const char *jsonStr);
  static IAnimation *CreateAnimation(const JsonObject &animationAsJsonObj);
  static void InitObjectMap(HSV leds_hsv[]);

private:
  typedef std::map<std::string, std::vector<HSV *> > LedObjectMap;
  static LedObjectMap object_map;

};

#endif // __ANIMATION_FACTORY_H__
