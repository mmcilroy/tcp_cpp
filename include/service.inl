inline service::service() :
    max_events_( 1024 )
{
    efd_ = epoll_create1( 0 );
    if( efd_ == -1 ) {
        throw std::runtime_error( "could not create epoll fd" );
    }

    events_ = ( struct epoll_event* )calloc( max_events_, sizeof( struct epoll_event ) );
    if( events_ == 0 ) {
        throw std::runtime_error( "could not create epoll events" );
    }
}

inline session& service::connect( const char* host, int port )
{
    return *add_session( tcp_connect( host, port ), false );
}

inline void service::accept( int port )
{
    add_session( tcp_bind( port, 512 ), true );
}

inline session* service::add_session( int fd, bool listening )
{
    session* sess = new session( fd, listening );

    struct epoll_event event;
    event.data.ptr = sess;
    event.events = EPOLLIN | EPOLLET;

    if( epoll_ctl( efd_, EPOLL_CTL_ADD, fd, &event ) == -1 ) {
        throw std::runtime_error( "could not watch fd" );
    }

    return sess;
}

inline void service::del_session( session& sess )
{
    epoll_ctl( efd_, EPOLL_CTL_DEL, sess.fd_, 0 );
    delete &sess;
}

template< class T >
inline void service::start( T* handler )
{
    while( 1 )
    {
        for( int i=0; i<epoll_wait( efd_, events_, max_events_, 250 ); i++ )
        {
            session* sess = (session*)events_[i].data.ptr;
            if( sess )
            {
                uint32_t events = events_[i].events;

                if( events & EPOLLERR || events & EPOLLHUP || !( events & EPOLLIN ) ) {
                    handler->on_close( *sess );
                } else if( !sess->listening_ ) {
                    handler->on_data( *sess );
                } else {
                    accept( *sess, *handler );
                }

                if( sess->is_closed() )
                {
                    handler->on_close( *sess );
                    del_session( *sess );
                }
            }
        }
    }
}

template< class T >
inline void service::accept( session& sess, T& handler )
{
    struct sockaddr in_addr;
    socklen_t len = sizeof( in_addr );

    int afd;
    while( ( afd = ::accept( sess.fd_, &in_addr, &len ) ) != -1 )
    {
        tcp_non_blocking( afd );
        session* new_sess = add_session( afd, false );
        handler.on_accept( *new_sess );
    }
}
