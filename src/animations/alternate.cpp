#include <animations/alternate.h>

AlternateAnimation::~AlternateAnimation() {
  
  delete alternateState;
  alternateState = NULL;

  delete hueShift;
  hueShift = NULL;
}

void AlternateAnimation::InitFromJson(const JsonObject &animation_params) {
    alternateState = BooleanFuncFactory(animation_params["alternate_state"]);
    numberOfPixels = animation_params["num_of_pixels"];
    hueShift = FloatAnimationFactory(animation_params["hue_shift"]);
}

void AlternateAnimation::Render(float rel_time) {

    bool currState = alternateState->GetValue(rel_time);
    double currHueShift = hueShift->GetValue(rel_time);

    int i=0;
    for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it, i++) {
        bool pixelState = (i % (2 * numberOfPixels)) < numberOfPixels;
        if(currState ^ pixelState) {
            HSV *pixel = *it;
            pixel->hue += currHueShift;
        }
    }
}
