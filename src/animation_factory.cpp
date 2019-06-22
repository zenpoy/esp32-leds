#include <animation_factory.h>

#include <animations/const_color.h>
#include <animations/fade_out.h>
#include <animations/fade_in.h>
#include <animations/rainbow.h>

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

void AnimationFactory::InitObjectMap(HSV leds_hsv[]) {
  object_map["one"] = GetPixelsFromStartToEnd(0, 44, leds_hsv);
  object_map["two"] = GetPixelsFromStartToEnd(55, 110, leds_hsv);
  object_map["three"] = GetPixelsFromStartToEnd(118, 196, leds_hsv);
  object_map["four"] = GetPixelsFromStartToEnd(222, 300, leds_hsv);
  object_map["t"] = GetPixelsFromStartToEnd(0, 4, leds_hsv);
  object_map["rt"] = GetPixelsFromStartToEnd(4, 11, leds_hsv);
  object_map["rm"] = GetPixelsFromStartToEnd(11, 14, leds_hsv);
  object_map["rb"] = GetPixelsFromStartToEnd(14, 25, leds_hsv);
  object_map["lb"] = GetPixelsFromStartToEnd(25, 32, leds_hsv);
  object_map["lm"] = GetPixelsFromStartToEnd(32, 35, leds_hsv);
  object_map["lt"] = GetPixelsFromStartToEnd(35, 45, leds_hsv);
  object_map["a"] = GetPixelsFromStartToEnd(0, 45, leds_hsv);
}

IAnimation *AnimationFactory::CreateAnimation(const char *jsonStr) {
  IAnimation *generated_animation = NULL;
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, jsonStr);

  const char * animation_name = doc["animation_name"];
  int timeout = doc["timeout"];
  const char *pixels_name = doc["pixels_name"];
  JsonObject animation_params = doc["animation_params"];

  if (strcmp(animation_name, "const_color") == 0) {
    generated_animation = new ConstColorAnimation();
  } else if(strcmp(animation_name, "fade_out") == 0) {
    generated_animation = new FadeOutAnimation();
  } else if(strcmp(animation_name, "fade_in") == 0) {
    generated_animation = new FadeInAnimation();
  } else if(strcmp(animation_name, "rainbow") == 0) {
    generated_animation = new RainbowAnimation();
  }

  if (generated_animation != NULL) {
    generated_animation->InitAnimation(object_map[pixels_name], timeout);
    generated_animation->InitFromJson(animation_params);
  }

  return generated_animation;
}
