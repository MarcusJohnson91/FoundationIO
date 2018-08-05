#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */
#include "../include/Math.h"           /* Included for Absolute */
#include "../include/BitIO.h"          /* Included for BitBuffer */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct AudioContainer {
        union Samples {
            uint32_t     **UInteger32;
            int32_t      **SInteger32;
            uint16_t     **UInteger16;
            int16_t      **SInteger16;
            uint8_t      **UInteger8;
            int8_t       **SInteger8;
        } Samples;
        Audio_ChannelMask *ChannelMask; // Internal
        uint64_t           NumSamples;
        uint8_t            BitDepth;
        uint8_t            NumChannels;
        bool               IsUnsigned;
    } AudioContainer;
    
    typedef struct ImageContainer {
        union Pixels {
            uint16_t   ****UInteger16;
            int16_t    ****SInteger16;
            uint8_t    ****UInteger8;
            int8_t     ****SInteger8;
        } Pixels;
        Image_ChannelMask *ChannelMask;
        uint64_t           Width;
        uint64_t           Height;
        uint8_t            BitDepth;
        uint8_t            NumChannels;
        bool               Is3D;
        bool               IsUnsigned;
    } ImageContainer;
    
    AudioContainer *AudioContainer_Init(bool IsUnsigned, uint8_t BitDepth, uint8_t NumChannels, uint64_t NumSamples) {
        /* We need to create an enum that contans a channel mask as well
         How should that work tho? the audo decoder should manage that for the user, and the extractor should just take in a mask saying which channel the user wants, and extract that, or remap it on the output...
         */
        AudioContainer *Audio = NULL;
        if (BitDepth > 0 && NumChannels > 0 && NumSamples > 0) {
            Audio = calloc(1, sizeof(AudioContainer));
            if (Audio != NULL) {
                Audio->BitDepth    = BitDepth;
                Audio->NumChannels = NumChannels;
                Audio->NumSamples  = NumSamples;
                Audio->ChannelMask = calloc(NumChannels, sizeof(Audio_ChannelMask));
                if (BitDepth <= 8 && IsUnsigned == Yes) {
                    Audio->Samples.UInteger8 = calloc(NumChannels * NumSamples, sizeof(uint8_t));
                } else if (BitDepth <= 8 && IsUnsigned == No) {
                    Audio->Samples.SInteger8 = calloc(NumChannels * NumSamples, sizeof(int8_t));
                } else if (BitDepth <= 16 && IsUnsigned == Yes) {
                    Audio->Samples.UInteger16 = calloc(NumChannels * NumSamples, sizeof(uint16_t));
                } else if (BitDepth <= 16 && IsUnsigned == No) {
                    Audio->Samples.SInteger16 = calloc(NumChannels * NumSamples, sizeof(int16_t));
                } else if (BitDepth <= 32 && IsUnsigned == Yes) {
                    Audio->Samples.UInteger32 = calloc(NumChannels * NumSamples, sizeof(uint32_t));
                } else if (BitDepth <= 32 && IsUnsigned == No) {
                    Audio->Samples.SInteger32 = calloc(NumChannels * NumSamples, sizeof(int32_t));
                }
                if (Audio->Samples.UInteger8 == NULL && Audio->Samples.SInteger8 == NULL && Audio->Samples.UInteger16 == NULL && Audio->Samples.SInteger16 == NULL && Audio->Samples.UInteger32 == NULL && Audio->Samples.SInteger32 == NULL) {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate room for the samples"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate the AudioContainer"));
            }
        } else if (BitDepth == 0) {
            Log(Log_ERROR, __func__, U8("BitDepth %d is invalid"), BitDepth);
        } else if (NumChannels == 0) {
            Log(Log_ERROR, __func__, U8("NumChannels %d is invalid"), NumChannels);
        } else if (NumSamples == 0) {
            Log(Log_ERROR, __func__, U8("NumSamples %llu is invalid"), NumSamples);
        }
        return Audio;
    }
    
    uint8_t AudioContainer_GetBitDepth(AudioContainer *Audio) {
        uint8_t BitDepth = 0;
        if (Audio != NULL) {
            BitDepth = Audio->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint8_t AudioContainer_GetNumChannels(AudioContainer *Audio) {
        uint8_t NumChannels = 0;
        if (Audio != NULL) {
            NumChannels = Audio->NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t AudioContainer_GetNumSamples(AudioContainer *Audio) {
        uint64_t NumSamples = 0;
        if (Audio != NULL) {
            NumSamples = Audio->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return NumSamples;
    }
    
    void AudioContainer_SetChannelMask(AudioContainer *Audio, uint8_t Channel, Audio_ChannelMask ChannelMask) {
        if (Audio != NULL) {
            Audio->ChannelMask[Channel] = ChannelMask;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    Audio_ChannelMask AudioContainer_GetChannelMask(AudioContainer *Audio, uint8_t Channel) {
        Audio_ChannelMask ChannelMask = Audio_UnknownChannel;
        if (Audio != NULL && Channel < Audio->NumChannels) {
            ChannelMask = Audio->ChannelMask[Channel];
        } else if (Audio == NULL) {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        } else if (Channel >= Audio->NumChannels) {
            Log(Log_ERROR, __func__, U8("Channel %d is out of bounds %d"), Channel, Audio->NumChannels - 1);
        }
        return ChannelMask;
    }
    
    uint8_t AudioContainer_GetChannel(AudioContainer *Audio, Audio_ChannelMask ChannelMask) {
        uint8_t Channel = 0;
        if (Audio != NULL) {
            Channel = Audio->ChannelMask[ChannelMask];
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Channel;
    }
    
    int64_t AudioContainer_GetAverage(AudioContainer *Audio, Audio_ChannelMask Channel) {
        int64_t Average = 0;
        if (Audio != NULL) {
            if (Audio->BitDepth <= 8 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.UInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample];
                }
            } else if (Audio->BitDepth <= 8 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.SInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample];
                }
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.UInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample];
                }
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.SInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample];
                }
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.UInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample];
                }
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.SInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample];
                }
            }
            Average /= Audio->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t AudioContainer_GetMax(AudioContainer *Audio, Audio_ChannelMask Channel) {
        int64_t Maximum = 0;
        if (Audio != NULL) {
            // We need a channel map variable jesus.
            if (Audio->BitDepth <= 8 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample] > Maximum) {
                        Maximum = Audio->Samples.UInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 8 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample] > Maximum) {
                        Maximum = Audio->Samples.SInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample] > Maximum) {
                        Maximum = Audio->Samples.UInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample] > Maximum) {
                        Maximum = Audio->Samples.SInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample] > Maximum) {
                        Maximum = Audio->Samples.UInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample] > Maximum) {
                        Maximum = Audio->Samples.SInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    int64_t AudioContainer_GetMin(AudioContainer *Audio, Audio_ChannelMask Channel) {
        int64_t Minimum = 9223372036854775807;
        if (Audio != NULL) {
            if (Audio->BitDepth <= 8 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample] < Minimum) {
                        Minimum = Audio->Samples.UInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 8 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample] < Minimum) {
                        Minimum = Audio->Samples.SInteger8[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample] < Minimum) {
                        Minimum = Audio->Samples.UInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample] < Minimum) {
                        Minimum = Audio->Samples.SInteger16[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == Yes) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample] < Minimum) {
                        Minimum = Audio->Samples.UInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == No) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample] < Minimum) {
                        Minimum = Audio->Samples.SInteger32[AudioContainer_GetChannel(Audio, Channel)][Sample];
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    void AudioContainer_Deinit(AudioContainer *Audio) {
        if (Audio != NULL) {
            if (Audio->BitDepth <= 8 && Audio->IsUnsigned == Yes) {
                free(Audio->Samples.UInteger8);
            } else if (Audio->BitDepth <= 8 && Audio->IsUnsigned == No) {
                free(Audio->Samples.SInteger8);
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == Yes) {
                free(Audio->Samples.UInteger16);
            } else if (Audio->BitDepth <= 16 && Audio->IsUnsigned == No) {
                free(Audio->Samples.SInteger16);
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == Yes) {
                free(Audio->Samples.UInteger32);
            } else if (Audio->BitDepth <= 32 && Audio->IsUnsigned == No) {
                free(Audio->Samples.SInteger32);
            }
            free(Audio);
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    ImageContainer *ImageContainer_Init(bool Is3D, bool IsUnsigned, uint8_t BitDepth, uint8_t NumChannels, uint64_t Width, uint64_t Height) {
        ImageContainer *Image = NULL;
        if (BitDepth > 0 && NumChannels > 0 && Width > 0 && Height > 0) {
            Image = calloc(1, sizeof(ImageContainer));
            if (Image != NULL) {
                Image->Is3D        = Is3D;
                Image->IsUnsigned  = IsUnsigned;
                Image->BitDepth    = BitDepth;
                Image->NumChannels = NumChannels;
                Image->Width       = Width;
                Image->Height      = Height;
                Image->ChannelMask = calloc(NumChannels, sizeof(Image_ChannelMask));
                if (BitDepth <= 8 && IsUnsigned == Yes) {
                    Image->Pixels.UInteger8 = calloc(Is3D == 0 ? 1 : 2 * NumChannels * Width * Height, sizeof(uint8_t));
                } else if (BitDepth <= 8 && IsUnsigned == No) {
                    Image->Pixels.SInteger8 = calloc(Is3D == 0 ? 1 : 2 * NumChannels * Width * Height, sizeof(int8_t));
                } else if (BitDepth <= 16 && IsUnsigned == Yes) {
                    Image->Pixels.UInteger16 = calloc(Is3D == 0 ? 1 : 2 * NumChannels * Width * Height, sizeof(uint16_t));
                } else if (BitDepth <= 16 && IsUnsigned == No) {
                    Image->Pixels.SInteger16 = calloc(Is3D == 0 ? 1 : 2 * NumChannels * Width * Height, sizeof(int16_t));
                }
                if (Image->Pixels.UInteger8 == NULL && Image->Pixels.SInteger8 == NULL && Image->Pixels.UInteger16 == NULL && Image->Pixels.SInteger16 == NULL) {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate room for the pixels"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate the ImageContainer"));
            }
        } else if (BitDepth == 0) {
            Log(Log_ERROR, __func__, U8("BitDepth %d is invalid"), BitDepth);
        } else if (NumChannels == 0) {
            Log(Log_ERROR, __func__, U8("NumChannels %d is invalid"), NumChannels);
        } else if (Width == 0) {
            Log(Log_ERROR, __func__, U8("Width %llu is invalid"), Width);
        } else if (Height == 0) {
            Log(Log_ERROR, __func__, U8("Height %llu is invalid"), Height);
        }
        return Image;
    }
    
    bool ImageContainer_Is3D(ImageContainer *Image) {
        bool Is3D = No;
        if (Image != NULL) {
            Is3D = Image->Is3D;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Is3D;
    }
    
    uint8_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        uint8_t BitDepth = 0;
        if (Image != NULL) {
            BitDepth = Image->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint8_t ImageContainer_GetNumChannels(ImageContainer *Image) {
        uint8_t NumChannels = 0;
        if (Image != NULL) {
            NumChannels = Image->NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t ImageContainer_GetWidth(ImageContainer *Image) {
        uint64_t Width = 0;
        if (Image != NULL) {
            Width = Image->Width;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t ImageContainer_GetHeight(ImageContainer *Image) {
        uint64_t Height = 0;
        if (Image != NULL) {
            Height = Image->Height;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Height;
    }
    
    void ImageContainer_SetChannelMask(ImageContainer *Image, uint8_t Index, Image_ChannelMask ChannelMask) {
        if (Image != NULL && Index < Image->NumChannels) {
            Image->ChannelMask[Index] = ChannelMask;
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        } else if (Index >= Image->NumChannels) {
            Log(Log_ERROR, __func__, U8("Channel %d is out of bounds %d"), Index, Image->NumChannels - 1);
        }
    }
    
    Image_ChannelMask ImageContainer_GetChannelMask(ImageContainer *Image, uint8_t Index) {
        Image_ChannelMask ChannelMask = Image_UnknownChannel;
        if (Image != NULL && Index < Image->NumChannels) {
            ChannelMask = Image->ChannelMask[Index];
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        } else if (Index >= Image->NumChannels) {
            Log(Log_ERROR, __func__, U8("Channel %d is out of bounds %d"), Index, Image->NumChannels - 1);
        }
        return ChannelMask;
    }
    
    uint8_t ImageContainer_GetView(ImageContainer *Image) {
        uint8_t View = 0;
        if (Image != NULL) {
            View = Image->Is3D == Yes ? 1 : 0;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return View;
    }
    
    uint8_t ImageContainer_GetChannel(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        uint8_t Channel = 0;
        if (Image != NULL) {
            Channel = Image->ChannelMask[ChannelMask];
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Channel;
    }
    
    int64_t ImageContainer_GetAverage(ImageContainer *Image, Image_Views View, Image_ChannelMask Channel) {
        int64_t Average = 0;
        if (Image != NULL) {
            if (Image->BitDepth <= 8 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        Average += Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                    }
                }
            } else if (Image->BitDepth <= 8 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        Average += Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                    }
                }
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        Average += Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                    }
                }
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        Average += Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                    }
                }
            } else if (Image->BitDepth <= 32 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        Average += Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                    }
                }
            } else if (Image->BitDepth <= 32 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        Average += Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                    }
                }
            }
            Average /= Image->Width * Image->Height;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Average;
        
    }
    
    int64_t ImageContainer_GetMax(ImageContainer *Image, Image_ChannelMask Channel) {
        int64_t Maximum = 0;
        if (Image != NULL) {
            // We need a channel map variable jesus.
            if (Image->BitDepth <= 8 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] > Maximum) {
                            Maximum = Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            } else if (Image->BitDepth <= 8 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.SInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] > Maximum) {
                            Maximum = Image->Pixels.SInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.UInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] > Maximum) {
                            Maximum = Image->Pixels.UInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.SInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] > Maximum) {
                            Maximum = Image->Pixels.SInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    int64_t ImageContainer_GetMin(ImageContainer *Image, Image_Views View, Image_ChannelMask Channel) {
        int64_t Minimum = 9223372036854775807;
        if (Image != NULL) {
            // We need a channel map variable jesus.
            if (Image->BitDepth <= 8 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] < Minimum) {
                            Minimum = Image->Pixels.UInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            } else if (Image->BitDepth <= 8 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.SInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] < Minimum) {
                            Minimum = Image->Pixels.SInteger8[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == Yes) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.UInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] < Minimum) {
                            Minimum = Image->Pixels.UInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == No) {
                for (uint64_t Width = 0; Width < Image->Width; Width++) {
                    for (uint64_t Height = 0; Height < Image->Height; Height++) {
                        if (Image->Pixels.SInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height] < Minimum) {
                            Minimum = Image->Pixels.SInteger16[ImageContainer_GetView(Image)][ImageContainer_GetChannel(Image, Channel)][Width][Height];
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        if (Image != NULL) {
            if (Image->BitDepth <= 8 && Image->IsUnsigned == Yes) {
                free(Image->Pixels.UInteger8);
            } else if (Image->BitDepth <= 8 && Image->IsUnsigned == No) {
                free(Image->Pixels.SInteger8);
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == Yes) {
                free(Image->Pixels.UInteger16);
            } else if (Image->BitDepth <= 16 && Image->IsUnsigned == No) {
                free(Image->Pixels.SInteger16);
            }
            free(Image);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    
    /* Audio/ImageContainer read/write2bitbuffer */
    
    AudioContainer *CreateAudioContainerFromBitBuffer() {
        return NULL;
    }
    
    ImageContainer *CreateImageContainerFromBitBuffer() {
        return NULL;
    }
    
    void WriteAudioContainer2BitBuffer(AudioContainer *Audio, BitBuffer *BitBuffer2Write) {
        
    }
    
    void WriteImageContainer2BitBuffer(ImageContainer *Image, BitBuffer *BitBuffer2Write) {
        
    }
    
#ifdef __cplusplus
}
#endif
