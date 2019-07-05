#ifndef __I_ANIMATIONS_H__
#define __I_ANIMATIONS_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <hsv.h>

class IAnimation {

public: virtual ~IAnimation() { }

public:
  virtual void InitFromJson(const JsonObject &animation_params) = 0;

protected:
  virtual void Render(float rel_time) = 0;

public:

  void Render(unsigned long curr_time) {
    float rel_time = ((float)(curr_time - start_time) / (float)(end_time - start_time));
    Render(rel_time);
  }

  void InitAnimation(const std::vector<HSV *> &pixels, unsigned int startTime, unsigned int endTime) {
    this->pixels = pixels;
    this->start_time = startTime;
    this->end_time = endTime;
  }

  bool IsActive(unsigned long curr_time) {
    return curr_time >= this->start_time && curr_time < this->end_time;
  }

protected:
  unsigned long start_time, end_time;
  std::vector<HSV *> pixels;
};

#endif // __I_ANIMATIONS_H__
