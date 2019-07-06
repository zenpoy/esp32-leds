#include <animations/fade_in.h>

FadeInAnimation::~FadeInAnimation() 
{

}

void FadeInAnimation::InitFromJson(const JsonObject &animation_params) 
{

}

void FadeInAnimation::Render(float rel_time) 
{
  float val_mult_factor = rel_time;
  for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it) {
    HSV *pixel = *it;
    pixel->val *= val_mult_factor;
  }
}
