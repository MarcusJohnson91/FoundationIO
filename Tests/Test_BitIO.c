#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"

#include "../libFoundationIO/include/BitIO.h"
#include "../libFoundationIO/include/Macros.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    int main(int argc, const char *argv[]) {
        // Well, we need to test all of the BitIO components, so let's set up a BitInput and BitOutput.
        
        // for testing sockets, we can use the loopback mechanism so only the test runner needs to be available.
        BitInput  *BitI = BitInput_Init();
        BitOutput *BitO = BitOutput_Init();
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
