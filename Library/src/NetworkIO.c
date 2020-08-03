/*!
@header           NetworkIO.h
@author           Marcus Johnson
@copyright        2020+
@version          1.0.0
@brief            This header contains types, functions, and structs for Networking.
@overview         IPv6, IMCPv6, TCPv6, and UDPv6 are the primary focus of NetworkIO and building it all on top of Raw Sockets.
*/

#include "../include/NetworkIO.h"          /* Included for our declarations */
#include "../include/TextIO/TextIOTypes.h" /* Included for Unicode types */
#include "../include/TextIO/LogIO.h"       /* Included for Unicode types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Asynchronous RAW Socket Networking API, the OS only has to manage the hardware is the goal of NetworkIO.

     BitIO needs to be split with BitInput/BitOutput becoming FileInput/FileOutput.

     BitIO it's self being renamed BufferIO and it's BitBuffer type being the intermediary for both FileIO and NetworkIO
     */

    /*
     Only focusing on UDPv6 and TCPv6, IPv4 is not my goal currently.
     */

    /*
     Addressing:

     We need a static function to handle IPv6 notations, like "::" means everything above is 0
     Leading zeros in each group are removed.

     Loopback address is "::1"

     Need to accept address strings as any Unicode encoding, especially the big 3 UTF-8, UTF-16, UTF-32.

     Link-Local Address: fe80::/10, the suffix is 64 bits, potentially the MAC Address? DO NOT USE THE MAC ADDRESS, RFC-7217.


     */

    typedef struct IPV6Address {
        uint8_t Address[16]; // big endian
    } IPV6Address;

    IPV6Address *IPV6Address_Init(void) {
        IPV6Address *Address = (IPV6Address*) calloc(16, sizeof(IPV6Address));
        return Address;
    }

    static IPV6Address *UTF8_String2IPV6Address(UTF8 *Address) {
        IPV6Address *IPV6 = NULL;
        if (Address != NULL) {
            IPV6          = IPV6Address_Init();
            if (IPV6 != NULL && Address != NULL) {
                uint8_t GroupCount = 0;
                uint8_t DigitCount = 0;
                uint8_t CodeUnit   = 1;
                while (Address[CodeUnit] != PlatformIO_NULLTerminator) {
                    if (Address[CodeUnit - 1] == U':' && Address[CodeUnit] == U':') {
                        
                    }
                    CodeUnit      += 1;
                }
                // GroupCount needs to equal 8 at the end
                // DigitCount needs to be reset for every group

                /*
                 Ok, so what do I need to do? Loop until the end of the string, building a correct address, little fucking endian.
                 */
                // Loop looking for double colons and counting digits for each

            } else if (IPV6 == NULL) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate IPV6Address"));
            }
        } else if (Address == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Address Pointer is NULL"));
        }
        return IPV6;
    }

    void IPV6Address_Deinit(IPV6Address *Address) {
        free(Address->Address);
        free(Address);
    }

    /*
     Get the Max MTU size by starting at a Packet size of 65535 and then slowly knocking it down as it goes.

     I want to be able to get the true size to the byte down in 16 packet attempts.

     So 65535 is the first, then 32768, then 16384, 8192, 4096,  2048, 1280 is the minimum IPv6.

     65535,
     40960
     20480
     10240
     5120
     2560
     1280
     Then once we find the area that we're in of those 7 brackets up there, we 
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
