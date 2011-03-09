#include "Decoder.h"
Decoder::Decoder(shared_ptr<OggPlay> player):
    mThread(0),
    mPlayer(player),
    mCompleted(false){}

bool Decoder::start()
{
    assert(!mThread);
    setCompleted(false);
    mThread = SDL_CreateThread(decode_thread, this);
    return mThread != 0;

}

bool Decoder::stop()
{
    setCompleted(true);
    // We need to release a buffer, as oggplay_step_decode() could be blocked
    // waiting for a free buffer.
    OggPlayCallbackInfo** info = oggplay_buffer_retrieve_next(mPlayer.get());
    if (info) {

        oggplay_buffer_release(mPlayer.get(), info);
    }
    SDL_WaitThread(mThread, NULL);
    mThread = 0;

}

OggPlay* Decoder::getPlayer()
{
    return mPlayer.get();
}

bool Decoder::isCompleted()
{
    return mCompleted;
}

void Decoder::seek( long target)
{
    stop();
    oggplay_seek(mPlayer.get(), target);
    start();
    mJustSeeked = true;
}


void Decoder::setCompleted(bool c)
{
    mCompleted =c;
}


bool Decoder::justSeeked()
{
    if (mJustSeeked) {

        mJustSeeked = false;
        return true;
    }
    return false;
}

int decode_thread(void* p) {

    Decoder* d = (Decoder*)p;
    OggPlay* player = d->getPlayer();

    // E_OGGPLAY_CONTINUE       = One frame decoded and put in buffer list
    // E_OGGPLAY_USER_INTERRUPT = One frame decoded, buffer list is now full
    // E_OGGPLAY_TIMEOUT        = No frames decoded, timed out
    int r = E_OGGPLAY_TIMEOUT;
    while (!d->isCompleted() &&
           (r == E_OGGPLAY_TIMEOUT ||
                        r == E_OGGPLAY_USER_INTERRUPT ||
            r == E_OGGPLAY_CONTINUE)) {

        r = oggplay_step_decoding(player);
    }
    d->setCompleted(true);
    return 0;
}
