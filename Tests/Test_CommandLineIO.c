#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"
#include "../libFoundationIO/include/CommandLineIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum Switches {
        NumSwitches = 1,
    } Switches;
    
    int main(int argc, const char *argv[]) {
        CommandLineIO *CLI = CommandLineIO_Init(NumSwitches);
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
