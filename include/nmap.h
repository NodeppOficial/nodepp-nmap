/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_NMAP
#define NODEPP_NMAP

#include "port.cpp"
#include "addr.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace nmap { namespace port {

    port_t scan( const nmap_port_t args ){ 
    port_t pid  ( args ); pid.pipe(); return pid;
    }
    
    array_t<uint> await( const nmap_port_t args ){
        array_t<uint> result; port_t pid ( args ); int done = 0;
        
        pid.onPort([&]( uint port ){ result.push( port ); });
        pid.pipe(); pid.onDrain([&](){ done = 1; });

        while( done != 1 ){ process::next(); } 
        return result;
    }

}}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace nmap { namespace addr {

    addr_t scan( const nmap_addr_t args ){ 
    addr_t pid  ( args ); pid.pipe(); return pid;
    }
    
    array_t<string_t> await( const nmap_addr_t args ){
        array_t<string_t> result; addr_t pid ( args ); int done = 0;
        
        pid.onAddress([&]( string_t address ){ result.push( address ); });
        pid.pipe(); pid.onDrain([&](){ done = 1; });

        while( done != 1 ){ process::next(); } 
        return result;
    }

}}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif