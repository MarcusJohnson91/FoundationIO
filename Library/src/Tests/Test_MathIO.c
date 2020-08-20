#include "../Library/include/TestIO.h"
#include "../Library/include/MathIO.h"
#include "../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void Test_CountDigits(void) {
        SecureRNG *Random      = SecureRNG_Init(8 * 1000000);
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t  NumBits   = SecureRNG_GenerateInteger(Random, 6);
            int64_t  Value     = SecureRNG_GenerateInteger(Random, NumBits);
            uint8_t  LogCeil   = Logarithm(10, -Value) - 1;
            uint8_t  NumDigits = NumDigitsInInteger(10, -Value);
            /*
             if (LogCeil != NumDigits) {
             //Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits %llu is incorrect"), NumBits);
             }
             */
        }
    }
    
    void Test_Logarithm(void) {
        SecureRNG *Random    = SecureRNG_Init(64 * 6 * 1000000);
        for (uint64_t Loop = 0ULL; Loop < 1000000; Loop++) {
            uint8_t  NumBits = SecureRNG_GenerateInteger(Random, 6);
            int64_t  Value   = SecureRNG_GenerateInteger(Random, NumBits);
            uint8_t  LogCeil = Logarithm(2, Value);
            if (LogCeil != NumBits) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits %llu is incorrect"), NumBits);
            }
        }
    }
    
    void Test_MinMax(void) {
        SecureRNG *Random    = SecureRNG_Init(4096);
        
        for (uint16_t Loop = 0; Loop < 512; Loop++) {
            uint8_t NumBits  = SecureRNG_GenerateInteger(Random, 8);
            int64_t Integer1 = SecureRNG_GenerateInteger(Random, NumBits);
            int64_t Integer2 = SecureRNG_GenerateInteger(Random, NumBits);
            
            int64_t Minimum1 = Minimum(Integer1, Integer2);
            int64_t Maximum1 = Maximum(Integer1, Integer2);
            
            if (Minimum1 > Maximum1) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Minimum/Maximum failed!"));
            }
        }
    }
    
    void Test_Decimals() {
        SecureRNG *Random  = SecureRNG_Init(4096);
        
        double  Decimal    = SecureRNG_GenerateDecimal(Random);
        double  Ceiled     = CeilD(Decimal);
        double  Floored    = FloorD(Decimal);
    }
    
    int main() {
        Test_CountDigits();
        //Test_Decimals();
        //Test_MinMax();
        return EXIT_SUCCESS;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
