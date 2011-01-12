#include "ClientSocket.h"
#include "SocketException.h"
#include "blah.hpp"

ClientSocket::ClientSocket ( std::string host, int port )
{
    if ( ! Socket::create() )
    {
        LOG << "Could not create client socket";
        throw SocketException ( "Could not create client socket." );
    }

    if ( ! Socket::connect ( host, port ) )
    {
        LOG << "Could not bind to port.";
        throw SocketException ( "Could not bind to port." );
    }
}


const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
{
    if ( ! Socket::send ( s ) )
    {
        LOG << "Could not write to socket";
        throw SocketException ( "Could not write to socket." );
    }

    return *this;
}


const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
{
    if ( ! Socket::recv ( s ) )
    {
        LOG << "Could not read from socket";
        throw SocketException ( "Could not read from socket." );
    }
    return *this;
}
