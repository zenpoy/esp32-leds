#ifndef __I_ANIMATIONS_H__
#define __I_ANIMATIONS_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <hsv.h>

class IAnimation {

public: ~IAnimation() {}

public:
  virtual void InitFromJson(const JsonObject &animation_params) = 0;

protected:
  virtual void Render(float rel_time) = 0;

public:

  void Render(unsigned long curr_time) {
    float rel_time = should_end ? ((float)(curr_time - start_time) / (float)(end_time - start_time)) : 0;
    Render(rel_time);
  }

  void InitAnimation(const std::vector<HSV *> &pixels, int timeout) {
    this->pixels = pixels;
    if (timeout == 0) {
      this->should_end = false;
    } else {
      this->should_end = true;
    }
    this->start_time = millis();
    this->end_time = this->start_time + timeout;
  }

  bool IsFinished(unsigned long current_time) {
    return should_end && this->end_time <= current_time;
  }

protected:
  unsigned long start_time, end_time;
  bool should_end;
  std::vector<HSV *> pixels;
};

#endif // __I_ANIMATIONS_H__
