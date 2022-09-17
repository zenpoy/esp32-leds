#define CONFIG_USE_ONLY_LWIP_SELECT 1

#include <Arduino.h>
#include <WiFi.h>
#include <secrets.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SD.h> // SD card library

#include <song_offset_tracker.h>
#include <fs_manager.h>

#include "SPIFFS.h"
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
#include "panel_config.h"
#include <esp_task_wdt.h>

#ifndef NUM_LEDS
#warning NUM_LEDS not definded. using default value of 300
#define NUM_LEDS 300
#endif // NUM_LEDS

#ifndef MQTT_BROKER_PORT
#define MQTT_BROKER_PORT 1883
#endif // MQTT_BROKER_PORT

const unsigned int WD_TIMEOUT_MS = 2000;

NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
NeoGamma<NeoGammaTableMethod> colorGamma;

SongOffsetTracker songOffsetTracker;
// AnimationsContainer animationsContainer;
FsManager fsManager;

TaskHandle_t Task1;

File aniFile;

uint8_t frameBuffer[headerSize];
int lastAnimationTime = -1;

struct NewSongMsg
{
  bool onlyUpdateTime;
  // const AnimationsList *anList;
  int32_t songStartTime;
};
QueueHandle_t anListQueue;
const int anListQueueSize = 10;
int32_t lastReportedSongStartTime = 0;

QueueHandle_t deleteAnListQueue;
const int deleteAnListQueueSize = 10;

QueueHandle_t wdQueue;
const int wdQueueSize = 10;

void PrintCorePrefix()
{
  Serial.print("[");
  Serial.print(xPortGetCoreID());
  Serial.print("]: ");
}

// StaticJsonDocument<40000> doc;

void Core0WDSend(unsigned int currMillis)
{
  static unsigned int lastWdSendTime = 0;
  if (currMillis - lastWdSendTime > WD_TIMEOUT_MS)
  {
    // Serial.println("[0] send wd msg from core 0 to core 1");
    int unused = 0;
    xQueueSend(wdQueue, &unused, 5);
    lastWdSendTime = currMillis;
  }
}

void Core0WdReceive(unsigned int currMillis)
{
  static unsigned int lastCore0WdReceiveTime = 0;
  int unused;
  if (xQueueReceive(wdQueue, &unused, 0) == pdTRUE)
  {
    lastCore0WdReceiveTime = currMillis;
  }

  unsigned int timeSinceWdReceive = currMillis - lastCore0WdReceiveTime;
  // Serial.print("[0] timeSinceWdReceive: ");
  // Serial.println(timeSinceWdReceive);
  if (timeSinceWdReceive > (3 * WD_TIMEOUT_MS))
  {
    ESP.restart();
  }
}

void CheckForSongStartTimeChange()
{
  if (!songOffsetTracker.IsSongPlaying())
    return;

  int32_t currStartTime = songOffsetTracker.GetSongStartTime();
  if (currStartTime == lastReportedSongStartTime)
    return;

  lastReportedSongStartTime = currStartTime;

  NewSongMsg msg;
  msg.onlyUpdateTime = true;
  msg.songStartTime = currStartTime;
  //! msg.anList = nullptr;
  Serial.println("updateing time of current song start");
  xQueueSend(anListQueue, &msg, portMAX_DELAY);
}

void SendAnListUpdate()
{
  Serial.println("SendAnListUpdate");
  NewSongMsg msg;
  if (songOffsetTracker.IsSongPlaying())
  {
    Serial.println("IsSongPlaying");
    String currFileName = songOffsetTracker.GetCurrentFile();
    PrintCorePrefix();
    Serial.print("currFileName: ");
    Serial.println(currFileName);
    lastReportedSongStartTime = songOffsetTracker.GetSongStartTime();
    msg.songStartTime = lastReportedSongStartTime;
    msg.onlyUpdateTime = false;
    if (msg.songStartTime != 0)
    {
      //! msg.anList = animationsContainer.SetFromJsonFile(currFileName, doc);
      Serial.println("todo: update animation file");
    }
    else
    {
      PrintCorePrefix();
      Serial.println("ignoring an list update since song start time is not valid yet");
      //! msg.anList = nullptr;
    }
  }
  else
  {
    PrintCorePrefix();
    Serial.println("no song is playing");
    lastReportedSongStartTime = 0;
    // msg.anList = nullptr;
    msg.songStartTime = 0;
    msg.onlyUpdateTime = false;
  }

  xQueueSend(anListQueue, &msg, portMAX_DELAY);
}

