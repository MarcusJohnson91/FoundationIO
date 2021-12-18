#include "../../include/TextIO/TextIOTypes.h"          /* Included for our declarations */

#include "../../include/TextIO/Private/TextIOTables.h" /* Included for the tables */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    TextIO_Slice TextIO_Slice_Init(size_t StartInCodeUnits, size_t EndInCodeUnits) {
        TextIO_Slice Slice;
        Slice.StartInCodeUnits = StartInCodeUnits;
        Slice.EndInCodeUnits   = EndInCodeUnits;
        return Slice;
    }

    size_t TextIO_Slice_GetStartInCodeUnits(TextIO_Slice Slice) {
        return Slice.StartInCodeUnits;
    }

    size_t TextIO_Slice_GetEndInCodeUnits(TextIO_Slice Slice) {
        return Slice.EndInCodeUnits;
    }
  

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
