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
    void UpdateWithNewAnimationsListLocked(const String &songName, std::list<IAnimation *> *animationsListPtr);
    static bool InitJsonDocFromFile(const String &songName, JsonDocument &docForParsing);

private:
    void DeleteAnimationsList(AnimationsList *listToDelete);
    void DeleteListIfPossible(AnimationsList *listToDelete);

private:
    std::map<String, AnimationsList *> availibleAnimations;
    const AnimationsList *emptyAnimationsList;

    // this animation list is in use by the rendered, and should not be deleted until it is safe
    volatile AnimationsList *lockedAnimationPtr = nullptr;
    volatile bool deleteLockedWhenUnused = false;

    SemaphoreHandle_t mapMutex;
};


#endif // __ANIMATIONS_CONTAINER_H__