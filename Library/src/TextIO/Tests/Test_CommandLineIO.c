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

    void *CommandLineIO_FixtureInit(PlatformIO_Unused(TestIO_Enviroment*) Enviroment) {
        CommandLineIO           *CLI     = CommandLineIO_Init(NumSwitches);
        CommandLineIO_SetHelpOption(CLI, Help);
        CommandLineIO_SetMinOptions(CLI, 2);

    TestIO_RegisterSuiteWithFixtures(CommandLineIO, CommandLineIO_FixtureInit, CommandLineIO_Deinit);

    static const TestIO_Suite *SuiteSet[] = {
//#repeat(TestIO_Internal_SuiteCount, TestIO_SuiteSet_WriteName)
    };
    
    int main(const int argc, const char *argv[]) {
        CommandLineIO *CLI = NULL; // Get TestIO to compile, gonna have to get this from the Fixture; So in TestIO_Suite we need a variable to contain Fixture data
        CommandLineIO_UTF8_ParseOptions(CLI, 4, FakeArgV);
        return 0;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