void SendStartTimeToRenderCore()
{
  if (!songOffsetTracker.IsSongPlaying())
    return;

  NewSongMsg msg;
  msg.onlyUpdateTime = true;
  msg.songStartTime = songOffsetTracker.GetSongStartTime();
  //! msg.anList = nullptr;
  PrintCorePrefix();
  Serial.println("updating time of current song start");
  xQueueSend(anListQueue, &msg, portMAX_DELAY);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  PrintCorePrefix();
  Serial.print("MQTT message on topic ");
  Serial.print(topic);
  Serial.print(" size: ");
  Serial.print(length);
  Serial.println("");
  // Serial.println("payload:");
  // Serial.println((char*)payload);

  if (strncmp("animations/", topic, 11) == 0)
  {
    int songNameStartIndex = 11 + strlen(THING_NAME) + 1;
    String songName = String(topic + songNameStartIndex);
    fsManager.SaveToFs((String("/music/") + songName).c_str(), payload, length);

    if (songOffsetTracker.GetCurrentFile() == songName)
    {
      SendAnListUpdate();
    }
  }
  else if (strcmp("current-song", topic) == 0)
  {
    songOffsetTracker.HandleCurrentSongMessage((char *)payload);
    SendAnListUpdate();
  }
  else if (strncmp("objects-config", topic, 14) == 0)
  {
    fsManager.SaveToFs("/objects-config", payload, length);
    ESP.restart();
  }

  Serial.print("done handling mqtt callback: ");
  Serial.println(topic);
}

void ConnectToWifi()
{

  if (WiFi.status() == WL_CONNECTED)
    return;

  while (true)
  {
    unsigned int connectStartTime = millis();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WIFI_PASSWORD);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SSID);
    while (millis() - connectStartTime < 10000)
    {
      Serial.print(".");
      Core0WDSend(millis());
      delay(1000);
      Core0WDSend(millis());
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("connected to wifi");
        return;
      }
    }
    Serial.println(" could not connect for 10 seconds. retry");
  }
}

WiFiClient net;
PubSubClient client(net);
void ConnectToMessageBroker()
{

  if (client.connected())
    return;

  client.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);
  client.setCallback(callback);
  StaticJsonDocument<128> json_doc;
  json_doc["ThingName"] = THING_NAME;
  json_doc["Alive"] = false;
  char lastWillMsg[128];
  serializeJson(json_doc, lastWillMsg);
  Serial.println("connecting to mqtt");
  if (client.connect(THING_NAME, MONITOR_TOPIC, 1, true, lastWillMsg))
  {
    Serial.print("  connected to message broker");
    Serial.print(". monitor topic:");
    Serial.print(MONITOR_TOPIC);
    Serial.println();

    String objectTopic = "objects-config/" + String(THING_NAME);
    bool ok = client.subscribe(objectTopic.c_str(), 1);
    Serial.printf("  subscribed to topic [%s] %s\n", objectTopic.c_str(), ok ? " ok " : "FAIL");

    String currentSongTopic = "current-song";
    ok = client.subscribe(currentSongTopic.c_str(), 1);
    Serial.printf("  subscribed to topic [%s] %s\n", currentSongTopic.c_str(), ok ? " ok " : "FAIL");

    String animationsTopic = "animations/" + String(THING_NAME) + "/#";
    client.subscribe(animationsTopic.c_str(), 1);
    Serial.printf("  subscribed to topic [%s] %s\n", animationsTopic.c_str(), ok ? " ok " : "FAIL");
  }
  else
  {
    Serial.print("mqtt connect failed. error state:");
    Serial.println(client.state());
  }
}

void ReadObjectsConfigFile(String filename)
{
  File file = SPIFFS.open(filename.c_str());
  if (file)
  {
    // int totalPixels = AnimationFactory::InitObjectsConfig(leds_hsv, doc, file);
    // if (AnimationFactory::objectsMapErrorString == NULL)
    if (file.size())
    {
      Serial.print("initialized object map. total pixels: ");
      // Serial.println(totalPixels);
    }
    else
    {
      Serial.print("objects map encountered an error while initializing: ");
      // Serial.println(AnimationFactory::objectsMapErrorString);
    }
  }
  else
  {
    Serial.println("Failed to open objects config file for reading");
  }
  file.close();
}

void DeleteAnListPtr()
{
  // const AnimationsList *ptrToDelete;
  // if (xQueueReceive(deleteAnListQueue, &ptrToDelete, 0) == pdTRUE)
  // {
  // for (IAnimation *an : (*ptrToDelete))
  // {
  //   delete an;
  // }
  // delete ptrToDelete;
  // }
}

