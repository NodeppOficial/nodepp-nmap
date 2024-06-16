#include <nodepp/nodepp.h>
#include <nmap.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){ 

    nmap_addr_t args; 
    args.from    = { 192, 168, 1, 1   };
    args.to      = { 192, 168, 1, 255 };
    args.IPPROTO = IPPROTO_TCP;
    args.maxconn = 1000;
    args.port    = 53;

    for( auto x: nmap::addr::await( args ) )
         console::log( "->", x );

}

/*────────────────────────────────────────────────────────────────────────────*/