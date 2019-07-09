#include "animations_container.h"

#include <animation_factory.h>

AnimationsContainer::AnimationsContainer() {
  emptyAnimationsList = new AnimationsList();
  mapMutex = xSemaphoreCreateMutex();
}

void AnimationsContainer::setup() {

}

void AnimationsContainer::SetFromJson(const std::string &songName, const char *jsonStr) {
  std::list<IAnimation *> *animationsListPtr = AnimationFactory::AnimationsListFromJson(jsonStr);

  // update the map
  xSemaphoreTake(mapMutex, portMAX_DELAY);

  std::map<std::string, AnimationsList *>::iterator it = availibleAnimations.find(songName);
  if(it == availibleAnimations.end()) 
  {
    availibleAnimations.insert(std::make_pair(songName, animationsListPtr));
  }
  else {
    animationsForDelete.push_back(it->second);
    // TODO: delete the old value, or move it to delete queue
    it->second = animationsListPtr;
  }
  xSemaphoreGive(mapMutex);
}

const AnimationsContainer::AnimationsList *AnimationsContainer::GetAnimationsList(const std::string &songName, unsigned int currentSongOffsetMs) 
{

  xSemaphoreTake(mapMutex, portMAX_DELAY);

  std::map<std::string, AnimationsList *>::iterator it = availibleAnimations.find(songName);
  if(it == availibleAnimations.end())
  {
    xSemaphoreGive(mapMutex);
    return emptyAnimationsList;
  }
  
  AnimationsContainer::AnimationsList *animationsList = it->second;
  xSemaphoreGive(mapMutex);
  return animationsList;
}

void AnimationsContainer::ClearUnusedAnimations() 
{
  for(std::list<AnimationsList *>::iterator it = animationsForDelete.begin(); it != animationsForDelete.end(); it++) {
    AnimationsList *listToDelete = *it;
    for(AnimationsList::iterator animationIt = listToDelete->begin(); animationIt != listToDelete->end(); animationIt++) {
      IAnimation *animation = *animationIt;
      delete animation;
    }
    delete listToDelete;
  }
  animationsForDelete.clear();
}