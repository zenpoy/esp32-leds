
#include <render_utils.h>

void RenderUtils::Show()
{
  for(int i=0; i<NUM_LEDS; i++) {
    const HSV &hsvVal = m_leds_hsv[i];
    uint8_t newHue = (uint8_t)((int)(hsvVal.hue * 255.0));
    uint8_t newSat = max(0, min(255, (int)(hsvVal.sat * 255.0)));
    uint8_t newVal = max(0, min(255, (int)(hsvVal.val * 255.0)));
    CHSV fastLedHSV(newHue, newSat, newVal);
    m_leds_rgb[i] = (CRGB)fastLedHSV;
  }

  FastLED.show();
}

void RenderUtils::Setup()
{
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(m_leds_rgb, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);

  for(int i=0; i<NUM_LEDS; i++) {
    m_leds_hsv[i].val = 0.0;
  }

}

void RenderUtils::Clear()
{
  for(int i=0; i<NUM_LEDS; i++) {
    m_leds_hsv[i].val = 0.0;
  }
}
