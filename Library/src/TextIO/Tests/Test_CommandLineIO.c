#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"
#include "../libFoundationIO/include/CommandLineIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum Switches {
        Input       = 0,
        Output      = 1,
        NumSwitches = 2,
    } Switches;
    
    int main(int argc, const char *argv[]) {
        CommandLineIO *CLI = CommandLineIO_Init(NumSwitches);
        
        CommandLineIO_SetMinOptions(CLI, 2);
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
