#include "../../include/TextIO/YarnIO.h"    /* Included for our declarations */
#include "../../include/TextIO/SliceIO.h" /* Included for Slices and SliceSets */
#include "../../include/TextIO/StringSet.h" /* Included for StringSets */
#include "../../include/AssertIO.h" /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct Yarn8 {
        char8_t      *Original;
        SliceSet      Slices;
        StringSet8   *Replacements;
    } Yarn8;
    
    typedef struct Yarn16 {
        char16_t     *Original;
        SliceSet      Slices;
        StringSet8   *Replacements;
    } Yarn16;
    
    typedef struct Yarn32 {
        char32_t     *Original;
        SliceSet      Slices;
        StringSet8   *Replacements;
    } Yarn32;
    
    Yarn8 *Yarn8_Init(UTF8 *Original, size_t NumReplacements, TextIO_Slice *Slices, UTF8 **Replacements) {
        // For now we're not gonna go HAM and make the whole thing be built at once, though we should know the number of replacements, so yeah we will.
        AssertIO(Original != nullptr);
        AssertIO(NumReplacements >= 1);
        AssertIO(Slices != nullptr);
        AssertIO(Replacements != nullptr);
        AssertIO(NumReplacements == StringSet8_GetSize(Replacements));
        
        Yarn8 *Yarn = calloc(1, sizeof(Yarn8));
        Yarn.Original = Original;
        Yarn.Slices = SliceSet_Init(NumReplacements);
        Yarn.Replacements = Replacements;
        return Yarn;
    }
    
    Yarn16 *Yarn16_Init(UTF16 *Original, size_t NumReplacements, TextIO_Slice *Slices, UTF16 **Replacements) {
        // For now we're not gonna go HAM and make the whole thing be built at once, though we should know the number of replacements, so yeah we will.
        AssertIO(Original != nullptr);
        AssertIO(NumReplacements >= 1);
        AssertIO(Slices != nullptr);
        AssertIO(Replacements != nullptr);
        AssertIO(NumReplacements == StringSet8_GetSize(Replacements));
        
        Yarn16 *Yarn = calloc(1, sizeof(Yarn16));
        Yarn.Original = Original;
        Yarn.Slices = SliceSet_Init(NumReplacements);
        Yarn.Replacements = Replacements;
        return Yarn;
    }
    
    Yarn32 *Yarn32_Init(UTF32 *Original, size_t NumReplacements, TextIO_Slice *Slices, UTF32 **Replacements) {
        // For now we're not gonna go HAM and make the whole thing be built at once, though we should know the number of replacements, so yeah we will.
        AssertIO(Original != nullptr);
        AssertIO(NumReplacements >= 1);
        AssertIO(Slices != nullptr);
        AssertIO(Replacements != nullptr);
        AssertIO(NumReplacements == StringSet8_GetSize(Replacements));
        
        Yarn32 *Yarn = calloc(1, sizeof(Yarn32));
        Yarn.Original = Original;
        Yarn.Slices = SliceSet_Init(NumReplacements);
        Yarn.Replacements = Replacements;
        return Yarn;
    }
    
    bool Yarn8_Compare2String(Yarn8 *Yarn, UTF8 *String) {
        AssertIO(Yarn != nullptr);
        AssertIO(String != nullptr);
    }
    
    bool Yarn16_Compare2String(Yarn16 *Yarn, UTF16 *String) {
        AssertIO(Yarn != nullptr);
        AssertIO(String != nullptr);
    }
    
    bool Yarn32_Compare2String(Yarn32 *Yarn, UTF32 *String) {
        AssertIO(Yarn != nullptr);
        AssertIO(String != nullptr);
    }
    
    UTF8 *Yarn8_Knit(Yarn8 *Yarn) {
        AssertIO(Yarn != nullptr);
        // So find the number of replacement strings, their sizes, and the size of the Spans, and do the replacements on a freshly allocated string; most of this code is already in FormatIO.
        // We also need to be able to compare a Yarn to a regular string as if the replacements had been done.
        size_t KnittedSize = UTF8_GetSizeInCodeUnits(Yarn.Original);
        size_t ReplacementsSize = StringSet8_GetSizeInCodeUnits(Yarn.Replacements);
        // Sum the distance between each slice, we can honestly add our own damn SliceIO header and get it overwith.
    }
    
    UTF16 *Yarn16_Knit(Yarn16 *Yarn) {
        AssertIO(Yarn != nullptr);
    }
    
    UTF32 *Yarn32_Knit(Yarn32 *Yarn) {
        AssertIO(Yarn != nullptr);
    }
    
    

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */