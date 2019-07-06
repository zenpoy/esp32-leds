#include <animations/rainbow.h>

RainbowAnimation::~RainbowAnimation() {

    delete start_hue;
    start_hue = NULL;
    
    delete end_hue;
    end_hue = NULL;
}

void RainbowAnimation::InitFromJson(const JsonObject &animation_params) {
  start_hue = FloatAnimationFactory(animation_params["start_hue"]);
  end_hue = FloatAnimationFactory(animation_params["end_hue"]);
}

void RainbowAnimation::Render(float rel_time) {

  if(start_hue == NULL || end_hue == NULL) {
    return;
  }

  const float curr_start_hue = start_hue->GetValue(rel_time);
  const float curr_end_hue = end_hue->GetValue(rel_time);
  const float curr_hue_diff = curr_end_hue - curr_start_hue;

  int i=0;
  float numberOfPixels = (float)pixels.size();
  for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it, i++) {
    HSV *pixel = *it;
    float rel_pos = ((float)i / numberOfPixels);
    pixel->hue = curr_start_hue + curr_hue_diff * rel_pos;
    pixel->sat = 1.0f;
    pixel->val = 1.0f;
  }
}
