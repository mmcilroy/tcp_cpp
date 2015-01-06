#pragma once

#include <iostream>

struct handler
{
    std::string name_;
    char buf_[256];

    handler( const std::string name ) :
        name_( name )
    {
    }

    void on_accept( tcp::session& )
    {
        std::cout << this << " " << name_ << ": on_accept" << std::endl;
    }

    void on_close( tcp::session& )
    {
        std::cout << this << " " << name_ << ": on_close" << std::endl;
    }

    void on_data( tcp::session& sess )
    {
        ssize_t n = sess.read( buf_, 256 );
        std::cout << this << " " << name_ << ": on_data " << n << std::endl;
    }
};
