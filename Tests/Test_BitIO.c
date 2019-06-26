#include "../libFoundationIO/include/BitIO.h"

#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"
#include "../libFoundationIO/include/Math.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    int main(int argc, const char *argv[]) {
        // Well, we need to test all of the BitIO components, so let's set up a BitInput and BitOutput.
        
        // for testing sockets, we can use the loopback mechanism so only the test runner needs to be available.
        
        /*
         Generate the number of times to loop
         
         For each loop generate a 1 to 64 bit number to write to the BitBuffer, and read it back.
         
         make sure they match, if they don't print out the BitOffset, NumBits, the input and output values.
         */
        Entropy *Random         = Entropy_Init(4194256); // 64 bits * 65535 entries, + 16 bits for the loop couter
        BitBuffer *BitB         = BitBuffer_Init(8);
        uint16_t Times2Loop     = Entropy_GenerateIntegerInRange(Random, 1, 0x7FFF);
        for (uint16_t Loop = 0; Loop < Times2Loop; Loop++) {
            uint64_t Value      = Entropy_GenerateIntegerInRange(Random, 1, 0x7FFFFFFFFFFFFFFF);
            uint8_t  NumBits    = Logarithm(2, Value);
            BitBuffer_WriteBits(BitB, MSByteFirst, MSBitFirst, NumBits, Value);
            BitBuffer_Seek(BitB, -NumBits);
            uint64_t Decoded    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, NumBits);
            if (Decoded != Value) {
                fprintf(stderr, "TEST FAILED: Decoded %llu does not match Value %llu, Offset = %llu, NumBits = %llu\n", Decoded, Value, BitBuffer_GetPosition(BitB), BitBuffer_GetSize(BitB));
            } else {
                fprintf(stdout, "TEST PASSED: Decoded %llu matches Value %llu", Decoded, Value);
            }
        }
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
