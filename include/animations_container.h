#ifndef __ANIMATIONS_CONTAINER_H__
#define __ANIMATIONS_CONTAINER_H__

#include <vector>

#include <animations/i_animation.h>

#ifndef MAX_ANIMATIONS_IN_SEGMENT
#define MAX_ANIMATIONS_IN_SEGMENT 100
#endif


class AnimationsContainer 
{

public:
    AnimationsContainer() :
        currentAnimations(MAX_ANIMATIONS_IN_SEGMENT)
    {}

public:
    typedef const std::vector<IAnimation *> ConstAnimationsVector;

public:
    ConstAnimationsVector &GetAnimationsList(unsigned int currentSongOffsetMs);
    void SetFromJson(const char *jsonStr);

private:
    std::vector<IAnimation *> currentAnimations;
};


#endif // __ANIMATIONS_CONTAINER_H__