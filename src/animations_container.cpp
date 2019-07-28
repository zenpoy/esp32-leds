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

  if(deleteLockedWhenUnused) {
    DeleteAnimationsList(lockedAnimationPtr);
  }
  lockedAnimationPtr = nullptr;
  deleteLockedWhenUnused = false;

  std::map<String, AnimationsList *>::iterator it = availibleAnimations.find(songName);
  if(it == availibleAnimations.end())
  {
    xSemaphoreGive(mapMutex);
    return emptyAnimationsList;
  }
  
  // lock this animations list so it will not be deleted from other core
  lockedAnimationPtr = it->second;

  xSemaphoreGive(mapMutex);
  return lockedAnimationPtr;
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
    DeleteListIfPossible(it->second);
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
    Serial.print("error deserializing animations json: ");
    Serial.println(err.c_str());
    return false;
  }

  Serial.print("loaded json from FS. with ");
  Serial.print(docForParsing.memoryUsage());
  Serial.println(" bytes");

  return true;
}

void AnimationsContainer::DeleteAnimationsList(AnimationsList *listToDelete)
{
  if(listToDelete == nullptr)
    return;

  Serial.print("deleting animations list with ");
  Serial.print(listToDelete->size());
  Serial.println(" animations");
  for(AnimationsList::iterator animationIt = listToDelete->begin(); animationIt != listToDelete->end(); animationIt++) {
    IAnimation *animation = *animationIt;
    delete animation;
  }
  delete listToDelete;
}

void AnimationsContainer::DeleteListIfPossible(AnimationsList *listToDelete)
{
  if(listToDelete == lockedAnimationPtr) {
    deleteLockedWhenUnused = true;
  }
  else {
    DeleteAnimationsList(listToDelete);
  }
}

