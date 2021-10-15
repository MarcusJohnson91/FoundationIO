#include "../Library/include/TestIO.h"
#include "../Library/include/TextIO/CommandLineIO.h"
#include "../Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum Switches {
        Input       = 0,
        Output      = 1,
        Help        = 2,
        NumSwitches = Help + 1,
    } Switches;
    
    int main(const int argc, const char *argv[]) {
        CommandLineIO *CLI = CommandLineIO_Init(NumSwitches);
        
        CommandLineIO_SetMinOptions(CLI, 2);

        uint64_t FakeArgC = 4;
        ImmutableStringSet_UTF8 FakeArgV = UTF8StringSet(UTF8String("--Input"), UTF8String("/Fake/Path/For/Testing.png"), UTF8String("--Output"), UTF8String("/Fake/Path/For/Testing.jpg"));
        /*
         Now we should test case conversion, and maybe option shortening and all kinds of other things
         */

        CommandLineIO_UTF8_ParseOptions(CLI, 4, FakeArgV);

        CommandLineIO_Deinit(CLI);
        
        return 0;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
