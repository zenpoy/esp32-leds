#include <animations/fade_out.h>

FadeOutAnimation::~FadeOutAnimation()
{

}

void FadeOutAnimation::InitFromJson(const JsonObject &animation_params) 
{

}

void FadeOutAnimation::Render(float rel_time) 
{
  float val_mult_factor = 1.0f - rel_time;
  for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it) {
    HSV *pixel = *it;
    pixel->val *= val_mult_factor;
  }
}
