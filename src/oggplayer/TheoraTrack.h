#ifndef THEORATRACK_H
#define THEORATRACK_H
#include "Track.h"
class TheoraTrack : public Track {

public:
    double mFramerate;

public:
    TheoraTrack(shared_ptr<OggPlay> player, int index, double framerate) :
        Track(player, OGGZ_CONTENT_THEORA, index), mFramerate(framerate) {
    }

    virtual string toString() const {

        ostringstream str;
        str << mIndex << ": Theora " << mFramerate << " fps";
        return str.str();
    }
};

#endif
