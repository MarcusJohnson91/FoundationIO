#include "../include/Macros.h"
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
        uint64_t            NumChannels;
        uint64_t            BitDepth;
        Audio_Types         Type;
    } AudioContainer;
    
    AudioContainer *AudioContainer_Init(Audio_Types Type, uint64_t BitDepth, uint64_t NumChannels, uint64_t SampleRate, uint64_t NumSamples) {
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
            Log(Log_ERROR, __func__, U8("BitDepth %llu is invalid"), BitDepth);
        } else if (NumChannels == 0) {
            Log(Log_ERROR, __func__, U8("NumChannels %llu is invalid"), NumChannels);
        } else if (NumSamples == 0) {
            Log(Log_ERROR, __func__, U8("NumSamples %llu is invalid"), NumSamples);
        }
        return Audio;
    }
    
    void AudioContainer_SetChannelMask(AudioContainer *Audio, uint64_t Channel, Audio_ChannelMask ChannelMask) {
        if (Audio != NULL) {
            Audio->ChannelMask[Channel] = ChannelMask;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    uint64_t AudioContainer_GetBitDepth(AudioContainer *Audio) {
        uint64_t BitDepth = 0ULL;
        if (Audio != NULL) {
            BitDepth = Audio->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint64_t AudioContainer_GetNumChannels(AudioContainer *Audio) {
        uint64_t NumChannels = 0ULL;
        if (Audio != NULL) {
            NumChannels = Audio->NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t AudioContainer_GetNumSamples(AudioContainer *Audio) {
        uint64_t NumSamples = 0ULL;
        if (Audio != NULL) {
            NumSamples = Audio->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return NumSamples;
    }
    
    uint64_t AudioContainer_GetChannelsIndex(AudioContainer *Audio, Audio_ChannelMask ChannelMask) {
        uint64_t Channel = 0ULL;
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
    
    int64_t AudioContainer_GetAverage(AudioContainer *Audio, uint64_t Channel) {
        int64_t Average = 0LL;
        if (Audio != NULL) {
            if (Audio->Type == AudioContainer_UInteger8) {
                uint8_t **Array = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_SInteger8) {
                int8_t **Array = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_UInteger16) {
                uint16_t **Array = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_SInteger16) {
                int16_t **Array = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_UInteger32) {
                uint32_t **Array = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == AudioContainer_SInteger32) {
                int32_t **Array = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            }
            Average /= Audio->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t AudioContainer_GetMax(AudioContainer *Audio, uint64_t Channel) {
        int64_t Maximum = 0LL;
        if (Audio != NULL && Channel < Audio->NumChannels) {
            if (Audio->Type == AudioContainer_UInteger8) {
                uint8_t **Array = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger8) {
                int8_t **Array = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger16) {
                uint16_t **Array = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger16) {
                int16_t **Array = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger32) {
                uint32_t **Array = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger32) {
                int32_t **Array = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    int64_t AudioContainer_GetMin(AudioContainer *Audio, uint64_t Channel) {
        int64_t Minimum = 9223372036854775807;
        if (Audio != NULL) {
            if (Audio->Type == AudioContainer_UInteger8) {
                uint8_t **Array = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger8) {
                int8_t **Array = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger16) {
                uint16_t **Array = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger16) {
                int16_t **Array = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_UInteger32) {
                uint32_t **Array = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == AudioContainer_SInteger32) {
                int32_t **Array = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
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
        uint64_t             BitDepth;
        uint64_t             NumChannels;
        uint64_t             NumViews;
        Image_Types          Type;
    } ImageContainer;
    
    ImageContainer *ImageContainer_Init(Image_Types Type, uint64_t BitDepth, uint64_t NumChannels, uint64_t Width, uint64_t Height) {
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
                
                if (Type == ImageContainer_2DUInteger8) {
                    Image->Pixels.UInteger8  = calloc(NumChannels * Width * Height, sizeof(int8_t));
                } else if (Type == ImageContainer_2DSInteger8) {
                    Image->Pixels.SInteger8  = calloc(NumChannels * Width * Height, sizeof(uint8_t));
                } else if (Type == ImageContainer_2DUInteger16) {
                    Image->Pixels.UInteger8  = calloc(NumChannels * Width * Height, sizeof(uint16_t));
                } else if (Type == ImageContainer_2DSInteger16) {
                    Image->Pixels.SInteger8  = calloc(NumChannels * Width * Height, sizeof(int16_t));
                } else if (Type == ImageContainer_3DUInteger8) {
                    Image->Pixels.UInteger8  = calloc(2 * NumChannels * Width * Height, sizeof(int8_t));
                } else if (Type == ImageContainer_3DSInteger8) {
                    Image->Pixels.SInteger8  = calloc(2 * NumChannels * Width * Height, sizeof(int8_t));
                } else if (Type == ImageContainer_3DUInteger16) {
                    Image->Pixels.UInteger16 = calloc(2 * NumChannels * Width * Height, sizeof(uint16_t));
                } else if (Type == ImageContainer_3DSInteger16) {
                    Image->Pixels.SInteger16 = calloc(2 * NumChannels * Width * Height, sizeof(int16_t));
                }
                if (Image->Pixels.UInteger8 == NULL && Image->Pixels.SInteger8 == NULL && Image->Pixels.UInteger16 == NULL && Image->Pixels.SInteger16 == NULL) {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate room for the pixels"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate the ImageContainer"));
            }
        } else if (BitDepth == 0) {
            Log(Log_ERROR, __func__, U8("BitDepth %llu is invalid"), BitDepth);
        } else if (NumChannels == 0) {
            Log(Log_ERROR, __func__, U8("NumChannels %llu is invalid"), NumChannels);
        } else if (Width == 0) {
            Log(Log_ERROR, __func__, U8("Width %llu is invalid"), Width);
        } else if (Height == 0) {
            Log(Log_ERROR, __func__, U8("Height %llu is invalid"), Height);
        }
        return Image;
    }
    
    void ImageContainer_SetChannelMask(ImageContainer *Image, uint64_t Index, Image_ChannelMask ChannelMask) {
        if (Image != NULL && Index < Image->NumChannels) {
            Image->ChannelMask[Index] = ChannelMask;
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        } else if (Index >= Image->NumChannels) {
            Log(Log_ERROR, __func__, U8("Channel %llu is out of bounds %llu"), Index, Image->NumChannels - 1);
        }
    }
    
    uint64_t ImageContainer_GetWidth(ImageContainer *Image) {
        uint64_t Width = 0ULL;
        if (Image != NULL) {
            Width = Image->Width;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t ImageContainer_GetHeight(ImageContainer *Image) {
        uint64_t Height = 0ULL;
        if (Image != NULL) {
            Height = Image->Height;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Height;
    }
    
    uint64_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        uint64_t BitDepth = 0ULL;
        if (Image != NULL) {
            BitDepth = Image->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint64_t ImageContainer_GetNumChannels(ImageContainer *Image) {
        uint64_t NumChannels = 0ULL;
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
    
    uint64_t ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint64_t Index = 0xFFFFFFFFFFFFFFFF;
        if (Image != NULL) {
            Index      = Image->NumChannels;
            for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                if (Image->ChannelMask[Channel] == Mask) {
                    Index = Channel;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    uint64_t ImageContainer_GetNumViews(ImageContainer *Image) {
        uint64_t NumViews = 0ULL;
        if (Image != NULL) {
            NumViews      = Image->NumViews;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumViews;
    }
    
    uint64_t ImageContainer_GetViewsIndex(ImageContainer *Image, Image_Views View) {
        uint64_t Index = 0xFFFFFFFFFFFFFFFF;
        if (Image != NULL) {
            Index      = Image->NumViews;
            for (uint64_t ViewIndex = 0ULL; ViewIndex < Image->NumViews; ViewIndex++) {
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
            if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                ImageArray = (void****) Image->Pixels.UInteger8;
            } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                ImageArray = (void****) Image->Pixels.SInteger8;
            } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                ImageArray = (void****) Image->Pixels.UInteger16;
            } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                ImageArray = (void****) Image->Pixels.SInteger16;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return ImageArray;
    }
    
    int64_t ImageContainer_GetAverage(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask) {
        int64_t Average = 0LL;
        if (Image != NULL) {
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ViewMask);
            if (Channel < Image->NumChannels) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            Average += Array[View][Width][Height][Channel];
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    int8_t ****Array = (int8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            Average += Array[View][Width][Height][Channel];
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            Average += Array[View][Width][Height][Channel];
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    int16_t ****Array = (int16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            Average += Array[View][Width][Height][Channel];
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
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ViewMask);
            if (Channel < Image->NumChannels) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] > Maximum) {
                                Maximum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    int8_t ****Array = (int8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] > Maximum) {
                                Maximum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    uint16_t ****Array = (uint16_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] > Maximum) {
                                Maximum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    int16_t  ****Array = (int16_t****)   ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] > Maximum) {
                                Maximum = Array[View][Width][Height][Channel];
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
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ViewMask);
            if (Channel < Image->NumChannels) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    uint8_t  ****Array = (uint8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] < Minimum) {
                                Minimum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    int8_t   ****Array = (int8_t****)   ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] < Minimum) {
                                Minimum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    uint16_t  ****Array = (uint16_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] < Minimum) {
                                Minimum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    int16_t   ****Array = (int16_t****)   ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                            if (Array[View][Width][Height][Channel] < Minimum) {
                                Minimum = Array[View][Width][Height][Channel];
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
    
    void ImageContainer_Flip(ImageContainer *Image, bool VerticalFlip, bool HorizontalFlip) {
        if (Image != NULL) {
            if (VerticalFlip == Yes) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    uint8_t  ****Array = (uint8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        uint8_t TopPixel            = Array[View][Width][TopLine][Channel];
                                        uint8_t BottomPixel         = Array[View][Width][BottomLine][Channel];
                                        
                                        Array[View][Width][TopLine]    = BottomPixel;
                                        Array[View][Width][BottomLine] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    int8_t   ****Array = (int8_t****)   ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        int8_t  TopPixel            = Array[View][Width][TopLine][Channel];
                                        int8_t  BottomPixel         = Array[View][Width][BottomLine][Channel];
                                        
                                        Array[View][Width][TopLine]    = BottomPixel;
                                        Array[View][Width][BottomLine] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        uint16_t TopPixel           = Array[View][Width][TopLine][Channel];
                                        uint16_t BottomPixel        = Array[View][Width][BottomLine][Channel];
                                        
                                        Array[View][Width][TopLine]    = BottomPixel;
                                        Array[View][Width][BottomLine] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                int16_t  ****Array = (int16_t****)  ImageContainer_GetArray(Image);
                for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                            for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                    int16_t TopPixel            = Array[View][Width][TopLine][Channel];
                                    int16_t BottomPixel         = Array[View][Width][BottomLine][Channel];
                                    
                                    Array[View][Width][TopLine]    = BottomPixel;
                                    Array[View][Width][BottomLine] = TopPixel;
                                }
                            }
                        }
                    }
                }
            }
            if (HorizontalFlip == Yes) {
                if (Image->Type == ImageContainer_2DUInteger8 || Image->Type == ImageContainer_3DUInteger8) {
                    uint8_t  ****Array = (uint8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        uint8_t LeftPixel                   = Array[View][Left][Height][Channel];
                                        uint8_t RightPixel                  = Array[View][Right][Height][Channel];
                                        
                                        Array[View][Left][Height][Channel]  = LeftPixel;
                                        Array[View][Right][Height][Channel] = RightPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger8 || Image->Type == ImageContainer_3DSInteger8) {
                    int8_t   ****Array = (int8_t****)   ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        int8_t LeftPixel                    = Array[View][Left][Height][Channel];
                                        int8_t RightPixel                   = Array[View][Right][Height][Channel];
                                        
                                        Array[View][Left][Height][Channel]  = LeftPixel;
                                        Array[View][Right][Height][Channel] = RightPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DUInteger16 || Image->Type == ImageContainer_3DUInteger16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        uint16_t LeftPixel                  = Array[View][Left][Height][Channel];
                                        uint16_t RightPixel                 = Array[View][Right][Height][Channel];
                                        
                                        Array[View][Left][Height][Channel]  = LeftPixel;
                                        Array[View][Right][Height][Channel] = RightPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageContainer_2DSInteger16 || Image->Type == ImageContainer_3DSInteger16) {
                    int16_t ****Array  = (int16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels; Channel++) {
                                        int16_t LeftPixel                   = Array[View][Left][Height][Channel];
                                        int16_t RightPixel                  = Array[View][Right][Height][Channel];
                                        
                                        Array[View][Left][Height][Channel]  = LeftPixel;
                                        Array[View][Right][Height][Channel] = RightPixel;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
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
