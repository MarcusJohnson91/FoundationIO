#include "../include/AsynchronousIO.h"    /* Included for our declarations */
#include "../include/TextIO/LogIO.h"      /* Included for Logging */
#include "../include/TextIO/StringIO.h"   /* Included for UTF-X operations */

#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#include <aio.h>                          /* Included for aio_read and aio_write */
#include <fcntl.h>                        /* Included for open */
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
#include <process.h>
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct AsyncIOStream {
        uint64_t            StreamPosition;
        int64_t             StreamSize;
        AsyncIO_Descriptor  StreamID;
        AsyncIO_StreamTypes StreamType;
    } AsyncIOStream;

    /* AsyncIOStream */
    AsyncIOStream *AsyncIOStream_Init(void) {
        AsyncIOStream *Stream = calloc(1, sizeof(AsyncIOStream));
        return Stream;
    }

    AsyncIO_Descriptor AsyncIOStream_GetDescriptor(AsyncIOStream *Stream) {
        return Stream->StreamID;
    }

    static void AsyncIOStream_FindSize(AsyncIOStream *Stream) {
        if (Stream != NULL && Stream->StreamSize <= 0 && (Stream->StreamType & StreamType_File) == StreamType_File) {
            int64_t OriginalPosition  = lseek(Stream->StreamID, 0, SEEK_CUR);
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            Stream->StreamSize        = lseek(Stream->StreamID, 0, SEEK_END);
            lseek(Stream->StreamID, OriginalPosition, SEEK_SET);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            Stream->StreamSize        = _lseeki64(Stream->StreamID, 0, SEEK_END);
            _lseeki64(Stream->StreamID, OriginalPosition, SEEK_SET);
#endif /* PlatformIO_TargetOS */
        }
    }

    int64_t AsyncIOStream_GetSize(AsyncIOStream *Stream) {
        if (Stream->StreamSize <= 0) {
            AsyncIOStream_FindSize(Stream);
        }
        return Stream->StreamSize;
    }

    int64_t AsyncIOStream_GetPosition(AsyncIOStream *Stream) {
        return Stream->StreamPosition;
    }

    int64_t AsyncIOStream_GetBytesRemaining(AsyncIOStream *Stream) {
        return Stream->StreamSize - Stream->StreamPosition;
    }

    void AsyncIOStream_SetDescriptor(AsyncIOStream *Stream, AsyncIO_Descriptor Descriptor) {
        Stream->StreamID = Descriptor;
    }

    void AsyncIOStream_SetPosition(AsyncIOStream *Stream, int64_t Position) {
        Stream->StreamPosition = Position;
    }

    bool AsyncIOStream_OpenUTF8(AsyncIOStream *Stream, PlatformIO_Immutable(UTF8 *) Path8, AsyncIO_FileModes Mode) {
        bool OpenedSucessfully   = No;
        if (Path8 != NULL && Mode != FileMode_Unspecified ) {
            uint64_t Path8Offset = 0;
            bool Path8HasBOM     = UTF8_HasBOM(Path8);
            if (Path8HasBOM) {
                Path8Offset      = UTF8BOMSizeInCodeUnits;
            }
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
#ifndef __STDC_LIB_EXT1__
            Stream->StreamID     = open(&Path8[Path8Offset], Mode); // Use O_LARGEFILE
#else
            Stream->StreamID     = open_s(&Path8[Path8Offset], Mode);
#endif
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            UTF16 *Path16        = UTF8_Convert(&Path8[Path8Offset]);
            Stream->StreamID     = _wsopen((wchar_t*) Path16, Mode, _SH_DENYNO, _S_IREAD | _S_IWRITE);
            UTF16_Deinit(Path16);
#endif
        } else if (Path8 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Path8 Pointer is NULL"));
        } else if (Mode == FileMode_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileIO_Mode is Invalid"));
        }
        return OpenedSucessfully;
    }

    bool AsyncIOStream_OpenUTF16(AsyncIOStream *Stream, PlatformIO_Immutable(UTF16 *) Path16, AsyncIO_FileModes Mode) {
        bool OpenedSucessfully   = No;
        if (Path16 != NULL && Mode != FileMode_Unspecified ) {
            uint64_t Path16Offset = 0;
            bool Path16HasBOM     = UTF16_HasBOM(Path16);
            if (Path16HasBOM) {
                Path16Offset      = UTF16BOMSizeInCodeUnits;
            }
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            UTF8 *Path8          = UTF16_Convert(&Path16[Path16Offset]);
#ifndef __STDC_LIB_EXT1__
            Stream->StreamID     = open(Path8, Mode);
#else
            Stream->StreamID     = open_s(Path8, Mode);
#endif
            UTF8_Deinit(Path8);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            Stream->StreamID     = _wsopen((wchar_t*) Path16, Mode, _SH_DENYNO, _S_IREAD | _S_IWRITE);
#endif
        } else if (Path16 == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Path8 Pointer is NULL"));
        } else if (Mode == FileMode_Unspecified) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FileIO_Mode is Invalid"));
        }
        return OpenedSucessfully;
    }

    uint64_t AsyncIOStream_Read(AsyncIOStream *Stream, void *Array, uint8_t ElementSize, uint8_t NumElements) {
        uint64_t NumBytesRead = 0ULL;
        if (Stream != NULL && Array != NULL) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            struct sigevent SignalEvent = {
                .sigev_notify            = 0,
                .sigev_signo             = 0,
                .sigev_value             = 0,
                .sigev_notify_function   = 0,
                .sigev_notify_attributes = 0,
            };

            struct aiocb Async = {
                .aio_fildes     = Stream->StreamID,
                .aio_offset     = Stream->StreamPosition,
                .aio_buf        = Array,
                .aio_nbytes     = ElementSize * NumElements,
                /* aio_reqprio  = Request Priority */
                .aio_sigevent   = SignalEvent,
                .aio_lio_opcode = 0, // What operation are we doing?
            };
            aio_read(&Async);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)
            
#endif
        }
        return NumBytesRead;
    }

    uint64_t AsyncIOStream_Write(AsyncIOStream *Stream, void *Array, uint8_t ElementSize, uint8_t NumElements) {
        uint64_t NumBytesWritten = 0ULL;
        if (Stream != NULL && Array != NULL) {
#if   ((PlatformIO_TargetOS & PlatformIO_TargetOSIsPOSIX) == PlatformIO_TargetOSIsPOSIX)
            struct sigevent SignalEvent = {
                .sigev_notify            = 0,
                .sigev_signo             = 0,
                .sigev_value             = 0,
                .sigev_notify_function   = 0,
                .sigev_notify_attributes = 0,
            };

            struct aiocb Async = {
                .aio_fildes     = Stream->StreamID,
                .aio_offset     = Stream->StreamPosition,
                .aio_buf        = Array,
                .aio_nbytes     = ElementSize * NumElements,
                /* aio_reqprio  = Request Priority */
                .aio_sigevent   = SignalEvent,
                .aio_lio_opcode = 0, // What operation are we doing?
            };
            aio_write(&Async);
#elif (PlatformIO_TargetOS == PlatformIO_TargetOSIsWindows)

#endif
        }
        return NumBytesWritten;
    }

    bool AsyncIOStream_Deinit(AsyncIOStream *Stream) {
        bool ClosedSucessfully     = No;
        if (Stream != NULL) {
            Stream->StreamType     = StreamType_Unspecified;
            Stream->StreamSize     = 0ULL;
            Stream->StreamPosition = 0ULL;
            close(Stream->StreamID);
        }
        return ClosedSucessfully;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
