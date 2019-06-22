#include "song_offset_tracker.h"

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Arduino.h>

void SongOffsetTracker::setup(const IPAddress &timeServerIP, uint16_t timeServerPort) {
    timesync.setup(timeServerIP, 123);
}

void SongOffsetTracker::loop() {
    timesync.loop();
}

void SongOffsetTracker::HandleCurrentSongMessage(char *data) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    isSongPlaying = doc["song_is_playing"];
    if(isSongPlaying) {
        songStartTimeEpoch = doc["start_time_millis_since_epoch"];
    }
}

int32_t SongOffsetTracker::GetOffsetMs(unsigned long currentEspMillis) {

    if(!timesync.m_isTimeValid)
        return -1;

    if(!isSongPlaying)
        return -2;

    // when esp's millis() function returned this time (songStartTime), the song started
    uint32_t songStartTime = (uint32_t)(songStartTimeEpoch - timesync.m_espStartTimeMs);
    return currentEspMillis - songStartTime;
}