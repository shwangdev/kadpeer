#include "oggutil.h"
#include "Track.h"
#include "TheoraTrack.h"
#include "VorbisTrack.h"
#include "KateTrack.h"
SDL gSDL;

shared_ptr<Track> handle_theora_metadata(shared_ptr<OggPlay> player,
                                         int index)
{

    int denom, num;
    int r = oggplay_get_video_fps(player.get(), index, &denom, &num);
    assert(r == E_OGGPLAY_OK);
    return msp(new TheoraTrack(player, index, static_cast<float>(num) / denom));
}

shared_ptr<Track> handle_vorbis_metadata(shared_ptr<OggPlay> player,
                                         int index)
{

    int rate, channels;
    int r = oggplay_get_audio_samplerate(player.get(), index, &rate);
    assert(r == E_OGGPLAY_OK);
    r = oggplay_get_audio_channels(player.get(), index, &channels);
    assert(r == E_OGGPLAY_OK);

    // What does this do??? Seems to be needed for audio to sync with system clock
    // and the value comes from the oggplay examples.
    oggplay_set_offset(player.get(), index, 250);

    return msp(new VorbisTrack(player,index, rate, channels));
}


shared_ptr<Track> handle_kate_metadata(shared_ptr<OggPlay> player,
                                       int index) {

    const char *language = "", *category = "";
    int r = oggplay_get_kate_language(player.get(), index, &language);
    assert(r == E_OGGPLAY_OK);
    r = oggplay_get_kate_category(player.get(), index, &category);
    assert(r == E_OGGPLAY_OK);
    return msp(new KateTrack(player, index, language, category));
}

shared_ptr<Track> handle_unknown_metadata(shared_ptr<OggPlay> player,
                                          OggzStreamContent type,
                                          int index)
{

    return msp(new UnknownTrack(player, type, index));
}


void dump_track(shared_ptr<Track> track) {

    cout << track->toString() << endl;
}


void handle_audio_data(shared_ptr<sa_stream_t> sound, OggPlayAudioData* data, int count)
{
    scoped_array<short> dest(new short[count]);
    float* source = reinterpret_cast<float*>(data);
    for (int i=0; i < count; ++i) {
        float scaled = floorf(0.5 + 32768 * source[i]);
        if (source[i] < 0.0)
            dest[i] = scaled < -32768.0 ? -32768 : static_cast<short>(scaled);
        else
            dest[i] = scaled > 32767.0 ? 32767 : static_cast<short>(scaled);
    }

    int sr = sa_stream_write(sound.get(), dest.get(), count * sizeof(short));
    assert(sr == SA_SUCCESS);
}




void handle_video_data(shared_ptr<SDL_Surface>& screen,
                       SeekBar& seekBar,
                       shared_ptr<Track> video,
                       OggPlayDataHeader* header) {
    shared_ptr<OggPlay> player(video->mPlayer);
    int y_width, y_height;
    int r = oggplay_get_video_y_size(player.get(), video->mIndex, &y_width, &y_height);
    assert(r == E_OGGPLAY_OK);

    int uv_width, uv_height;
    r = oggplay_get_video_uv_size(player.get(), video->mIndex, &uv_width, &uv_height);
    assert(r == E_OGGPLAY_OK);

    if (!screen && !gSDL.fuzz_mode) {
        screen = gSDL.setVideoMode(y_width, y_height, SDL_DOUBLEBUF);
        assert(screen);
    }

    OggPlayVideoData* data = oggplay_callback_info_get_video_data(header);

    if (gSDL.use_sdl_yuv && screen) {
        if (!gSDL.yuv_surface) {
            gSDL.yuv_surface = shared_ptr<SDL_Overlay>(
                SDL_CreateYUVOverlay(y_width,
                                     y_height,
                                     SDL_YV12_OVERLAY,
                                     screen.get()),
                SDL_FreeYUVOverlay);
            assert(gSDL.yuv_surface);
        }

        r = SDL_LockYUVOverlay(gSDL.yuv_surface.get());
        assert(r == 0);

        memcpy(gSDL.yuv_surface->pixels[0], data->y, gSDL.yuv_surface->pitches[0] * y_height);
        memcpy(gSDL.yuv_surface->pixels[2], data->u, gSDL.yuv_surface->pitches[2] * uv_height);
        memcpy(gSDL.yuv_surface->pixels[1], data->v, gSDL.yuv_surface->pitches[1] * uv_height);

        SDL_UnlockYUVOverlay(gSDL.yuv_surface.get());
        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = y_width;
        rect.h = y_height;
        SDL_DisplayYUVOverlay(gSDL.yuv_surface.get(), &rect);
    } else {
        OggPlayYUVChannels yuv;
        yuv.ptry = data->y;
        yuv.ptru = data->u;
        yuv.ptrv = data->v;
        yuv.uv_width = uv_width;
        yuv.uv_height = uv_height;
        yuv.y_width = y_width;
        yuv.y_height = y_height;

        int size = y_width * y_height * 4;
        scoped_array<unsigned char> buffer(new unsigned char[size]);
        assert(buffer);

        OggPlayRGBChannels rgb;
        rgb.ptro = buffer.get();
        rgb.rgb_width = y_width;
        rgb.rgb_height = y_height;

#if SDL_BYTE_ORDER == SDL_BIG_ENDIAN
        oggplay_yuv2argb(&yuv, &rgb);
#else
        oggplay_yuv2bgra(&yuv, &rgb);
#endif

        shared_ptr<SDL_Surface> rgb_surface(
            SDL_CreateRGBSurfaceFrom(buffer.get(),
                                     y_width,
                                     y_height,
                                     32,
                                     4 * y_width,
                                     0, 0, 0, 0),
            SDL_FreeSurface);
        assert(rgb_surface);

        if (screen) {
            r = SDL_BlitSurface(rgb_surface.get(),
                                NULL,
                                screen.get(),
                                NULL);
            assert(r == 0);
        }
    }

    if (screen) {
        seekBar.draw(screen);

        r = SDL_Flip(screen.get());
        assert(r == 0);
    }
}

