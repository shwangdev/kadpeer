/**
 * @file   ServerSocket.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Wed Aug 18 16:54:20 2010
 *
 * @brief  Server socket wapper
 *
 *
 */

#include "ServerSocket.h"
#include "SocketException.h"
#include "blah.hpp"
/**
 * Constructor
 *
 * @param port Port numer in listing in Server
 */
ServerSocket::ServerSocket ( int port )
{

    if ( ! Socket::create() )
    {
        LOG << "Could not create server socket";
        throw SocketException ( "Could not create server socket." );
    }

    if ( ! Socket::bind ( port ) )
    {
        LOG << "Could not bind to port";
        throw SocketException ( "Could not bind to port." );
    }

    if ( ! Socket::listen() )
    {
        LOG << "Could not listen to socket";
        throw SocketException ( "Could not listen to socket." );
    }

}

ServerSocket::~ServerSocket()
{
}

/**
 * Sender
 *
 * @param s string that needs to be transformed
 *
 * @return
 */
const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
    if ( ! Socket::send ( s ) )
    {
        LOG << "Could not write to socket";
        throw SocketException ( "Could not write to socket." );
    }

    return *this;
}

/**
 * Receiver
 *
 * @param s string that received from remote client
 *
 * @return this
 */
const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
    if ( ! Socket::recv ( s ) )
    {
        LOG << "Could not read from socket";
        throw SocketException ( "Could not read from socket." );
    }
    return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
    if ( ! Socket::accept ( sock ) )
    {
        LOG << "Could not accept socket";
        throw SocketException ( "Could not accept socket." );
    }
}
