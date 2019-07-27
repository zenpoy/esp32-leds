#include <animations/alternate_coloring.h>

AlternateColoringAnimation::~AlternateColoringAnimation() {

}

void AlternateColoringAnimation::InitFromJson(const JsonObject &animation_params) {

  JsonObject color1Json = animation_params["c1"];
  color1 = HSV(color1Json);

  JsonObject color2Json = animation_params["c2"];
  color2 = HSV(color2Json);

  numberOfPixels = animation_params["numPix"];
}

void AlternateColoringAnimation::Render(float rel_time) {

    int i=0;
    for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it) {

        HSV *pixel = *it;

        bool pixelState = (i % (2 * numberOfPixels)) < numberOfPixels;
        if(pixelState) {
            // TODO: this can be done better
            pixel->hue = color1.hue;
            pixel->sat = color1.sat;
            pixel->val = color1.val;
        }
        else {
            pixel->hue = color2.hue;
            pixel->sat = color2.sat;
            pixel->val = color2.val;
        }

        i++;
    }
}
