#include "SeekBar.h"
void SeekBar::setCurrentTime(int64_t timeMs) {
    mCurrentTimeMs = timeMs;
}

void SeekBar::setStartTime(int64_t timeMs) {
    mStartTimeMs = timeMs;
}

void SeekBar::draw(shared_ptr<SDL_Surface>& screen) {
    if (!isVisible(screen) || !screen) {
        return;
    }

    SDL_Rect border = getBorderRect(screen);
    unsigned white = SDL_MapRGB(screen->format, 255, 255, 255);
    int err = SDL_FillRect(screen.get(), &border, white);
    assert(err == 0);

    SDL_Rect background = getBackgroundRect(screen);
    unsigned black = SDL_MapRGB(screen->format, 0, 0, 0);
    err = SDL_FillRect(screen.get(), &background, black);
    assert(err == 0);

    SDL_Rect progress = getProgressRect(screen);
    unsigned gray = SDL_MapRGB(screen->format, 0xd6, 0xd6, 0xd6);
    err = SDL_FillRect(screen.get(), &progress, gray);

    assert(err == 0);
}

SDL_Rect SeekBar::getBorderRect(shared_ptr<SDL_Surface> screen) {
    SDL_Rect border;
    border.x = mPadding;
    border.y = screen->h - mPadding - mHeight;
    border.w = screen->w - mPadding * 2;
    border.h = mHeight;
    return border;
}

SDL_Rect SeekBar::getBackgroundRect(shared_ptr<SDL_Surface> screen) {
    SDL_Rect background;
    background.x = mPadding + mBorder;
    background.y = screen->h - mPadding - mHeight + mBorder;
    background.w = screen->w - 2 * mPadding - 2 * mBorder;
    background.h = mHeight - 2 * mBorder;
    return background;
}

SDL_Rect SeekBar::getProgressRect(shared_ptr<SDL_Surface> screen) {
    if (mEndTimeMs == -1) {
        mEndTimeMs = oggplay_get_duration(mPlayer.get());
    }
    double duration = mEndTimeMs - mStartTimeMs;
    double position = mCurrentTimeMs - mStartTimeMs;
    SDL_Rect background = getBackgroundRect(screen);
    double maxWidth = background.w - 2 * mBorder;
    SDL_Rect progress;
    progress.x = background.x + mBorder;
    progress.y = background.y + mBorder;
    progress.h = background.h - 2 * mBorder;
    progress.w = max(1, (int)(maxWidth * position / duration));
    return progress;
}


bool SeekBar::isInside(int x, int y, const SDL_Rect& rect) {
    return x > rect.x &&
        x < rect.x + rect.w &&
        y > rect.y &&
        y < rect.y + rect.h;
}

bool SeekBar::isVisible(shared_ptr<SDL_Surface> screen) {
    int x=0, y=0;
    SDL_GetMouseState(&x, &y);
    SDL_Rect background = getBackgroundRect(screen);
    return second_clock::local_time() < mHideTime ||
        isInside(x, y, background);
}

void SeekBar::updateHideTime() {
    mHideTime = second_clock::local_time() + mVisibleDuration;
}

bool SeekBar::handleEvent(shared_ptr<SDL_Surface> screen, SDL_Event const& event) {
    if (event.type == SDL_MOUSEMOTION) {
        updateHideTime();
        return true;
    } else if (event.type == SDL_MOUSEBUTTONDOWN &&
               event.button.button == SDL_BUTTON_LEFT) {
        int x = event.button.x;
        int y = event.button.y;
        SDL_Rect background = getBackgroundRect(screen);
        SDL_Rect progress = getProgressRect(screen);
        if (isInside(x, y, background)) {
            double progressWidth = background.w - 2 * mBorder;
            double proportion = (x - progress.x) / progressWidth;
            double duration = mEndTimeMs - mStartTimeMs;
            double seekTime = duration * proportion;
            int64_t seekTimeMs = mStartTimeMs + (int64_t)seekTime;
            mDecoder.seek(seekTimeMs);
            return true;
        }
        return false;
    }
    return false;
}

