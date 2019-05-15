#include "../include/Macros.h"         /* Included for FoundationIO's macros */
#include "../include/TestIO.h"         /* Included for our declarations */

#include "../include/Log.h"            /* Included for error reporting */

#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
#include <time.h>                      /* Included for timespec_get */
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
#include <mach/mach_time.h>
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
#ifndef   WIN32_LEAN_AND_MEAN
#define   WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */
#ifndef   VC_EXTRALEAN
#define   VC_EXTRALEAN
#endif /* VC_EXTRALEAN */
#include <Windows.h>                   /* Included for WinBase.h */
#include <winbase.h>                   /* Included for QueryPerformanceCounter, Windows.h MUST be included before winbase */
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Design.
     
     So, we need a way to have there be various comparison functions, and a way to set the expected result, and it needs to be runtime checking not compile time so no macros except make a _Generic comparison function.
     */
    
    // We need a very high resolution timer so we can compare performance.
    
    // we might also want to have a good randomnes generator (maybe even Unicode)
    
    uint64_t GetTimerFrequency(void) {
        uint64_t TimerFrequency = 0LL;
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
        clock_getres(TimerFrequency);
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
        
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
        QueryPerformanceFrequency(TimerFrequency);
#endif
        return TimerFrequency;
    }
    
    uint64_t GetTime(void) {
        uint64_t Time        = 0ULL;
        uint64_t CurrentTime = 0ULL;
        for (uint8_t Loop = 1; Loop <= 3; Loop++) {
#if   (FoundationIOTargetOS == FoundationIOPOSIXOS)
            clock_getres(CurrentTime);
#elif (FoundationIOTargetOS == FoundationIOAppleOS)
            CurrentTime      = mach_absolute_time(); // or mach_continuous_time?
#elif (FoundationIOTargetOS == FoundationIOWindowsOS)
            QueryPerformanceFrequency(CurrentTime);
#endif
            Time            += CurrentTime;
            Time            /= Loop;
        }
        return Time;
    }
    
    /*
     I need a high quality random number generator that is configurable.
     
     Being able to specify ranges and possibly even multiple range at once to create random numbers would be VERY useful, especially for testing Unicode strings.
     */
    
    /* AES Encryption for Cryptographically ecure Random Number Generation, my idea is to generate a random number, encrypt it with AES, then use that as a seed for another round of random number generation which is finally output to the user, and I don't trust hardware implementations. */
    
    /* API:
     GenerateRandomInteger_Insecure();
     GenerateRandomInteger_Secure();
     GenerateRandomCodePoint();
     GenerateRandomDecimal_Insecure();
     GenerateRandomDecimal_Secure();
     */
    
    typedef enum EntropyConstants {
        AES256Alignment          = 256 / 8,
        AES192Alignment          = 192 / 8,
        AES128Alignment          = 128 / 8,
        AES256Rounds             = 14,
        AES192Rounds             = 12,
        AES128Rounds             = 10,
        AESSubstitutionArraySize = 256,
    } EntropyConstants;
    
    typedef struct Entropy {
        uint8_t *Data;
        uint64_t Size;
        uint64_t Offset;
    } Entropy;
    
    Entropy *Entropy_Init(uint8_t Size) {
        Entropy *Entropy        = calloc(1, sizeof(Entropy));
        if (Entropy != NULL) {
            Entropy->Data       = calloc(Size * AES256Alignment, sizeof(uint8_t));
            if (Entropy->Data != NULL) {
                Entropy->Size   = Size * AES256Alignment;
                Entropy->Offset = 0ULL;
                /* Now we need to go ahead and get our entropy source, AES it, Mix it, re-AES it, and remix it */
            } else {
                Entropy_Deinit(Entropy);
            }
        }
        return Entropy;
    }
    
    static void Entropy_Encrypt(Entropy *Entropy) {
        if (Entropy != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    static void Entropy_Mix(Entropy *Entropy) {
        if (Entropy != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    void Entropy_Reseed(Entropy *Entropy) {
        if (Entropy != NULL) {
            // Reseed reads from whatever function there is, basically /dev/random on POSIX, AESes it, mixes it, re-AESes it, and re-mixes it.
            for (uint64_t Byte = 0ULL; Byte < Entropy->Size; Byte++) { // Reset the entropy array
                Entropy->Data[Byte] = 0;
            }
            
            
        } else {
            Log(Log_ERROR, __func__, U8("Entropy Pointer is NULL"));
        }
    }
    
    void Entropy_Deinit(Entropy *Entropy) {
        free(Entropy->Data);
        free(Entropy);
    }
    
#ifdef __cplusplus
}
#endif
