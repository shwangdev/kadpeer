#ifndef DECODER_H
#define DECODER_H

#include "TheoraTrack.h"
#include "VorbisTrack.h"
#include "KateTrack.h"

int decode_thread(void* p);

class Decoder
{
public:
    Decoder ( shared_ptr<OggPlay> player);
    ~Decoder(){
    }

    bool start();
    bool stop();
    OggPlay * getPlayer();
    bool isCompleted();
    void seek( long target);
    void setCompleted(bool c);

    bool justSeeked();
private:
    SDL_Thread* mThread;
    shared_ptr<OggPlay> mPlayer;
    bool mCompleted;
    bool mJustSeeked;

};

#endif
