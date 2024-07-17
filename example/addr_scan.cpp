#include <nodepp/nodepp.h>
#include <nmap/nmap.h>

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

    auto scanner = nmap::addr::scan( args );

    scanner.onClose([=](){
        console::log( "scanner closed" );
    });

    scanner.onAddress([=]( string_t addr ){
        console::log( "->", addr );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/