/*!
 @header         FileIO.h
 @author         Marcus Johnson
 @copyright      2020+
 @version        1.0.0
 @brief          This header contains code for reading and writing files, and utilities to manage them.
 */

#include "PlatformIO.h"               /* Included for Platform Independence macros */
#include "UnicodeIO/UnicodeIOTypes.h" /* Included for UTFX types */

#pragma once

#ifndef FoundationIO_FileIO_H
#define FoundationIO_FileIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @typedef      BitBuffer
     @abstract                                     Forward declaration of BufferIO's BitBuffer.
     */
    typedef struct BitBuffer BitBuffer;

    /*!
     @typedef      FileInput
     @abstract                                     Contains File/Socket pointers for reading to a BitBuffer.
     */
    typedef struct FileInput                        FileInput;

    /*!
     @typedef      FileOutput
     @abstract                                     Contains File/Socket pointers for writing from a BitBuffer.
     */
    typedef struct FileOutput                       FileOutput;

    /* FileInput */
    /*!
     @abstract                                     Creates a FileInput.
     @return                                       Returns a pointer to said FileInput structure.
     */
    FileInput     *FileInput_Init(void);

    /*!
     @abstract                                     Sets FileInput's FILE.
     @param        BitI                            The FileInput to set the File to.
     @param        File                            The File to set as FileInput's file.
     */
    void           FileInput_SetFile(FileInput *BitI, FILE *File);

    /*!
     @abstract                                     Opens a socket for reading.
     @param        BitI                            FileInput Pointer.
     @param        Domain                          What domain should communication take place on?
     @param        Type                            What type of Socket are you trying to open?
     @param        Protocol                        What protocol are you trying to use over this socket?
     */
    void           FileInput_OpenSocket(FileInput *BitI, int Domain, int Type, int Protocol);

    /*!
     @abstract                                     Connects FileInput to a socket.
     @param        BitI                            FileInput Pointer.
     @param        SocketAddress                   The address of the socket.
     @param        SocketSize                      I honestly have no idea what this means.
     */
    void           FileInput_ConnectSocket(FileInput *BitI, struct sockaddr *SocketAddress, uint32_t SocketSize);

    /*!
     @abstract                                     Gets the size of the FileInput file.
     @param        BitI                            FileInput Pointer.
     @return                                       Returns the value in BitI->FileSize if it exists.
     */
    uint64_t       FileInput_GetFileSize(FileInput *BitI);

    /*!
     @abstract                                     Gets the position of the FileInput file from the start.
     @param        BitI                            FileInput Pointer.
     @return                                       Returns the position of the file in bytes from the beginning
     */
    uint64_t       FileInput_GetFilePosition(FileInput *BitI);

    /*!
     @abstract                                     Computes the number of bytes left in the file.
     @param        BitI                            FileInput Pointer.
     @return                                       Returns the number of bytes left in the file.
     */
    uint64_t       FileInput_BytesRemaining(FileInput *BitI);

    /*!
     @abstract                                     Deallocates FileInput.
     @remark                                       For use when changing files, or exiting the program.
     @param        BitI                            FileInput Pointer you want to free.
     */
    void           FileInput_Deinit(FileInput *BitI);
    /* FileInput */

    /* FileOutput */
    /*!
     @abstract                                     Creates a FileOutput structure.
     @return                                       Returns a pointer to said FileOutput structure.
     */
    FileOutput     *FileOutput_Init(void);

    /*!
     @abstract                                     Sets FileOutput's FILE.
     @param        BitO                            The FileOutput to set the File to.
     @param        File                            The File to set as FileOutput's file.
     */
    void           FileOutput_SetFile(FileOutput *BitO, FILE *File);

    /*!
     @abstract                                     Deallocates the instance of FileOutput pointed to by BitO.
     @remark                                       For use when changing files, or exiting the program.
     @param        BitO                            FileOutput Pointer you want to free.
     */
    void           FileOutput_Deinit(FileOutput *BitO);
    /* FileOutput */

    /* BitBuffer */
    /*!
     @abstract                                     Reads data from FileInput to BitBuffer.
     @param        Input                           The FileInput to read from.
     @param        BitB                            The BitBuffer to read to.
     */
    void           FileInput_ReadBitBuffer(FileInput *Input, BitBuffer *BitB); // FileInput_ReadBitBuffer, FileOutput_WriteBitBuffer

    /*!
     @abstract                                     Reads fresh data to a BitBuffer from a BitInput.
     @remark                                       IS destructive, it will NOT keep any data in the buffer.
     @param        Input                           The FileInput to refresh the BitBuffer with.
     @param        BitB                            The BitBuffer to refresh.
     */
    void           BitBuffer_Refresh(FileInput *Input, BitBuffer *BitB);

    /*!
     @abstract                                     Writes a BitBuffer to BitOutput.
     @remark                                       Only writes actually used data.
     @param        Output                          The FileOutput to write to.
     @param        BitB                            The BitBuffer to write.
     */
    void           BitBuffer_Write(FileOutput *Output, BitBuffer *BitB);
    /* BitBuffer */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_FileIO_H */
