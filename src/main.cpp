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

#include "SPIFFS.h"


HSV leds_hsv[NUM_LEDS];
RenderUtils renderUtils(leds_hsv, NUM_LEDS);
SongOffsetTracker songOffsetTracker;
AnimationsContainer animationsContainer;
FsManager fsManager;

TaskHandle_t Task1;

StaticJsonDocument<32768> doc;

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

  if (strncmp("animations/", topic, 11) == 0) {
    int songNameStartIndex = 11 + strlen(thingname) + 1;
    String songName = String(topic + songNameStartIndex);
    fsManager.SaveToFs((String("/music/") + songName).c_str(), payload, length);
    animationsContainer.SetFromJsonFile(songName, doc);
    
  } else if(strcmp("current-song", topic) == 0) {
    bool fileChanged = songOffsetTracker.HandleCurrentSongMessage((char *)payload);
    if(fileChanged) {
      Serial.println("current song changed, loading animations");
      animationsContainer.SetFromJsonFile(songOffsetTracker.GetCurrentFile(), doc);
    }
    
  } else if(strncmp("objects-config", topic, 14) == 0) {
    fsManager.SaveToFs("/objects-config", payload, length);
    ESP.restart();
  }

}

void connectToWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Thing name: ");
    Serial.println(thingname);
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
        client.subscribe((String("objects-config/") + String(thingname)).c_str(), 1);
        client.subscribe("current-song", 1);
        client.subscribe((String("animations/") + String(thingname) + String("/#")).c_str(), 1);
    }
    else {
        Serial.print("error state:");
        Serial.println(client.state());
    }

}

void HandleObjectsConfig(File &f) {

  int totalPixels = AnimationFactory::InitObjectsConfig(leds_hsv, doc, f);
  if(AnimationFactory::objectsMapErrorString == NULL) {
    Serial.print("total pixels: ");
    Serial.println(totalPixels);
  } else {
    Serial.print("objects map encountered an error while initializing: ");
    Serial.println(AnimationFactory::objectsMapErrorString);
  }
}

void MonitorLoop( void * parameter) {
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

  File file = SPIFFS.open("/objects-config");
  if(file){
      HandleObjectsConfig(file);
      file.close();
  }
  else {
      Serial.println("Failed to open objects config file for reading");
  }

  xTaskCreatePinnedToCore(
      MonitorLoop, /* Function to implement the task */
      "MonitorTask", /* Name of the task */
      4096,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

}

// we keep this object once so we don't need to create the string on every loop
CurrentSongDetails songDetails;

void loop() {

  renderUtils.Clear();

  unsigned long currentMillis = millis();
  bool hasValidSong = songOffsetTracker.GetCurrentSongDetails(currentMillis, &songDetails);
  if(hasValidSong && songDetails.offsetMs >= 0) {
    unsigned long songOffset = (unsigned long)songDetails.offsetMs;
    const AnimationsContainer::AnimationsList *currList = animationsContainer.GetAnimationsList(songDetails.songName, songOffset);
    // Serial.print("number of animations: ");
    // Serial.println(currList->size());
    for(AnimationsContainer::AnimationsList::const_iterator it = currList->begin(); it != currList->end(); it++) {
      IAnimation *animation = *it;
      if(animation != nullptr && animation->IsActive(songOffset)) {
        animation->Render((unsigned long)songOffset);
      }
    }
  }
  unsigned long renderLoopTime = millis() - currentMillis;
  // Serial.print("loop time ms: ");
  // Serial.println(renderLoopTime);
  renderUtils.Show();
}
