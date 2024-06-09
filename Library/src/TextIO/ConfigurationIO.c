#include "../../include/TextIO/ConfigurationIO.h"    /* Included for our declarations */
#include "../../include/AssertIO.h" /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct TOMLKeyValue {
        UTF8 *Key;
        UTF8 *Value;
    } TOMLKeyValue;
    
    /*TOML supports stuff I probably won't need, like dates and times and stuff like that, but it's a standard and who knows what the future holds */
    
    /*
    so, basically TOML supports a bunch of kinds of data, and with that I need to support a whole bunch of key value pairs that can and probably will be nested, it also supports arrays and tables, Arrays = [], Tables = {}, Section Names also start with [], but they have to be at the front of the line and Arrays have to be after an equal sign.
    */
    
    /* So, we can actually probably define a lot of our tests in TOML... that's useful. */
    
    
    
    size_t TOML_GetNumTables(UTF8 *TOML) {
        AssertIO(TOML != NULL);
        size_t NumTables = 0;
        // Loop over the whole document, counting the number of [ followed by a non-whitespace, non-digit character
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */