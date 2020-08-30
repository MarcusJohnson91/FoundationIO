#include "../Library/include/TestIO.h"
#include "../Library/include/BufferIO.h"
#include "../Library/include/CryptographyIO.h"
#include "../Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif


    
    bool Test_ReadWriteBitsNearNear(BitBuffer *BitB, SecureRNG *Random) {
        bool TestPassed                = Yes;
        
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 3); // 6
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract);
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, NumBits2Extract);
            if (ReadInteger != RandomInteger) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ReadInteger: %llu does not match WrittenInteger: %llu"), ReadInteger, RandomInteger);
            }
            BitBuffer_Erase(BitB, 0);
        }
        
        return TestPassed;
    }

    bool Test_ReadWriteBitsFarFar(BitBuffer *BitB, SecureRNG *Random) {
        bool TestPassed                = Yes;

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 3); // 6
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract);

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, NumBits2Extract);
            if (ReadInteger != RandomInteger) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ReadInteger: %llu does not match WrittenInteger: %llu"), ReadInteger, RandomInteger);
            }
            BitBuffer_Erase(BitB, 0);
        }

        return TestPassed;
    }

    bool Test_ReadWriteBitsNearFar(BitBuffer *BitB, SecureRNG *Random) {
        bool TestPassed                = Yes;

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 3); // 6
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract);

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, NumBits2Extract);
            if (ReadInteger != RandomInteger) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ReadInteger: %llu does not match WrittenInteger: %llu"), ReadInteger, RandomInteger);
            }
            BitBuffer_Erase(BitB, 0);
        }

        return TestPassed;
    }

    bool Test_ReadWriteBitsFarNear(BitBuffer *BitB, SecureRNG *Random) {
        bool TestPassed                = Yes;

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 3);
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract); // 2^3 = 8, 8 possible values

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, NumBits2Extract);
            if (ReadInteger != RandomInteger) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ReadInteger: %llu does not match WrittenInteger: %llu"), ReadInteger, RandomInteger);
            }
            BitBuffer_Erase(BitB, 0);
        }

        return TestPassed;
    }
    
    int main(int argc, const char *argv[]) {
        BitBuffer *BitB              = BitBuffer_Init(8);
        SecureRNG *Random            = SecureRNG_Init(8000000);
        bool NearNearPassed          = Test_ReadWriteBitsNearNear(BitB, Random);
        bool FarFarPassed            = Test_ReadWriteBitsFarFar(BitB, Random);
        bool NearFarPassed           = Test_ReadWriteBitsNearFar(BitB, Random);
        bool FarNearPassed           = Test_ReadWriteBitsFarNear(BitB, Random);
        return (NearNearPassed + FarFarPassed + NearFarPassed + FarNearPassed) / 4;

    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
