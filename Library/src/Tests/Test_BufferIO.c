#include "../../include/BufferIO.h"         /* Included for our declarations */

#include "../../include/AssertIO.h"         /* Included for Assertions */
#include "../../include/TestIO.h"           /* Included for testing */
#include "../../include/MathIO.h"
#include "../../include/CryptographyIO.h"

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

    bool Test_WriteReadBits(InsecurePRNG *Insecure) {
        bool TestPassed   = No;
        BitBuffer *BitB   = BitBuffer_Init(8);

        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize2, Field2); // BitB->Buffer[0] should = 0xB9
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize3, Field3);

        uint8_t NearNear1 = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize1);
        uint8_t NearNear2 = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize2);
        uint8_t NearNear3 = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize3);

        AssertIO(NearNear1 == Field1);
        AssertIO(NearNear2 == Field2);
        AssertIO(NearNear3 == Field3);

        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Left2Right, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Left2Right, FieldSize2, Field2);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Left2Right, FieldSize3, Field3);

        uint8_t NearFar1  = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize1);
        uint8_t NearFar2  = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize2);
        uint8_t NearFar3  = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize3);

        AssertIO(NearFar1 == Field1);
        AssertIO(NearFar2 == Field2);
        AssertIO(NearFar3 == Field3);

        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, FieldSize2, Field2);
        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, FieldSize3, Field3);

        uint8_t FarNear1  = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize1);
        uint8_t FarNear2  = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize2);
        uint8_t FarNear3  = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize3);

        AssertIO(FarNear1 == Field1);
        AssertIO(FarNear2 == Field2);
        AssertIO(FarNear3 == Field3);

        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, FieldSize1, Field1);
        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, FieldSize2, Field2);
        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, FieldSize3, Field3);

        uint8_t FarFar1   = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize1);
        uint8_t FarFar2   = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize2);
        uint8_t FarFar3   = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, FieldSize3);

        AssertIO(FarFar1 == Field1);
        AssertIO(FarFar2 == Field2);
        AssertIO(FarFar3 == Field3);

        return TestPassed;
    }
    
    bool Test_ReadWriteBitsNearNear(InsecurePRNG *Insecure) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);
        
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = InsecurePRNG_CreateInteger(Insecure, 6);
            int64_t    RandomInteger   = InsecurePRNG_CreateInteger(Insecure, NumBits2Extract);
            
            BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, NumBits2Extract);
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

    bool Test_ReadWriteBitsFarFar(InsecurePRNG *Insecure) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Write   = InsecurePRNG_CreateInteger(Insecure, 3); // 6
            int64_t    RandomInteger   = InsecurePRNG_CreateInteger(Insecure, NumBits2Write);
            /*
#define NumBits2Write 64
#define RandomInteger 0x0807060504030201
             */
            BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, NumBits2Write, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Write));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, NumBits2Write);
            AssertIO(ReadInteger == RandomInteger);
            BitBuffer_Erase(BitB, 0);
        }
        /*
#undef NumBits2Write
#undef RandomInteger
         */
        return TestPassed;
    }

    bool Test_ReadWriteBitsNearFar(InsecurePRNG *Insecure) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = InsecurePRNG_CreateInteger(Insecure, 3); // 6
            int64_t    RandomInteger   = InsecurePRNG_CreateInteger(Insecure, NumBits2Extract);

            BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, NumBits2Extract);
            AssertIO(ReadInteger == RandomInteger);
            BitBuffer_Erase(BitB, 0);
        }

        return TestPassed;
    }

    bool Test_ReadWriteBitsFarNear(InsecurePRNG *Insecure) {
        bool TestPassed                = Yes;

        BitBuffer *BitB                = BitBuffer_Init(8);

        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t    NumBits2Extract = InsecurePRNG_CreateInteger(Insecure, 3);
            int64_t    RandomInteger   = InsecurePRNG_CreateInteger(Insecure, NumBits2Extract); // 2^3 = 8, 8 possible values

            BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, NumBits2Extract, RandomInteger);
            BitBuffer_Seek(BitB, -(NumBits2Extract));
            int64_t ReadInteger        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, NumBits2Extract);
            AssertIO(ReadInteger == RandomInteger);
            BitBuffer_Erase(BitB, 0);
        }

        return TestPassed;
    }
    
    int main(const int argc, const char *argv[]) {
        TestIO_RunTests(BufferIOTests);

        InsecurePRNG *Insecure       = InsecurePRNG_Init(0);

        bool NearNearPassed          = Test_ReadWriteBitsNearNear(Insecure);
        bool FarFarPassed            = Test_ReadWriteBitsFarFar(Insecure);
        bool NearFarPassed           = Test_ReadWriteBitsNearFar(Insecure);
        bool FarNearPassed           = Test_ReadWriteBitsFarNear(Insecure);
        uint8_t AllTestsPassed       = (NearNearPassed + FarFarPassed + NearFarPassed + FarNearPassed) == 4;
        return EXIT_SUCCESS;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
