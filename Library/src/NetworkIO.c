#include "../include/NetworkIO.h" /* Included for our declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Asynchronous RAW Socket Networking API, the OS only has to manage the hardware is the goal of NetworkIO.

     BitIO needs to be split with BitInput/BitOutput becoming FileInput/FileOutput.

     BitIO it's self being renamed BufferIO and it's BitBuffer type being the intermediary for both FileIO and NetworkIO
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
