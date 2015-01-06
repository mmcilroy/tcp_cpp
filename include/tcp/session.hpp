#pragma once

#include <algorithm>

namespace tcp {

class session
{
friend class service;

public:
    session( int fd, bool listening );

    ssize_t read( char*, size_t );

    ssize_t skip( size_t& );

    void send( const char*, size_t );

    void close();

    bool is_closed();

private:
    int fd_;
    bool listening_;
    bool closed_;
};

#include "tcp/session.inl"

}
