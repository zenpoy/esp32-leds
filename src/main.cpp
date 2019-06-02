#include <Arduino.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

#include <aws_connectivity.h>
#include <animations/i_animation.h>
#include <animation_factory.h>
#include <render_utils.h>

AwsConnectivity awsConnectivity;

HSV leds_hsv[NUM_LEDS];
RenderUtils renderUtils(leds_hsv);

SemaphoreHandle_t animationsListMutex;
IAnimation *curr_animation = NULL;

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

  if (strcmp("animations/clear", topic) == 0) {
    // for(AnimationsList::iterator it = animationsList.begin(); it != animationsList.end(); ++it) {
    //   IAnimation *animation = *it;
    //   delete animation;
    // }
    // animationsList.clear();
  } else if (strcmp("animations/set", topic) == 0){
    unsigned long start_time = millis();
    IAnimation *generated_animation = AnimationFactory::CreateAnimation((char *)payload);
    Serial.print("took (ms): ");
    Serial.println(millis() - start_time);
    if(xSemaphoreTake(animationsListMutex, portMAX_DELAY) == pdTRUE) {
      curr_animation = generated_animation;
      xSemaphoreGive(animationsListMutex);
    }

    // animationsList.push_back(generated_animation);
  }

}

void Task1code( void * parameter) {
  awsConnectivity.setup(callback);
  for(;;) {
    awsConnectivity.loop();
  }
}

void setup() {
  Serial.begin(112500);
  animationsListMutex = xSemaphoreCreateMutex();

  renderUtils.Setup();
  AnimationFactory::InitObjectMap(leds_hsv);

  xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      50000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

}

void loop() {

  if(xSemaphoreTake(animationsListMutex, portMAX_DELAY) == pdTRUE) {
    IAnimation *animationCopy = curr_animation;
    xSemaphoreGive(animationsListMutex);
    if(animationCopy != NULL) {
      animationCopy->Render(millis());
    }
  }

  renderUtils.Show();
}

