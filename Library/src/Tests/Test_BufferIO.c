#include "../Library/include/BitIO.h"
#include "../Library/include/MathIO.h"
#include "../Library/include/TestIO.h"
#include "../Library/include/UnicodeIO/LogIO.h"

#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
extern "C" {
#endif
    
    bool Test_WriteReadBits(void) {
        bool TestPassed              = Yes;
        BitBuffer *BitB              = BitBuffer_Init(8);
        Entropy   *Random            = Entropy_Init(8000000);
        
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = Entropy_GenerateInteger(Random, 3); // 6
            int64_t    RandomInteger   = Entropy_GenerateInteger(Random, NumBits2Extract);
            uint8_t    ByteOrder       = Entropy_GenerateInteger(Random, 1) + 1;
            uint8_t    BitOrder        = Entropy_GenerateInteger(Random, 1) + 1;
            
            BitBuffer_WriteBits(BitB, ByteOrder, BitOrder, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder, BitOrder, NumBits2Extract);
            BitBuffer_Erase(BitB, 0); // Clear the BitBuffer in between each run just to be sure.
        }
        
        return TestPassed;
    }
    
    int main(int argc, const char *argv[]) {
        // Well, we need to test all of the BitIO components, so let's set up a BitInput and BitOutput.
        
        // for testing sockets, we can use the loopback mechanism so only the test runner needs to be available.
        
        /*
         Generate the number of times to loop
         
         For each loop generate a 1 to 64 bit number to write to the BitBuffer, and read it back.
         
         make sure they match, if they don't print out the BitOffset, NumBits, the input and output values.
         */
        /*
         Entropy *Random         = Entropy_Init(4194256); // 64 bits * 65535 entries, + 16 bits for the loop couter
         BitBuffer *BitB         = BitBuffer_Init(8);
         uint16_t Times2Loop     = Entropy_GenerateIntegerInRange(Random, 1, 0x7FFF);
         for (uint16_t Loop = 0; Loop < Times2Loop; Loop++) {
         uint64_t Value      = Entropy_GenerateIntegerInRange(Random, 1, 0x7FFFFFFFFFFFFFFF);
         uint8_t  NumBits    = Logarithm(2, Value);
         BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, NumBits, Value);
         BitBuffer_Seek(BitB, -NumBits);
         uint64_t Decoded    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, NumBits);
         if (Decoded != Value) {
         fprintf(stderr, "TEST FAILED: Decoded %llu does not match Value %llu, Offset = %llu, NumBits = %llu\n", Decoded, Value, BitBuffer_GetPosition(BitB), BitBuffer_GetSize(BitB));
         } else {
         fprintf(stdout, "TEST PASSED: Decoded %llu matches Value %llu", Decoded, Value);
         }
         }
         */
        //Test_WriteBits();
        return Test_WriteReadBits();
    }
    
#if (FoundationIOLanguage == FoundationIOLanguageIsCXX)
}
#endif
