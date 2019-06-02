#include <animations/const_color.h>

void ConstColorAnimation::InitFromJson(const JsonObject &animation_params) {
  JsonObject color = animation_params["color"];
  colorToFill = HSV(color);
}

void ConstColorAnimation::Render(float rel_time) {
  for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it) {
    HSV *pixel = *it;
    pixel->hue = colorToFill.hue;
    pixel->sat = colorToFill.sat;
    pixel->val = colorToFill.val;
  }
}
