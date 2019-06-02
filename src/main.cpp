#include <Arduino.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#include <aws_connectivity.h>
AwsConnectivity awsConnectivity;

#define DATA_PIN    2
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

TaskHandle_t Task1;

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print(xPortGetCoreID());
  Serial.print("  Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}

void Task1code( void * parameter) {
  awsConnectivity.setup(callback);
  for(;;) {
    awsConnectivity.loop();
  }
}

void setup() {
  Serial.begin(112500);

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

}

void loop() {
  // Serial.println(millis());
  // Serial.println(xPortGetCoreID());
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();  
  EVERY_N_MILLISECONDS( 20 ) { gHue+=5; } // slowly cycle the "base color" through the rainbow
}

