#include <Arduino.h>
#include <WiFi.h>
#include "FastLED.h"
#include <secrets.h>
#include <PubSubClient.h>

FASTLED_USING_NAMESPACE

//#include <aws_connectivity.h>
#include <animations/i_animation.h>
#include <animation_factory.h>
#include <render_utils.h>
#include <song_offset_tracker.h>
#include <animations_container.h>

//AwsConnectivity awsConnectivity;

HSV leds_hsv[NUM_LEDS];
RenderUtils renderUtils(leds_hsv);
SongOffsetTracker songOffsetTracker;
AnimationsContainer animationsContainer;

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
  } else if(strcmp("current-song", topic) == 0) {
    songOffsetTracker.HandleCurrentSongMessage((char *)payload);
  }

}

void connectToWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.printf("Attempting to connect to SSID: ");
    Serial.printf(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("ok!");
}

WiFiClient net;
PubSubClient client(net);
void connectToMessageBroker() {
    client.setServer(mqttHost, 1883);
    client.setCallback(callback);
    if(client.connect(thingname)) {
        Serial.println("connected to message broker");
        client.subscribe("animations/#");
        client.subscribe("current-song");
    }
    else {
        Serial.print("error state:");
        Serial.println(client.state());
    }

}

void Task1code( void * parameter) {
  connectToWifi();
  connectToMessageBroker();
  IPAddress timeServerIP(10, 0, 0, 102);
  //songOffsetTracker.setup(timeServerIP, 123);
  for(;;) {
    client.loop();
    //songOffsetTracker.loop();
  }
}

void setup() {
  Serial.begin(112500);
  animationsListMutex = xSemaphoreCreateMutex();

  renderUtils.Setup();
  AnimationFactory::InitObjectMap(leds_hsv);

  const char *jsonStr = "{\"animation_name\":\"rainbow\",\"pixels_name\":\"a\",\"timeout\":5000,\"animation_params\":{\"start_hue\":{\"type\":\"sin\",\"params\":{\"min_value\":0.0,\"max_value\":1.0,\"phase\":0.0,\"repeats\":1.0}},\"end_hue\":{\"type\":\"sin\",\"params\":{\"min_value\":1.0,\"max_value\":2.0,\"phase\":0.0,\"repeats\":1.0}}}}";
  animationsContainer.SetFromJson(jsonStr);

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

  renderUtils.Clear();

  unsigned long currentMillis = millis();
  //int32_t songOffset = songOffsetTracker.GetOffsetMs(currentMillis);
  //if(songOffset >= 0) {
    // AnimationsContainer::ConstAnimationsVector &currList = animationsContainer.GetAnimationsList(songOffset);
    // for(AnimationsContainer::ConstAnimationsVector::const_iterator it = currList.begin(); it != currList.end(); it++) {
    //   IAnimation *animation = *it;
    //   if(animation != nullptr) {
    //     animation->Render((unsigned long)songOffset);
    //   }
    // }
  //}

  // // if(xSemaphoreTake(animationsListMutex, portMAX_DELAY) == pdTRUE) {
  // //   IAnimation *animationCopy = curr_animation;
  // //   xSemaphoreGive(animationsListMutex);
  // //   if(animationCopy != NULL) {
  // //     animationCopy->Render(millis());
  // //   }
  // // }
  
  renderUtils.Show();
}

