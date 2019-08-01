#ifndef __CONFETTI_H__
#define __CONFETTI_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>

class ConfettiAnimation : public IAnimation {

public:
    ConfettiAnimation();
    ~ConfettiAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time);

private:
    void Fade(float relTime);

private:
    // from config
    IFloatFunc *fadeAmount = nullptr;

private:
  float *brightPerPixel = nullptr;
  unsigned int shinedSoFar = 0;
};


#endif // __CONFETTI_H__
