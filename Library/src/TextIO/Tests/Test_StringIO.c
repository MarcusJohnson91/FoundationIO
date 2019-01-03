#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"

#include "../libFoundationIO/include/StringIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
    bool UTF8_BasicTest(void) {
        bool      TestPassed     = true;
        UTF8      TestString8[]  = U8("Size: 7");
        UTF32    *TestString32   = UTF8_Decode(TestString8);
        uint64_t  TestStringSize = UTF32_GetStringSizeInCodePoints(TestString32);
        if (TestStringSize != 7) {
            TestPassed           = false;
            Log(Log_TEST, __func__, U8("String \"%s\" is supposed to be 7 CodePoints long, but is actually %llu"), TestString8, TestStringSize);
        }
        return TestPassed;
    }
    
    int main(int argc, const char *argv[]) {
        bool TestSuitePassed = true;
        
        return TestSuitePassed;
    }
    
#ifdef __cplusplus
}
#endif