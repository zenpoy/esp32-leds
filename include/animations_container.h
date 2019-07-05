#ifndef __ANIMATIONS_CONTAINER_H__
#define __ANIMATIONS_CONTAINER_H__

#include <list>
#include <map>

#include <hsv.h>
#include <animations/i_animation.h>

class AnimationsContainer 
{

public:
    AnimationsContainer();

public:
    void setup(HSV leds_hsv[]);

public:
    typedef std::list<IAnimation *> AnimationsList;

public:
    const AnimationsList *GetAnimationsList(const std::string &songName, unsigned int currentSongOffsetMs);
    void SetFromJson(const std::string &songName, const char *jsonStr);

private:
    void ClearUnusedAnimations();

private:
    std::map<std::string, AnimationsList *> availibleAnimations;
    const AnimationsList *emptyAnimationsList;

    // we cannot delete lists from the map from core 0 since core 1 might still be using them.
    // so we put it in this list, and erase from it when it's safe
    std::list<AnimationsList *> animationsForDelete;

    SemaphoreHandle_t mapMutex;
};


#endif // __ANIMATIONS_CONTAINER_H__