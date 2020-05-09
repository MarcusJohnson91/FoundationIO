#include "../Library/include/ContainerIO.h"
#include "../Library/include/TestIO.h"
#include "../Library/include/UnicodeIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    int main(int argc, const char *argv[]) {
        ImageContainer *Image = ImageContainer_Init(ImageType_Integer8, ImageMask_Red | ImageMask_Green | ImageMask_Blue, 3840, 2160);
        
        ImageHistogram *Hist  = ImageHistogram_Init(Image);
        
        Audio2DContainer *Audio = Audio2DContainer_Init(AudioType_Unsigned | AudioType_Integer32, 2, AudioMask_FrontLeft | AudioMask_FrontRight, 48000, 4096);
        return 0;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
