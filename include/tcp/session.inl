inline session::session( int fd, bool listening ) :
    fd_( fd ),
    listening_( listening ),
    closed_( false )
{
}

inline ssize_t session::read( char* buf, size_t count )
{
    ssize_t bytes_read = ::read( fd_, buf, count );
    if( bytes_read == 0 || ( bytes_read == -1 && errno != EINTR && errno != EAGAIN ) ) {
        close();
    }

    return bytes_read;
}

inline ssize_t session::skip( size_t& count )
{
    static char buf[256];

    while( count > 0 )
    {
        ssize_t n = ::read( fd_, buf, std::min( (size_t)256, count ) );
        if( n <= 0 ) {
            return n;
        } else {
            count -= n;
        }
    }

    return 1;
}

inline void session::send( const char* buf, size_t count )
{
    ::send( fd_, buf, count, 0 );
}

inline void session::close()
{
    ::close( fd_ );
    closed_ = true;
}

inline bool session::is_closed()
{
    return closed_;
}
