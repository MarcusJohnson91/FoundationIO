/*!
 @header          FileIO.h
 @author          Marcus Johnson
 @copyright       2020+
 @version         1.0.0
 @brief           This header contains code for reading and writing files, and utilities to manage them.
 */

#include "PlatformIO.h"         /* Included for Platform Independence macros */
#include "TextIO/TextIOTypes.h" /* Included for Text types */

#pragma once

#ifndef FoundationIO_FileIO_H
#define FoundationIO_FileIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @enum         FileIO_FileModes
     @constant     FileMode_Unspecified                 Invalid mode
     @constant     FileMode_Read                        Read the data in a file
     @constant     FileMode_Write                       Delete the contents of the file and write new data
     @constant     FileMode_Append                      Add new data at the end of old data without overwriting it
     @constant     FileMode_Text                        Processes newlines and other text specific stuff
     @constant     FileMode_Binary                      Raw access as opposed to FileMode_Text
     */
    typedef enum FileIO_FileModes {
                   FileMode_Unspecified                 = 0,
                   FileMode_Read                        = 1,
                   FileMode_Write                       = 2,
                   FileMode_Append                      = 4,
                   FileMode_Text                        = 16,
                   FileMode_Binary                      = 32,
    } FileIO_FileModes;
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX && PlatformIO_LanguageVersionCXX >= PlatformIO_LanguageVersionCXX11)
    extern "C++" {
        constexpr inline FileIO_FileModes operator | (FileIO_FileModes A, FileIO_FileModes B) {
            return static_cast<FileIO_FileModes>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
        }

        constexpr inline FileIO_FileModes operator & (FileIO_FileModes A, FileIO_FileModes B) {
            return static_cast<FileIO_FileModes>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
        }

        constexpr inline FileIO_FileModes operator |= (FileIO_FileModes A, FileIO_FileModes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FileIO_FileModes>(A1 | B1);
        }

        constexpr inline FileIO_FileModes operator &= (FileIO_FileModes A, FileIO_FileModes B) {
            uint8_t A1 = static_cast<uint8_t>(A);
            uint8_t B1 = static_cast<uint8_t>(B);
            return static_cast<FileIO_FileModes>(A1 & B1);
        }
    }
