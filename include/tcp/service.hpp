#pragma once

#include "tcp/tcp.hpp"
#include "tcp/session.hpp"

namespace tcp {

class service
{
public:
    service();

    session& connect( const char* host, int port );

    void accept( int port );

    template< class T >
    void start( T* handler );

private:
    template< class T >
    void accept( session&, T& handler );

    session* add_session( int fd, bool listening );

    void del_session( session& sess );

    struct epoll_event* events_;
    int efd_;
    int max_events_;
};

#include "tcp/service.inl"

}
