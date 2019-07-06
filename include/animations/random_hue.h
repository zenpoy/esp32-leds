#ifndef __RANDOM_HUE_H__
#define __RANDOM_HUE_H__

#include <animations/i_animation.h>


class RandomHueAnimation : public IAnimation {

public:
  ~RandomHueAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time);

private:
    float *huePerPixel;
};


#endif // __RANDOM_HUE_H__
