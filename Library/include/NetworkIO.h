/*!
 @header              NetworkIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains types, functions, and tables for Asynchronous Networking.
 */

#include "PlatformIO.h"         /* Included for Platform Independence macros */
#include "TextIO/TextIOTypes.h" /* Included for Text types */

#pragma once

#ifndef FoundationIO_NetworkIO_H
#define FoundationIO_NetworkIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    // We can write MAC Address and IPv4 and IPv6 address string to number functions?

    // We can theoretically do very low level stuff, where we do the DNS lookups ourselves and all that and we just use Raw Sockets?

    // RAW Sockets operate on the Ethernet level, therefore we need to handle TCP/UDP and IPv4/IPv6.

    // There is no need to provide the port and IP address to a raw socket, unlike in the case of stream and datagram sockets

    // So we'll need to get the MAC address of the router or whatever is on the other end of the device that the cable or signal is connected to, how do we do this?
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_NetworkIO_H */
