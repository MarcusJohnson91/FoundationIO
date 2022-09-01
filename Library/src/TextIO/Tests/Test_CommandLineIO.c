#include "../../../include/TestIO.h"                 /* Included for testing */
#include "../../../include/TextIO/CommandLineIO.h"   /* Included for our declarations */

#include "../../../include/TextIO/StringIO.h"        /* Included for StringIO */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#define Suite_CommandLineIO_RegisteredTests TestIO_NULLCase
#define Suite_CommandLineIO_NumTests 0
    
    typedef enum Switches : uint8_t {
        Input       = 0,
        Output      = 1,
        Help        = 2,
        NumSwitches = Help + 1,
    } Switches;

    void CommandLineIO_Fixture_Init(TestIO_Suite *Suite) {
        Suite->FixtureStruct             = CommandLineIO_Init(NumSwitches);
        CommandLineIO_SetHelpOption(Suite->FixtureStruct, Help);
        CommandLineIO_SetMinOptions(Suite->FixtureStruct, 2);
    }

    void CommandLineIO_Fixture_Deinit(TestIO_Suite *Suite) {
        CommandLineIO_Deinit(Suite->FixtureStruct);
        Suite->FixtureStruct = NULL;
        free(Suite->Init);
        free(Suite->Deinit);
        free(Suite->Name);
        free(Suite->Tests);
        Suite->NumTests = 0;
    }

    TestIO_RegisterSuiteWithFixtures(CommandLineIO, CommandLineIO_Fixture_Init, CommandLineIO_Fixture_Deinit);

    void Test_DisplayProgress(TestIO_Suite *Suite) {
        CommandLineIO *CLI = Suite->FixtureStruct;
        CommandLineIO_ShowProgress(CLI, 2, UTF32StringSet(UTF32String("Wat"), UTF32String("Blah")), 1, 2);
    }

    TestIO_RegisterTest(Suite_CommandLineIO_RegisteredTests, Suite_CommandLineIO_NumTests, Test_DisplayProgress, TestState_Enabled, Outcome_Passed);

    /*
     We need to know which suite macro to redefine,
     we need to know which counter macro to redefine,
     we need to have the function pointer to call.
     we need to know if the test is enabled or disabled
     we need to know the expected outcome.
     and a name variable could be useful, but we'll just generate that in the macro if possible.
     */

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
