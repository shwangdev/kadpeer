#include "UDTClientSocket.h"
#include "SocketException.h"
#include "blah.hpp"

UDTClientSocket::UDTClientSocket ( std::string host, int port )
{
    if ( ! UDTSocket::create() )
    {
        LOG << "Could not create client UDT socket";
        throw SocketException ( "Could not create client UDT socket." );
    }

    if ( ! UDTSocket::connect ( host, port ) )
    {
        LOG << "UDT host: " << host <<" Could not connet to port: "<< port << " .";
        throw SocketException ( "UDT Could not bind to port." );
    }
}


const UDTClientSocket& UDTClientSocket::operator << ( const std::string& s ) const
{
    if ( ! UDTSocket::send ( s ) )
    {
        LOG << "Could not write to UDT socket";
        throw SocketException ( "Could not write to UDT socket." );
    }
    return *this;
}


const UDTClientSocket& UDTClientSocket::operator >> ( std::string& s ) const
{
    if ( ! UDTSocket::recv ( s ) )
    {
        LOG << "Could not read from UDT socket";
        throw SocketException ( "Could not read from UDT socket." );
    }
    return *this;
}
