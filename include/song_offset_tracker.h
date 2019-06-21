#ifndef __SONG_OFFSET_TRACKER_H__
#define __SONG_OFFSET_TRACKER_H__

#include <timesync.h>

class SongOffsetTracker
{
    public:
        void setup(const IPAddress &timeServerIP, uint16_t timeServerPort);
        void loop();

    public:
        void HandleCurrentSongMessage(char *data);

    public:
        /*
        TODO: this function is not syncrhonized. can cause problem if called from core 1
        */
        int32_t GetOffsetMs(unsigned long currentEspMillis);

    private:
        TimeSync timesync;

    private:
        bool isSongPlaying = false;
        uint64_t songStartTimeEpoch; // valid only if isSongPlaying is true
};


#endif //__SONG_OFFSET_TRACKER_H__
