#include "tcp/service.hpp"
#include "handler.hpp"

int main()
{
    handler hndl( "server" );
    tcp::service svc;
    svc.accept( 14441 );
    svc.start( &hndl );
}
