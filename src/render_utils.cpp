
#include <render_utils.h>

void RenderUtils::Show()
{
  for(int i=0; i<m_numberOfLeds; i++) {
    const HSV &hsvVal = m_leds_hsv[i];
    HsbColor neoPixelColor(hsvVal.hue, hsvVal.sat, hsvVal.val);
    m_leds_rgb.SetPixelColor(i, neoPixelColor);
  }

  m_leds_rgb.Show();
}

void RenderUtils::Setup()
{
  m_leds_rgb.Begin();
  for(int i=0; i<m_numberOfLeds; i++) {
    m_leds_hsv[i].val = 0.0;
  }

}

void RenderUtils::Clear()
{
  for(int i=0; i<m_numberOfLeds; i++) {
    m_leds_hsv[i].val = 0.0;
  }
}
