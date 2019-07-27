#include <animation_factory.h>

#include <animations/const_color.h>
#include <animations/set_brightness.h>
#include <animations/hue_shift.h>
#include <animations/rainbow.h>
#include <animations/alternate.h>
#include <animations/fill.h>
#include <animations/snake.h>
#include <animations/alternate_coloring.h>

#include <Arduino.h>

AnimationFactory::LedObjectMap AnimationFactory::object_map;
const char *AnimationFactory::objectsMapErrorString = "no configuration availible - initialized not called";

int AnimationFactory::InitObjectsConfig(HSV ledsArr[], JsonDocument &doc, File &f) {

  DeserializationError jsonError = deserializeJson(doc, f);
  if(jsonError) {
    objectsMapErrorString = "json deserialize error";
    return 0;
  }

  int totalPixels = doc["total_pixels"];
  if(totalPixels < 0) {
    objectsMapErrorString = "total pixels is negative";
    return 0;
  }
  if(totalPixels > MAX_SUPPORTED_PIXELS) {
    objectsMapErrorString = "total pixels too large";
    return 0;
  }
  if(totalPixels == 0) {
    objectsMapErrorString = "total pixels is zero (probably error in json format)";
    return 0;
  }

  const JsonObject &objectsMap = doc["objects"];
  const char *objectsMapErr = InitObjectsMap(ledsArr, totalPixels, objectsMap);
  if(objectsMapErr != NULL) {
    object_map = AnimationFactory::LedObjectMap();
    objectsMapErrorString = objectsMapErr;
    return 0;
  }

  // all went well
  objectsMapErrorString = NULL;
  return totalPixels;
}

const char *AnimationFactory::InitObjectsMap(HSV ledsArr[], int totalPixels, const JsonObject &objectsMap) 
{
  for (JsonPair p : objectsMap) {

    const char* key = p.key().c_str();
    JsonArray indices = p.value().as<JsonArray>();

    size_t numIndices = indices.size();
    if(numIndices == 0)
        return "object in the map has no configured indices (probably error in json format)";

    object_map[key].reserve(numIndices);
    for(JsonVariant index : indices) {

      if(!index.is<int>())
        return "pixel index not integer";
      int pixelIndex = index.as<int>();

      // check for pixel index out of bounds
      if(pixelIndex >= totalPixels || pixelIndex < 0)
        return "pixel index out of range";

      HSV *pixelPtr = &(ledsArr[pixelIndex]);
      object_map[key].push_back(pixelPtr);
    }
  }

  return NULL;
}

std::list<IAnimation *> *AnimationFactory::AnimationsListFromJson(JsonDocument &doc) {

  JsonArray array = doc.as<JsonArray>();

  std::list<IAnimation *> *animationsList = new std::list<IAnimation *>();
  for(int i=0; i<array.size(); i++) {
    IAnimation *animationObj = CreateAnimation(array.getElement(i).as<JsonObject>()); 
    animationsList->push_back(animationObj);
  }

  Serial.print("successfully read animations from file. found ");
  Serial.print(array.size());
  Serial.println(" animations");

  return animationsList;
}

IAnimation *AnimationFactory::CreateAnimation(const JsonObject &animationAsJsonObj) {
  
  IAnimation *generated_animation = NULL;

  const char * animation_name = animationAsJsonObj["t"];
  int startTime = animationAsJsonObj["s"];
  int endTime = animationAsJsonObj["e"];
  const char *pixels_name = animationAsJsonObj["p"];
  JsonObject animation_params = animationAsJsonObj["params"];

  if (strcmp(animation_name, "const") == 0) {
    generated_animation = new ConstColorAnimation();
  } else if(strcmp(animation_name, "brightness") == 0) {
    generated_animation = new SetBrightnessAnimation();
  } else if(strcmp(animation_name, "hue_shift") == 0) {
    generated_animation = new HueShiftAnimation();
  } else if(strcmp(animation_name, "rainbow") == 0) {
    generated_animation = new RainbowAnimation();
  } else if(strcmp(animation_name, "alternate") == 0) {
    generated_animation = new AlternateAnimation();
  } else if(strcmp(animation_name, "fill") == 0) {
    generated_animation = new FillAnimation();
  } else if(strcmp(animation_name, "snake") == 0) {
    generated_animation = new SnakeAnimation();
  } else if(strcmp(animation_name, "al") == 0) {
    generated_animation = new AlternateColoringAnimation();
  }
  

  if (generated_animation != NULL) {
    generated_animation->InitAnimation(object_map[pixels_name], startTime, endTime);
    generated_animation->InitFromJson(animation_params);
  }

  return generated_animation;
}