void handle_overlay_data(shared_ptr<SDL_Surface>& screen,
                         SeekBar& seekBar,
                         shared_ptr<Track> video,
                         OggPlayDataHeader* header) {
    shared_ptr<OggPlay> player(video->mPlayer);
    OggPlayOverlayData* data = oggplay_callback_info_get_overlay_data(header);

    int width = data->width, height = data->height;

    if (!screen) {
        screen = gSDL.setVideoMode(width, height, SDL_DOUBLEBUF);
        assert(screen);
    }

    void *buffer = data->rgb ? data->rgb : data->rgba;
    shared_ptr<SDL_Surface> rgb_surface(
        SDL_CreateRGBSurfaceFrom(buffer,
                                 width,
                                 height,
                                 32,
                                 4 * width,
                                 0, 0, 0, 0),
        SDL_FreeSurface);
    assert(rgb_surface);

    int r = SDL_BlitSurface(rgb_surface.get(),
                            NULL,
                            screen.get(),
                            NULL);
    assert(r == 0);

    seekBar.draw(screen);

    r = SDL_Flip(screen.get());
    assert(r == 0);
}

void handle_text_data( shared_ptr<KateTrack> kate,
                       OggPlayTextData* header) {
    cout << (const char*)header << endl;
}

bool handle_sdl_event(shared_ptr<SDL_Surface> screen, SDL_Event const& event) {
    switch (event.type) {
    case SDL_KEYDOWN:
        return handle_key_press(screen, event);
    case SDL_QUIT:
        return false;
    default:
        return true;
    }
}

bool handle_key_press(shared_ptr<SDL_Surface> screen, SDL_Event const& event) {
  if (event.key.keysym.sym == SDLK_ESCAPE)
    return false;
  else if(event.key.keysym.sym == SDLK_SPACE)
    SDL_WM_ToggleFullScreen(screen.get());

  return true;
}


