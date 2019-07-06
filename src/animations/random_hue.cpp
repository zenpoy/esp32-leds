#include <animations/random_hue.h>

RandomHueAnimation::~RandomHueAnimation() {
    delete[] huePerPixel;
}

void RandomHueAnimation::InitFromJson(const JsonObject &animation_params) {
    huePerPixel = new float[pixels.size()];
    for(int i=0; i<pixels.size(); i++) {
        huePerPixel[i] = random(1024) / 1024.0f;
    }
}

void RandomHueAnimation::Render(float rel_time) {
    for(int i=0; i<pixels.size(); i++) {
        HSV *pixel = pixels[i];
        pixel->hue = huePerPixel[i];
        pixel->sat = 1.0f;
        pixel->val = 1.0f;
    }
}
