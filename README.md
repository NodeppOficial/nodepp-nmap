# NODEPP-NMAP

A Nmap implementation using Nodepp.

# Usage

```cpp
#include <nodepp/nodepp.h>
#include <nmap.h>

using namespace nodepp;

void onMain(){ 

    nmap_port_t args; 
    args.host    = "google.com";
    args.IPPROTO = IPPROTO_TCP;
    args.maxport = 10000;
    args.minport = 0;
    args.maxconn = 1000;

    console::log( "scanning", args.host );
    auto scanner = nmap::port::scan( args );

    scanner.onClose([=](){
        console::log( "scanner closed" );
    });

    scanner.onPort([=]( uint port ){
        console::log( "->", port );
    });

}
```

# Build & Run

- 🪟: `g++ -o main main.cpp -I ./include -lws2_32 ; ./main`
- 🐧: `g++ -o main main.cpp -I ./include ; ./main`
