#ifndef __ANIMATIONS_CONTAINER_H__
#define __ANIMATIONS_CONTAINER_H__

#include <ArduinoJson.h>

#include <hsv.h>
#include <animations/i_animation.h>

class AnimationsContainer
{

public:
    const AnimationsList *SetFromJsonFile(const String &songName, JsonDocument &docForParsing);

private:
    static bool InitJsonDocFromFile(const String &songName, JsonDocument &docForParsing);
};

#endif // __ANIMATIONS_CONTAINER_H__