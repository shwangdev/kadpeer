#ifndef UDTSOCKET_H
#define UDTSOCKET_H

#include "blah.hpp"
#include "udt.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class UDTSocket{
public:

    UDTSocket();

    virtual ~UDTSocket();

    bool create();
    bool bind ( const int port );
    bool listen() const;
    bool accept ( UDTSocket& ) const;

    // Client initialization
    bool connect ( const std::string host, const int port );

    // Data Transimission
    bool send ( const std::string ) const;
    int recv ( std::string& ) const;

//    void set_non_blocking ( const bool );

    bool is_valid() const { return m_usock != UDT::INVALID_SOCK; }

private:
    UDTSOCKET  m_usock;
    sockaddr_in m_addr;
};
#endif
