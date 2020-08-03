#include "../Library/include/TestIO.h"
#include "../Library/include/TextIO/CommandLineIO.h"
#include "../Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
