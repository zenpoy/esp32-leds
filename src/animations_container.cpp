#include "animations_container.h"

#include <animation_factory.h>

void AnimationsContainer::setup(HSV leds_hsv[]) {
  AnimationFactory::InitObjectMap(leds_hsv);
}

void AnimationsContainer::SetFromJson(const char *jsonStr) {
    currentAnimations = AnimationFactory::AnimationsListFromJson(jsonStr);
}

const std::vector<IAnimation *> &AnimationsContainer::GetAnimationsList(unsigned int currentSongOffsetMs) {
    return currentAnimations;
}
