#include "animations_container.h"

#include <SPIFFS.h>
#include <animation_factory.h>

AnimationsContainer::AnimationsContainer() {
  emptyAnimationsList = new AnimationsList();
  mapMutex = xSemaphoreCreateMutex();
}

void AnimationsContainer::setup() {

}

void AnimationsContainer::SetFromJsonFile(const String &songName, JsonDocument &docForParsing) {

  bool success = InitJsonDocFromFile(songName, docForParsing);
  if(!success) {
    return;
  }
  std::list<IAnimation *> *animationsListPtr = AnimationFactory::AnimationsListFromJson(docForParsing);
  if(animationsListPtr == NULL) {
    return;
  }

  UpdateWithNewAnimationsList(songName, animationsListPtr);
}

const AnimationsContainer::AnimationsList *AnimationsContainer::GetAnimationsList(const String &songName, unsigned int currentSongOffsetMs) 
{

  xSemaphoreTake(mapMutex, portMAX_DELAY);

  std::map<String, AnimationsList *>::iterator it = availibleAnimations.find(songName);
  if(it == availibleAnimations.end())
  {
    xSemaphoreGive(mapMutex);
    return emptyAnimationsList;
  }
  
  AnimationsContainer::AnimationsList *animationsList = it->second;
  xSemaphoreGive(mapMutex);
  return animationsList;
}

void AnimationsContainer::UpdateWithNewAnimationsList(const String &songName, std::list<IAnimation *> *animationsListPtr)
{
  xSemaphoreTake(mapMutex, portMAX_DELAY);
  UpdateWithNewAnimationsListLocked(songName, animationsListPtr);
  xSemaphoreGive(mapMutex);
}

void AnimationsContainer::UpdateWithNewAnimationsListLocked(const String &songName, std::list<IAnimation *> *animationsListPtr)
{
  std::map<String, AnimationsList *>::iterator it = availibleAnimations.find(songName);
  if(it == availibleAnimations.end()) 
  {
    availibleAnimations.insert(std::make_pair(songName, animationsListPtr));
  }
  else {
    animationsForDelete.push_back(it->second);
    // TODO: delete the old value, or move it to delete queue
    it->second = animationsListPtr;
  }
}

bool AnimationsContainer::InitJsonDocFromFile(const String &songName, JsonDocument &docForParsing) 
{
  File file = SPIFFS.open((String("/music/") + songName).c_str());
  if(!file) {
    Serial.print("could not find file in FS: ");
    Serial.println(songName);
    return false;
  }

  DeserializationError err = deserializeJson(docForParsing, file);
  file.close();
  if(err) {
    Serial.println("error deserializing animations json");
    return false;
  }

  return true;
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