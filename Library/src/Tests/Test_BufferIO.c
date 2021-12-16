#include "../Library/include/TestIO.h"
#include "../Library/include/BufferIO.h"
#include "../Library/include/CryptographyIO.h"
#include "../Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void BitBuffer_Fixture_Init(TestIO_Suite *Suite) { // , PlatformIO_Unused(TestIO_Enviroment *Enviroment)
        Suite->FixtureStruct = BitBuffer_Init(64);
        uint8_t   *Array     = BitBuffer_GetArray(Suite->FixtureStruct);
        BufferIO_MemorySet8(Array, 0xFE, 64);
    }

    void BitBuffer_Fixture_Deinit(TestIO_Suite *Suite) { // , PlatformIO_Unused(TestIO_Enviroment)
        BitBuffer_Deinit(Suite->FixtureStruct);
        Suite->FixtureStruct = NULL;
    }

    TestIO_RegisterSuiteWithFixtures(BufferIOTests, BitBuffer_Fixture_Init, BitBuffer_Fixture_Deinit);

     /*
      Write 3 fields, all with a leading and trailing one, and a variable number of 0's in between.

     0b101
     0b110011
     0b1000001
     16 bits total.

     0b1011100111000001

     Then write these in each bbyte and bit order combinartion.
     */

#define FieldSize1 3
#define Field1     0x5

#define FieldSize2 6
#define Field2     0x33

#define FieldSize3 7
#define Field3     0x41

    bool Test_WriteReadBits(SecureRNG *Secure) {
        bool TestPassed   = No;
        BitBuffer *BitB   = BitBuffer_Init(8);

        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize2, Field2); // BitB->Buffer[0] should = 0xB9
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize3, Field3);

        uint8_t NearNear1 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize1);
        uint8_t NearNear2 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize2);
        uint8_t NearNear3 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize3);

        if (NearNear1 == Field1 && NearNear2 == Field2 && NearNear3 == Field3) {
            TestPassed    = Yes;
        } else if (NearNear1 != Field1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NearNear1 is incorrect: %d"), NearNear1);
        } else if (NearNear2 != Field2) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NearNear2 is incorrect: %d"), NearNear2);
        } else if (NearNear3 != Field3) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NearNear3 is incorrect: %d"), NearNear3);
        }

        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, FieldSize2, Field2);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, FieldSize3, Field3);

        uint8_t NearFar1  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize1);
        uint8_t NearFar2  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize2);
        uint8_t NearFar3  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize3);

        if (NearFar1 == Field1 && NearFar2 == Field2 && NearFar3 == Field3) {
            TestPassed    = Yes;
        } else if (NearFar1 != Field1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NearFar1 is incorrect: %d"), NearFar1);
        } else if (NearFar2 != Field2) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NearFar2 is incorrect: %d"), NearFar2);
        } else if (NearFar3 != Field3) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NearFar3 is incorrect: %d"), NearFar3);
        }

        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, FieldSize2, Field2);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, FieldSize3, Field3);

        uint8_t FarNear1  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize1);
        uint8_t FarNear2  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize2);
        uint8_t FarNear3  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize3);

        if (FarNear1 == Field1 && FarNear2 == Field2 && NearFar3 == FarNear3) {
            TestPassed    = Yes;
        } else if (FarNear1 != Field1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FarNear1 is incorrect: %d"), FarNear1);
        } else if (FarNear2 != Field2) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FarNear2 is incorrect: %d"), FarNear2);
        } else if (FarNear3 != Field3) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FarNear3 is incorrect: %d"), FarNear3);
        }

        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FieldSize2, Field2);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FieldSize3, Field3);

        uint8_t FarFar1   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize1);
        uint8_t FarFar2   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize2);
        uint8_t FarFar3   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, FieldSize3);

        if (FarFar1 == Field1 && FarFar2 == Field2 && FarFar3 == Field3) {
            TestPassed    = Yes;
        } else if (FarFar1 != Field1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FarFar1 is incorrect: %d"), FarFar1);
        } else if (FarFar2 != Field2) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FarFar2 is incorrect: %d"), FarFar2);
        } else if (FarFar3 != Field3) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("FarFar3 is incorrect: %d"), FarFar3);
        }

        return TestPassed;
    }
    
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
            .State      = TestState_Enabled,
            .Expectation = Outcome_Passed,
        };
        return TestPassed;
    }

    bool Test_ReadWriteBitsFarFar(SecureRNG *Random) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Write   = SecureRNG_GenerateInteger(Random, 3); // 6
            int64_t    RandomInteger   = SecureRNG_GenerateInteger(Random, NumBits2Write);
            /*
#define NumBits2Write 64
#define RandomInteger 0x0807060504030201
             */
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, NumBits2Write, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Write));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, NumBits2Write);
            if (ReadInteger != RandomInteger) {
                TestPassed = No;
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ReadInteger: %llu does not match WrittenInteger: %llu"), ReadInteger, RandomInteger);
            }
            BitBuffer_Erase(BitB, 0);
        }
        /*
#undef NumBits2Write
#undef RandomInteger
         */
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

        bool NearNearPassed          = Test_ReadWriteBitsNearNear(Random);
        bool FarFarPassed            = Test_ReadWriteBitsFarFar(Random);
        bool NearFarPassed           = Test_ReadWriteBitsNearFar(Random);
        bool FarNearPassed           = Test_ReadWriteBitsFarNear(Random);
        uint8_t AllTestsPassed       = (NearNearPassed + FarFarPassed + NearFarPassed + FarNearPassed) == 4;
        return AllTestsPassed == true ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
