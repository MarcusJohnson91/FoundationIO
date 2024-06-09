#include "../../include/TextIO/SliceIO.h"    /* Included for our declarations */

#include "../../include/AssertIO.h"        /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    Slice Slice_Init(size_t Header, size_t Footer) {
        AssertIO(Header != nullptr);
        AssertIO(Footer != nullptr);
        AssertIO(Header < Footer);
        Slice = {};
        Slice.Header = Header;
        Slice.Footer = Footer;
        return Slice;
    }
    
    SliceSet SliceSet_Init(size_t NumSlices) {
        AssertIO(NumSlices > 0);
        SliceSet = {};
        SliceSet.Slices = calloc(NumSlices, sizeof(Slice));
        AssertIO(SliceSet.Slices != nullptr);
        SliceSet.NumSlices = NumSlices;
        return SliceSet;
    }
    
    size_t SliceSet_Sum(SliceSet Set) {
        AssertIO(Set.Slices != nullptr);
        size_t Sum = 0;
        for (size_t Slice = 0; Slice < Set.NumSlices; Slice++) {
            Sum += (Set.Slices[Slice].Footer - Set.Slices[Slice].Header); 
        }
        return Sum;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
