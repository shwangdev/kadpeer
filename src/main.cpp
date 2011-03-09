/**
 * @file   main.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Tue Aug 17 17:57:07 2010
 *
 * @brief  Currently, this file was just for testing daemon or some basic function working status
 *
 *
 */

#include "UDTServerSocket.h"
#include "SocketException.h"
#include <string>
#include "UDTClientSocket.h"
#include "SocketException.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include "TThreadPool.hh"
#include <sstream>
#include <unistd.h>
#include "tinyxml.h"
#include "blah.hpp"
#include "kadid.h"
#include "contact.h"
#include "TTimer.hh"
#include "Decoder.h"
#include "KateTrack.h"
#include "oggutil.h"
#include "SeekBar.h"
using namespace kad;
extern SDL gSDL;

class Server:public TThreadPool::TJob
{

    UDTServerSocket *server;

public:
    Server(UDTServerSocket *ser)
    {
        server=ser;
    }
    ~Server()
    {
        delete server;
    }

    virtual void run(void *)
    {
        UDTServerSocket new_sock;
        server->accept ( new_sock );
        try
        {
            std::string data;
            new_sock >> data;
            LOG <<"Receive Data: "<<data<<std::endl;
            TiXmlDocument doc;
            doc.Parse(data.c_str());
            if ( doc.Error() )
			{
				printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
				exit( 1 );
			}

            TiXmlNode* node = 0;
            TiXmlElement* todoElement = 0;
            TiXmlElement* itemElement = 0;
            //node = doc.FirstChild( "Command" );
            node = doc.RootElement();
            //std::cout<<node->Value()<<std::endl;
            assert( node );
            if ( 0 != strcmp("Command",node->Value()))
            {
                std::cerr<<" Parsing Error"<<std::endl;
                std::cout<<node->Value()<<std::endl;
            }
            todoElement = node->ToElement();
            assert( todoElement  );

            node = todoElement->FirstChildElement();
            assert( node );
            itemElement = node->ToElement();
            assert( itemElement  );
            //std::cout<<itemElement->Value()<<std::endl;
            itemElement->SetAttribute( "ID", "0x002");
            std::ostringstream response;
            response << doc;
            new_sock << response.str();
        }
        catch ( SocketException &  ) {}
    }
};

class Client:public TThreadPool::TJob
{
    UDTClientSocket *client_socket;
public:
    Client()
    {
        client_socket= new UDTClientSocket("127.0.0.1",30000);
    }
    ~Client()
    {
        delete client_socket;
    }
    /**
     *
     *
     * @param args
     */
    virtual void  run(void * args){
        try
        {
            std::string reply;
            const char* demoStart =
                "<?xml version=\"1.0\"  standalone='no' >\n";
            TiXmlDocument doc( "demotest.xml" );
            doc.Parse( demoStart );
            TiXmlElement *root = new TiXmlElement("Command");
            doc.LinkEndChild(root);
            TiXmlElement *cmd = new TiXmlElement("Find");
            cmd->SetAttribute("IP","127.0.0.1");
            cmd->SetAttribute("ID","0x001");
            root->LinkEndChild(cmd);
            if ( doc.Error() )
            {
                printf( "Error in %s: %s\n", doc.Value(), doc.ErrorDesc() );
                pthread_exit(NULL);
            }
            int num=*(int *)args;
            try
            {
                std::ostringstream request;
                request<<doc;
                (*client_socket) << request.str();
                usleep(100);
                (*client_socket) >> reply;
                LOG << "We received this response from the server:\n\"" << reply <<"\"\n";
            }
            catch ( SocketException& ) {}
        }
        catch ( SocketException& e )
        {
            LOG << "Exception was caught:" << e.description();
        }
    }
};

int main(int argc, char * argv[])
{

    UDT::startup();
    assert( argc > 1 );
    LOG <<"running.....";
    if ( strcmp( argv[1] ,"Server") == 0 )
    {
        tp_init(25);
        UDTServerSocket server(30000);
        while(1)
        {
            Server *ser = new Server(&server);
            tp_run(ser, NULL, false);
        }
        tp_sync_all();
        tp_done();
    }
    else if ( strcmp(argv[1] ,"Client") == 0 )
    {
        tp_init(25);

        for( int i = 0 ; i < 10 ; i++)
        {
            Client client;
            tp_run(&client, &i, false);
            tp_sync(&client);
            sleep(1);
        }
        tp_done();
    }
    else
    {

        int video_track = UNSELECTED, audio_track = UNSELECTED, kate_track = UNSELECTED;

        char* path = NULL;
        for (int n=1; n<argc; ++n) {
            if (argv[n][0] == '-') {
                if (strcmp(argv[n], "--sdl-yuv") == 0) {
                    gSDL.use_sdl_yuv = true;
                }
                else if (strcmp(argv[n], "--fuzz-mode") == 0) {
                    gSDL.fuzz_mode = true;
                }
                else if (!parse_track_index_parameter(argc, argv, n, "--video-track", video_track)) {
                }
                else if (!parse_track_index_parameter(argc, argv, n, "--audio-track", audio_track)) {
                }
                else if (!parse_track_index_parameter(argc, argv, n, "--kate-track", kate_track)) {
                }
                else {
                    usage();
                }
            }
            else {
                if (path) {
                    cerr << "Only one stream may be specified" << endl;
                }
                else {
                    path = (char*)argv[n];
                }
            }
        }

        if (!path) {
            usage();
        }

        OggPlayReader* reader = 0;
        if (strncmp(path, "http://", 7) == 0)
            reader = oggplay_tcp_reader_new(path, NULL, 0);
        else
            reader = oggplay_file_reader_new(path);

        assert(reader);

        shared_ptr<OggPlay> player(oggplay_open_with_reader(reader), oggplay_close);
        assert(player);

        vector<shared_ptr<Track> > tracks;
        load_metadata(player, back_inserter(tracks));
        for_each(tracks.begin(), tracks.end(), dump_track);

        shared_ptr<TheoraTrack> video(get_track<TheoraTrack>(video_track, tracks.begin(), tracks.end()));
        shared_ptr<VorbisTrack> audio(get_track<VorbisTrack>(audio_track, tracks.begin(), tracks.end()));
        shared_ptr<KateTrack> kate(get_track<KateTrack>(kate_track, tracks.begin(), tracks.end()));

        cout << "Using the following tracks: " << endl;
        if (video) {
            video->setActive();
            oggplay_set_callback_num_frames(player.get(), video->mIndex, 1);
            cout << "  " << video->toString() << endl;

        }
        if (audio) {
            audio->setActive();
            if (!video)
                oggplay_set_callback_num_frames(player.get(), audio->mIndex, 2048);

            cout << "  " << audio->toString() << endl;
        }

        if (kate) {
            kate->setActive();
            if (video) {
                oggplay_convert_video_to_rgb(player.get(), video->mIndex, 1, 0);
                oggplay_overlay_kate_track_on_video(player.get(), kate->mIndex, video->mIndex);
            }
            else {
                oggplay_set_kate_tiger_rendering(player.get(), kate->mIndex, 1, 0, 640, 480);
            }
            if (!audio && !video)
                oggplay_set_callback_period(player.get(), kate->mIndex, 40);

            cout << "  " << kate->toString() << endl;
        }

        play(player, audio, video, kate);

        return 0;


    }
    UDT::cleanup();
}
