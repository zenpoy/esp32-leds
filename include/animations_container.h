#ifndef __ANIMATIONS_CONTAINER_H__
#define __ANIMATIONS_CONTAINER_H__

#include <list>
#include <map>

#include <ArduinoJson.h>

#include <hsv.h>
#include <animations/i_animation.h>

class AnimationsContainer 
{

public:
    AnimationsContainer();

public:
    void setup();

public:
    typedef std::list<IAnimation *> AnimationsList;

public:
    const AnimationsList *GetAnimationsList(const String &songName, unsigned int currentSongOffsetMs);
    void SetFromJsonFile(const String &songName, JsonDocument &docForParsing);

private:
    void UpdateWithNewAnimationsList(const String &songName, std::list<IAnimation *> *animationsListPtr);
    static bool InitJsonDocFromFile(const String &songName, JsonDocument &docForParsing);

private:
    void ClearUnusedAnimations();

private:
    std::map<String, AnimationsList *> availibleAnimations;
    const AnimationsList *emptyAnimationsList;

    // we cannot delete lists from the map from core 0 since core 1 might still be using them.
    // so we put it in this list, and erase from it when it's safe
    std::list<AnimationsList *> animationsForDelete;

    SemaphoreHandle_t mapMutex;
};


#endif // __ANIMATIONS_CONTAINER_H__