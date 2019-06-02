#ifndef __RENDER_UTILS_H__
#define __RENDER_UTILS_H__

#include <num_leds.h>
#include <hsv.h>

#include <FastLED.h>

#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

class RenderUtils {

public:
    RenderUtils(HSV leds_hsv[NUM_LEDS]) : m_leds_hsv(leds_hsv) {}

public:
    void Setup();
    void Show();
    void Clear();

private:
    HSV *m_leds_hsv;
    CRGB m_leds_rgb[NUM_LEDS];

};

#endif // __RENDER_UTILS_H__
