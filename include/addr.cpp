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

namespace nodepp { struct nmap_addr_t {
    ptr_t<uchar> from; // ({ 192, 168, 0, 0 })
    ptr_t<uchar> to;   // ({ 192, 168, x, x })
    uint IPPROTO  = IPPROTO_TCP;
    uint maxconn  = 1000;
    uint timeout  = 100;
    uint port     = 53;
    int  state    = 0;
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace nmap { class addr_t {
protected:

    ptr_t<nmap_addr_t> obj;

private:

    bool compare( const array_t<uchar>& host ) const noexcept {
        auto  value = memcmp( obj->from.get(), obj->to.get(), obj->to.size() );
        if  ( value < 0 ){ return memcmp( obj->to  .get(), host.get(), host.size() ) == 0; }
        elif( value > 0 ){ return memcmp( obj->from.get(), host.get(), host.size() ) == 0; }
        else return true;
    }

    array_t<uchar> get_host() const noexcept {
        auto  value = memcmp( obj->from.get(), obj->to.get(), obj->to.size() );
        if  ( value<= 0 ){ return obj->from; } return obj->to;
    }

    void next_host( const array_t<uchar>& host ) const noexcept {
         for( ulong x=host.size(); x--; ){
              host[x] ++ ;
         if ( host[x] == 0 ){ continue; } break;
         }
    }

public: addr_t () noexcept : obj( new nmap_addr_t() ){}

    event_t<string_t>  onData;
    event_t<>          onDrain;
    event_t<>          onClose;
    event_t<except_t>  onError;
    event_t<string_t>  onAddress;

    addr_t ( nmap_addr_t args ) noexcept : obj( type::bind( args ) ) 
           { obj->state = 1; }

   ~addr_t () noexcept { if( obj.count()> 1 ) { return; } free(); }

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

    void pipe() const {
        ptr_t<socket_t> list( obj->maxconn );
        array_t<uchar>  host( get_host() );
        auto self = type::bind( this );

        if( obj->from.size() != obj->to.size() ){
            process::error("Invalid host size");
        }

        process::add([=](){ static ulong timeout = 0;
            if( self->is_closed() ){ self->unpipe(); return -1; }
        coStart

            for( auto &x: list ){
            if ( self->compare( host ) ){ break; }
                 x = socket_t(); 
                 x.onError([]( ... ){});
                 x.IPPROTO = self->obj->IPPROTO;
                             self->next_host( host );
                 self->onData.emit( host.join(".") );
                 x.socket( host.join("."), self->obj->port );
            }

            coNext;

            timeout = process::millis() + self->obj->timeout;
            while( process::millis() < timeout ){
                for( auto &x: list ){ int c = 0;
                if ( (c=x._connect()) < 0 ){ continue; }
                     self->onAddress.emit(x.get_peername());
                     x.free();
                }    coNext;
            }

            coNext;

            if ( self->compare( host ) )
               { self->unpipe(); coEnd; } 
            for( auto &x: list ){ x.free(); }

        coGoto(0);            
        coStop
        });

    }

};}}

/*────────────────────────────────────────────────────────────────────────────*/