#ifndef __ANIMATION_FACTORY_H__
#define __ANIMATION_FACTORY_H__

#include <animations/i_animation.h>
#include <hsv.h>

#include <vector>
#include <map>

class AnimationFactory {

public:
  static IAnimation *CreateAnimation(const char *jsonStr);
  static void InitObjectMap(HSV leds_hsv[]);

private:
  typedef std::map<std::string, std::vector<HSV *> > LedObjectMap;
  static LedObjectMap object_map;

};

#endif // __ANIMATION_FACTORY_H__
