/*!
 @header          FileIO.h
 @author          Marcus Johnson
 @copyright       2020+
 @version         1.0.0
 @brief           This header contains code for reading and writing files, and utilities to manage them.
 */

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
     @abstract                                          Returns just the filename portion of a path string
     @remark                                            Equilivent to `basename` command
     @remark                                            On Windows checks '/' and '\' as directory seperators, on POSIX checks '\'
     @param             Path32                          Path is a UTF32 encoded string.
     */
    UTF32              *FileIO_UTF32_GetFileName(ImmutableString_UTF32 Path32);

    /*!
     @abstract                                         Returns the extension from a filename
     @remark                                           Stops at the earliest period, e.g. '.tar.gz' is extracted properly
     @param             Path8                          Path is a UTF8 encoded string
     */
    UTF8               *FileIO_UTF8_GetFileExtension(ImmutableString_UTF8 Path8);

    /*!
     @abstract                                         Returns the extension from a filename
     @remark                                           Stops at the earliest period, e.g. '.tar.gz' is extracted properly
     @param             Path16                         Path is a UTF16 encoded string
     */
    UTF16              *FileIO_UTF16_GetFileExtension(ImmutableString_UTF16 Path16);
    
    /*!
     @abstract                                         Returns the extension from a filename
     @remark                                           Stops at the earliest period, e.g. '.tar.gz' is extracted properly
     @param             Path32                         Path is a UTF32 encoded string
     */
    UTF32              *FileIO_UTF32_GetFileExtension(ImmutableString_UTF32 Path32);
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
     @param             AsyncIOStream                   FileIO_Input or FileIO_Output
     @return                                            Returns the size of the file
     */
    size_t              FileIO_GetSize(PlatformIO_Immutable(void *) AsyncIOStream);
    
    /*!
     @abstract                                          Gets the size of the elements in the Stream
     @remark                                            Primarily intended for character type identification, 8/16/32 bit characters
     @remark                                            Size is in bytes
     @param             Stream                          FileIO_Input or FileIO_Output
     @return                                            Returns the size of the file's elements
     */
    uint8_t             FileIO_GetEncodingSize(AsyncIOStream *Stream);

    /*!
     @abstract                                          Reads data from a file.
     @param             File2Read                       The File to read the data to.
     @param             Buffer                          Where to put the data to read.
     @param             BufferElementSize               The size of Buffer's elements in bytes.
     @param             Elements2Read                   The number of elements to read.
     @return                                            Returns the amount of data actually read.
     */
    size_t              FileIO_Read(const FILE *const File2Read, void *Buffer, uint8_t BufferElementSize, size_t Elements2Read);

    /*!
     @abstract                                          Seeks around a file.
     @param             File2Seek                       The File to seek around.
     @param             SeekSizeInBytes                 The number of bytes to seek.
     @param             SeekType                        The kind of seeking to do.
     @return                                            Returns true if sucessful.
     */
    bool                FileIO_Seek(FILE *File2Seek, ssize_t SeekSizeInBytes, AsyncIO_SeekTypes SeekType);

    /*!
     @abstract                                          Writes data to a file.
     @param             File2Write                      The File to write the data to.
     @param             Buffer                          Where to get the data to write.
     @param             BufferElementSize               The size of Buffer's elements in bytes.
     @param             Elements2Write                  The number of Elements to write.
     @return                                            Returns the amount of data actually written in bytes.
     */
    size_t              FileIO_Write(FILE *File2Write, PlatformIO_Immutable(void *) Buffer, uint8_t BufferElementSize, size_t Elements2Write);

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
