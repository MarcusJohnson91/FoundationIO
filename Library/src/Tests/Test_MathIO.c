#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"
#include "../libFoundationIO/include/Math.h"
#include "../libFoundationIO/include/CryptographyIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void Test_Logarithm(void) {
        int64_t  Value   = -4096;
        uint8_t  NumBits = Logarithm(2, Value);
        if (NumBits != 13) {
            Log(Log_DEBUG, __func__, U8("NumBits %llu is incorrect"), NumBits);
        }
    }
    
    void Test_MinMax(void) {
        Entropy *Random      = Entropy_Init(4096);
        
        for (uint16_t Loop = 0; Loop < 512; Loop++) {
            uint8_t NumBits  = Entropy_GenerateIntegerInRange(Random, 8);
            int64_t Integer1 = Entropy_GenerateIntegerInRange(Random, NumBits);
            int64_t Integer2 = Entropy_GenerateIntegerInRange(Random, NumBits);
            
            int64_t Minimum1 = Minimum(Integer1, Integer2);
            int64_t Maximum1 = Maximum(Integer1, Integer2);
            
            if (Minimum1 > Maximum1) {
                Log(Log_DEBUG, __func__, U8("Minimum/Maximum failed!"));
            } else {
                Log(Log_DEBUG, __func__, U8("Min/Max FAILED"));
                //Log(Log_DEBUG, __func__, U8("Minimum: %d is smaller than Maximum %d, Test Passed"), Minimum1, Maximum1);
            }
        }
    }
    
    void Test_Decimals() {
        Entropy *Random  = Entropy_Init(4096);
        
        double  Decimal  = Entropy_GenerateDecimal(Random);
        double  Ceiled   = CeilD(Decimal);
        double  Floored  = FloorD(Decimal);
        printf("Ceiled: %F\n", Ceiled);
        printf("Floored: %F\n", Floored);
    }
    
    int main() {
        Test_Logarithm();
        //Test_Decimals();
        //Test_MinMax();
        return EXIT_SUCCESS;
    }
    
#ifdef __cplusplus
}
#endif
