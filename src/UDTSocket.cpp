/**
 * @file   UDTSocket.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Mon Sep 20 23:02:33 2010
 *
 * @brief  UDT Socket file
 *
 *
 */

#include "UDTSocket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include "blah.hpp"

UDTSocket::UDTSocket()
{

    memset ( &m_addr,
             0,
             sizeof ( m_addr ) );
}


UDTSocket::~UDTSocket()
{
    if ( is_valid() )
        UDT::close ( m_usock );
}

bool UDTSocket::create()
{
    if ( ! is_valid() )
    {
        LOG << "UDT socket create error";
        return false;
    }

    m_usock = UDT::socket(AF_INET, SOCK_STREAM, 0);

    if ( m_usock == UDT::INVALID_SOCK)
    {
        LOG << "INVALID_SOCK";
        return false;
    }

    int on = 1;
    if ( UDT::setsockopt ( m_usock, SOL_SOCKET, UDT_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
        return false;

    // bool rendezvous;
    // if ( -1 == UDT::setsockopt( m_usock, 0, UDT_RENDEZVOUS, &rendezvous, sizeof(bool)) )
    // {
    //     LOG << "UDT Set RendezVous error";
    //     return false;
    // }

    return true;
}



bool UDTSocket::bind ( const int port )
{

    if ( ! is_valid() )
    {
        return false;
    }

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_addr.sin_port = htons ( port );

    if (UDT::ERROR ==  UDT::bind ( m_usock,( struct sockaddr * ) &m_addr,sizeof ( m_addr ) ))
    {
        LOG << "UDT Bind Error";
        return false;
    }
    LOG << "Bind port "<< port ;
    return true;
}


bool UDTSocket::listen() const
{
    if ( ! is_valid() )
    {
        return false;
    }

    if ( UDT::ERROR  == UDT::listen ( m_usock, MAXCONNECTIONS ) )
    {
        LOG << "UDT Listen Error";
        return false;
    }
    return true;
}


bool UDTSocket::accept ( UDTSocket& new_socket ) const
{
    int  addr_length = sizeof ( m_addr );
    new_socket.m_usock = UDT::accept ( m_usock, ( sockaddr * ) &m_addr, &addr_length );

    if ( new_socket.m_usock == UDT::ERROR )
        return false;
    else
        return true;
}


bool UDTSocket::send ( const std::string s ) const
{
    if ( UDT::ERROR == UDT::send ( m_usock, s.c_str(), s.size(), MSG_NOSIGNAL ) )
    {
        LOG << "UDT Send data fail";
        return false;
    }
    else
    {
        return true;
    }
}


int UDTSocket::recv ( std::string& s ) const
{
    char buf [ MAXRECV + 1 ];

    s = "";
    memset ( buf, 0, MAXRECV + 1 );

    if ( UDT::ERROR == UDT::recv ( m_usock, buf, MAXRECV, 0 ) )
    {
        LOG << "UDT RECV Data errno == " << errno << "  in Socket::recv\n";
        return 0;
    }
    else
    {
        s = buf;
        return 1;
    }
}


bool UDTSocket::connect ( const std::string host, const int port )
{
    if ( ! is_valid() ) return false;

    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( port );
    int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

    if ( errno == EAFNOSUPPORT )
    {
        LOG<< "inet_pton error";
        return false;
    }


    if ( UDT::ERROR == UDT::connect ( m_usock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) ))
    {
        LOG <<"recv: " << UDT::getlasterror().getErrorMessage() ;
        return false;
    }
    else{
        return true;
    }
}
