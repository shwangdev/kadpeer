/**
 * @file   UDTServerSocket.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Sep 20 23:41:41 2010
 *
 * @brief
 *
 *
 */

#include "UDTServerSocket.h"
#include "SocketException.h"
#include "blah.hpp"
/**
 * Constructor
 *
 * @param port Port numer in listing in Server
 */
UDTServerSocket::UDTServerSocket ( int port )
{

    if ( ! UDTSocket::create() )
    {
        LOG << "Could not create server UDT socket";
        throw SocketException ( "Could not create server UDT socket." );
    }

    if ( ! UDTSocket::bind ( port ) )
    {
        LOG << "Could not bind to port";
        throw SocketException ( "Could not bind to port." );
    }

    if ( ! UDTSocket::listen() )
    {
        LOG << "Could not listen to UDT socket";
        throw SocketException ( "Could not listen to UDT socket." );
    }

}

UDTServerSocket::~UDTServerSocket()
{
}

/**
 * Sender
 *
 * @param s string that needs to be transformed
 *
 * @return
 */
const UDTServerSocket& UDTServerSocket::operator << ( const std::string& s ) const
{
    if ( ! UDTSocket::send ( s ) )
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
const UDTServerSocket& UDTServerSocket::operator >> ( std::string& s ) const
{
    if ( ! UDTSocket::recv ( s ) )
    {
        LOG << "Could not read from socket";
        throw SocketException ( "Could not read from socket." );
    }
    return *this;
}

void UDTServerSocket::accept ( UDTServerSocket& sock )
{
    if ( ! UDTSocket::accept ( sock ) )
    {
        LOG << "Could not accept UDT socket";
        throw SocketException ( "Could not accept UDT socket." );
    }
}
