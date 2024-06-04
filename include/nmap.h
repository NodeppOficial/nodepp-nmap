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

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/socket.h>
#include <nodepp/dns.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { struct nmap_config_t {
    string_t host = "localhost";
    uint IPPROTO  = IPPROTO_TCP;
    uint maxport  = 9999;
    uint minport  = 0;
    uint maxconn  = 1000;
    uint timeout  = 3000;
    int  state    = 0;
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class nmap_t {
protected:

    ptr_t<nmap_config_t> obj;

public: nmap_t () noexcept : obj( new nmap_config_t() ){}

    event_t<uint>      onPort;
    event_t<>          onDrain;
    event_t<>          onClose;
    event_t<except_t>  onError;
    event_t<uint,uint> onProgress;

   ~nmap_t () noexcept {
        if( obj.count()> 1 )
          { return; } free();
    }

    nmap_t ( nmap_config_t args ) noexcept : obj( type::bind( args ) ) 
           { obj->state = 1; }
           
    bool is_closed() const noexcept { 
         return obj->state==0; 
    }

    void free() const noexcept { 
        if( obj->state == 0 ){ return; }
            obj->state =  0; 
            onClose.emit();
    }

    void unpipe() const noexcept {
         obj->state = 0;
    }

    void pipe() const noexcept { 
        auto port = type::bind( obj->minport );
        ptr_t<socket_t> list ( obj->maxconn );
        auto self = type::bind( this );

        process::add([=](){ static ulong timeout = 0;
            if( self->is_closed() ){ self->onDrain.emit(); return -1; }
        coStart

            for( auto &x: list ){ *port += 1;
            if ( *port >= self->obj->maxport ){ break; }
                 x = socket_t(); 
                 x.onError([]( ... ){});
                 x.IPPROTO = self->obj->IPPROTO;
                 x.socket( dns::lookup(self->obj->host), *port );
            }

            coNext;

            timeout = process::millis() + self->obj->timeout;
            while( process::millis() < timeout ){
                for( auto &x: list ){ int c = 0;
                if ( (c=x._connect()) < 0 ){ continue; }
                     self->onPort.emit(x.get_sockport());
                     x.free();
                }    coNext;
            }

            coNext;

            self->onProgress.emit( (uint)(*port*100/self->obj->maxport), *port );
            if ( *port >= self->obj->maxport ){ self->onDrain.emit(); coEnd; }
            for( auto &x: list ){ x.free(); }

        coGoto(0);            
        coStop
        });

    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace nmap {

    nmap_t async( const nmap_config_t args ){ 
    nmap_t pid  ( args ); pid.pipe(); return pid;
    }
    
    /*─······································································─*/
    
    array_t<uint> await( const nmap_config_t args ){
        array_t<uint> result; nmap_t pid ( args ); int done = 0;
        
        pid.pipe(); pid.onDrain([&](){ done = 1; });
        pid.onPort([&]( uint port ){
            result.push( port );
        });

        while( done != 1 ){ process::next(); } 
        return result;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/