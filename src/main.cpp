//----------------------------------------------------------------------
// NeoPixelTopologyTest
// This will display specific colors in specific locations on the led panels
//
// This is useful in confirming the layout of your panels
//
// It does require that you have the actual panels connected
//----------------------------------------------------------------------

#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>

// uncomment one of these that matches your panel pixel layouts
// rotation is ignored for mosaic as it applies a rotation for you
// that is specific to the location of the panel within the mosaic
// to reduce connection lengths

// typedef ColumnMajorAlternatingLayout MyPanelLayout;
// typedef ColumnMajorLayout MyPanelLayout;
// typedef RowMajorAlternatingLayout MyPanelLayout;
// typedef RowMajorLayout MyPanelLayout;
// typedef RowMajorAlternating270Layout MyPanelLayout;
typedef ColumnMajorAlternating180Layout MyPanelLayout;

// make sure to set these panel values to the sizes of yours
const uint8_t PanelWidth = 10; // 8 pixel x 8 pixel matrix of leds
const uint8_t PanelHeight = 20;
const uint8_t TileWidth = 1; // laid out in 4 panels x 2 panels mosaic
const uint8_t TileHeight = 1;

const uint16_t PixelCount = PanelWidth * PanelHeight * TileWidth * TileHeight;
const uint8_t PixelPin = 2;

NeoMosaic<MyPanelLayout> mosaic(
    PanelWidth,
    PanelHeight,
    TileWidth,
    TileHeight);

NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// for esp8266 omit the pin
// NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);

RgbColor red(128, 0, 0);
RgbColor green(0, 128, 0);
RgbColor blue(0, 0, 128);
RgbColor white(128);
RgbColor black(0);

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  Serial.println();
  Serial.println("Initializing...");

  strip.Begin();
  strip.Show();

  Serial.println();
  Serial.println("Running...");
}

void loop()
{
  delay(2500);

  Serial.println();
  Serial.println("If your panel is correctly defined, you should see:");
  Serial.println("Upper Left  0,0 is white ^-  ");
  Serial.println("Upper Right 0,1 is Red    -^ ");
  Serial.println("Lower Right 1,1 is Green  -v ");
  Serial.println("Lower Left  1,0 is Blue  v- ");

  // use the topo to map the 2d cordinate to the pixel
  // and use that to SetPixelColor
  const uint16_t x0 = 0;
  const uint16_t x1 = PanelWidth - 1;
  const uint16_t y0 = 0;
  const uint16_t y1 = PanelHeight - 1;

  RgbColor c00 = white, c10 = red;
  RgbColor c01 = blue, c11 = green;

  strip.SetPixelColor(mosaic.Map(x0, y0), white);
  strip.Show();
  delay(300);
  strip.SetPixelColor(mosaic.Map(x1, y0), red);
  strip.Show();
  delay(300);
  strip.SetPixelColor(mosaic.Map(x1, y1), green);
  strip.Show();
  delay(300);
  strip.SetPixelColor(mosaic.Map(x0, y1), blue);
  strip.Show();
  delay(1300);

  for (uint16_t i = x0; i <= x1; i++)
  {
    for (uint16_t j = y0; j <= y1; j++)
    {
      float x = (1.0 * i - x0) / x1 - x0;
      float y = (1.0 * j - y0) / y1 - y0;
      strip.SetPixelColor(mosaic.Map(i, j), RgbColor::BilinearBlend(c00, c01, c10, c11, x, y));
    }
  }
  strip.Show();

  delay(3000);

  Serial.println();
  Serial.println("Cleared to black ...");
  strip.ClearTo(black);
  strip.Show();
}
