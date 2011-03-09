#ifndef OGG_UTIL_H
#define OGG_UTIL_H

#include "Track.h"
#include "TheoraTrack.h"
#include "VorbisTrack.h"
#include "KateTrack.h"
#include "SeekBar.h"

extern SDL gSDL;
shared_ptr<Track> handle_theora_metadata(shared_ptr<OggPlay> player,
                                         int index);

shared_ptr<Track> handle_vorbis_metadata(shared_ptr<OggPlay> player,
                                         int index);


shared_ptr<Track> handle_kate_metadata(shared_ptr<OggPlay> player,
                                       int index);

shared_ptr<Track> handle_unknown_metadata(shared_ptr<OggPlay> player,
                                          OggzStreamContent type,
                                          int index);


void dump_track(shared_ptr<Track> track);



template <class OutputIterator>
void load_metadata(shared_ptr<OggPlay> player, OutputIterator out) {

    int num_tracks = oggplay_get_num_tracks(player.get());

    for (int i=0; i < num_tracks; ++i) {

        OggzStreamContent type = oggplay_get_track_type(player.get(), i);
        switch (type) {

        case OGGZ_CONTENT_THEORA:
            *out++ = handle_theora_metadata(player, i);
            break;

        case OGGZ_CONTENT_VORBIS:
            *out++ = handle_vorbis_metadata(player, i);
            break;

        case OGGZ_CONTENT_KATE:
            *out++ = handle_kate_metadata(player, i);
            break;

        default:
            *out++ = handle_unknown_metadata(player, type, i);
            break;
        }
    }
}

template <class TrackType, class InputIterator>
shared_ptr<TrackType> get_track(int trackidx,
                                InputIterator first,
                                InputIterator last)
{

    while (first != last) {

        shared_ptr<TrackType> track(dynamic_pointer_cast<TrackType>(*first));
        if (trackidx == UNSELECTED) {

            if (track != 0) return track; /* unselected ? use the first one we find */
        }
        else if (trackidx >= 0) {

            if (!trackidx--) return track; /* use a track by index - returns NULL if not the correct type */
        }
        ++first;
    }
    return shared_ptr<TrackType>();
}
void handle_audio_data(shared_ptr<sa_stream_t> sound, OggPlayAudioData* data, int count);
void handle_video_data(shared_ptr<SDL_Surface>& screen,
                       SeekBar& seekBar,
                       shared_ptr<Track> video,
                       OggPlayDataHeader* header) ;

void handle_overlay_data(shared_ptr<SDL_Surface>& screen,
                         SeekBar& seekBar,
                         shared_ptr<Track> video,
                         OggPlayDataHeader* header) ;


void handle_text_data( shared_ptr<KateTrack> kate,
                       OggPlayTextData* header);

bool handle_key_press(shared_ptr<SDL_Surface> screen, SDL_Event const& event);
bool handle_sdl_event(shared_ptr<SDL_Surface> screen, SDL_Event const& event);
void usage();

void play(shared_ptr<OggPlay> player, shared_ptr<VorbisTrack> audio, shared_ptr<TheoraTrack> video,
          shared_ptr<KateTrack> kate) ;

int parse_track_index_parameter(int argc, char *argv[], int &n, const char *name, int &idx);

#endif
