#include <nodepp/nodepp.h>
#include <nmap.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain(){ 

    nmap_config_t args; 
    args.host    = "google.com";
    args.IPPROTO = IPPROTO_TCP;
    args.maxport = 9999;
    args.minport = 0;
    args.maxconn = 1000;
    args.timeout = 3000;

    for( auto x: nmap::await( args ) )
         console::log( "->", x );

}

/*────────────────────────────────────────────────────────────────────────────*/