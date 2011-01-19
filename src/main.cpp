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

using namespace kad;


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

        ContactInfo info;
        KadID min_id(BigInt::Rossi(1));
        KadID max_id(BigInt::Rossi(10));
        KadID max1,min1;
        kad::KadID::SplitRange(min_id,max_id,& min1,&max1);
        //std::cout<<max1<<"\t"<<min1<<std::endl;
        //std::cout<<KadID::KMaxID()<<std::endl;
        //std::cout<<KadID::KMinID()<<std::endl;
        TTimer timer;
        std::cout<<static_cast<uint32_t>(timer.current())<<std::endl;
    }
    UDT::cleanup();
}
