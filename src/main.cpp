#include <Arduino.h>
#include <WiFi.h>
#include <secrets.h>
#include <PubSubClient.h>

#include <animations/i_animation.h>
#include <render_utils.h>
#include <song_offset_tracker.h>
#include <animations_container.h>
#include <fs_manager.h>
#include <num_leds.h>

HSV leds_hsv[NUM_LEDS];
RenderUtils renderUtils(leds_hsv, NUM_LEDS);
SongOffsetTracker songOffsetTracker;
AnimationsContainer animationsContainer;
FsManager fsManager;

SemaphoreHandle_t animationsListMutex;
IAnimation *curr_animation = NULL;

TaskHandle_t Task1;

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived, ");
  Serial.print(length);
  Serial.print(" [");
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
    // unsigned long start_time = millis();
    // IAnimation *generated_animation = AnimationFactory::CreateAnimation((char *)payload);
    // Serial.print("took (ms): ");
    // Serial.println(millis() - start_time);
    // if(xSemaphoreTake(animationsListMutex, portMAX_DELAY) == pdTRUE) {
    //   curr_animation = generated_animation;
    //   xSemaphoreGive(animationsListMutex);
    // }
  } else if (strcmp("animations/alterego", topic) == 0) {
    fsManager.SaveToFs(payload, length);
    animationsContainer.SetFromJson("alterego", (const char *)payload);
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
        client.subscribe("animations/#", 1);
        client.subscribe("current-song", 1);
    }
    else {
        Serial.print("error state:");
        Serial.println(client.state());
    }

}

void Task1code( void * parameter) {
  connectToWifi();
  connectToMessageBroker();
  IPAddress timeServerIP(10, 0, 0, 200);
  songOffsetTracker.setup(timeServerIP, 123);
  for(;;) {
    client.loop();
    songOffsetTracker.loop();
  }
}

void setup() {
  Serial.begin(115200);

  fsManager.setup();
  disableCore0WDT();

  animationsListMutex = xSemaphoreCreateMutex();

  animationsContainer.setup(leds_hsv);
  renderUtils.Setup();

  char buf[2048];
  memset(buf, 0, 2048);
  int bytesRead = fsManager.ReadFromFs((uint8_t *)buf, 2048);
  if(bytesRead > 0) {
    animationsContainer.SetFromJson("alterego", buf);
  }

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
  int32_t songOffset = songOffsetTracker.GetOffsetMs(currentMillis);
  if(songOffset >= 0) {
    const AnimationsContainer::AnimationsList *currList = animationsContainer.GetAnimationsList("alterego", songOffset);
    for(AnimationsContainer::AnimationsList::const_iterator it = currList->begin(); it != currList->end(); it++) {
      IAnimation *animation = *it;
      if(animation != nullptr && animation->IsActive(songOffset)) {
        animation->Render((unsigned long)songOffset);
      }
    }
  }
  unsigned long renderLoopTime = millis() - currentMillis;
  renderUtils.Show();
}
