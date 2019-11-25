#include "song_offset_tracker.h"

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Arduino.h>

#ifndef TIME_SERVER_PORT
#define TIME_SERVER_PORT 123
#endif // TIME_SERVER_PORT


void SongOffsetTracker::setup() {
    timesync.setup(TIME_SERVER_IP, TIME_SERVER_PORT);
}

void SongOffsetTracker::loop() {
    timesync.loop();
}

bool SongOffsetTracker::HandleCurrentSongMessage(char *data) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, data);

    bool currSongChanged = false;

    bool wasSongPlaying = isSongPlaying;
    isSongPlaying = doc["song_is_playing"];
    if(isSongPlaying) {
        // saving this value as int64 for easier calculations later
        songStartTimeEpoch = doc["start_time_millis_since_epoch"];
        const char *fileIdPtr = doc["file_id"];
        currSongChanged = strcmp(fileIdPtr, fileNameFromPlayer.c_str()) != 0;
        fileNameFromPlayer = fileIdPtr;

        fileName = fileNameFromPlayer;
        int dotIndex = fileName.indexOf('.');
        if(dotIndex >= 0) {
            fileName.remove(dotIndex);
        }
    }
    else  {
        fileName = "";
        currSongChanged = (wasSongPlaying != isSongPlaying);
    }

    return currSongChanged;
}

// void printInt64(int64_t num) {
//     char rev[128]; 
//     char *p = rev+1;

//     if(num < 0) {
//         Serial.print("-");
//         num = -num;
//     }

//     while (num > 0) {
//         *p++ = '0' + ( num % 10);
//         num/= 10;
//     }
//     p--;
//     /*Print the number which is now in reverse*/
//     while (p > rev) {
//         Serial.print(*p--);
//     }
// }

int32_t SongOffsetTracker::GetSongStartTime() {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    if(!timesync.m_isTimeValid) {
        portEXIT_CRITICAL(&mux);
        return 0;
    }

    // when esp's millis() function returned this time (songStartTime), the song started
    int32_t songStartTime = (int32_t)(songStartTimeEpoch - timesync.m_espStartTimeMs);
    portEXIT_CRITICAL(&mux);
    return songStartTime;
}

