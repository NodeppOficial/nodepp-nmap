/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/socket.h>
#include <nodepp/dns.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { struct nmap_port_t {
    string_t host = "localhost";
    uint IPPROTO  = IPPROTO_TCP;
    uint maxport  = 9999;
    uint minport  = 0;
    uint maxconn  = 1000;
    uint timeout  = 100;
    int  state    = 0;
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace nmap { class port_t {
protected:

    ptr_t<nmap_port_t> obj;

public: port_t () noexcept : obj( new nmap_port_t() ){}

    event_t<uint>      onData;
    event_t<uint>      onPort;
    event_t<>          onDrain;
    event_t<>          onClose;
    event_t<except_t>  onError;

    port_t ( nmap_port_t args ) noexcept : obj( type::bind( args ) ) 
           { obj->state = 1; }

   ~port_t () noexcept { if( obj.count()> 1 ) { return; } free(); }

    bool is_closed() const noexcept { return obj->state<=0; }

    void close() const noexcept { free(); }

    void unpipe() const noexcept {
         if( obj->state <= 0 ){ return; }
         onDrain.emit(); obj->state = 0;
    }

    void free() const noexcept { 
        if( obj->state ==-1 ){ return; }
        obj->state = -1; onClose.emit();
    }

    void pipe() const noexcept { 
        auto port = type::bind( obj->minport );
        ptr_t<socket_t>   list( obj->maxconn );
        auto self = type::bind( this );

        process::add([=](){ static ulong timeout = 0;
            if( self->is_closed() ){ self->unpipe(); return -1; }
        coStart

            for( auto &x: list ){ *port += 1;
            if ( *port >= self->obj->maxport ){ break; }
                 x = socket_t(); 
                 x.onError([]( ... ){});
                 self->onData.emit( *port );
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

            if ( *port >= self->obj->maxport ){ self->unpipe(); coEnd; }
            for( auto &x: list ){ x.free(); }

        coGoto(0);            
        coStop
        });

    }

};}}

/*────────────────────────────────────────────────────────────────────────────*/