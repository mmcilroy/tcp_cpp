#pragma once

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sstream>

namespace tcp {

inline int tcp_create_socket( const char* host, int port )
{
    struct addrinfo hints;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::stringstream ss;
    ss << port;
    struct addrinfo* result;
    int s = getaddrinfo( host, ss.str().c_str(), &hints, &result );

    if( s != 0 ) {
        throw std::runtime_error( gai_strerror( s ) );
    }

    struct addrinfo* rp;
    int sfd;

    for( rp = result; rp; rp = rp->ai_next )
    {
        sfd = socket( rp->ai_family, rp->ai_socktype, rp->ai_protocol );

        if( sfd == -1 ) {
            continue;
        }

        if( host )
        {
            s = ::connect( sfd, rp->ai_addr, rp->ai_addrlen );
        }
        else
        {
            int o = 1;
            ::setsockopt( sfd, SOL_SOCKET, SO_REUSEADDR, &o, sizeof( int ) );
            s = ::bind( sfd, rp->ai_addr, rp->ai_addrlen );
        }

        if( s == 0 ) {
            break;
        } else {
            close( sfd );
        }
    }

    if( !rp ) {
        throw std::runtime_error( "could not create socket" );
    }

    freeaddrinfo( result );

    return sfd;
}

inline int tcp_non_blocking( int fd )
{
    int flags = fcntl( fd, F_GETFL, 0 );

    if( flags == -1 ) {
        throw std::runtime_error( "fcntl get failed" );
    }

    if( fcntl( fd, F_SETFL, flags | O_NONBLOCK ) == -1 ) {
        throw std::runtime_error( "fcntl set failed" );
    }

    return fd;
}

inline int tcp_no_delay( int fd )
{
    int flag = 1;
    if( setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof( int ) ) < 0 ) {
        throw std::runtime_error( "setsockopt TCP_NODELAY failed" );
    }

    return fd;
}

inline int tcp_connect( const char* host, int port )
{
    int fd = tcp_create_socket( host, port );
    tcp_non_blocking( fd );
    tcp_no_delay( fd );

    return fd;
}

inline int tcp_bind( int port, int backlog )
{
    int fd = tcp_create_socket( 0, port );
    tcp_non_blocking( fd );
    tcp_no_delay( fd );

    if( ::listen( fd, backlog ) == -1 ) {
        throw std::runtime_error( "could not bind socket" );
    }

    return fd;
}

}
