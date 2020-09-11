#include "../Library/include/TestIO.h"
#include "../Library/include/BufferIO.h"
#include "../Library/include/CryptographyIO.h"
#include "../Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    TestSuite *BufferIOTests;
    
    bool Test_ReadWriteBitsNearNear(SecureRNG *Random) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);
        
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 6);
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract);
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, NumBits2Extract);
            if (ReadInteger != RandomInteger) {
                TestPassed = No;
            }
            BitBuffer_Erase(BitB, 0);
        }

        TestCase ReadWriteBitsNN = {
            .Function       = Test_ReadWriteBitsNearNear,
            .TestState      = TestState_Enabled,
            .FunctionName   = PlatformIO_FunctionName,
            .ExpectedResult = ExpectedResult_Passed,
        };



        //BufferIOTests->Tests[0] = ReadWriteBitsNN;
        
        return TestPassed;
    }

    bool Test_ReadWriteBitsFarFar(SecureRNG *Random) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            //uint8_t    NumBits2Extract = SecureRNG_GenerateInteger(Random, 3); // 6
            //int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Extract);
#define NumBits2Write 64
#define RandomInteger 0x0807060504030201

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, NumBits2Write, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Write));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, NumBits2Write);
            if (ReadInteger != RandomInteger) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ReadInteger: %llu does not match WrittenInteger: %llu"), ReadInteger, RandomInteger);
            }
            BitBuffer_Erase(BitB, 0);
        }
#undef NumBits2Write
#undef RandomInteger

        return TestPassed;
    }

    bool Test_ReadWriteBitsNearFar(SecureRNG *Random) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

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

    bool Test_ReadWriteBitsFarNear(SecureRNG *Random) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

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
    
    int main(const int argc, const char *argv[]) {
        TestIO_RunTests(BufferIOTests);

        SecureRNG *Random            = SecureRNG_Init(8000000);
        //bool NearNearPassed          = Test_ReadWriteBitsNearNear(Random);
        bool FarFarPassed            = Test_ReadWriteBitsFarFar(Random);
        bool NearFarPassed           = Test_ReadWriteBitsNearFar(Random);
        bool FarNearPassed           = Test_ReadWriteBitsFarNear(Random);
        return (FarFarPassed + NearFarPassed + FarNearPassed) /3;

    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