void play(shared_ptr<OggPlay> player, shared_ptr<VorbisTrack> audio, shared_ptr<TheoraTrack> video,
          shared_ptr<KateTrack> kate) {
    shared_ptr<SDL_Surface> screen;

    shared_ptr<sa_stream_t> sound(static_cast<sa_stream_t*>(NULL), sa_stream_destroy);

    if (audio && !gSDL.fuzz_mode) {
        sa_stream_t* s;
        int sr = sa_stream_create_pcm(&s,
                                      NULL,
                                      SA_MODE_WRONLY,
                                      SA_PCM_FORMAT_S16_NE,
                                      audio->mRate,
                                      audio->mChannels);
        assert(sr == SA_SUCCESS);
        sound.reset(s, sa_stream_destroy);

        sr = sa_stream_open(sound.get());
        if (sr != SA_SUCCESS) {
            cerr << "Failed to open sound" << endl;
        }
    }

    int r = oggplay_use_buffer(player.get(), 20);
    assert(r == E_OGGPLAY_OK);
    SDL_Event event;
    ptime start(microsec_clock::universal_time());
    Decoder decoder(player);

    SeekBar seekBar(player, decoder, seconds(5), 10, 10, 1);
    long first_frame_time = -1;

    if (!decoder.start())
        return;

    while (!decoder.isCompleted()) {
        while (SDL_PollEvent(&event) == 1) {
            if (!seekBar.handleEvent(screen, event) &&
                !handle_sdl_event(screen, event)) {
                decoder.setCompleted(true);
                break;
            }
        }
        if (decoder.isCompleted())
            break;

        OggPlayCallbackInfo** info = oggplay_buffer_retrieve_next(player.get());
        if (!info)
            continue;
        int num_tracks = oggplay_get_num_tracks(player.get());
        assert(!audio || audio && audio->mIndex < num_tracks);
        assert(!video || video && video->mIndex < num_tracks);
        assert(!kate || kate && kate->mIndex < num_tracks);

        if (audio && oggplay_callback_info_get_type(info[audio->mIndex]) == OGGPLAY_FLOATS_AUDIO) {
            OggPlayDataHeader** headers = oggplay_callback_info_get_headers(info[audio->mIndex]);
            double time = oggplay_callback_info_get_presentation_time(headers[0]) / 1000.0;
            int required = oggplay_callback_info_get_required(info[audio->mIndex]);
            for (int i=0; i<required;++i) {
                int size = oggplay_callback_info_get_record_size(headers[i]);
                OggPlayAudioData* data = oggplay_callback_info_get_audio_data(headers[i]);
                if (sound) {
                    handle_audio_data(sound, data, size * audio->mChannels);
                }
            }
        }

        if (video || kate) {
            int idx = video ? video->mIndex : kate->mIndex;
            int required = oggplay_callback_info_get_required(info[idx]);
            if (required > 0) {
                int type = oggplay_callback_info_get_type(info[idx]);
                if (type == OGGPLAY_YUV_VIDEO || type == OGGPLAY_RGBA_VIDEO) {
                    OggPlayDataHeader** headers = oggplay_callback_info_get_headers(info[idx]);
                    long video_ms = oggplay_callback_info_get_presentation_time(headers[0]);

                    if (first_frame_time == -1) {
                        first_frame_time = video_ms;
                        seekBar.setStartTime(first_frame_time);
                    }
                    seekBar.setCurrentTime(video_ms);

                    if (decoder.justSeeked()) {
                        first_frame_time = video_ms;
                        start = microsec_clock::universal_time();
                    }

                    ptime now(microsec_clock::universal_time());
                    time_duration duration(now - start);
                    long system_ms = duration.total_milliseconds();
                    long diff = video_ms - first_frame_time - system_ms;

                    if (diff > 0 && !gSDL.fuzz_mode) {
                        SDL_Delay(diff);
                    }
                    shared_ptr<Track> track = video;
                    if (!track) track = kate;
                    if (type == OGGPLAY_YUV_VIDEO) {
                        handle_video_data(screen, seekBar, track, headers[0]);
                    }
                    else if (type == OGGPLAY_RGBA_VIDEO) {
                        printf("handle_overlay_data()\n");
                        handle_overlay_data(screen, seekBar, track, headers[0]);
                    }
                }
            }
        }
        if (kate && oggplay_callback_info_get_type(info[kate->mIndex]) == OGGPLAY_KATE) {
            OggPlayDataHeader** headers = oggplay_callback_info_get_headers(info[kate->mIndex]);
            double time = oggplay_callback_info_get_presentation_time(headers[0]) / 1000.0;
            int required = oggplay_callback_info_get_required(info[kate->mIndex]);
            for (int i=0; i<required;++i) {
                OggPlayTextData* data = oggplay_callback_info_get_text_data(headers[i]);
                handle_text_data(kate, data);
            }
        }

        oggplay_buffer_release(player.get(), info);
    }

    oggplay_prepare_for_close(player.get());
    decoder.stop();
}

void usage() {
    cout << "Usage: oggplayer [options] <filename>" << endl;
    cout << "  --sdl-yuv            Use SDL's YUV conversion routines" << endl;
    cout << "  --fuzz-mode          Disable A/V sync and frame display" << endl;
    cout << "  --video-track <n>    Select which video track to use (-1 to disable)" << endl;
    cout << "  --audio-track <n>    Select which audio track to use (-1 to disable)" << endl;
    cout << "  --kate-track <n>     Select which kate track to use (-1 to disable)" << endl;
    exit(EXIT_FAILURE);
}

int parse_track_index_parameter(int argc, char *argv[], int &n, const char *name, int &idx)
{
    if (strcmp(argv[n], name) == 0) {
        if (idx == UNSELECTED) {
            char *end = NULL;
            if (n == argc-1 || (idx=strtol(argv[n+1], &end, 10), *end)) usage(); else ++n;
            if (idx < 0) idx = -1;
        }
        else {
            usage();
        }
        return 0;
    }
    return 1;
}

