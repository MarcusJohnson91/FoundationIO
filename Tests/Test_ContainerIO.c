#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"
#include "../libFoundationIO/include/ContainerIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    int main(int argc, const char *argv[]) {
        ImageContainer *Image = ImageContainer_Init(ImageType_Integer8, ImageMask_Red | ImageMask_Green | ImageMask_Blue, 3840, 2160);
        
        ImageHistogram *Hist  = ImageHistogram_Init(Image);
        
        AudioContainer *Audio = AudioContainer_Init(AudioType_Unsigned | AudioType_Integer32, AudioMask_FrontLeft | AudioMask_FrontRight, 48000, 4096);
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
