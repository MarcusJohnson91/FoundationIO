#include "../include/BitIO.h"

#ifdef __cplusplus
extern "C" {
#endif
	/* Start Anciliary Functions */
	int BitIOCurrentArgument = 1;
	
	uint16_t SwapEndian16(uint16_t Data2Swap) {
		return ((Data2Swap & 0xFF00) >> 8) || ((Data2Swap & 0x00FF) << 8);
	}
	
	uint32_t SwapEndian32(uint32_t Data2Swap) {
		return ((Data2Swap & 0xFF000000) >> 24) | ((Data2Swap & 0x00FF0000) >> 8) | ((Data2Swap & 0x0000FF00) << 8) | ((Data2Swap & 0x000000FF) << 24);
	}
	
	uint64_t SwapEndian64(uint64_t Data2Swap) {
		return (((Data2Swap & 0xFF00000000000000) >> 56) | ((Data2Swap & 0x00FF000000000000) >> 40) | ((Data2Swap & 0x0000FF0000000000) >> 24) | ((Data2Swap & 0x000000FF00000000) >> 8)  | ((Data2Swap & 0x00000000FF000000) << 8)  | ((Data2Swap & 0x0000000000FF0000) << 24) | ((Data2Swap & 0x000000000000FF00) << 40) | ((Data2Swap & 0x00000000000000FF) << 56));
	}
	
	bool IsOdd(int64_t Input) {
		return Input % 2;
	}
	
	uint64_t Bits2Bytes(uint64_t Bits) {
		return Bits / 8;
	}
	
	uint64_t Bytes2Bits(uint64_t Bytes) {
		return Bytes * 8;
	}
	
	uint8_t BitsRemaining(uint64_t BitsAvailable) {
		return 8 - (BitsAvailable % 8); // 8 - (32639 % 8) aka return 1.
	}
	
	uint64_t Signed2Unsigned(int64_t Signed) {
		if (Signed == (-0|0)) {
			return 0;
		}
		//return ~Signed + 1;
		return (uint64_t)Signed;
	}
	
	int64_t Unsigned2Signed(uint64_t Unsigned) {
		return (int64_t)Unsigned;
	}
	
	uint64_t OnesCompliment2TwosCompliment(uint64_t Input) { // FIXME: Test this.
		return Input ^ 0xFFFFFFFFFFFFFFFF;
	}
	
	uint64_t TwosCompliment2OnesCompliment(uint64_t Input) {
		return Input ^ 0xFFFFFFFFFFFFFFFF + 1;
	}
	
	uint8_t FindHighestBitSet(uint64_t InputData) {
		// Just return 0... that should never appear, so you're good...
		uint8_t HighestBitSet = 0;
		for (uint8_t Bit = 64; Bit > 0; Bit--) { // stop at the first 1 bit, and return that.
			if ((InputData & (1 << (Bit - 1))) == 1) {
				HighestBitSet = Bit;
			}
			/*
			while ((InputData & (CurrentBit >>= Bit)) == 0) { // FIXME: Wtf is this here for?
				
			}
			if ((InputData & Bit) == 1) {
				HighestBitSet = Bit;
				break;
			}
			 */
		}
		return HighestBitSet;
	}
	
	uint8_t CountBits(uint64_t Input) {
		if (Input == 0) {
			return 0;
		}
		uint8_t Count = 0;
		for (size_t Bit = 64; Bit > 0; Bit--) {
			if ((Input & (1 << (Bit - 1))) == 1) {
				Count++;
			}
		}
		return Count;
	}
	
	void AlignInputBits2Byte(BitInput *BitI) {
		BitI->BitsAvailable   -= BitsRemaining(BitI->BitsAvailable);
		BitI->BitsUnavailable += BitsRemaining(BitI->BitsAvailable);
	}
	
	void AlignInputBits2Bytes(BitInput *BitI, uint8_t Bytes2Align) { // TODO: TEST
		BitI->BitsAvailable   -= BitsRemaining(BitI->BitsAvailable);
		BitI->BitsUnavailable += BitsRemaining(BitI->BitsAvailable);
		if (Bytes2Align > 1) {
			uint64_t Wat = Bytes2Bits(Bytes2Align -1);
			BitI->BitsAvailable   -= Wat;
			BitI->BitsUnavailable += Wat;
		}
	}
	
	void AlignOutputBits2Byte(BitOutput *BitO) {
		BitO->BitsAvailable   -= BitsRemaining(BitO->BitsAvailable);
		BitO->BitsUnavailable += BitsRemaining(BitO->BitsAvailable);
	}
	
	void AlignOutputBits2Bytes(BitOutput *BitO, uint8_t Bytes2Align) { // TODO: TEST
		BitO->BitsAvailable   += BitsRemaining(BitO->BitsAvailable);
		BitO->BitsUnavailable -= BitsRemaining(BitO->BitsUnavailable);
		if (Bytes2Align > 1) {
			uint64_t Wat           = Bytes2Bits(Bytes2Align -1);
			BitO->BitsAvailable   -= Wat;
			BitO->BitsUnavailable += Wat;
		}
	}
	
	uint64_t Power2Mask(uint8_t Exponent) { // Bitshifting, so it's limited to base 2.
		if (Exponent > 64) {
			return 0;
		}
		/*
		uint64_t Mask = pow(2, Exponent) - 1;
		 */
		uint64_t Mask1 = 0, Mask2 = 0;
		Mask1 = 1 << (Exponent - 1);
		Mask2 = Mask1 - 1;
		return Mask1 + Mask2;
		
		// Shift 1 (Exponent - 1) times, then take the result, subtract one, and add that to the result.
	}
	/* End Anciliary Functions */
	
	/* Start NEW Path processing functions */
	void SanitizePath(const char Path2Sanitize[]) {
		
	}
	
	/* Start Obselete Path processing functions */
	void PrintHelp(void) {
		fprintf(stderr, "Usage: -i <input> -o <output>\n");
	}
	
	
	void ParseInputOptions(BitInput *BitI, int argc, const char *argv[]) {
		while (BitI->File == NULL) {
			for (int Argument = BitIOCurrentArgument; Argument < argc; Argument++) {
				if (strcasecmp(argv[Argument], "-i")             == 0) {
					Argument += 2; // Account for the space...
					if (strcasecmp(argv[Argument], "-")          == 0) { // hyphen: -, en dash: –, em dash: —;
						BitI->File = freopen(argv[Argument], "rb", stdin);
					} else {
						BitI->File = fopen(argv[Argument], "rb"); // Why is this not fucking working?
						if (BitI->File == NULL) {
							Log(SYSCritical, BitI->ErrorStatus->ParseInputOptions, FopenFailed, "BitIO", "ParseInputOptions", strerror(errno), NULL);
						}
					}
				}
				BitIOCurrentArgument += Argument - 1;
			}
		}
	}
	
	void ParseOutputOptions(BitOutput *BitO, int argc, const char *argv[]) {
		//glob_t *GlobBuffer = 0;
		while (BitO->File == NULL) {
			for (int Argument = BitIOCurrentArgument; Argument < argc; Argument++) {
				if (strcasecmp(argv[Argument], "-o")             == 0) {
					//Argument += 1; // FIXME: The For loop already handles this?
					Argument += 2; // Account for the space...
					if (strcasecmp(argv[Argument], "-")          == 0) {
						BitO->File = freopen(argv[Argument], "wb", stdout);
					}
					/*
					else if (strcasecmp(argv[Argument], "*")   == 0) {
						glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
					} else if (strcasecmp(argv[Argument], "?")   == 0) {
						glob(argv[Argument], (GLOB_ERR|GLOB_TILDE), 0, GlobBuffer);
					} 
					 */
					 else {
						BitO->File = fopen(argv[Argument], "wb");
						if (BitO->File == NULL) {
							Log(SYSCritical, BitO->ErrorStatus->ParseOutputOptions, FopenFailed, "BitIO", "ParseOutputOptions", strerror(errno), NULL);
						}
					}
				}
				BitIOCurrentArgument += Argument - 1;
			}
		}
	}
	
	void InitBitInput(BitInput *BitI, ErrorStatus *ES, int argc, const char *argv[]) {
		// FIXME: Remove any quotes on the path, or the issue could be that the -i and location are specified together...
		if (argc < 3) {
			PrintHelp();
		} else {
			if (BitI->ErrorStatus == NULL) {
				BitI->ErrorStatus  = ES;
			}
			setvbuf(BitI->File, NULL, _IONBF, BitInputBufferSize);
			// This needs to be called before the buffer is filled?
			ParseInputOptions(BitI, argc, argv);
			
			fseek(BitI->File, 0, SEEK_END);
			BitI->FileSize         = (uint64_t)ftell(BitI->File);
			fseek(BitI->File, 0, SEEK_SET);
			uint64_t Bytes2Read    = BitI->FileSize > BitInputBufferSize ? BitInputBufferSize : BitI->FileSize;
			uint64_t BytesRead     = fread(BitI->Buffer, 1, BitInputBufferSize, BitI->File);
			if (BytesRead < Bytes2Read) {
				Log(SYSCritical, BitI->ErrorStatus->InitBitInput, FreadReturnedTooLittleData, "BitIO", "InitBitInput", strerror(errno), NULL);
			}
			BitI->BitsAvailable = Bytes2Bits(BytesRead);
			BitI->BitsUnavailable         = 0;
		}
	}
	
	void InitBitOutput(BitOutput *BitO, ErrorStatus *ES, int argc, const char *argv[]) {
		if (argc < 3) {
			PrintHelp();
		} else {
			if (BitO->ErrorStatus == NULL) {
				BitO->ErrorStatus = ES;
			}
			ParseOutputOptions(BitO, argc, argv);
			BitO->BitsAvailable    = BitOutputBufferSizeInBits;
			BitO->BitsUnavailable  = 0;
		}
	}
	
	void ExitBitIO(BitInput *Input, BitOutput *Output) {
		CloseBitInput(Input);
		CloseBitOutput(Output);
		closelog();
	}
	
	void CloseBitInput(BitInput *BitI) {
		fclose(BitI->File);
		memset(BitI->Buffer,     0, Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable));
		free(BitI);
	}
	
	void CloseBitOutput(BitOutput *BitO) {
		fclose(BitO->File);
		memset(BitO->Buffer,    0, Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable));
		free(BitO);
	}
	/* End Input parsing functions */
	
	static void UpdateInputBuffer(BitInput *BitI, int64_t RelativeOffset) {
		if (RelativeOffset == 0) {
			Log(SYSCritical, BitI->ErrorStatus->UpdateInputBuffer, NumberNotInRange, "BitIO", "UpdateInputBuffer", NULL, NULL);
		}
		uint64_t Bytes2Read = BitI->FileSize - BitI->FilePosition > Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable) ? Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable) : BitI->FileSize - BitI->FilePosition;
		fseek(BitI->File, RelativeOffset, SEEK_CUR);
		memset(BitI, 0, Bytes2Read);
		uint64_t BytesRead = fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
		if (BytesRead != Bytes2Read) {
			Log(SYSWarning, BitI->ErrorStatus->UpdateInputBuffer, FreadReturnedTooLittleData, "BitIO", "UpdateInputBuffer", NULL, NULL);
		}
	}
	
	uint64_t ReadBits(BitInput *BitI, uint8_t Bits2Read) {
		uint64_t OutputData = 0;
		
		if (Bits2Read <= 0) {
			Log(SYSCritical, BitI->ErrorStatus->ReadBits, NumberNotInRange, "BitIO", "ReadBits", "Read too few bits", NULL);
		} else if (Bits2Read > 64) {
			Log(SYSCritical, BitI->ErrorStatus->ReadBits, NumberNotInRange, "BitIO", "ReadBits", "Read too many bits", NULL);
		} else {
			OutputData             = PeekBits(BitI, Bits2Read);
			if (BitI->ErrorStatus->PeekBits != Success) {
				BitI->ErrorStatus->ReadBits  = BitI->ErrorStatus->PeekBits;
			} else {
				BitI->BitsUnavailable += Bits2Read;
				BitI->BitsAvailable   -= Bits2Read;
			}
		}
		return OutputData;
	}
	
	uint64_t ReadExpGolomb(BitInput *BitI, bool IsSigned, bool IsTruncated) {
		uint64_t Zeros = 0;
		uint64_t  Data = 0;
		Data = ReadBits(BitI, 1);
		if ((Zeros == 0) && (Data == 1)) { // If this is the first loop?
			while (ReadBits(BitI, 1) == 0) {
				Zeros += 1;
			}
			Data  = (1 << Zeros);
			Data += ReadBits(BitI, Zeros);
		}
		if (IsSigned == true) {
			Data  = Unsigned2Signed(Data);
		}
		if (IsTruncated == true) {
			Data  = 2 * Zeros - 1; // Left bit first
			Data += ReadBits(BitI, Zeros);
		}
		return Data;
	}
	
	uint64_t ReadRICE(BitInput *BitI, bool StopBit) {
		uint64_t BitCount = 0;
		if (StopBit != (0|1)) {
			BitI->ErrorStatus->ReadRICE = NumberNotInRange;
		} else {
			while (PeekBits(BitI, 1) != StopBit) {
				BitCount += 1;
			}
		}
		return BitCount;
	}
	
	void WriteRICE(BitOutput *BitO, bool StopBit, uint64_t Data2Write) {
		if (StopBit != (0|1)) {
			BitO->ErrorStatus->WriteRICE = NumberNotInRange;
		} else {
			for (uint64_t Bit = 0; Bit < Data2Write; Bit++) {
				WriteBits(BitO, (1 ^ StopBit), 1);
			}
			WriteBits(BitO, StopBit, 1);
		}
	}
	
	uint64_t PeekBits(BitInput *BitI, uint8_t Bits2Peek) {
	//WARNING: You CAN'T read byte by byte, it simply won't work so stop trying. Also, this is faster, instruction wise.
		uint64_t OutputData = 0;
		uint8_t  Data       = 0;
		uint8_t  BitsLeft   = Bits2Peek;
		uint8_t  BitMask    = 0;
		
		if (Bits2Peek > 64) {
			BitI->ErrorStatus->PeekBits = TriedReadingTooManyBits;
		} else if (Bits2Peek <= 0) {
			BitI->ErrorStatus->PeekBits = TriedReadingTooFewBits;
		} else if (BitI->BitsUnavailable > (BitI->BitsAvailable - BitsLeft)) {
			// InputBuffer can't completely satisfy the request, update it.
			UpdateInputBuffer(BitI, 0);
		} else {
			while (BitsLeft > 0) {
				OutputData           <<= 1;
				BitMask                = 1 << ((8 - (BitI->BitsUnavailable % 8)) -1);
				Data                   = BitI->Buffer[Bits2Bytes(BitI->BitsUnavailable)] & BitMask;
				Data                 >>= ((8 - (BitI->BitsUnavailable % 8)) -1);
				OutputData            += Data;
				BitI->BitsAvailable   -= 1;
				BitI->BitsUnavailable += 1;
				BitsLeft              -= 1;
			}
			BitI->BitsAvailable       += Bits2Peek;
			BitI->BitsUnavailable     -= Bits2Peek;
		}
		return OutputData;
	}
	
	void WriteBits(BitOutput *BitO, uint64_t Data2Write, size_t NumBits) {
		if (NumBits <= 0) {
			BitO->ErrorStatus->WriteBits = TriedWritingTooFewBits;
		} else {
			
			if (BitO->BitsAvailable < NumBits) {
				// Write the completed bytes out, save the uncompleted ones in the array.
			}
			while (BitO->BitsUnavailable >= Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable)) {
				fwrite(BitO->Buffer, 1, Bits2Bytes(BitO->BitsUnavailable), BitO->File);
				fflush(BitO->File);
				memcpy(&BitO->Buffer, &BitO->Buffer + Bits2Bytes(BitO->BitsUnavailable), (Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable) - Bits2Bytes(BitO->BitsUnavailable)));
				memset(BitO->Buffer, 0, Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable));
				BitO->BitsUnavailable = (BitO->BitsUnavailable - ((BitO->BitsUnavailable / 8) * 8));
			}
			while (NumBits > 0) {
				uint64_t X = Data2Write & BitsRemaining(BitO->BitsUnavailable);
				BitO->Buffer[Bits2Bytes(BitO->BitsUnavailable)] += X;
				BitO->BitsUnavailable++;
				NumBits -= 1;
			}
			// FIXME: We need to just enlarge the buffer, the call a function to flush it all out to disk.
		}
	}
	
	void WriteBuffer(BitOutput *BitO, uintptr_t *Buffer2Write, size_t BufferSize) {
		uint8_t  Bits2Keep          = 0;
		uint8_t  Byte2Keep          = 0;
		uint64_t ExpandedBufferSize = 0;
		if (BitO->BitsAvailable + BitO->BitsUnavailable < Bytes2Bits(BufferSize)) {
			// It won't fit, so we have to enlarge the array.
			
			/* This part is just WriteBits */
			Bits2Keep = BitsRemaining(BitO->BitsUnavailable); // BitO->BitsUnavailable % 8;
			fwrite(BitO->Buffer, 1, Bits2Bytes(BitO->BitsUnavailable - Bits2Keep), BitO->File); // Write all but the uncompleted byte
			fflush(BitO->File);
			for (uint64_t Byte = Bits2Bytes(BitO->BitsUnavailable); Byte <Bits2Bytes(BitO->BitsUnavailable)+1; Byte++) {
				Byte2Keep              = BitO->Buffer[Byte];
			}
			realloc(BitO->Buffer, BufferSize);
			ExpandedBufferSize     = Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable) - BufferSize; // FIXME: is this correct?
			for (uint64_t Byte = 0; Byte < Bits2Bytes(BitO->BitsUnavailable + BitO->BitsAvailable); Byte++) {
				BitO->Buffer[Byte] = 0;
			}
			BitO->Buffer[0]        = Byte2Keep; // Replace the unfinished byte in the first element.
			BitO->BitsAvailable    = BitOutputBufferSizeInBits - Bits2Keep;
			BitO->BitsUnavailable  = Bits2Keep;
			
			for (uint64_t Byte = 0; Byte < BufferSize; Byte++) {
				for (uint8_t Bit = 0; Bit < 8; Bit++) {
					uint8_t BitData = Buffer2Write[Byte] & (1 << Bit);
					WriteBits(BitO, BitData, 1);
				}
			}
		}
	}
	
	void SeekBits(BitInput *BitI, int64_t Bits) {
		if ((BitI->BitsUnavailable + Bits) >= BitI->BitsAvailable) {
			UpdateInputBuffer(BitI, 0); // FIXME: This is ugly as hell
		} else {
			uint64_t Bytes2Read = BitI->BitsAvailable < BitOutputBufferSizeInBits ? BitI->BitsAvailable : BitOutputBufferSizeInBits;
			BitI->FilePosition = (uint64_t)ftell(BitI->File);
			fseek(BitI->File, BitOutputBufferSizeInBits - (BitI->BitsUnavailable + Bits), SEEK_CUR);
			memset(BitI->Buffer, 0, Bits2Bytes(BitI->BitsUnavailable + BitI->BitsAvailable));
			fread(BitI->Buffer, 1, Bytes2Read, BitI->File);
		}
	}
	
	void SkipBits(BitInput *BitI, uint8_t Bits2Skip) {
		if (Bits2Skip <= 0 | Bits2Skip > 64) {
			BitI->ErrorStatus->SkipBits = NumberNotInRange;
			exit(EXIT_FAILURE);
		}
		SeekBits(BitI, Bits2Skip);
	}
	
	uint64_t GenerateCRC(uintptr_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool Init) {
		uint64_t Output = ~Init;
		
		for (size_t Byte = 0; Byte < BufferSize; Byte++) {
			
		}
		
		return Output;
		
		/*
		uint64_t DataInput  = 0;
		uint64_t DataOutput = 0;
		for (size_t BufferIndex = 0; BufferIndex < BufferSize; BufferIndex++) { // needs to iterate over bytes the size of DataSize
			DataInput = DataBuffer[BufferIndex];
			for (uint8_t Bit = 0; Bit < CRCSize * 8; Bit++) {
				if ((Init & 0x1) ^ (DataInput & 0x1)) {
					Init ^= Poly;
				} else {
					Init >>= 1;
				}
			}
		}
		return DataOutput;
		 */
	}
	
	bool VerifyCRC(uintptr_t *DataBuffer, size_t BufferSize, uint64_t Poly, bool Init, uint64_t EmbeddedCRC) {
		uint64_t GeneratedCRC = GenerateCRC(DataBuffer, BufferSize, Poly, Init);
		if (GeneratedCRC == EmbeddedCRC) {
			return true;
		} else {
			return false;
		}
	}
	
	char ReadUUID(BitInput *BitI) {
		uint8_t UUID[21] = {0};
		for (uint8_t Character = 0; Character < 20; Character++) {
			if (Character == (4|7|10|13|21)) {
				if (Character == (4|7|10|13)) {
					UUID[Character] = "-";
				}
			}
			UUID[Character] = ReadBits(BitI, 8);
		}
		return UUID;
	}
	
	void WriteUUID(BitOutput *BitO, const char UUIDString) {
		if (sizeof(UUIDString) != 21) {
			BitO->ErrorStatus->WriteUUID = WrongStringSize;
		}
		for (uint8_t Character = 0; Character < 21; Character++) {
			if (Character != (4|7|10|13|21)) { // Don't write the NULL terminating char.
				WriteBits(BitO, UUIDString, 8);
			}
		}
	}
	
	void Log(int64_t ErrorType, ErrorStatus *ErrorVariable, int64_t ESError, char Library[], char Function[], char Description[], char FormatSpecifier) {
		openlog(Library, ErrorType, (LOG_PERROR|LOG_MAIL|LOG_USER));
		
		time_t CurrentTime;
		char DateTime[26];
		time(&CurrentTime);
		strftime(DateTime, 26, "%A, %B %e, %g+1000: %I:%M:%S %p %Z", CurrentTime);
		
		ErrorVariable = ESError;
		syslog(ErrorType, "%s: %s - %s: %s\n", DateTime, Library, Function, Description, FormatSpecifier);
		free(&CurrentTime);
	}
	
	/* Default Huffman table for reading the embedded table */
	
	
	/* Huffman Decoding functions */
	
	void DecodeHuffman(BitInput *BitI, Huffman *Huff, size_t HuffmanSize) { // 3 alphabets, literal, "alphabet of bytes", or <length 8, distance 15>
																			// the first 2 are combined, 0-255 = literal, 256 = End of Block, 257-285 = length
		
		// FIXME: WARNING: The Tilde ~ symbol is the negation symbol in C!!!!!
		
		uint8_t  DecodedData[32768] = {0};
		/* Parse Huffman block header */
		bool     IsLastHuffmanBlock     = ReadBits(BitI, 1);
		uint8_t  HuffmanCompressionType = ReadBits(BitI, 2); // 0 = none, 1 = fixed, 2 = dynamic, 3 = invalid.
		int32_t  DataLength             = 0;
		int32_t  OnesComplimentOfLength = 0; // Ones Compliment of DataLength
		
		if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != HuffmanSize) { // Make sure the numbers match up
			Log(SYSWarning, BitI->ErrorStatus->DecodeHuffman, InvalidData, "BitIO", "DecodeHuffman", "1s Compliment of Length != Length", NULL);
		}
		
		if (IsLastHuffmanBlock == true) {
			
		}
		
		if (HuffmanCompressionType == 0) { // No compression.
			AlignInputBits2Byte(BitI); // Skip the rest of the current byte
			DataLength             = ReadBits(BitI, 32);
			OnesComplimentOfLength = ReadBits(BitI, 32);
			if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != DataLength) {
				// Exit because there's an issue.
			}
			for (size_t Byte = 0; Byte < DataLength; Byte++) {
				DecodedData[Byte] = ReadBits(BitI, 8);
			}
		} else if (HuffmanCompressionType == 1) { // Static Huffman.
			uint8_t  Length   = (ReadBits(BitI, 8) - 254);
			uint16_t Distance = ReadBits(BitI, 5);
			
		} else if (HuffmanCompressionType == 2) { // Dynamic Huffman.
			Huff->Dynamic->Length     = ReadBits(BitI, 5) + 257;
			Huff->Dynamic->Distance   = ReadBits(BitI, 5) + 1;
			Huff->Dynamic->CodeLength = ReadBits(BitI, 4) + 4;
		} else { // Invalid.
				 // Reject the stream.
		}
		/*
		 if compressed with dynamic Huffman codes
		 read representation of code trees (see
		 subsection below)
		 loop (until end of block code recognized)
		 decode literal/length value from input stream
		 if value < 256
		 copy value (literal byte) to output stream
		 otherwise
		 if value = end of block (256)
		 break from loop
		 otherwise (value = 257..285)
		 decode distance from input stream
		 
		 move backwards distance bytes in the output
		 stream, and copy length bytes from this
		 position to the output stream.
		 end loop
		 while not last block
		 */
	}
	
	void ParseDeflate(BitInput *BitI) {
		uint8_t CompressionInfo    = ReadBits(BitI, 4); // 7 = LZ77 window size 32k
		uint8_t CompressionMethod  = ReadBits(BitI, 4); // 8 = DEFLATE
		uint8_t CheckCode          = ReadBits(BitI, 5); // for the previous 2 fields, MUST be multiple of 31
		bool    DictionaryPresent  = ReadBits(BitI, 1); // false
		uint8_t CompressionLevel   = ReadBits(BitI, 2); // Fixed Huffman
		if (DictionaryPresent == true) {
			uint16_t Dictionary   = ReadBits(BitI, 16);
		}
	}
	
	uint32_t GenerateAdler32(uint8_t *Data, size_t DataSize) {
		// Add all values up, then modulo it by 65521 for Sum1. byte bound.
		// Sum2 is Sum1 ran through the algorithm again.
		// Sum2 is stored before Sum1. Big Endian.
		
		/* My Implementation */
		uint32_t Adler  = 1;
		uint32_t Sum1   = Adler & 0xFFFF;
		uint32_t Sum2   = (Adler >> 16) & 0xFFFF;
		
		for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
			Sum1 += Data[Byte] % 65521;
			Sum2 += Sum1 % 65521;
		}
		return (Sum2 << 16) + Sum1;
	}
	
	bool VerifyAdler32(uint8_t *Data, size_t DataSize, uint32_t EmbeddedAdler32) {
		uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
		if (GeneratedAdler32 != EmbeddedAdler32) {
			return false;
		} else {
			return true;
		}
	}
	
#ifdef __cplusplus
}
#endif
