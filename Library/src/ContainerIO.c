#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */
#include "../include/Math.h"           /* Included for Absolute */
#include "../include/BitIO.h"          /* Included for BitBuffer */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct AudioContainer {
        union Samples {
            uint32_t      **UInteger32;
            int32_t       **SInteger32;
            uint16_t      **UInteger16;
            int16_t       **SInteger16;
            uint8_t       **UInteger8;
            int8_t        **SInteger8;
        } Samples;
        Audio_ChannelMask  *ChannelMask;
        uint64_t            NumSamples;
        uint64_t            SampleRate;
        uint8_t             BitDepth;
        uint8_t             NumChannels;
        Audio_Types         Type;
    } AudioContainer;
    
    AudioContainer *AudioContainer_Init(Audio_Types Type, uint8_t BitDepth, uint8_t NumChannels, uint64_t SampleRate, uint64_t NumSamples) {
        AudioContainer *Audio = NULL;
        if (BitDepth > 0 && NumChannels > 0 && NumSamples > 0) {
            Audio = calloc(1, sizeof(AudioContainer));
            if (Audio != NULL) {
                Audio->BitDepth    = BitDepth;
                Audio->NumChannels = NumChannels;
                Audio->SampleRate  = SampleRate;
                Audio->NumSamples  = NumSamples;
                Audio->ChannelMask = calloc(NumChannels, sizeof(Audio_ChannelMask));
                if (Type == AudioContainer_UInteger8) {
                    Audio->Samples.UInteger8 = calloc(NumChannels * NumSamples, sizeof(uint8_t));
                } else if (Type == AudioContainer_SInteger8) {
                    Audio->Samples.SInteger8 = calloc(NumChannels * NumSamples, sizeof(int8_t));
                } else if (Type == AudioContainer_UInteger16) {
                    Audio->Samples.UInteger16 = calloc(NumChannels * NumSamples, sizeof(uint16_t));
                } else if (Type == AudioContainer_SInteger16) {
                    Audio->Samples.SInteger16 = calloc(NumChannels * NumSamples, sizeof(int16_t));
                } else if (Type == AudioContainer_UInteger32) {
                    Audio->Samples.UInteger32 = calloc(NumChannels * NumSamples, sizeof(uint32_t));
                } else if (Type == AudioContainer_SInteger32) {
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
    
    void AudioContainer_SetChannelMask(AudioContainer *Audio, uint8_t Channel, Audio_ChannelMask ChannelMask) {
        if (Audio != NULL) {
            Audio->ChannelMask[Channel] = ChannelMask;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
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
    
    uint8_t AudioContainer_GetChannelsIndex(AudioContainer *Audio, Audio_ChannelMask ChannelMask) {
        uint8_t Channel = 0;
        if (Audio != NULL) {
            Channel = Audio->ChannelMask[ChannelMask];
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Channel;
    }
    
    Audio_Types AudioContainer_GetType(AudioContainer *Audio) {
        Audio_Types Type = AudioContainer_UnknownType;
        if (Audio != NULL) {
            Type                 = Audio->Type;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void **AudioContainer_GetArray(AudioContainer *Audio) {
        void **AudioArray = NULL;
        if (Audio != NULL) {
            if (Audio->Type == AudioContainer_SInteger8) {
                AudioArray = (void**) Audio->Samples.SInteger8;
            } else if (Audio->Type == AudioContainer_UInteger8) {
                AudioArray = (void**) Audio->Samples.UInteger8;
            } else if (Audio->Type == AudioContainer_SInteger16) {
                AudioArray = (void**) Audio->Samples.SInteger16;
            } else if (Audio->Type == AudioContainer_UInteger16) {
                AudioArray = (void**) Audio->Samples.UInteger16;
            } else if (Audio->Type == AudioContainer_SInteger32) {
                AudioArray = (void**) Audio->Samples.SInteger32;
            } else if (Audio->Type == AudioContainer_UInteger32) {
                AudioArray = (void**) Audio->Samples.UInteger32;
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return AudioArray;
    }
    
    int64_t AudioContainer_GetAverage(AudioContainer *Audio, uint8_t Channel) {
        int64_t Average = 0;
        if (Audio != NULL) {
            if (Audio->Type == AudioContainer_UInteger8) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.UInteger8[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_SInteger8) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.SInteger8[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_UInteger16) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.UInteger16[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_SInteger16) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.SInteger16[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_UInteger32) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.UInteger32[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_SInteger32) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    Average += Audio->Samples.SInteger32[Channel][Sample];
                }
            }
            Average /= Audio->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t AudioContainer_GetMax(AudioContainer *Audio, uint8_t Channel) {
        int64_t Maximum = 0;
        if (Audio != NULL && Channel < Audio->NumChannels) {
            if (Audio->Type == AudioContainer_UInteger8) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger8[Channel][Sample] > Maximum) {
                        Maximum = Audio->Samples.UInteger8[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger8) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger8[Channel][Sample] > Maximum) {
                        Maximum = Audio->Samples.SInteger8[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger16) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger16[Channel][Sample] > Maximum) {
                        Maximum = Audio->Samples.UInteger16[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger16) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger16[Channel][Sample] > Maximum) {
                        Maximum = Audio->Samples.SInteger16[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger32) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger32[Channel][Sample] > Maximum) {
                        Maximum = Audio->Samples.UInteger32[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger32) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger32[Channel][Sample] > Maximum) {
                        Maximum = Audio->Samples.SInteger32[Channel][Sample];
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    int64_t AudioContainer_GetMin(AudioContainer *Audio, uint8_t Channel) {
        int64_t Minimum = 9223372036854775807;
        if (Audio != NULL) {
            if (Audio->Type == AudioContainer_UInteger8) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger8[Channel][Sample] < Minimum) {
                        Minimum = Audio->Samples.UInteger8[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger8) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger8[Channel][Sample] < Minimum) {
                        Minimum = Audio->Samples.SInteger8[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger16) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger16[Channel][Sample] < Minimum) {
                        Minimum = Audio->Samples.UInteger16[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger16) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger16[Channel][Sample] < Minimum) {
                        Minimum = Audio->Samples.SInteger16[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger32) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.UInteger32[Channel][Sample] < Minimum) {
                        Minimum = Audio->Samples.UInteger32[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger32) {
                for (uint64_t Sample = 0; Sample < Audio->NumSamples; Sample++) {
                    if (Audio->Samples.SInteger32[Channel][Sample] < Minimum) {
                        Minimum = Audio->Samples.SInteger32[Channel][Sample];
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
            if (Audio->Type == AudioContainer_UInteger8) {
                free(Audio->Samples.UInteger8);
            } else if (Audio->Type == AudioContainer_SInteger8) {
                free(Audio->Samples.SInteger8);
            } else if (Audio->Type == AudioContainer_UInteger16) {
                free(Audio->Samples.UInteger16);
            } else if (Audio->Type == AudioContainer_SInteger16) {
                free(Audio->Samples.SInteger16);
            } else if (Audio->Type == AudioContainer_UInteger32) {
                free(Audio->Samples.UInteger32);
            } else if (Audio->Type == AudioContainer_SInteger32) {
                free(Audio->Samples.SInteger32);
            }
            free(Audio);
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    typedef struct ImageContainer {
        union Pixels {
            uint16_t     ****UInteger16;
            int16_t      ****SInteger16;
            uint8_t      ****UInteger8;
            int8_t       ****SInteger8;
        } Pixels;
        Image_ChannelMask   *ChannelMask;
        Image_Views         *Views;
        uint64_t             Width;
        uint64_t             Height;
        uint8_t              BitDepth;
        uint8_t              NumChannels;
        uint8_t              NumViews;
        Image_Types          Type;
    } ImageContainer;
    
    ImageContainer *ImageContainer_Init(Image_Types Type, uint8_t BitDepth, uint8_t NumChannels, uint64_t Width, uint64_t Height) {
        ImageContainer *Image = NULL;
        if (BitDepth > 0 && NumChannels > 0 && Width > 0 && Height > 0) {
            Image = calloc(1, sizeof(ImageContainer));
            if (Image != NULL) {
                Image->Type                  = Type;
                Image->BitDepth              = BitDepth;
                Image->NumChannels           = NumChannels;
                Image->Width                 = Width;
                Image->Height                = Height;
                Image->ChannelMask           = calloc(NumChannels, sizeof(Image_ChannelMask));
                if (Type == ImageContainer_2DSInteger8) {
                    Image->Pixels.SInteger8  = calloc(NumChannels * Width * Height, sizeof(uint8_t));
                } else if (Type == ImageContainer_2DUInteger8) {
                    Image->Pixels.UInteger8  = calloc(NumChannels * Width * Height, sizeof(int8_t));
                } else if (Type == ImageContainer_2DSInteger16) {
                    Image->Pixels.SInteger8  = calloc(NumChannels * Width * Height, sizeof(int16_t));
                } else if (Type == ImageContainer_2DUInteger16) {
                    Image->Pixels.UInteger8  = calloc(NumChannels * Width * Height, sizeof(uint16_t));
                } else if (Type == ImageContainer_3DSInteger8) {
                    Image->Pixels.SInteger8  = calloc(2 * NumChannels * Width * Height, sizeof(int8_t));
                } else if (Type == ImageContainer_3DUInteger8) {
                    Image->Pixels.UInteger8  = calloc(2 * NumChannels * Width * Height, sizeof(int8_t));
                } else if (Type == ImageContainer_3DSInteger16) {
                    Image->Pixels.SInteger16 = calloc(2 * NumChannels * Width * Height, sizeof(int16_t));
                } else if (Type == ImageContainer_3DUInteger16) {
                    Image->Pixels.UInteger16 = calloc(2 * NumChannels * Width * Height, sizeof(uint16_t));
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
    
    void ImageContainer_SetChannelMask(ImageContainer *Image, uint8_t Index, Image_ChannelMask ChannelMask) {
        if (Image != NULL && Index < Image->NumChannels) {
            Image->ChannelMask[Index] = ChannelMask;
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        } else if (Index >= Image->NumChannels) {
            Log(Log_ERROR, __func__, U8("Channel %d is out of bounds %d"), Index, Image->NumChannels - 1);
        }
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
            if (Image->NumViews > 0) {
                NumChannels = Image->NumChannels / Image->NumViews;
            } else {
                NumChannels = Image->NumChannels;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint8_t ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint8_t Index = 0xFF;
        if (Image != NULL) {
            Index     = Image->NumChannels;
            for (uint8_t Channel = 0; Channel < Image->NumChannels; Channel++) {
                if (Image->ChannelMask[Channel] == Mask) {
                    Index = Channel;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    uint8_t ImageContainer_GetNumViews(ImageContainer *Image) {
        uint8_t NumViews = 0;
        if (Image != NULL) {
            NumViews     = Image->NumViews;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumViews;
    }
    
    uint8_t ImageContainer_GetViewsIndex(ImageContainer *Image, Image_Views View) {
        uint8_t Index = 0xFF;
        if (Image != NULL) {
            Index     = Image->NumViews;
            for (uint8_t ViewIndex = 0; ViewIndex < Image->NumViews; ViewIndex++) {
                if (Image->Views[ViewIndex] == View) {
                    Index = ViewIndex;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Index;
    }

    Image_Types ImageContainer_GetType(ImageContainer *Image) {
        Image_Types Type = ImageContainer_UnknownType;
        if (Image != NULL) {
            Type                 = Image->Type;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void ****ImageContainer_GetArray(ImageContainer *Image) {
        void ****ImageArray = NULL;
        if (Image != NULL) {
            if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                ImageArray = (void****) Image->Pixels.SInteger8;
            } else if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                ImageArray = (void****) Image->Pixels.UInteger8;
            } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                ImageArray = (void****) Image->Pixels.SInteger16;
            } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                ImageArray = (void****) Image->Pixels.UInteger16;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return ImageArray;
    }
    
    int64_t ImageContainer_GetAverage(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask) {
        int64_t Average = 0LL;
        if (Image != NULL) {
            uint8_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint8_t View    = ImageContainer_GetViewsIndex(Image, ViewMask);
            if (Channel < Image->NumChannels) {
                if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            Average += Image->Pixels.SInteger8[View][Channel][Width][Height];
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            Average += Image->Pixels.UInteger8[View][Channel][Width][Height];
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            Average += Image->Pixels.SInteger16[View][Channel][Width][Height];
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            Average += Image->Pixels.UInteger16[View][Channel][Width][Height];
                        }
                    }
                }
                Average /= Image->Width * Image->Height;
            } else {
                Log(Log_ERROR, __func__, U8("You tried getting the average from a nonexistant channel"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t ImageContainer_GetMax(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask) {
        int64_t Maximum = -9223372036854775807LL;
        if (Image != NULL) {
            uint8_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint8_t View    = ImageContainer_GetViewsIndex(Image, ViewMask);
            if (Channel < Image->NumChannels) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.UInteger8[View][Channel][Width][Height] > Maximum) {
                                Maximum = Image->Pixels.UInteger8[View][Channel][Width][Height];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.SInteger8[View][Channel][Width][Height] > Maximum) {
                                Maximum = Image->Pixels.SInteger8[View][Channel][Width][Height];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.UInteger16[View][Channel][Width][Height] > Maximum) {
                                Maximum = Image->Pixels.UInteger16[View][Channel][Width][Height];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.SInteger16[View][Channel][Width][Height] > Maximum) {
                                Maximum = Image->Pixels.SInteger16[View][Channel][Width][Height];
                            }
                        }
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("You tried getting the max from a nonexistant channel"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    int64_t ImageContainer_GetMin(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask) {
        int64_t Minimum = 9223372036854775807LL;
        if (Image != NULL) {
            uint8_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint8_t View    = ImageContainer_GetViewsIndex(Image, ViewMask);
            if (Channel < Image->NumChannels) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.UInteger8[View][Channel][Width][Height] < Minimum) {
                                Minimum = Image->Pixels.UInteger8[View][Channel][Width][Height];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.SInteger8[View][Channel][Width][Height] < Minimum) {
                                Minimum = Image->Pixels.SInteger8[View][Channel][Width][Height];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.UInteger16[View][Channel][Width][Height] < Minimum) {
                                Minimum = Image->Pixels.UInteger16[View][Channel][Width][Height];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    for (uint64_t Width = 0; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0; Height < Image->Height; Height++) {
                            if (Image->Pixels.SInteger16[View][Channel][Width][Height] < Minimum) {
                                Minimum = Image->Pixels.SInteger16[View][Channel][Width][Height];
                            }
                        }
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("You tried getting the min from a nonexistant channel"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        if (Image != NULL) {
            if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                free(Image->Pixels.SInteger8);
            } else if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                free(Image->Pixels.UInteger8);
            } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                free(Image->Pixels.SInteger16);
            } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                free(Image->Pixels.UInteger16);
            }
            free(Image);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
