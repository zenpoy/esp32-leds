#include "song_offset_tracker.h"

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Arduino.h>

void SongOffsetTracker::setup(const IPAddress &timeServerIPp, uint16_t timeServerPort) {
    IPAddress timeServerIP(10,0,0,102);
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
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

    if(!isSongPlaying)
        return -1;

    portENTER_CRITICAL(&mux);
    if(!timesync.m_isTimeValid) {
        portEXIT_CRITICAL(&mux);
        return -1;
    }
    // when esp's millis() function returned this time (songStartTime), the song started
    uint32_t songStartTime = (uint32_t)(songStartTimeEpoch - timesync.m_espStartTimeMs);
    portEXIT_CRITICAL(&mux);
    return currentEspMillis - songStartTime;
}
