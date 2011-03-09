#ifndef VORBISTRACK_H
#define VORBISTRACK_H

#include "Track.h"

class VorbisTrack : public Track {

public:
    int mRate;
    int mChannels;

public:
    VorbisTrack(shared_ptr<OggPlay> player, int index, int rate, int channels) :
        Track(player, OGGZ_CONTENT_VORBIS, index), mRate(rate), mChannels(channels) {
    }

    virtual string toString() const {

        ostringstream str;
        str << mIndex << ": Vorbis " << mRate << " KHz " << mChannels << " channels";
        return str.str();
    }
};

#endif
