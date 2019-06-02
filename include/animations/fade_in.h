#ifndef __FADE_IN_H__
#define __FADE_IN_H__

#include <animations/i_animation.h>


class FadeInAnimation : public IAnimation {

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time);

private:
  
};


#endif // __FADE_IN_H__
