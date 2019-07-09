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
#include <animation_factory.h>

HSV leds_hsv[NUM_LEDS];
RenderUtils renderUtils(leds_hsv, NUM_LEDS);
SongOffsetTracker songOffsetTracker;
AnimationsContainer animationsContainer;
FsManager fsManager;

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

  if (strcmp("animations/alterego", topic) == 0) {
    fsManager.SaveToFs("/music/alterego", payload, length);
    animationsContainer.SetFromJson("alterego", (const char *)payload);
  } else if(strcmp("current-song", topic) == 0) {
    songOffsetTracker.HandleCurrentSongMessage((char *)payload);
  } else if(strcmp("objects-config", topic) == 0) {
    fsManager.SaveToFs("/objects-config", payload, length);
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
        client.subscribe("objects-config", 1);
    }
    else {
        Serial.print("error state:");
        Serial.println(client.state());
    }

}

void HandleObjectsConfig(const char *jsonBuf) {

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, jsonBuf);

  int totalPixels = doc["total_pixels"];
  Serial.println(totalPixels);

  AnimationFactory::InitObjectMap(leds_hsv, doc["objects"]);
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
  disableCore0WDT();

  fsManager.setup();
  animationsContainer.setup();
  renderUtils.Setup();

  char buf[2048];
  int bytesRead;

  memset(buf, 0, 2048);
  bytesRead = fsManager.ReadFromFs("/objects-config", (uint8_t *)buf, 2048);
  if(bytesRead > 0) {
    HandleObjectsConfig(buf);
  }

  memset(buf, 0, 2048);
  bytesRead = fsManager.ReadFromFs("/music/alterego", (uint8_t *)buf, 2048);
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
