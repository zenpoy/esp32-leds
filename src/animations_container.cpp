#include "animations_container.h"

#include <animation_factory.h>

void AnimationsContainer::SetFromJson(const char *jsonStr) {
    IAnimation *generated_animation = AnimationFactory::CreateAnimation(jsonStr);
    currentAnimations.push_back(generated_animation);
}

const std::vector<IAnimation *> &AnimationsContainer::GetAnimationsList(unsigned int currentSongOffsetMs) {
    return currentAnimations;
}