// const AnimationsList *global_anList;
int32_t global_songStartTime;

void SendMonitorMsg(char *buffer, size_t bufferSize)
{
  StaticJsonDocument<128> json_doc;
  json_doc["ThingName"] = THING_NAME;
  json_doc["Alive"] = true;
  json_doc["WifiSignal"] = WiFi.RSSI();
  json_doc["millis"] = millis();
  // json_doc["global song start time"] = global_songStartTime;
  json_doc["global time"] = ((int32_t)(millis())) - global_songStartTime;
  serializeJson(json_doc, buffer, bufferSize);
  // report to monitor what song is running, animations, etc.
}

void MonitorLoop(void *parameter)
{

  ConnectToWifi();
  songOffsetTracker.setup();
  unsigned int lastReportTime = millis();
  unsigned int lastMonitorTime = millis();
  for (;;)
  {
    DeleteAnListPtr();
    ConnectToWifi();
    ConnectToMessageBroker();
    unsigned int currTime = millis();
    Core0WDSend(currTime);
    if (currTime - lastMonitorTime >= 1000)
    {
      char monitorMsg[128];
      SendMonitorMsg(monitorMsg, 128);
      client.publish(MONITOR_TOPIC, monitorMsg, true);
      lastMonitorTime = currTime;
    }
    if (currTime - lastReportTime >= 5000)
    {
      lastReportTime = currTime;

      PrintCorePrefix();
      Serial.print("status: millis: ");
      Serial.print(millis());
      Serial.print(" wifi:");
      Serial.print(WiFi.status() == WL_CONNECTED);
      Serial.print(" mqtt:");
      Serial.print(client.connected());
      // Serial.print(" hasValidSong:");
      // Serial.print(global_anList != nullptr);
      Serial.print(" songOffset:");
      Serial.print(((int32_t)millis()) - global_songStartTime);
      Serial.println();
    }
    client.loop();
    bool clockChanged, clockFirstValid;
    songOffsetTracker.loop(&clockChanged, &clockFirstValid);
    if (clockChanged)
    {
      if (clockFirstValid)
      {
        SendAnListUpdate();
      }
      else
      {
        SendStartTimeToRenderCore();
      }
    }

    vTaskDelay(5);
  }
}

void readBufferFromFile(File &file, uint8_t *buf, uint32_t pos,size_t size)
{
  // Serial.print("Reading file: ");
  // Serial.println(path);

  // File file = fs.open(path);
  // if (!file)
  // {
  //   Serial.println("Failed to open file for reading");
  //   return;
  // }

  Serial.printf("Read from file: %i -> %i\n", pos, size);
  // disableCore1WDT();
  // esp_task_wdt_delete(0);
  uint32_t current_pos = file.position();
  file.seek(0, fs::SeekEnd);
  uint32_t end_of_file = file.position();
  file.seek(current_pos, fs::SeekSet);
  if (pos + size > end_of_file)
  { 
    Serial.println("Trying to read beyond file");
    return;
  }
  if (file.seek(pos, fs::SeekSet))
  {
    file.read(buf, size);
  }
  else
  {
    current_pos = file.position();
    Serial.printf("Failed to read file: current %i, desired %i\n", current_pos, pos);
  }
  // enableCore1WDT();
  // esp_task_wdt_add(0);
  // file.close();
}

void beginSDCard(){
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
}

void setup()
{
  Serial.begin(115200);
  
  beginSDCard();

  aniFile = SD.open(filename);

  if (!aniFile)
  {
    Serial.println("Opening file failed");
    return;
  }

  delay(500);

  strip.Begin();
  strip.Show();

  disableCore0WDT();

  //! global_anList = nullptr;
  global_songStartTime = 0;

  anListQueue = xQueueCreate(anListQueueSize, sizeof(NewSongMsg));
  //! deleteAnListQueue = xQueueCreate(deleteAnListQueueSize, sizeof(const AnimationsList *));
  wdQueue = xQueueCreate(wdQueueSize, sizeof(int));

  PrintCorePrefix();
  Serial.println("=== setup ===");
  Serial.print("Thing name: ");
  Serial.println(THING_NAME);

  bool ok = false;
  PrintCorePrefix();
  Serial.print("fsManager.setup() ");
  ok = fsManager.setup();
  Serial.println(ok ? " ok " : "FAIL");

  PrintCorePrefix();
  // Serial.println("ReadObjectsConfigFile ");
  // ReadObjectsConfigFile("/objects-config");

  xTaskCreatePinnedToCore(
      MonitorLoop,   /* Functirenon to implement the task */
      "MonitorTask", /* Name of the task */
      16384,         /* Stack size in words */
      NULL,          /* Task input parameter */
      0,             /* Priority of the task */
      &Task1,        /* Task handle. */
      0);            /* Core where the task should run */
}

