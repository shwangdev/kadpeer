#ifndef KATETRACK_H
#define KATETRACK_H

#include "Track.h"
class KateTrack : public Track {

public:
    std::string mLanguage;
    std::string mCategory;

public:
    KateTrack(shared_ptr<OggPlay> player, int index, const std::string &language, const std::string &category) :
        Track(player, OGGZ_CONTENT_KATE, index), mLanguage(language), mCategory(category) {
    }

    virtual string toString() const {

        ostringstream str;
        str << mIndex << ": Kate language \"" << mLanguage << "\" category \"" << mCategory << "\"";
        return str.str();
    }
};

#endif
