/*!
 @header          FileIO.h
 @author          Marcus Johnson
 @copyright       2020+
 @version         1.0.0
 @brief           This header contains code for reading and writing files, and utilities to manage them.
 */

#include "PlatformIO.h"         /* Included for Platform Independence macros */
#include "TextIO/TextIOTypes.h" /* Included for Text types */

#include "AsynchronousIO.h"     /* Included for Asynchronous IO operations */

#pragma once

#ifndef FoundationIO_FileIO_H
#define FoundationIO_FileIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /* Path Operations */
    /*!
     @abstract                                          Returns just the filename portion of a path string
     @remark                                            Equilivent to `basename` command,
     @remark                                            On Windows checks '/' and '\' as directory seperators, on POSIX checks '\'
     @param             Path8                           Path is a UTF8 encoded string.
     */
    UTF8               *FileIO_UTF8_GetFileName(ImmutableString_UTF8 Path8);

    /*!
     @abstract                                          Returns just the filename portion of a path string
     @remark                                            Equilivent to `basename` command
     @remark                                            On Windows checks '/' and '\' as directory seperators, on POSIX checks '\'
     @param             Path16                          Path is a UTF16 encoded string.
     */
    UTF16              *FileIO_UTF16_GetFileName(ImmutableString_UTF16 Path16);

    /*!
     @abstract                                         Returns the extension from a filename
     @param             Path8                          Path is a UTF8 encoded string
     */
    UTF8               *FileIO_UTF8_GetFileExtension(ImmutableString_UTF8 Path8);

    /*!
     @abstract                                         Returns the extension from a filename
     @param             Path16                         Path is a UTF16 encoded string
     */
    UTF16              *FileIO_UTF16_GetFileExtension(ImmutableString_UTF16 Path16);
    /* Path Operations */

    /* File Operations */
    /*!
     @abstract                                          Detects the Unicode encoding the File has been opened for
     @param             Stream                          The stream
     @param             StringType                      What width should the Stream use
     */
    bool                FileIO_SetFileOrientation(AsyncIOStream *Stream, TextIO_StringTypes StringType);

    /*!
     @abstract                                          Detects the Unicode encoding the File has been opened for
     @param             File                            The file to check
     */
    TextIO_StringTypes  FileIO_GetFileOrientation(PlatformIO_Immutable(FILE *) File);

    /*!
     @abstract                                          Gets the size of the Stream
     @param             AsyncIOStream                    FileIO_Input or FileIO_Output
     @return                                            Returns the size of the file
     */
    int64_t             FileIO_GetSize(PlatformIO_Immutable(void *) AsyncIOStream);

    /*!
     @abstract                                          Reads data from a file.
     @param             File2Read                       The File to read the data to.
     @param             Buffer                          Where to put the data to read.
     @param             BufferElementSize               The size of Buffer's elements in bytes.
     @param             Elements2Read                   The number of bytes to read.
     @return                                            Returns the amount of data actually read.
     */
    uint64_t            FileIO_Read(PlatformIO_Immutable(FILE *) File2Read, void *Buffer, uint8_t BufferElementSize, uint64_t Elements2Read);

    /*!
     @abstract                                          Seeks around a file.
     @param             File2Seek                       The File to seek around.
     @param             SeekSizeInBytes                 The number of bytes to seek.
     @param             SeekType                        The kind of seeking to do.
     @return                                            Returns true if sucessful.
     */
    bool                FileIO_Seek(PlatformIO_Immutable(FILE *) File2Seek, int64_t SeekSizeInBytes, AsyncIO_SeekTypes SeekType);

    /*!
     @abstract                                          Writes data to a file.
     @param             File2Write                      The File to write the data to.
     @param             Buffer                          Where to get the data to write.
     @param             BufferElementSize               The size of Buffer's elements in bytes.
     @param             Elements2Write                  The number of Elements to write.
     @return                                            Returns the amount of data actually written in bytes.
     */
    uint64_t            FileIO_Write(PlatformIO_Immutable(FILE *) File2Write, PlatformIO_Immutable(void *) Buffer, uint8_t BufferElementSize, uint64_t Elements2Write);

    /*!
     @abstract                                          Flushes the File stream and closes it.
     @param             File                            The file you want to close.
     @return                                            Returns true if the file was sucessfully closed.
     */
    bool                FileIO_Close(FILE *File);
    /* File Operations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_FileIO_H */