unsigned int lastPrint1Time = millis();
unsigned int lastSecond = 0;

template <typename T_COLOR_FEATURE, typename T_METHOD>
void renderFrame(uint8_t *buffer, NeoPixelBus<T_COLOR_FEATURE, T_METHOD> &strip)
{

#ifdef MONDEB
  int p = 0;
  for (int j = 0; j < PanelWidth; j++)
  {
    for (int i = 0; i < PanelHeight; i++)
    {
      if (j % 2 == 1)
        p = j * PanelHeight + (PanelHeight - i);
      else 
        p = j * PanelHeight + i;

      int r = buffer[timeSize + 3 * p + 0];
      int g = buffer[timeSize + 3 * p + 1];
      int b = buffer[timeSize + 3 * p + 2];

      RgbColor color(r, g, b);
      color = colorGamma.Correct(color);

      r = color.G;

      if (r < 64)
        Serial.print(" ");
      else if (r < 128)
        Serial.print("░");
      else if (r < 192)
        Serial.print("▒");
      else if (r < 256)
        Serial.print("▓");
    }
    Serial.println("");
  }
#endif

  for (int i = 0; i < strip.PixelCount(); i++)
  {
    int r = buffer[timeSize + 3 * i + 0];
    int g = buffer[timeSize + 3 * i + 1];
    int b = buffer[timeSize + 3 * i + 2];

    RgbColor color(r, g, b);
    color = colorGamma.Correct(color);
    strip.SetPixelColor(i, color);
  }
}

unsigned long int frame = -1;
void loop()
{
  unsigned long currentMillis = millis();
  Core0WdReceive(currentMillis);

  if (currentMillis - lastPrint1Time >= 5000)
  {
    PrintCorePrefix();
    Serial.println("core 1 alive");
    lastPrint1Time = currentMillis;
  }

  NewSongMsg newMsg;
  if (xQueueReceive(anListQueue, &newMsg, 0) == pdTRUE)
  {
    PrintCorePrefix();
    Serial.println("received message on NewSongMsg queue");

    PrintCorePrefix();
    Serial.print("songStartTime: ");
    Serial.println(newMsg.songStartTime);

    PrintCorePrefix();
    Serial.print("onlyUpdateTime: ");
    Serial.println(newMsg.onlyUpdateTime);

    if (newMsg.onlyUpdateTime)
    {
      PrintCorePrefix();
      Serial.print("only update time: ");
      global_songStartTime = newMsg.songStartTime;
    }
    else
    {
      // if (global_anList != nullptr)
      // {
      //   Serial.println("sending animation ptr for deleteing to core 0");
      //   xQueueSend(deleteAnListQueue, &global_anList, portMAX_DELAY);
      // }
      // global_anList = newMsg.anList;
      global_songStartTime = newMsg.songStartTime;
    }
  }

  int32_t songOffset = ((int32_t)(currentMillis)) - global_songStartTime;
  int32_t currentFrame = songOffset / fileSampleRateMs;
  // int32_t currentFrame = 0;
  // int32_t frames[] = {0, 1500000};
  // int32_t currentFrame = frame;
  if (frame != currentFrame)
  {
    // Serial.println(currentFrame);
    aniFile.seek(currentFrame * headerSize);
    frame = currentFrame;
    if (aniFile.available() && aniFile.read(frameBuffer, headerSize) == headerSize)
    {
      // if (frame % 1000 == 0)
      // {
      //   Serial.println(frame);
      // }
      renderFrame(frameBuffer, strip);
      // delay(fileSampleRateMs);
    }
  }
  strip.Show();

  // while (true)
  // {
  //   currentFrame = frame;
  //   // if (currentFrame % 1000 == 0)
  //   // {
  //   Serial.print("Current frame: ");
  //   Serial.println(currentFrame);
  // // }

  //   readBufferFromFile(aniFile, frameBuffer, currentFrame * headerSize, headerSize);

  //   frame++;
  // }

  int animationTime = frameBuffer[0];

  // if (currentFrame % 1000 == 0)
  // {
  //   Serial.print("Animation time: ");
  //   Serial.println(animationTime);
  // }

  // if (animationTime != lastAnimationTime) {
  //   renderFrame(frameBuffer, strip);
  //   lastAnimationTime = animationTime;
  // }

  vTaskDelay(5);
  }
