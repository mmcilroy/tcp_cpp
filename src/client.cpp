#include "tcp/service.hpp"
#include "handler.hpp"

int main()
{
    handler hndl( "client" );
    tcp::service svc;
    tcp::session& sess = svc.connect( "0.0.0.0", 14441 );
    sess.send( "hi", 2 );
    sess.close();
}
