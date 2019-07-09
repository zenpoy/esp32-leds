#include <animation_factory.h>

#include <animations/const_color.h>
#include <animations/set_brightness.h>
#include <animations/hue_shift.h>
#include <animations/fade_out.h>
#include <animations/fade_in.h>
#include <animations/rainbow.h>
#include <animations/random_hue.h>
#include <animations/alternate.h>
#include <animations/fill.h>

#include <ArduinoJson.h>
#include <Arduino.h>

AnimationFactory::LedObjectMap AnimationFactory::object_map;

std::vector<HSV *> GetPixelsFromStartToEnd(int start, int end, HSV leds_hsv[]) {
  int size = end-start;
  std::vector<HSV *> pixelsVec(size);
  int globalIndex = start;
  for(int i=0; i<size; ++i) {
    pixelsVec[i] = &(leds_hsv[globalIndex]);
    globalIndex++;
  }
  return pixelsVec;
}

std::vector<HSV *> GetPixelsFromIndices(const std::vector<int> &indices, HSV leds_hsv[])
{
  std::vector<HSV *> pixelsVec(indices.size());
  for(int i=0; i<indices.size(); ++i) {
    int globalIndex = indices[i];
    pixelsVec[i] = &(leds_hsv[globalIndex]);
  }
  return pixelsVec;
}

void AnimationFactory::InitObjectMap(HSV leds_hsv[], const JsonObject &objectsMap) {

  for (JsonPair p : objectsMap) {
      const char* key = p.key().c_str();
      JsonArray indices = p.value().as<JsonArray>();

      object_map[key].reserve(indices.size());
      for(JsonVariant index : indices) {
        int pixelIndex = index.as<int>();
        HSV *pixelPtr = &(leds_hsv[pixelIndex]);
        object_map[key].push_back(pixelPtr);
      }
  }

}

std::list<IAnimation *> *AnimationFactory::AnimationsListFromJson(const char *jsonStr) {
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, jsonStr);
  JsonArray array = doc.as<JsonArray>();

  std::list<IAnimation *> *animationsList = new std::list<IAnimation *>();
  for(int i=0; i<array.size(); i++) {
    IAnimation *animationObj = CreateAnimation(array.getElement(i).as<JsonObject>()); 
    animationsList->push_back(animationObj);
  }

  return animationsList;
}

IAnimation *AnimationFactory::CreateAnimation(const JsonObject &animationAsJsonObj) {
  
  IAnimation *generated_animation = NULL;

  const char * animation_name = animationAsJsonObj["animation_name"];
  int startTime = animationAsJsonObj["start_time"];
  int endTime = animationAsJsonObj["end_time"];
  const char *pixels_name = animationAsJsonObj["pixels_name"];
  JsonObject animation_params = animationAsJsonObj["animation_params"];

  if (strcmp(animation_name, "const_color") == 0) {
    generated_animation = new ConstColorAnimation();
  } else if(strcmp(animation_name, "set_brightness") == 0) {
    generated_animation = new SetBrightnessAnimation();
  } else if(strcmp(animation_name, "hue_shift") == 0) {
    generated_animation = new HueShiftAnimation();
  } else if(strcmp(animation_name, "fade_out") == 0) {
    generated_animation = new FadeOutAnimation();
  } else if(strcmp(animation_name, "fade_in") == 0) {
    generated_animation = new FadeInAnimation();
  } else if(strcmp(animation_name, "rainbow") == 0) {
    generated_animation = new RainbowAnimation();
  } else if(strcmp(animation_name, "random_hue") == 0) {
    generated_animation = new RandomHueAnimation();
  } else if(strcmp(animation_name, "alternate") == 0) {
    generated_animation = new AlternateAnimation();
  } else if(strcmp(animation_name, "fill") == 0) {
    generated_animation = new FillAnimation();
  }

  if (generated_animation != NULL) {
    generated_animation->InitAnimation(object_map[pixels_name], startTime, endTime);
    generated_animation->InitFromJson(animation_params);
  }

  return generated_animation;
}
