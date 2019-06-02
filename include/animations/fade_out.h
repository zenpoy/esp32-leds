#ifndef __FADE_OUT_H__
#define __FADE_OUT_H__

#include <animations/i_animation.h>


class FadeOutAnimation : public IAnimation {

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time);

private:
  
};


#endif // __FADE_OUT_H__
