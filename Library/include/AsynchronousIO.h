/*!
 @header          AsynchronousIO.h
 @author          Marcus Johnson
 @copyright       2020+
 @version         1.0.0
 @brief           This header contains code for Asynchronous IO, and utilities to manage it.
 */

#pragma once

#ifndef FoundationIO_AsynchronousIO_H
#define FoundationIO_AsynchronousIO_H

#include "TextIO/TextIOTypes.h" /* Included for Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     This is the Low Level variant; We're not dealing with BitBuffer or any of it's shit here
     */
    
    /*!
     @enum         AsynchronousIO_DescriptorTypes
     @constant     DescriptorType_Unspecified           Invalid type
     @constant     DescriptorType_File                  Stream is a File
     @constant     DescriptorType_Socket                Stream is a Socket
     @constant     DescriptorType_Pipe                  Stream is a Pipe
     */
    typedef enum AsynchronousIO_DescriptorTypes {
                   DescriptorType_Unspecified           = 0,
                   DescriptorType_File                  = 1,
                   DescriptorType_Socket                = 2,
                   DescriptorType_Pipe                  = 4,
    } AsynchronousIO_DescriptorTypes;

    /*!
     @enum         AsynchronousIO_FileModes
     @constant     FileMode_Unspecified                 Invalid mode
     @constant     FileMode_Read                        Read the data in a file
     @constant     FileMode_Write                       Delete the contents of the file and write new data
     @constant     FileMode_Create                      Create the file if it doesn't exist
     @constant     FileMode_Append                      Add new data at the end of old data without overwriting it
     @constant     FileMode_Text                        Processes newlines and other text specific stuff
     @constant     FileMode_Binary                      Raw access as opposed to FileMode_Text
     */
    typedef enum AsynchronousIO_FileModes {
                   FileMode_Unspecified                 = 0,
                   FileMode_Read                        = 1,
                   FileMode_Write                       = 2,
                   FileMode_Create                      = 4,
                   FileMode_Append                      = 8,
                   FileMode_Text                        = 16,
                   FileMode_Binary                      = 32,
    } AsynchronousIO_FileModes;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline AsynchronousIO_FileModes operator | (AsynchronousIO_FileModes A, AsynchronousIO_FileModes B) {
            return static_cast<AsynchronousIO_FileModes>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
        }

        constexpr inline AsynchronousIO_FileModes operator & (AsynchronousIO_FileModes A, AsynchronousIO_FileModes B) {
            return static_cast<AsynchronousIO_FileModes>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
        }

        constexpr inline AsynchronousIO_FileModes operator |= (AsynchronousIO_FileModes A, AsynchronousIO_FileModes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<AsynchronousIO_FileModes>(A1 | B1);
        }

        constexpr inline AsynchronousIO_FileModes operator &= (AsynchronousIO_FileModes A, AsynchronousIO_FileModes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<AsynchronousIO_FileModes>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */
    
    /*!
     @enum        AsynchronousIO_SeekTypes
     @constant    SeekType_Beginning                    Seek from the beginning of the file.
     @constant    SeekType_Current                      Seek from the current position of the file.
     @constant    SeekType_End                          Seek from the end of the file.
     */
    typedef enum AsynchronousIO_SeekTypes {
                  SeekType_Beginning                    = 0,
                  SeekType_Current                      = 1,
                  SeekType_End                          = 2,
    } AsynchronousIO_SeekTypes;

    /*!
     @enum        AsynchronousIO_PathTypes
     @constant    PathType_Unspecified                  Invalid PathType.
     @constant    PathType_File                         The PathType is a File.
     @constant    PathType_Directory                    The PathType is a Folder, it can contain other files.
     @constant    PathType_Link_Soft                    The PathType is a Softlink/Symbolic Link; It points to another file.
     @constant    PathType_Link_Hard                    The PathType is a Hardlink; Multiple files can point to it.
     @constant    PathType_Socket                       The PathType is a Socket; for network access.
     @constant    PathType_Special_Char                 The PathType is a Character Block Device, Hardware represented as a file.
     @constant    PathType_Special_IPCPipe              The PathType is a IPC Block Device; used fr Inter-Process Communication.
     @constant    PathType_Special_Block                The PathType is a Block Device.
     */
    typedef enum AsynchronousIO_PathTypes {
                  PathType_Unspecified                  = 0,
                  PathType_File                         = 1,
                  PathType_Directory                    = 2,
                  PathType_Link_Soft                    = 4,
                  PathType_Link_Hard                    = 8,
                  PathType_Socket                       = 16,
                  PathType_Special_Char                 = 32,
                  PathType_Special_IPCPipe              = 64,
                  PathType_Special_Block                = 128,
    } AsynchronousIO_PathTypes;

    /*!
     @typedef           AsynchronousIOStream
     @abstract                                          Contains File/Socket descriptors for file management.
     */
    typedef struct      AsynchronousIOStream                   AsynchronousIOStream;

    /*!
     @typedef           AsynchronousIO_Descriptor
     @abstract                                          Index to File/Socket pointers for writing from a BitBuffer.
     */
    typedef             int                             AsynchronousIO_Descriptor;

    AsynchronousIOStream      *AsynchronousIOStream_Init(void);

    /*!
     @abstract                                          Gets AsynchronousIOStream's FileDescriptor
     @param             Stream                          The AsynchronousIOStream to get the Descriptor from
     */
    AsynchronousIO_Descriptor  AsynchronousIOStream_GetDescriptor(AsynchronousIOStream *Stream);

    /*!
     @abstract                                          Sets AsynchronousIOStream's FileDescriptor.
     @param             Stream                          The AsynchronousIOStream to set the File to.
     @param             FileID                          The File to set as AsynchronousIOStream's file.
     */
    void                AsynchronousIOStream_SetDescriptor(AsynchronousIOStream *Stream, AsynchronousIO_Descriptor FileID);

    /*!
     @abstract                                          Gets the size of the AsynchronousIOStream file.
     @param             Stream                          AsynchronousIOStream Pointer.
     @return                                            Returns the value in BitI->FileSize if it exists.
     */
    size_t              AsynchronousIOStream_GetSize(AsynchronousIOStream *Stream);

    /*!
     @abstract                                          Gets the position of the AsynchronousIOStream file from the start.
     @param             Stream                          AsynchronousIOStream Pointer.
     @return                                            Returns the position of the file in bytes from the beginning
     */
    size_t              AsynchronousIOStream_GetPosition(AsynchronousIOStream *Stream);

    /*!
     @abstract                                          Gets the number of bytes remaining
     @remark                                            This only makes sense on read streams
     @param             Stream                          The stream in question
     @return                                            Returns the number of bytes remaining
     */
    size_t              AsynchronousIOStream_GetBytesRemaining(AsynchronousIOStream *Stream);
    /* AsynchronousIOStream */
    
    /* BitBuffer */
    /*!
     @abstract                                          Opens a UTF8 encoded path and assigns it to Stream
     @param             Stream                          A fresh instance of AsynchronousIOStream
     @param             Path8                           The UTF-8 encoded string to open
     @param             FileMode                        The configuration to use when Opening
     */
    bool                AsynchronousIOStream_OpenPathUTF8(AsynchronousIOStream *Stream, ImmutableString_UTF8 Path8, AsynchronousIO_FileModes FileMode);
    
    /*!
     @abstract                                          Opens a UTF16 encoded path and assigns it to Stream
     @param             Stream                          A fresh instance of AsynchronousIOStream
     @param             Path16                          The UTF-16 encoded string to open
     @param             FileMode                        The configuration to use when Opening
     */
    bool                AsynchronousIOStream_OpenPathUTF16(AsynchronousIOStream *Stream, ImmutableString_UTF16 Path16, AsynchronousIO_FileModes FileMode);
    
    /*!
     @abstract                                          Reads data from Stream to Array
     @remark                                            ElementSize = 2 Bytes, NumElements = 2; 4 bytes read
     @param             Stream                          The Stream to read the data from
     @param             Array                           A pointer to the Array to write the data to
     @param             ElementSize                     The size in bytes of the Array elements, aka the Array's underlying type
     @param             NumElements                     The number of ElementSize elements to read
     @return                                            Returns the number of bytes actually read
     */
    size_t              AsynchronousIOStream_Read(AsynchronousIOStream *Stream, void *Array, uint8_t ElementSize, size_t NumElements);
    
    /*!
     @abstract                                          Writes data from Array to Stream
     @remark                                            ElementSize = 2 Bytes, NumElements = 2; 4 bytes read
     @param             Stream                          The Stream to write the data to
     @param             Array                           A pointer to the Array to write the data from
     @param             ElementSize                     The size in bytes of the Array elements, aka the Array's underlying type
     @param             NumElements                     The number of ElementSize elements to write
     @return                                            Returns the number of bytes actually written
     */
    size_t              AsynchronousIOStream_Write(AsynchronousIOStream *Stream, void *Array, uint8_t ElementSize, size_t NumElements);
    
    /*!
     @abstract                                          Closes the Descriptor, after flushing any unwritten data
     @param             Stream                          The Stream to close
     @return                                            Returns true if the Descriptor was sucessfully (flushed, if necessary) and closed
     */
    bool                AsynchronousIOStream_Deinit(AsynchronousIOStream *Stream);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_AsynchronousIO_H */
