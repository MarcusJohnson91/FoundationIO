#include "../Library/include/TestIO.h"
#include "../Library/include/MathIO.h"
#include "../Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    TestSuite *MathIO;
    
    /* 3 levels of testing:
     Module/Component, Data Structure/Function, Test Case
     Individual Tests are registered to a Data Structure/Function Suite
     and there may be multiple suites that all need to be tested.
     
     So, just write a macro that will create
     */
    
    /*
     
     
     Solution: transfers_1[0] = (nrf_twi_mngr_transfer_t)NRF_TWI_MNGR_WRITE(//Your arguments);
     
     So, for each Test Case, we need to register it with a suite.
     
     the suite needs to know all of the functions in the test suite, each test's enabled/disabled status, the correct value for comparison, and is that it?
     */
    
    
    
    bool Test_Exponentiate(InsecurePRNG *Insecure) { // Even, Odd, Positive, Negative
        TestIO_Register(MathIO, Test_Exponentiate, TestState_Enabled, Outcome_Passed);
        bool Passed = No;
        int64_t Even = Exponentiate(2, 2); // 4
        int64_t Odd  = Exponentiate(2, 3); // 8
        int64_t NegE = Exponentiate(2, -2); // .25
        int64_t NegO = Exponentiate(2, -1); // .5
        if (Even == 4 && Odd == 8) {
            Passed = Yes;
        }
        return Passed;
    }
    
    void Test_CountDigits(InsecurePRNG *Insecure) {
        uint8_t  NumBits   = InsecurePRNG_CreateInteger(Insecure, 6);
        int64_t  Value     = InsecurePRNG_CreateInteger(Insecure, NumBits);
        uint8_t  LogCeil   = Logarithm(10, -Value) - 1;
        uint8_t  NumDigits = NumDigitsInInteger(10, -Value);
        
        if (LogCeil != NumDigits) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits %u is incorrect"), NumBits);
        }
    }
    
    void Test_Logarithm(InsecurePRNG *Insecure) {
        uint8_t  NumBits = InsecurePRNG_CreateInteger(Insecure, 6);
        int64_t  Value   = InsecurePRNG_CreateInteger(Insecure, NumBits);
        uint8_t  LogCeil = Logarithm(2, Value);
        if (LogCeil != NumBits) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumBits %hhu is incorrect"), NumBits);
        }
    }
    
    void Test_MinMax(InsecurePRNG *Insecure) {
        uint8_t NumBits  = InsecurePRNG_CreateInteger(Insecure, 8);
        int64_t Integer1 = InsecurePRNG_CreateInteger(Insecure, NumBits);
        int64_t Integer2 = InsecurePRNG_CreateInteger(Insecure, NumBits);
        
        int64_t Minimum1 = Minimum(Integer1, Integer2);
        int64_t Maximum1 = Maximum(Integer1, Integer2);
        
        if (Minimum1 > Maximum1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Minimum/Maximum failed!"));
        }
    }
    
    void Test_Decimals(InsecurePRNG *Insecure) {
        double  Decimal    = InsecurePRNG_CreateDecimal(Insecure);
        int32_t Ceiled     = CeilD(Decimal);
        int32_t Floored    = FloorD(Decimal);
    }
    
    int main(void) {
        InsecurePRNG *Insecure = InsecurePRNG_Init(0);
        TestIO_RunTests(MathIO);
        //Test_Exponentiate(Insecure);
        //Test_CountDigits(Insecure);
        //Test_Decimals();
        //Test_MinMax();
        return EXIT_SUCCESS;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
