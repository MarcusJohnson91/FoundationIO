#include "../Library/include/BufferIO.h"
#include "../Library/include/MathIO.h"
#include "../Library/include/TestIO.h"
#include "../Library/include/UnicodeIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    bool Test_WriteReadBits(void) {
        bool TestPassed                = Yes;
        BitBuffer *BitB                = BitBuffer_Init(8);
        SecureRNG   *Random            = SecureRNG_Init(8000000);
        
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 3); // 6
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract);
            uint8_t    ByteOrder       = SecureRNG_GenerateInteger(Random, 1) + 1;
            uint8_t    BitOrder        = SecureRNG_GenerateInteger(Random, 1) + 1;
            
            BitBuffer_WriteBits(BitB, ByteOrder, BitOrder, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder, BitOrder, NumBits2Extract);
            BitBuffer_Erase(BitB, 0); // Clear the BitBuffer in between each run just to be sure.
        }
        
        return TestPassed;
    }
    
    int main(int argc, const char *argv[]) {
        return Test_WriteReadBits();
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
