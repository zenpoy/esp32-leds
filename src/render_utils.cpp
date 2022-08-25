
#include <render_utils.h>
#include <math.h>

void RenderUtils::Show()
{
  for (int i = 0; i < m_numberOfLeds; i++)
  {
    const HSV &hsvVal = m_leds_hsv[i];
    float normalizedBrightness = hsvVal.val * hsvVal.val;
    HsbColor neoPixelColor(fmod(hsvVal.hue, 1.0f), hsvVal.sat, normalizedBrightness);
    m_leds_rgb.SetPixelColor(i, neoPixelColor);
  }

  m_leds_rgb.Show();
}

void RenderUtils::Setup()
{
  m_leds_rgb.Begin();
  Clear();
}

void RenderUtils::Clear()
{
  for (int i = 0; i < m_numberOfLeds; i++)
  {
    m_leds_hsv[i].val = 0.0;
  }
}

void RenderUtils::ShowTestPattern()
{
  for (int i = 0; i < m_numberOfLeds; i++)
  {
    float t = ((i * 5) + millis() % 3000);
    t /= 3000;
    HsbColor neoPixelColor(t, .9, 0.1);
    m_leds_rgb.SetPixelColor(i, neoPixelColor);
  }
  m_leds_rgb.Show();
}