#endif /* PlatformIO_Language */

    /*!
     @enum        FileIO_SeekTypes
     @constant    SeekType_Beginning                    Seek from the beginning of the file.
     @constant    SeekType_Current                      Seek from the current position of the file.
     @constant    SeekType_End                          Seek from the end of the file.
     */
    typedef enum FileIO_SeekTypes {
                  SeekType_Beginning                    = 0,
                  SeekType_Current                      = 1,
                  SeekType_End                          = 2,
    } FileIO_SeekTypes;

    /*!
     @enum        FileIO_PathTypes
     @constant    PathType_Unknown                      Invalid PathType.
     @constant    PathType_File                         The PathType is a File.
     @constant    PathType_Directory                    The PathType is a Folder, it can contain other files.
     @constant    PathType_Link_Soft                    The PathType is a Softlink/Symbolic Link; It points to another file.
     @constant    PathType_Link_Hard                    The PathType is a Hardlink; Multiple files can point to it.
     @constant    PathType_Socket                       The PathType is a Socket; for network access.
     @constant    PathType_Special_Char                 The PathType is a Character Block Device, Hardware represented as a file.
     @constant    PathType_Special_IPCPipe              The PathType is a IPC Block Device; used fr Inter-Process Communication.
     @constant    PathType_Special_Block                The PathType is a Block Device.
     */
    typedef enum FileIO_PathTypes {
                  PathType_Unknown                      = 0,
                  PathType_File                         = 1,
                  PathType_Directory                    = 2,
                  PathType_Link_Soft                    = 4,
                  PathType_Link_Hard                    = 8,
                  PathType_Socket                       = 16,
                  PathType_Special_Char                 = 32,
                  PathType_Special_IPCPipe              = 64,
                  PathType_Special_Block                = 128,
    } FileIO_PathTypes;

    /*!
     @typedef           BitBuffer
     @abstract                                          Forward declaration of BufferIO's BitBuffer.
     */
    typedef struct      BitBuffer                       BitBuffer;

    /*!
     @typedef           FileInput
     @abstract                                          Contains File/Socket pointers for reading to a BitBuffer.
     */
    typedef struct      FileInput                       FileInput;

    /*!
     @typedef           FileOutput
     @abstract                                          Contains File/Socket pointers for writing from a BitBuffer.
     */
    typedef struct      FileOutput                      FileOutput;

    /* FileInput */
    /*!
     @abstract                                          Creates a FileInput.
     @return                                            Returns a pointer to said FileInput structure.
     */
    FileInput          *FileInput_Init(void);

    /*!
     @abstract                                          Sets FileInput's FILE.
     @param             BitI                            The FileInput to set the File to.
     @param             File                            The File to set as FileInput's file.
     */
    void                FileInput_SetFile(FileInput *BitI, FILE *File);

    /*!
     @abstract                                          Opens a socket for reading.
     @param             BitI                            FileInput Pointer.
     @param             Domain                          What domain should communication take place on?
     @param             Type                            What type of Socket are you trying to open?
     @param             Protocol                        What protocol are you trying to use over this socket?
     */
    void                FileInput_OpenSocket(FileInput *BitI, int Domain, int Type, int Protocol);

    /*!
     @abstract                                          Connects FileInput to a socket.
     @param             BitI                            FileInput Pointer.
     @param             SocketAddress                   The address of the socket.
     @param             SocketSize                      I honestly have no idea what this means.
     */
    void                FileInput_ConnectSocket(FileInput *BitI, struct sockaddr *SocketAddress, uint32_t SocketSize);

    /*!
     @abstract                                          Gets the size of the FileInput file.
     @param             BitI                            FileInput Pointer.
     @return                                            Returns the value in BitI->FileSize if it exists.
     */
    uint64_t            FileInput_GetFileSize(FileInput *BitI);

    /*!
     @abstract                                          Gets the position of the FileInput file from the start.
     @param             BitI                            FileInput Pointer.
     @return                                            Returns the position of the file in bytes from the beginning
     */
    uint64_t            FileInput_GetFilePosition(FileInput *BitI);

    /*!
     @abstract                                          Computes the number of bytes left in the file.
     @param             BitI                            FileInput Pointer.
     @return                                            Returns the number of bytes left in the file.
     */
    uint64_t            FileInput_BytesRemaining(FileInput *BitI);

    /*!
     @abstract                                          Deallocates FileInput.
     @remark                                            For use when changing files, or exiting the program.
     @param             BitI                            FileInput Pointer you want to free.
     */
    void                FileInput_Deinit(FileInput *BitI);
    /* FileInput */

    /* FileOutput */
    /*!
     @abstract                                          Creates a FileOutput structure.
     @return                                            Returns a pointer to said FileOutput structure.
     */
    FileOutput         *FileOutput_Init(void);

    /*!
     @abstract                                          Sets FileOutput's FILE.
     @param             BitO                            The FileOutput to set the File to.
     @param             File                            The File to set as FileOutput's file.
     */
    void                FileOutput_SetFile(FileOutput *BitO, FILE *File);

    /*!
     @abstract                                          Deallocates the instance of FileOutput pointed to by BitO.
     @remark                                            For use when changing files, or exiting the program.
     @param             BitO                            FileOutput Pointer you want to free.
     */
    void                FileOutput_Deinit(FileOutput *BitO);
    /* FileOutput */

    /* BitBuffer */
    /*!
     @abstract                                          Reads data from FileInput to BitBuffer.
     @param             Input                           The FileInput to read from.
     @param             BitB                            The BitBuffer to read to.
     */
    void                FileInput_ReadBitBuffer(FileInput *Input, BitBuffer *BitB);

    /*!
     @abstract                                          Reads fresh data to a BitBuffer from a BitInput.
     @remark                                            IS destructive, it will NOT keep any data in the buffer.
     @param             Input                           The FileInput to refresh the BitBuffer with.
     @param             BitB                            The BitBuffer to refresh.
     */
    void                BitBuffer_Refresh(FileInput *Input, BitBuffer *BitB);

    /*!
     @abstract                                          Writes a BitBuffer to BitOutput.
     @remark                                            Only writes actually used data.
     @param             Output                          The FileOutput to write to.
     @param             BitB                            The BitBuffer to write.
     */
    void                FileOutput_WriteBitBuffer(FileOutput *Output, BitBuffer *BitB);
    /* BitBuffer */

    /* Path Operations */
    /*!
     @abstract                                          Returns just the filename portion of a path string
     @remark                                            Equilivent to `basename` command,
     @remark                                            On Windows checks '/' and '\' as directory seperators, on POSIX checks '\'
     @param             Path8                           Path is a UTF8 encoded string.
     */
    UTF8               *FileIO_UTF8_GetFileName(PlatformIO_Immutable(UTF8 *) Path8);

    /*!
     @abstract                                          Returns just the filename portion of a path string
     @remark                                            Equilivent to `basename` command
     @remark                                            On Windows checks '/' and '\' as directory seperators, on POSIX checks '\'
     @param             Path16                          Path is a UTF16 encoded string.
     */
    UTF16              *FileIO_UTF16_GetFileName(PlatformIO_Immutable(UTF16 *) Path16);

    /*!
     @abstract                                         Returns the extension from a filename
     @param             Path8                          Path is a UTF8 encoded string
     */
    UTF8               *FileIO_UTF8_GetFileExtension(PlatformIO_Immutable(UTF8 *) Path8);

    /*!
     @abstract                                         Returns the extension from a filename
     @param             Path16                         Path is a UTF16 encoded string
     */
    UTF16              *FileIO_UTF16_GetFileExtension(PlatformIO_Immutable(UTF16 *) Path16);

    /* Function to return the type of a path, folder, file, soft/hard link? */
    /* Path Operations */

    /* File Operations */
    /*!
     @abstract                                          Opens the file at location Path with Mode.
     @param             Path8                           Path is a UTF8 encoded string.
     @param             Mode                            Mode is an ORable bitmask specifying the type of file to open.
     */
    FILE               *FileIO_OpenUTF8(PlatformIO_Immutable(UTF8 *) Path8, FileIO_FileModes Mode);

    /*!
     @abstract                                          Opens the file at location Path with Mode.
     @param             Path16                          Path is a UTF16 encoded string.
     @param             Mode                            Mode is an ORable bitmask specifying the type of file to open.
     */
    FILE               *FileIO_OpenUTF16(PlatformIO_Immutable(UTF16 *) Path16, FileIO_FileModes Mode);

    /*!
     @abstract                                          Detects the Unicode encoding the File has been opened for
     @param             File                            The file to check
     */
    TextIO_StringTypes  FileIO_GetFileOrientation(PlatformIO_Immutable(FILE *) File);

    /*!
     @abstract                                          Gets the size of the FILE.
     @param             File                            The file to get the size of.
     @return                                            Returns the size of the file.
     */
    uint64_t            FileIO_GetSize(PlatformIO_Immutable(FILE *) File);

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
    bool                FileIO_Seek(PlatformIO_Immutable(FILE *) File2Seek, int64_t SeekSizeInBytes, FileIO_SeekTypes SeekType);

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
