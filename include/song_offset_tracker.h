#ifndef __SONG_OFFSET_TRACKER_H__
#define __SONG_OFFSET_TRACKER_H__

#include <timesync.h>

struct CurrentSongDetails {
    bool valid;
    String songName;
    int32_t offsetMs;
};

class SongOffsetTracker
{
    public:
        SongOffsetTracker();
        void setup(const IPAddress &timeServerIP, uint16_t timeServerPort);
        void loop();

    public:
        
        bool HandleCurrentSongMessage(char *data);
        const String GetCurrentFile() const { return fileName; }
        bool IsSongPlaying() { return isSongPlaying; }

    public:
        int32_t GetSongStartTime();
        bool GetCurrentSongDetails(unsigned long currentEspMillis, CurrentSongDetails *outSongDetails);

    private:
        TimeSync timesync;

    private:
        SemaphoreHandle_t songDataMutex; // mutex to access these fields from deferent cores
        bool isSongPlaying = false;
        int64_t songStartTimeEpoch; // valid only if isSongPlaying is true
        String fileNameFromPlayer; // // valid only if isSongPlaying is true
        String fileName; // valid only if isSongPlaying is true
};


#endif //__SONG_OFFSET_TRACKER_H__
