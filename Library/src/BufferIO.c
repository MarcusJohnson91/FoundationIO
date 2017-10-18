#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#endif

#include "../include/BitIO.h"
#include "../include/Private/Endian.h"

#ifdef __cplusplus
extern "C" {
#endif
	
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
	
	static FILE           *BitIOGlobalLogFile = NULL;
	
	struct                 BitBuffer {
		uint64_t           NumBits;
		uint64_t           BitOffset;
		uint8_t           *Buffer;
	};
	
	struct BitInput {
		uint8_t            SourceType;
		FILE              *File;
		int                Socket;
		fpos_t             FileSize;
		fpos_t             FilePosition;
		uint64_t           FileSpecifierNum;
	};
	
	struct BitOutput {
		uint8_t            DrainType;
		FILE              *File;
		int                Socket;
		fpos_t             FilePosition;
		uint64_t           FileSpecifierNum;
	};
	
	BitInput *InitBitInput(void) {
		BitInput *BitI        = calloc(1, sizeof(BitInput));
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "InitBitInput", "Not enough memory to allocate this instance of BitInput");
		}
		return BitI;
	}
	
	BitOutput *InitBitOutput(void) {
		BitOutput *BitO       = calloc(1, sizeof(BitOutput));
		if (BitO == NULL) {
			Log(LOG_ERR, "libBitIO", "InitBitOutput", "Not enough memory to allocate this instance of BitOutput");
		}
		return BitO;
	}
	
	BitBuffer *InitBitBuffer(const uint64_t BitBufferSize) {
		BitBuffer *BitB       = calloc(1, sizeof(BitBuffer));
		BitB->Buffer          = calloc(1, BitBufferSize);
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "InitBitBuffer", "Not enough memory to allocate this instance of BitBuffer");
		} else if (BitB->Buffer == NULL) {
			Log(LOG_ERR, "libBitIO", "InitBitBuffer", "Not enough memory to allocate %d bits for BitBuffer's buffer", BitBufferSize);
		}
		return BitB;
	}
	
	void DeinitBitInput(BitInput *BitI) {
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "DeinitBitInput", "Pointer to BitInput is NULL");
		} else {
			fclose(BitI->File);
			close(BitI->Socket);
			free(BitI);
		}
	}
	
	void DeinitBitOutput(BitOutput *BitO) {
		if (BitO == NULL) {
			Log(LOG_ERR, "libBitIO", "DeinitBitOutput", "Pointer to BitOutput is NULL");
		} else {
			fflush(BitO->File);
			fclose(BitO->File);
			close(BitO->Socket);
			free(BitO);
		}
	}
	
	void DeinitBitBuffer(BitBuffer *BitB) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "DeinitBitBuffer", "Pointer to BitBuffer is NULL");
		} else {
			free(BitB->Buffer);
			free(BitB);
		}
	}
	
	void DeinitBitIOGlobalLogFile(void) {
		fclose(BitIOGlobalLogFile);
	}
	
	inline uint64_t Power(const uint64_t Base, const uint64_t Exponent) {
		uint64_t Result = 1ULL;
		for (uint64_t i = 0; i < Exponent; i++) {
			Result *= Base;
		}
		return Result;
	}
	
	inline uint8_t SwapNibble(const uint8_t Byte2Swap) {
		return ((Byte2Swap & 0xF0 >> 4) | (Byte2Swap & 0x0F << 4));
	}
	
	inline uint16_t SwapEndian16(const uint16_t Data2Swap) {
		return ((Data2Swap & 0xFF00) >> 8) | ((Data2Swap & 0x00FF) << 8);
	}
	
	inline uint32_t SwapEndian32(const uint32_t Data2Swap) {
		return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
	}
	
	inline uint64_t SwapEndian64(const uint64_t Data2Swap) {
		return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | \
				((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >>  8) | \
				((Data2Swap & 0x00000000FF000000) <<  8) | ((Data2Swap & 0x0000000000FF0000) << 24) | \
				((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
	}
	
	inline int64_t Bytes2Bits(const int64_t Bytes) {
		return Bytes * 8;
	}
	
	inline int64_t Bits2Bytes(const int64_t Bits, const bool RoundUp) {
		if (RoundUp == true) {
			return (Bits / 8) + (8 - (Bits % 8));
		} else {
			return Bits / 8;
		}
	}
	
	inline uint64_t NumBits2StoreSymbol(uint64_t Symbol) {
		uint64_t Bits = 0ULL;
		if (Symbol == 0) {
			Bits = 1;
		} else {
			while (Symbol > 0) {
				Bits += 1;
				Symbol >>= 1;
			}
		}
		return Bits;
	}
	
	static inline uint8_t CreateBitMaskLSBit(const uint8_t Bits2Extract) {
		return (uint8_t) Power(2, Bits2Extract) << (8 - Bits2Extract);
	}
	
	static inline uint8_t CreateBitMaskMSBit(const uint8_t Bits2Extract) {
		return (uint8_t) Power(2, Bits2Extract) >> (8 - Bits2Extract);
	}
	
	static inline uint8_t NumBits2ExtractFromByte(const uint64_t BitOffset, const uint8_t Bits2Extract) {
		uint8_t Bits2ExtractFromThisByte = 0;
		uint8_t BitsInThisByte           = BitOffset % 8;
		if (Bits2Extract >= BitsInThisByte) {
			Bits2ExtractFromThisByte = BitsInThisByte;
		} else {
			Bits2ExtractFromThisByte = Bits2Extract;
		}
		return Bits2ExtractFromThisByte;
	}
	
	inline uint8_t SwapBits(const uint8_t Byte) {
		return ((Byte & 0x80 >> 7)|(Byte & 0x40 >> 5)|(Byte & 0x20 >> 3)|(Byte & 0x10 >> 1)|(Byte & 0x8 << 1)|(Byte & 0x4 << 3)|(Byte & 0x2 << 5)|(Byte & 0x1 << 7));
	}
	
	inline bool IsOdd(const int64_t Number2Check) {
		bool X = false;
		if (Number2Check % 2 == 0) {
			X = true;
		}
		return X;
	}
	
	fpos_t BitInputBytesRemainingInFile(BitInput *BitI) { // BitInputBytesRemainingInFile
		fpos_t BytesLeft = 0LL;
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputBytesRemainingInFile", "Pointer to BitInput is NULL");
		} else {
			BytesLeft = BitI->FileSize - BitI->FilePosition;
		}
		return BytesLeft;
	}
	
	fpos_t BitInputGetFileSize(BitInput *BitI) {
		fpos_t InputSize = 0LL;
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputGetFileSize", "Pointer to BitInput is NULL");
		} else {
			if (BitI->FileSize == 0) {
				BitInputFindFileSize(BitI);
			} else {
				InputSize = BitI->FileSize;
			}
		}
		return InputSize;
	}
	
	fpos_t BitInputGetFilePosition(BitInput *BitI) {
		fpos_t Position = 0LL;
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputGetFilePosition", "Pointer to BitInput is NULL");
		} else {
			if (BitI->FilePosition == 0) {
				BitInputFindFileSize(BitI);
			} else {
				Position = BitI->FilePosition;
			}
		}
		return Position;
	}
	
	uint64_t BitBufferGetPosition(BitBuffer *BitB) {
		uint64_t Position = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferGetPosition", "Pointer to BitBuffer is NULL");
		} else {
			Position = BitB->BitOffset;
		}
		return Position;
	}
	
	uint64_t BitBufferGetSize(BitBuffer *BitB) {
		uint64_t BitBufferSize = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferGetSize", "Pointer to BitBuffer is NULL");
		} else {
			BitBufferSize = sizeof(BitB->Buffer);
		}
		return BitBufferSize;
	}
	
	void BitInputFindFileSize(BitInput *BitI) {
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputFindFileSize", "Pointer to BitInput is NULL");
		} else {
			fseek(BitI->File, 0, SEEK_END);
			fgetpos(BitI->File, &BitI->FileSize);
			fseek(BitI->File, 0, SEEK_SET);
			fgetpos(BitI->File, &BitI->FilePosition);
		}
	}
	
	void BitInputOpenFile(BitInput *BitI, const char *Path2Open) {
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputOpenFile", "Pointer to BitInput is NULL");
		} else if (Path2Open == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputOpenFile", "Pointer to Path2Open is NULL");
		} else {
			BitI->SourceType        = BitIOFile;
			uint64_t Path2OpenSize  = strlen(Path2Open) + BitIOStringNULLSize;
			char *NewPath           = calloc(1, Path2OpenSize);
			snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitI->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
			BitI->FileSpecifierNum += 1;
			BitI->File = fopen(Path2Open, "rb");
			if (BitI->File == NULL) {
				Log(LOG_ERR, "libBitIO", "BitInputOpenFile", "Couldn't open file: Check that the file exists and the permissions are correct");
			} else {
				setvbuf(BitI->File, NULL, _IONBF, 0);
			}
			free(NewPath);
		}
	}
	
	void BitOutputOpenFile(BitOutput *BitO, const char *Path2Open) {
		if (BitO == NULL) {
			Log(LOG_ERR, "libBitIO", "BitOutputOpenFile", "Pointer to BitOutput is NULL");
		} else if (Path2Open == NULL) {
			Log(LOG_ERR, "libBitIO", "BitOutputOpenFile", "Pointer to Path2Open is NULL");
		} else {
			BitO->DrainType         = BitIOFile;
			uint64_t Path2OpenSize  = strlen(Path2Open) + BitIOStringNULLSize;
			char *NewPath           = calloc(1, Path2OpenSize);
			snprintf(NewPath, Path2OpenSize, "%s%llu", Path2Open, BitO->FileSpecifierNum); // FIXME: HANDLE FORMAT STRINGS BETTER
			BitO->FileSpecifierNum += 1;
			BitO->File = fopen(Path2Open, "wb");
			if (BitO->File == NULL) {
				Log(LOG_ALERT, "libBitIO", "BitOutputOpenFile", "Couldn't open output file; Check that the path exists and the permissions are correct");
			} else {
				setvbuf(BitO->File, NULL, _IONBF, 0);
			}
			free(NewPath);
		}
	}
	
	void OpenLogFile(const char *LogFilePath) {
		if (LogFilePath == NULL) {
			BitIOGlobalLogFile = NULL;
			Log(LOG_ERR, "libBitIO", "OpenLogFile", "Pointer to LogFilePath is NULL");
		} else {
			BitIOGlobalLogFile = fopen(LogFilePath, "a+");
		}
	}
	
	bool BitBufferIsAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) { // BitBufferIsAligned
		bool AlignmentStatus = 0;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferIsAligned", "Pointer to BitBuffer is NULL");
		} else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
			Log(LOG_ERR, "libBitIO", "BitBufferIsAligned", "BytesOfAlignment: %d isn't an integer power of 2", BytesOfAlignment);
		} else {
			if (Bytes2Bits(BytesOfAlignment) - (8 - (BitB->BitOffset % 8)) == 0) {
				AlignmentStatus = true;
			} else {
				AlignmentStatus = false;
			}
		}
		return AlignmentStatus;
	}
	
	void BitBufferMakeAligned(BitBuffer *BitB, const uint8_t BytesOfAlignment) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferMakeAligned", "Pointer to BitBuffer is NULL");
		} else if (BytesOfAlignment % 2 != 0 && BytesOfAlignment != 1) {
			Log(LOG_ERR, "libBitIO", "BitBufferMakeAligned", "BytesOfAlignment: %d isn't a power of 2 (or 1)", BytesOfAlignment);
		} else {
			uint8_t Bits2Align = Bytes2Bits(BytesOfAlignment) - (8 - (BitB->BitOffset % 8));
			if (Bits2Align + BitB->BitOffset > BitB->NumBits) {
				BitB->Buffer = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Align, true));
			}
			BitB->BitOffset   += Bits2Align;
		}
	}
	
	void BitBufferSkip(BitBuffer *BitB, const int64_t Bits2Skip) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferSkip", "Pointer to BitBuffer is NULL");
		} else {
			if (Bits2Skip + BitB->BitOffset > BitB->NumBits) {
				BitB->Buffer = realloc(BitB->Buffer, Bits2Bytes(BitB->NumBits + Bits2Skip, true));
			}
			BitB->BitOffset += Bits2Skip;
		}
	}
	
	static inline void InsertBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
		uint8_t Bits = NumBits2Insert;
		while (Bits > 0) {
			uint64_t Bits2Put      = NumBits2ExtractFromByte(BitB->BitOffset, Bits);
			uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskLSBit(Bits2Put);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
			// Extract as is
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
			uint8_t FinalByte      = SwapBits(Data); // Extract and flip bit order
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
			// Extract and flip byte order
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
			uint8_t FinalByte      = SwapBits(Data); // Extract and flip the byte order AND bit order
#endif
			Bits                  -= Bits2Put;
		}
	}
	
	static inline void InsertBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
		// Write data from LSByte (LSBit is default) to LSByte,LSBit
		// What variables do we need to take into account? Just swapping bit order, BitsAvailable, and looping...
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
	}
	
	static inline void InsertBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
		// Write data from LSByte (LSBit is default) to MSByte, LSBit.
		// What variables do we need to take into account? Just swapping byte order, BitsAvailable, and looping...
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
	}
	
	static inline void InsertBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Insert, uint64_t Data2Insert) {
		// Write data from LSByte (LSBit is default) to MSByte, LSBit.
		// What variables do we need to take into account? Just swapping byte and bit order, BitsAvailable, and looping...
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
	}
	
	static inline uint64_t ExtractBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Extract) { // So this function reads data FROM Little endian, Least Significant Bit first
		uint64_t OutputData        = 0ULL;
		uint8_t  UserRequestedBits = Bits2Extract;
		uint8_t  FinalByte         = 0;
		uint8_t  Times2Shift       = 0;
		
		while (UserRequestedBits > 0) {
			uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
			uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskLSBit(Bits2Get);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
			OutputData           <<= Bits2Get;
			OutputData            += Data;
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
		 	FinalByte              = SwapBits(Data);
			Times2Shift            = 8 - (Bits2Get % 8);
			FinalByte            >>= Times2Shift;
			OutputData           <<= Bits2Get;
			OutputData            += Data;
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
			OutputData           <<= Bits2Get;
			OutputData            += Data;
			OutputData             = SwapEndian64(OutputData);
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
		 	FinalByte              = SwapBits(Data);
			OutputData           >>= Bits2Get;
			OutputData            += Data;
			OutputData             = SwapEndian64(OutputData);
#endif
			UserRequestedBits     -= Bits2Get;
		}
		return OutputData;
	}
	
	static inline uint64_t ExtractBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Extract) { // So the bits are in MSByte, MSBit format.
		uint64_t OutputData        = 0ULL;
		uint8_t  UserRequestedBits = Bits2Extract;
		
		while (UserRequestedBits > 0) {
			uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
			uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskMSBit(Bits2Get);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
			OutputData            &= (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
			uint8_t FinalByte      = SwapBits(Data);
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
			OutputData             & (0xFF << (Bits2Extract - Bits2Get)); // Byte Shift
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
			uint8_t FinalByte      = SwapBits(Data);
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
			OutputData           >>= BitB->BitOffset - Bits2Get;
			OutputData            += Data;
			UserRequestedBits     -= Bits2Get;
		}
		return 0ULL;
	}
	
	static inline uint64_t ExtractBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Extract) {
		uint64_t OutputData        = 0ULL;
		uint8_t  UserRequestedBits = Bits2Extract;
		
		while (UserRequestedBits > 0) {
			uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
			uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskMSBit(Bits2Get);
			
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
			uint8_t FinalByte      = SwapBits(Data);
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
			uint8_t FinalByte      = SwapBits(Data);
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
#endif
			OutputData           >>= BitB->BitOffset - Bits2Get;
			OutputData            += Data;
			UserRequestedBits     -= Bits2Get;
		}
		return 0ULL;
	}
	
	static inline uint64_t ExtractBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Extract) { // So the data needs to be 0x6E7C
		uint64_t OutputData        = 0ULL;
		uint8_t  UserRequestedBits = Bits2Extract;
		while (UserRequestedBits > 0) {
			uint64_t Bits2Get      = NumBits2ExtractFromByte(BitB->BitOffset, UserRequestedBits);
			uint8_t  Data          = BitB->Buffer[Bits2Bytes(BitB->BitOffset / 8, false)] & CreateBitMaskLSBit(Bits2Get);
#if   (RuntimeByteOrder == LSByte && RuntimeBitOrder == LSBit)
			/*
			 Extract data from Big Endian MSBit first, to little endian least significant bit first
			 So, if we need to extract 3 bits because the buffer is full, we need to extract them from the left aka mask with 0xE0
			 and we need to apply them as 0x7, aka rightshift 8 - Bits2Read=3 aka 5.
			 */
			// SO the bits need to be extracted from BitBuffer as LSBit? and applied as LSBit
			// It is 0x7C6E
#elif (RuntimeByteOrder == LSByte && RuntimeBitOrder == MSBit)
			// Extract the bits as LSBit, and apply them as MSBit.
			uint8_t FinalByte      = SwapBits(Data);
			// Is is 0x3E76
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == LSBit)
			// Extract the bits as LSBit and apply them as LSBit
			// Swap Bytes
			// It is 0x6E7C
#elif (RuntimeByteOrder == MSByte && RuntimeBitOrder == MSBit)
			// Extract the bits as LSBit and apply them as MSBit
			uint8_t FinalByte      = SwapBits(Data);
			// It is 0x763E
#endif
			OutputData           >>= BitB->BitOffset - Bits2Get;
			OutputData            += Data;
			UserRequestedBits     -= Bits2Get;
		}
		return OutputData;
	}
	
	uint64_t PeekBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsLSByteLSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
		} else {
			OutputData = ExtractBitsAsLSByteLSBit(BitB, Bits2Peek);
		}
		return OutputData;
	}
	
	uint64_t PeekBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsLSByteMSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsLSByteMSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
		} else {
			OutputData = ExtractBitsAsLSByteMSBit(BitB, Bits2Peek);
		}
		return OutputData;
	}
	
	uint64_t PeekBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsMSByteLSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsMSByteLSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
		} else {
			OutputData = ExtractBitsAsMSByteLSBit(BitB, Bits2Peek);
		}
		return OutputData;
	}
	
	uint64_t PeekBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Peek) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsMSByteMSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Peek <= 0 || Bits2Peek > 64 || Bits2Peek > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "PeekBitsAsMSByteMSBit", "Bits2Peek %d is greater than BitBuffer can provide %d, or greater than PeekBits can satisfy 1-64", Bits2Peek, BitB->BitOffset);
		} else {
			OutputData = ExtractBitsAsMSByteMSBit(BitB, Bits2Peek);
		}
		return OutputData;
	}
	
	uint64_t ReadBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsLSByteLSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
		} else {
			OutputData         = ExtractBitsAsLSByteLSBit(BitB, Bits2Read);
			BitB->BitOffset   += Bits2Read;
		}
		return OutputData;
	}
	
	uint64_t ReadBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsLSByteMSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsLSByteMSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
		} else {
			OutputData         = ExtractBitsAsLSByteMSBit(BitB, Bits2Read);
			BitB->BitOffset   += Bits2Read;
		}
		return OutputData;
	}
	
	uint64_t ReadBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsMSByteLSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsMSByteLSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
		} else {
			OutputData         = ExtractBitsAsMSByteLSBit(BitB, Bits2Read);
			BitB->BitOffset   += Bits2Read;
		}
		return OutputData;
	}
	
	uint64_t ReadBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t Bits2Read) {
		uint64_t OutputData = 0ULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsMSByteMSBit", "Pointer to BitBuffer is NULL");
		} else if (Bits2Read <= 0 || Bits2Read > 64 || Bits2Read > BitB->BitOffset) {
			Log(LOG_ERR, "libBitIO", "ReadBitsAsMSByteMSBit", "Bits2Read %d is greater than BitBuffer can provide %d, or greater than ReadBits can satisfy 1-64", Bits2Read, BitB->BitOffset);
		} else {
			OutputData         = ExtractBitsAsMSByteMSBit(BitB, Bits2Read);
			BitB->BitOffset   += Bits2Read;
		}
		return OutputData;
	}
	
	void     WriteBitsAsLSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else if (NumBits2Write <= 0 || NumBits2Write > 64) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteLSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
		} else {
			InsertBitsAsLSByteLSBit(BitB, NumBits2Write, Bits2Write);
		}
	}
	
	void     WriteBitsAsLSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteMSBit", "Pointer to BitBuffer is NULL");
		} else if (NumBits2Write <= 0 || NumBits2Write > 64) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsLSByteMSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
		} else {
			InsertBitsAsLSByteMSBit(BitB, NumBits2Write, Bits2Write);
		}
	}
	
	void     WriteBitsAsMSByteLSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteLSBit", "Pointer to BitBuffer is NULL");
		} else if (NumBits2Write <= 0 || NumBits2Write > 64) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteLSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
		} else {
			InsertBitsAsMSByteLSBit(BitB, NumBits2Write, Bits2Write);
		}
	}
	
	void     WriteBitsAsMSByteMSBit(BitBuffer *BitB, const uint8_t NumBits2Write, const uint64_t Bits2Write) {
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteMSBit", "Pointer to BitBuffer is NULL");
		} else if (NumBits2Write <= 0 || NumBits2Write > 64) {
			Log(LOG_ERR, "libBitIO", "WriteBitsAsMSByteMSBit", "NumBits2Write %d is greater than BitBuffer can provide %d, or greater than WriteBits can satisfy 1-64", NumBits2Write);
		} else {
			InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Bits2Write);
		}
	}
	
	uint64_t ReadUnaryAsLSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsTruncated == true) {
			Value += 1;
		}
		while (ExtractBitsAsLSByteLSBit(BitB, 1) != StopBit) {
			Value += 1;
		}
		return IsZeroAvailable ? Value + 1 : Value;
	}
	
	uint64_t ReadUnaryAsLSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsTruncated == true) {
			Value += 1;
		}
		while (ExtractBitsAsLSByteMSBit(BitB, 1) != StopBit) {
			Value += 1;
		}
		return IsZeroAvailable ? Value + 1 : Value;
	}
	
	uint64_t ReadUnaryAsMSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsTruncated == true) {
			Value += 1;
		}
		while (ExtractBitsAsMSByteLSBit(BitB, 1) != StopBit) {
			Value += 1;
		}
		return IsZeroAvailable ? Value + 1 : Value;
	}
	
	uint64_t ReadUnaryAsMSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsTruncated == true) {
			Value += 1;
		}
		while (ExtractBitsAsMSByteMSBit(BitB, 1) != StopBit) {
			Value += 1;
		}
		return IsZeroAvailable ? Value + 1 : Value;
	}
	
	void     WriteUnaryAsLSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write) {
		bool UnaryBit = !StopBit;
		uint64_t UnaryBits2Write = NumBits2StoreSymbol(Field2Write);
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteUnaryAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else {
			if (IsTruncated == true) {
				UnaryBits2Write -= 1;
			}
			if (IsZeroAvailable == true) {
				Field2Write += 1;
			}
			while (Field2Write > 0) {
				InsertBitsAsLSByteLSBit(BitB, 1, UnaryBit);
				Field2Write--;
			}
			InsertBitsAsLSByteLSBit(BitB, 1, StopBit & 1);
		}
	}
	
	void     WriteUnaryAsLSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write) {
		bool UnaryBit = !StopBit;
		uint64_t UnaryBits2Write = NumBits2StoreSymbol(Field2Write);
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteUnaryAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else {
			if (IsTruncated == true) {
				UnaryBits2Write -= 1;
			}
			if (IsZeroAvailable == true) {
				Field2Write += 1;
			}
			while (Field2Write > 0) {
				InsertBitsAsLSByteMSBit(BitB, 1, UnaryBit);
				Field2Write--;
			}
			InsertBitsAsLSByteMSBit(BitB, 1, StopBit & 1);
		}
	}
	
	void     WriteUnaryAsMSByteLSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write) {
		bool UnaryBit = !StopBit;
		uint64_t UnaryBits2Write = NumBits2StoreSymbol(Field2Write);
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteUnaryAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else {
			if (IsTruncated == true) {
				UnaryBits2Write -= 1;
			}
			if (IsZeroAvailable == true) {
				Field2Write += 1;
			}
			while (Field2Write > 0) {
				
				InsertBitsAsMSByteLSBit(BitB, 1, UnaryBit);
				Field2Write--;
			}
			InsertBitsAsMSByteLSBit(BitB, 1, StopBit & 1);
		}
	}
	
	void     WriteUnaryAsMSByteMSBit(BitBuffer *BitB, const bool IsZeroAvailable, const bool IsTruncated, const bool StopBit, uint64_t Field2Write) {
		bool UnaryBit = !StopBit;
		uint64_t UnaryBits2Write = NumBits2StoreSymbol(Field2Write);
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "WriteUnaryAsLSByteLSBit", "Pointer to BitBuffer is NULL");
		} else {
			if (IsTruncated == true) {
				UnaryBits2Write -= 1;
			}
			if (IsZeroAvailable == true) {
				Field2Write += 1;
			}
			while (Field2Write > 0) {
				InsertBitsAsMSByteMSBit(BitB, 1, UnaryBit);
				Field2Write--;
			}
			InsertBitsAsMSByteMSBit(BitB, 1, StopBit & 1);
		}
	}
	
	uint64_t ReadExpGolombAsLSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsSigned == false) {
			uint64_t Bits2Read = ReadUnaryAsLSByteLSBit(BitB, Yes, Yes, StopBit);
			Value              = ExtractBitsAsLSByteLSBit(BitB, Bits2Read);
		} else {
		}
		return Value;
	}
	
	uint64_t ReadExpGolombAsLSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsSigned == false) {
			uint64_t Bits2Read = ReadUnaryAsLSByteMSBit(BitB, Yes, Yes, StopBit);
			Value              = ExtractBitsAsLSByteMSBit(BitB, Bits2Read);
		} else {
		}
		return Value;
	}
	
	uint64_t ReadExpGolombAsMSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsSigned == false) {
			uint64_t Bits2Read = ReadUnaryAsMSByteLSBit(BitB, Yes, Yes, StopBit);
			Value              = ExtractBitsAsMSByteLSBit(BitB, Bits2Read);
		} else {
		}
		return Value;
	}
	
	uint64_t ReadExpGolombAsMSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit) {
		uint64_t Value = 0ULL;
		if (IsSigned == false) {
			uint64_t Bits2Read = ReadUnaryAsMSByteMSBit(BitB, Yes, Yes, StopBit);
			Value              = ExtractBitsAsMSByteMSBit(BitB, Bits2Read);
		} else {
		}
		return Value;
	}
	
	void     WriteExpGolombAsLSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write) {
		uint64_t NumBits2Write = NumBits2StoreSymbol(Field2Write);
		WriteUnaryAsLSByteLSBit(BitB, Yes, Yes, StopBit, Field2Write);
		InsertBitsAsLSByteLSBit(BitB, 1, StopBit);
		if (IsSigned == false) {
			InsertBitsAsLSByteLSBit(BitB, NumBits2Write + 1, Field2Write + 1);
		} else {
			if (Field2Write < 0) { // Negative
			} else { // Positive
				InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Field2Write + 1);
			}
		}
	}
	
	void     WriteExpGolombAsLSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write) {
		uint64_t NumBits2Write = NumBits2StoreSymbol(Field2Write);
		WriteUnaryAsLSByteMSBit(BitB, Yes, Yes, StopBit, Field2Write);
		InsertBitsAsLSByteMSBit(BitB, 1, StopBit);
		if (IsSigned == false) {
			InsertBitsAsLSByteMSBit(BitB, NumBits2Write + 1, Field2Write + 1);
		} else {
			if (Field2Write < 0) { // Negative
			} else { // Positive
				InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Field2Write + 1);
			}
		}
	}
	
	void     WriteExpGolombAsMSByteLSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write) {
		uint64_t NumBits2Write = NumBits2StoreSymbol(Field2Write);
		WriteUnaryAsMSByteLSBit(BitB, Yes, Yes, StopBit, Field2Write);
		InsertBitsAsMSByteLSBit(BitB, 1, StopBit);
		if (IsSigned == false) {
			InsertBitsAsMSByteLSBit(BitB, NumBits2Write + 1, Field2Write + 1);
		} else {
			if (Field2Write < 0) { // Negative
			} else { // Positive
				InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Field2Write + 1);
			}
		}
	}
	
	void     WriteExpGolombAsMSByteMSBit(BitBuffer *BitB, const bool IsSigned, const bool StopBit, const int64_t Field2Write) {
		uint64_t NumBits2Write = NumBits2StoreSymbol(Field2Write);
		WriteUnaryAsMSByteMSBit(BitB, Yes, Yes, StopBit, Field2Write);
		InsertBitsAsMSByteMSBit(BitB, 1, StopBit);
		if (IsSigned == false) {
			InsertBitsAsMSByteMSBit(BitB, NumBits2Write + 1, Field2Write + 1);
		} else {
			if (Field2Write < 0) { // Negative
			} else { // Positive
				InsertBitsAsMSByteMSBit(BitB, NumBits2Write, Field2Write + 1);
			}
		}
	}
	
	uint8_t *ReadGUUIDAsUUIDString(BitBuffer *BitB) {
		uint8_t *UUIDString      = NULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadGUUIDAsUUIDString", "Pointer to BitBuffer is NULL");
		} else {
			UUIDString           = calloc(1, BitIOGUUIDStringSize);
			if (UUIDString == NULL) {
				Log(LOG_ERR, "libBitIO", "ReadGUUIDAsUUIDString", "Not enough memory to allocate UUIDString");
			} else {
				uint32_t Section1    = ExtractBitsAsMSByteLSBit(BitB, 32);
				BitBufferSkip(BitB, 8);
				uint16_t Section2    = ExtractBitsAsMSByteLSBit(BitB, 16);
				BitBufferSkip(BitB, 8);
				uint16_t Section3    = ExtractBitsAsMSByteLSBit(BitB, 16);
				BitBufferSkip(BitB, 8);
				uint16_t Section4    = ExtractBitsAsMSByteLSBit(BitB, 16);
				BitBufferSkip(BitB, 8);
				uint64_t Section5    = ExtractBitsAsMSByteLSBit(BitB, 48);
				sprintf((char*)UUIDString, "%d-%d-%d-%d-%llu", Section1, Section2, Section3, Section4, Section5);
			}
		}
		return UUIDString;
	}
	
	uint8_t *ReadGUUIDAsGUIDString(BitBuffer *BitB) {
		uint8_t *GUIDString      = NULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadGUUIDAsGUIDString", "Pointer to BitBuffer is NULL");
		} else {
			GUIDString           = calloc(1, BitIOGUUIDStringSize);
			if (GUIDString == NULL) {
				Log(LOG_ERR, "libBitIO", "ReadGUUIDAsGUIDString", "Not enough memory to allocate GUIDString");
			} else {
				uint32_t Section1    = ExtractBitsAsLSByteLSBit(BitB, 32);
				BitBufferSkip(BitB, 8);
				uint16_t Section2    = ExtractBitsAsLSByteLSBit(BitB, 16);
				BitBufferSkip(BitB, 8);
				uint16_t Section3    = ExtractBitsAsLSByteLSBit(BitB, 16);
				BitBufferSkip(BitB, 8);
				uint16_t Section4    = ExtractBitsAsLSByteLSBit(BitB, 16);
				BitBufferSkip(BitB, 8);
				uint64_t Section5    = ExtractBitsAsMSByteLSBit(BitB, 48);
				sprintf((char*)GUIDString, "%d-%d-%d-%d-%llu", Section1, Section2, Section3, Section4, Section5);
			}
		}
		return GUIDString;
	}
	
	uint8_t *ReadGUUIDAsBinaryUUID(BitBuffer *BitB) {
		uint8_t *BinaryUUID  = NULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadGUUIDAsBinaryUUID", "Pointer to BitBuffer is NULL");
		} else {
			BinaryUUID       = calloc(1, BitIOBinaryGUUIDSize);
			if (BinaryUUID == NULL) {
				Log(LOG_ERR, "libBitIO", "ReadGUUIDAsBinaryUUID", "Not enough memory to allocate BinaryUUID");
			} else {
				for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
					BinaryUUID[Byte] = ExtractBitsAsMSByteLSBit(BitB, 8);
				}
			}
		}
		return BinaryUUID;
	}
	
	uint8_t *ReadGUUIDAsBinaryGUID(BitBuffer *BitB) {
		uint8_t *BinaryGUID = NULL;
		if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "ReadGUUIDAsBinaryGUID", "Pointer to BitBuffer is NULL");
		} else {
			BinaryGUID      = calloc(1, BitIOBinaryGUUIDSize);
			if (BinaryGUID == NULL) {
				Log(LOG_ERR, "libBitIO", "ReadGUUIDAsBinaryGUID", "Not enough memory to allocate BinaryGUID");
			} else {
				for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
					BinaryGUID[Byte] = ExtractBitsAsLSByteLSBit(BitB, 8);
				}
			}
		}
		return BinaryGUID;
	}
	
	bool     CompareGUUIDs(const uint8_t *GUUID1, const uint8_t *GUUID2, const uint8_t GUUIDSize) {
		bool GUUIDsMatch = true;
		for (uint8_t GUUIDByte = 0; GUUIDByte < GUUIDSize; GUUIDByte++) {
			if (GUUID1[GUUIDByte] != GUUID2[GUUIDByte]) {
				GUUIDsMatch = false;
			}
		}
		return GUUIDsMatch;
	}
	
	uint8_t *ConvertGUUIDString2BinaryGUUID(const uint8_t *GUUIDString) {
		uint8_t *BinaryGUUID = NULL;
		if (GUUIDString == NULL) {
			Log(LOG_ERR, "libBitIO", "ConvertGUUIDString2BinaryGUUID", "Pointer to GUUIDString is NULL");
		} else {
			BinaryGUUID = calloc(1, BitIOBinaryGUUIDSize);
			if (BinaryGUUID == NULL) {
				Log(LOG_ERR, "libBitIO", "ConvertGUUIDString2BinaryGUUID", "Not enough memory to allocate BinaryGUUID");
			} else {
				BinaryGUUID[0] = GUUIDString[0];
				BinaryGUUID[1] = GUUIDString[1];
				BinaryGUUID[2] = GUUIDString[2];
				BinaryGUUID[3] = GUUIDString[3];
				
				BinaryGUUID[4] = GUUIDString[5];
				BinaryGUUID[5] = GUUIDString[6];
				
				BinaryGUUID[6] = GUUIDString[8];
				BinaryGUUID[7] = GUUIDString[9];
				
				BinaryGUUID[8] = GUUIDString[11];
				BinaryGUUID[9] = GUUIDString[12];
				
				BinaryGUUID[10] = GUUIDString[14];
				BinaryGUUID[11] = GUUIDString[15];
				BinaryGUUID[12] = GUUIDString[16];
				BinaryGUUID[13] = GUUIDString[17];
				BinaryGUUID[14] = GUUIDString[18];
				BinaryGUUID[15] = GUUIDString[19];
			}
		}
		return NULL;
	}
	
	uint8_t *ConvertBinaryGUUID2GUUIDString(const uint8_t *BinaryGUUID) {
		uint8_t *GUUIDString   = NULL;
		if (BinaryGUUID == NULL) {
			Log(LOG_ERR, "libBitIO", "ConvertBinaryGUUID2GUUIDString", "Pointer to BinaryGUUID is NULL");
		} else {
			GUUIDString        = calloc(1, BitIOGUUIDStringSize);
			if (GUUIDString == NULL) {
				Log(LOG_ERR, "libBitIO", "ConvertBinaryGUUID2GUUIDString", "Not enough memory to calloc %d bytes", BitIOGUUIDStringSize);
			} else {
				GUUIDString[0] = BinaryGUUID[0];
				GUUIDString[1] = BinaryGUUID[1];
				GUUIDString[2] = BinaryGUUID[2];
				GUUIDString[3] = BinaryGUUID[3];
				
				GUUIDString[4] = 0x2D;
				
				GUUIDString[5] = BinaryGUUID[4];
				GUUIDString[6] = BinaryGUUID[5];
				
				GUUIDString[7] = 0x2D;
				
				GUUIDString[8] = BinaryGUUID[6];
				GUUIDString[9] = BinaryGUUID[7];
				
				GUUIDString[10] = 0x2D;
				
				GUUIDString[11] = BinaryGUUID[8];
				GUUIDString[12] = BinaryGUUID[9];
				
				GUUIDString[13] = 0x2D;
				
				GUUIDString[14] = BinaryGUUID[10];
				GUUIDString[15] = BinaryGUUID[11];
				GUUIDString[16] = BinaryGUUID[12];
				GUUIDString[17] = BinaryGUUID[13];
				GUUIDString[18] = BinaryGUUID[14];
				GUUIDString[19] = BinaryGUUID[15];
			}
		}
		return GUUIDString;
	}
	
	uint8_t *SwapGUUIDString(const uint8_t *GUUIDString) {
		uint8_t *SwappedGUUIDString    = NULL;
		if (GUUIDString == NULL) {
			Log(LOG_ERR, "libBitIO", "SwapGUUIDString", "Pointer to GUUIDString is NULL");
		} else {
			SwappedGUUIDString         = calloc(1, BitIOGUUIDStringSize);
			if (SwappedGUUIDString == NULL) {
				Log(LOG_ERR, "libBitIO", "SwapGUUIDString", "Not enough memory to calloc %d bytes", BitIOGUUIDStringSize);
			} else {
				SwappedGUUIDString[0]  = GUUIDString[3];
				SwappedGUUIDString[1]  = GUUIDString[2];
				SwappedGUUIDString[2]  = GUUIDString[1];
				SwappedGUUIDString[3]  = GUUIDString[0];
				
				SwappedGUUIDString[4]  = GUUIDString[4];
				
				SwappedGUUIDString[5]  = GUUIDString[6];
				SwappedGUUIDString[6]  = GUUIDString[5];
				
				SwappedGUUIDString[7]  = GUUIDString[7];
				
				SwappedGUUIDString[8]  = GUUIDString[9];
				SwappedGUUIDString[9]  = GUUIDString[8];
				
				SwappedGUUIDString[10] = GUUIDString[10];
				
				SwappedGUUIDString[11] = GUUIDString[12];
				SwappedGUUIDString[12] = GUUIDString[11];
				
				for (uint8_t EndBytes = 13; EndBytes < BitIOGUUIDStringSize; EndBytes++) {
					SwappedGUUIDString[EndBytes] = GUUIDString[EndBytes];
				}
			}
		}
		return SwappedGUUIDString;
	}
	
	uint8_t *SwapBinaryGUUID(const uint8_t *BinaryGUUID) {
		uint8_t *SwappedBinaryGUUID   = NULL;
		if (BinaryGUUID == NULL) {
			Log(LOG_ERR, "libBitIO", "SwapBinaryGUUID", "Pointer to BinaryGUUID is NULL");
		} else {
			SwappedBinaryGUUID        = calloc(1, BitIOBinaryGUUIDSize);
			if (SwappedBinaryGUUID == NULL) {
				Log(LOG_ERR, "libBitIO", "SwapBinaryGUUID", "Not enough memory to calloc %d bytes", BitIOBinaryGUUIDSize);
			} else {
				SwappedBinaryGUUID[0] = BinaryGUUID[3];
				SwappedBinaryGUUID[1] = BinaryGUUID[2];
				SwappedBinaryGUUID[2] = BinaryGUUID[1];
				SwappedBinaryGUUID[3] = BinaryGUUID[0];
				
				SwappedBinaryGUUID[4] = BinaryGUUID[5];
				SwappedBinaryGUUID[5] = BinaryGUUID[4];
				
				SwappedBinaryGUUID[6] = BinaryGUUID[7];
				SwappedBinaryGUUID[7] = BinaryGUUID[6];
				
				SwappedBinaryGUUID[8] = BinaryGUUID[9];
				SwappedBinaryGUUID[9] = BinaryGUUID[8];
				
				for (uint8_t EndBytes = 10; EndBytes < BitIOBinaryGUUIDSize; EndBytes++) {
					SwappedBinaryGUUID[EndBytes] = BinaryGUUID[EndBytes];
				}
			}
		}
		return SwappedBinaryGUUID;
	}
	
	void WriteGUUIDAsUUIDString(BitBuffer *BitB, const uint8_t *UUIDString) {
		for (uint8_t Byte = 0; Byte < BitIOGUUIDStringSize; Byte++) {
			InsertBitsAsMSByteLSBit(BitB, 8, UUIDString[Byte]);
		}
	}
	
	void WriteGUUIDAsGUIDString(BitBuffer *BitB, const uint8_t *GUIDString) {
		for (uint8_t Byte = 0; Byte < BitIOGUUIDStringSize; Byte++) {
			InsertBitsAsLSByteLSBit(BitB, 8, GUIDString[Byte]);
		}
	}
	
	void WriteGUUIDAsBinaryUUID(BitBuffer *BitB, const uint8_t *BinaryUUID) {
		for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
			InsertBitsAsMSByteLSBit(BitB, 8, BinaryUUID[Byte]);
		}
	}
	
	void WriteGUUIDAsBinaryGUID(BitBuffer *BitB, const uint8_t *BinaryGUID) {
		for (uint8_t Byte = 0; Byte < BitIOBinaryGUUIDSize; Byte++) {
			InsertBitsAsLSByteLSBit(BitB, 8, BinaryGUID[Byte]);
		}
	}
	
	void DeinitGUUID(uint8_t *GUUID) {
		free(GUUID);
	}
	
	void BitBufferReadFromBitInput(BitInput *BitI, BitBuffer *BitB, const uint64_t Bytes2Read) { // BitBufferUpdateFromBitInput
		uint64_t BytesRead              = 0ULL;
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferReadFromBitInput", "Pointer to BitInput is NULL");
		} else if (BitB == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferReadFromBitInput", "Pointer to BitBuffer is NULL");
		} else if (Bytes2Read > (uint64_t)(BitI->FileSize - BitI->FilePosition)) {
			Log(LOG_ERR, "libBitIO", "BitBufferReadFromBitInput", "You tried reading more data: % than is available: %d in the file", Bytes2Read, BitI->FileSize - BitI->FilePosition);
		} else {
			if (BitB->Buffer != NULL) {
				free(BitB->Buffer);
			}
			BitB->Buffer                  = calloc(1, Bytes2Read);
			if (BitB->Buffer == NULL) {
				Log(LOG_ERR, "libBitIO", "BitBufferReadFromBitInput", "Not enough memory to allocate Buffer in BitBuffer");
			} else {
				if (BitI->SourceType == BitIOFile) {
					// Read from File
					BytesRead = fread(BitB->Buffer, 1, Bytes2Read, BitI->File);
				} else if (BitI->SourceType == BitIOSocket) {
					// Read from Socket
					BytesRead = read(BitI->Socket, BitB->Buffer, Bytes2Read);
				}
				if (BytesRead != Bytes2Read && BitI->SourceType == BitIOFile) {
					Log(LOG_ERR, "libBitIO", "BitBufferReadFromBitInput", "Fread read: %d bytes, but you requested: %d", BytesRead, Bytes2Read);
				} else {
					BitB->NumBits = Bytes2Bits(BytesRead);
				}
			}
		}
	}
	
	void BitBufferWrite2BitOutput(BitOutput *BitO, BitBuffer *Buffer2Write, const uint64_t Bytes2Write) {
		uint64_t BytesWritten           = 0ULL;
		if (BitO == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferWrite2BitOutput", "Pointer to BitInput is NULL");
		} else if (Buffer2Write == NULL) {
			Log(LOG_ERR, "libBitIO", "BitBufferWrite2BitOutput", "Pointer to BitBuffer is NULL");
		} else {
			if (BitO->DrainType == BitIOFile) {
				BytesWritten               = fwrite(Buffer2Write->Buffer, 1, Bytes2Write, BitO->File);
			} else if (BitO->DrainType == BitIOSocket) {
				BytesWritten               = write(BitO->Socket, Buffer2Write->Buffer, Bits2Bytes(Buffer2Write->NumBits, true));
			}
			if (BytesWritten != Bytes2Write) {
				Log(LOG_ERR, "libBitIO", "BitBufferWrite2BitOutput", "Fwrite wrote: %d bytes, but you requested: %d", BytesWritten, Bytes2Write);
			} else {
				Buffer2Write->NumBits -= Bytes2Bits(BytesWritten);
			}
		}
	}
	
	void BitInputOpenSocket(BitInput *BitI, const int Domain, const int Type, const int Protocol) {
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputOpenSocket", "Pointer to BitInput is NULL");
		} else {
			BitI->SourceType     = BitIOSocket;
#ifdef _POSIX_VERSION
			BitI->Socket         = socket(Domain, Type, Protocol);
#elif  _WIN32
#endif
		}
	}
	
	void BitOutputOpenSocket(BitOutput *BitO, const int Domain, const int Type, const int Protocol) {
		if (BitO == NULL) {
			Log(LOG_ERR, "libBitIO", "BitOutputOpenSocket", "Pointer to BitInput is NULL");
		} else {
			BitO->DrainType      = BitIOSocket;
#ifdef _POSIX_VERSION
			BitO->Socket         = socket(Domain, Type, Protocol);
#elif  _WIN32
#endif
		}
	}
	
	void BitInputConnectSocket(BitInput *BitI, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
		if (BitI == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputConnectSocket", "Pointer to BitInput is NULL");
		} else if (SocketAddress == NULL) {
			Log(LOG_ERR, "libBitIO", "BitInputConnectSocket", "Pointer to SocketAddress is NULL");
		} else {
			BitI->SourceType = BitIOSocket;
			connect(BitI->Socket, SocketAddress, SocketSize);
		}
	}
	
	void BitOutputConnectSocket(BitOutput *BitO, struct sockaddr *SocketAddress, const uint64_t SocketSize) {
		if (BitO == NULL) {
			Log(LOG_ERR, "libBitIO", "BitOutputConnectSocket", "Pointer to BitOutput is NULL");
		} else if (SocketAddress == NULL) {
			Log(LOG_ERR, "libBitIO", "BitOutputConnectSocket", "Pointer to SocketAddress is NULL");
		} else {
			BitO->DrainType = BitIOSocket;
			connect(BitO->Socket, SocketAddress, SocketSize);
		}
	}
	
	void Log(const uint8_t ErrorSeverity, const char *__restrict LibraryOrProgram, const char *__restrict FunctionName, const char *__restrict Description, ...) {
		uint8_t ErrorCodeSize = 0;
		static const char *ErrorCodePrefix = NULL;
		if (ErrorSeverity == LOG_EMERG) {
			ErrorCodeSize    = 10;
			ErrorCodePrefix  = "EMERGENCY!";
		} else if (ErrorSeverity == LOG_ALERT) {
			ErrorCodeSize    = 6;
			ErrorCodePrefix  = "ALERT!";
		} else if (ErrorSeverity == LOG_CRIT) {
			ErrorCodeSize    = 9;
			ErrorCodePrefix  = "CRITICAL!";
		} else if (ErrorSeverity == LOG_ERR) {
			ErrorCodeSize    = 6;
			ErrorCodePrefix  = "ERROR!";
		} else if (ErrorSeverity == LOG_WARNING) {
			ErrorCodeSize    = 7;
			ErrorCodePrefix  = "WARNING";
		} else if (ErrorSeverity == LOG_NOTICE) {
			ErrorCodeSize    = 6;
			ErrorCodePrefix  = "NOTICE";
		} else if (ErrorSeverity == LOG_INFO) {
			ErrorCodeSize    = 11;
			ErrorCodePrefix  = "INFORMATION";
		} else if (ErrorSeverity == LOG_DEBUG) {
			ErrorCodeSize    = 5;
			ErrorCodePrefix  = "DEBUG";
		} else {
			ErrorCodeSize    = 8;
			ErrorCodePrefix  = "UNKNOWN!";
		}
		
		int   EasyStringSize = ErrorCodeSize + strlen(LibraryOrProgram) + strlen(FunctionName) + strlen(Description) + BitIOStringNULLSize;
		char *EasyString     = calloc(1, EasyStringSize);
		
		snprintf(EasyString, EasyStringSize, "%s: %s - %s", ErrorCodePrefix, FunctionName, Description);
		
		va_list Arguments;
		va_start(Arguments, Description);
		int HardStringSize = vsnprintf(NULL, 0, "%s", Arguments);
		char *HardString   = calloc(1, HardStringSize);
		vsprintf(HardString, "%s", Arguments);
		va_end(Arguments);
		
		uint64_t ErrorStringSize = EasyStringSize + HardStringSize + BitIONewLineSize;
		char *ErrorString = calloc(1, ErrorStringSize);
		snprintf(ErrorString, ErrorStringSize, "%s%s%s", EasyString, HardString, BitIOLineEnding);
		if (BitIOGlobalLogFile == NULL) {
			fprintf(stderr, "%s", ErrorString);
		} else {
			fprintf(BitIOGlobalLogFile, "%s", ErrorString);
		}
		free(EasyString);
		free(HardString);
		free(ErrorString);
	}
	
#ifdef __cplusplus
}
#endif
