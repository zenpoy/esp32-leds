//----------------------------------------------------------------------
// Test
// This will display on the led panels
//
// This is useful
//
// It does
//----------------------------------------------------------------------

#include <SPIFFS.h>
// #include "SD_MMC.h"
#include <SD.h> // SD card library
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>


#include "panel_config.h"

NeoMosaic<MyPanelLayout> mosaic(
    PanelWidth,
    PanelHeight,
    TileWidth,
    TileHeight);

NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
NeoGamma<NeoGammaTableMethod> colorGamma;

RgbColor red(128, 0, 0);
RgbColor green(0, 128, 0);
RgbColor blue(0, 0, 128);
RgbColor white(128);
RgbColor black(0);

NeoPixelAnimator animations(PixelCount, NEO_MILLISECONDS);
void SetupAnimationSet(uint16_t duration, const uint8_t peak = 128)
{
  for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
  {
    RgbColor originalColor = strip.GetPixelColor(pixel);
    RgbColor targetColor = RgbColor(random(peak), random(peak), random(peak));
    AnimEaseFunction easing;
    switch (random(3))
    {
    case 0:
      easing = NeoEase::CubicIn;
      break;
    case 1:
      easing = NeoEase::CubicOut;
      break;
    case 2:
      easing = NeoEase::QuadraticInOut;
      break;
    }

    AnimUpdateCallback animUpdate = [=](const AnimationParam &param)
    {
      // progress will start at 0.0 and end at 1.0
      // we convert to the curve we want
      float progress = easing(param.progress);

      // use the curve value to apply to the animation
      RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
      updatedColor = colorGamma.Correct(updatedColor);

      strip.SetPixelColor(pixel, updatedColor);
    };

    // now use the animation properties we just calculated and start the animation
    // which will continue to run and call the update function until it completes
    animations.StartAnimation(pixel, duration, animUpdate);
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void setup()
{

  Serial.begin(115200);

  if (!SD.begin())
  {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  strip.Begin();
  strip.Show();

  Serial.println();
  Serial.println("Running...");

  Serial.println();
  Serial.println("If your panel is correctly defined, you should see:");
  Serial.println("Upper Left  0,0 is white ^-  ");
  Serial.println("Upper Right 0,1 is Red    -^ ");
  Serial.println("Lower Right 1,1 is Green  -v ");
  Serial.println("Lower Left  1,0 is Blue  v- ");
    }

void loop()
{

  const uint16_t x0 = 0;
  const uint16_t x1 = PanelWidth - 1;
  const uint16_t y0 = 0;
  const uint16_t y1 = PanelHeight - 1;

  RgbColor c00 = white, c10 = red;
  RgbColor c01 = blue, c11 = green;

  // four corners
  Serial.println("four corners");
  strip.SetPixelColor(mosaic.Map(x0, y0), white);
  strip.SetPixelColor(mosaic.Map(x1, y0), red);
  strip.SetPixelColor(mosaic.Map(x1, y1), green);
  strip.SetPixelColor(mosaic.Map(x0, y1), blue);
  delay(500);

  Serial.println("Bilinear blend");
  // bilinear blend
  for (uint16_t i = x0; i <= x1; i++)
  {
    for (uint16_t j = y0; j <= y1; j++)
    {
      float x = (1.0 * i - x0) / (x1 - x0);
      float y = (1.0 * j - y0) / (y1 - y0);
      strip.SetPixelColor(mosaic.Map(i, j), RgbColor::BilinearBlend(c00, c01, c10, c11, x, y));
    }
  }
  strip.Show();
  delay(500);

  // darken
  Serial.println("darken");
  for (int steps = 100, d = 0; d < steps; d++)
  {
    for (uint16_t p = 0; p < strip.PixelCount(); p++)
    {
      RgbColor c = strip.GetPixelColor(p);
      c.Darken(1); // subtracts 1 from r,g,b. eq: r--,g--,b--;
      strip.SetPixelColor(p, c);
    }
    strip.Show();
    delay(20 / steps);
  }

  // red vertical line | all rows slide from right to left
  Serial.println("red - line");

  for (uint16_t ti = x0; ti <= x1; ti++)
  {
    for (uint16_t i = x0; i <= x1; i++)
    {
      for (uint16_t j = y0; j <= y1; j++)
      {
        RgbColor color = i == ti ? red : black;
        strip.SetPixelColor(mosaic.Map(i, j), color);
      }
    }

    strip.Show();
    delay(30);
  }

  // green line | all rows slide from bottom to top
  Serial.println("green | line");
  for (uint16_t tj = y0; tj <= y1; tj++)
  {
    for (uint16_t i = x0; i <= x1; i++)
    {
      for (uint16_t j = y0; j <= y1; j++)
      {
        RgbColor color = j == tj ? green : black;
        strip.SetPixelColor(mosaic.Map(i, j), color);
      }
    }
    strip.Show();
    delay(30);
  }

  strip.ClearTo(black);
  strip.Show();
  delay(10);

  String filename = "/tstst1024.bin";

  // play()
  Serial.println("play file");

  if (!SD.begin())
  {
    Serial.println("Failed to mount card");
    return;
  }

  auto file = SD.open(filename);

  if (!file)
  {
    Serial.println("Opening file to write failed");
    return;
  }

  const int bufSize = PixelCount * 3;
  const int headerSize = bufSize + 4;
  uint8_t buffer[headerSize];

  int f = 0;          // frame counter
  int t0 = millis();
  while (file.available() && file.read(buffer, headerSize) == headerSize)
  {
    f++;

    if (f % 1000 == 0) {
      int t1 = millis();
      Serial.print(t1 - t0);

      Serial.print (" frames:");
      Serial.println(f);
    }
    for (int i = 0; i < PixelCount; i++)
    {
      int r = buffer[4 + 3 * i + 0];
      int g = buffer[4 + 3 * i + 1];
      int b = buffer[4 + 3 * i + 2];

      RgbColor color(r, g, b);
      color = colorGamma.Correct(color);
      strip.SetPixelColor(i, color);
    }
    strip.Show();
    // delay(1);
  }
  file.close();

  int t1 = millis();
  Serial.println("actual time");
  Serial.println(t1 - t0);

  Serial.println("frames:");
  Serial.println(f);
  delay(500);
}