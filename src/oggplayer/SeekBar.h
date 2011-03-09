#ifndef __SEEK_BAR_H_
#define __SEEK_BAR_H_
#include "TheoraTrack.h"
#include "VorbisTrack.h"
#include "KateTrack.h"
#include "Decoder.h"
class SeekBar
{
public:
    SeekBar(shared_ptr<OggPlay> player,
            Decoder& decoder,
            time_duration visibleDuration,
            int height,
            int padding,
            int border)
        : mPlayer(player),
          mDecoder(decoder),
          mStartTimeMs(0),
          mEndTimeMs(-1),
          mCurrentTimeMs(0),
          mVisibleDuration(visibleDuration),
          mHeight(height),
          mPadding(padding),
          mBorder(border)
    {
        updateHideTime();
    }

    void setCurrentTime(int64_t timeMs);
    void setStartTime(int64_t timeMs);
    void draw(shared_ptr<SDL_Surface>& screen);
    bool handleEvent(shared_ptr<SDL_Surface> screen,
                     SDL_Event const& event);
private:

    SDL_Rect getBorderRect(shared_ptr<SDL_Surface> screen);
    SDL_Rect getBackgroundRect(shared_ptr<SDL_Surface> screen);
    SDL_Rect getProgressRect(shared_ptr<SDL_Surface> screen);
    bool isInside(int x, int y, const SDL_Rect& rect) ;
    bool isVisible(shared_ptr<SDL_Surface> screen) ;
    void updateHideTime() ;
    shared_ptr<OggPlay> mPlayer;
    Decoder& mDecoder;

    int64_t mStartTimeMs;
    int64_t mEndTimeMs;
    int64_t mCurrentTimeMs;

    // Height of the seek bar, in pixels, including borders,
    // and progress bar.
    int mHeight;
    // Number of pixels between bottom and sides of screen and the seek bar.
    int mPadding;
    // Thickness of borders in pixels.
    int mBorder;
    //
    ptime mHideTime;
    time_duration mVisibleDuration;

};
#endif
