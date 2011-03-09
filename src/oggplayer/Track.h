#ifndef TRACK_H
#define TRACK_H


#include <iostream>
#include <sstream>
#include <vector>
#include <string>

extern "C" {

#include <sydney_audio.h>
}
#include <boost/date_time/posix_time/posix_time.hpp>
#include <SDL/SDL.h>
#include <oggplay/oggplay.h>
#include <oggplay/oggplay_tools.h>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
#define UNSELECTED -2
template <class T>
shared_ptr<T> msp(T* t) {

    return shared_ptr<T>(t);
}


class SDL {

public:
    SDL(unsigned long flags = 0) : init_flags(flags), initialized(false), use_sdl_yuv(false), fuzz_mode(false) {

        int r = SDL_Init(init_flags | SDL_INIT_NOPARACHUTE);
        assert(r == 0);
    }

    ~SDL() {

        if (initialized) {

            yuv_surface.reset();
            SDL_Quit();
        }
    }

    shared_ptr<SDL_Surface> setVideoMode(int width,
                                         int height,
                                         unsigned long flags) {

        int r = SDL_InitSubSystem(SDL_INIT_VIDEO);
        assert(r == 0);
        initialized = true;
        return shared_ptr<SDL_Surface>(SDL_SetVideoMode(width, height, 32, flags),
                                       SDL_FreeSurface);
    }

    bool use_sdl_yuv;
    bool fuzz_mode;
    shared_ptr<SDL_Overlay> yuv_surface;

private:
    unsigned long init_flags;
    bool initialized;
};


class Track{
public:
    shared_ptr<OggPlay> mPlayer;
    OggzStreamContent mType;
    int mIndex;

public:
    Track(shared_ptr<OggPlay> player, OggzStreamContent type, int index) :
        mPlayer(player), mType(type), mIndex(index) {
    }

    // Convert the track to a one line string summary for display
    virtual string toString() const = 0;

    // Set the track to be active
    void setActive(bool active = true) {

        int r = oggplay_set_track_active(mPlayer.get(), mIndex);
        assert(r == E_OGGPLAY_OK);
    }


};

class UnknownTrack : public Track {
  public:
    UnknownTrack(shared_ptr<OggPlay> player, OggzStreamContent type, int index) :
      Track(player, type, index) { }

    virtual string toString() const {
      ostringstream str;
      str << mIndex << ": Unknown type " << oggplay_get_track_typename(mPlayer.get(), mIndex);
      return str.str();
    }
};

#endif
