#include "../include/Macros.h"         /* Included for FoundationIO's macros */

#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */
#include "../include/StringIO.h"       /* Included for StringIO's declarations */

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
        Audio_ChannelMask *ChannelMask;
        uint64_t           NumSamples;
        uint64_t           SampleRate;
        uint64_t           NumChannels;
        uint64_t           BitDepth;
        Audio_Types        Type;
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
                if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                    Audio->Samples.UInteger8 = calloc(NumChannels * NumSamples, 1);
                } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                    Audio->Samples.SInteger8 = calloc(NumChannels * NumSamples, 1);
                } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                    Audio->Samples.UInteger16 = calloc(NumChannels * NumSamples, 2);
                } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                    Audio->Samples.SInteger16 = calloc(NumChannels * NumSamples, 2);
                } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                    Audio->Samples.UInteger32 = calloc(NumChannels * NumSamples, 4);
                } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                    Audio->Samples.SInteger32 = calloc(NumChannels * NumSamples, 4);
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
            Channel      = Audio->ChannelMask[ChannelMask];
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Channel;
    }
    
    Audio_Types AudioContainer_GetType(AudioContainer *Audio) {
        Audio_Types Type = AudioType_Unknown;
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
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                AudioArray = (void**) Audio->Samples.UInteger8;
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                AudioArray = (void**) Audio->Samples.SInteger8;
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                AudioArray = (void**) Audio->Samples.UInteger16;
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                AudioArray = (void**) Audio->Samples.SInteger16;
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                AudioArray = (void**) Audio->Samples.UInteger32;
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                AudioArray = (void**) Audio->Samples.SInteger32;
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return AudioArray;
    }
    
    int64_t AudioContainer_GetAverage(AudioContainer *Audio, uint64_t Channel) {
        int64_t Average = 0LL;
        if (Audio != NULL) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Array = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Array = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Array = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    Average += Array[Channel][Sample];
                }
            }
            if (Average != 0) {
                Average /= Audio->NumSamples;
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t AudioContainer_GetMax(AudioContainer *Audio, uint64_t Channel) {
        int64_t Maximum = INT64_MIN;
        if (Audio != NULL && Channel < Audio->NumChannels - 1) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Array = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Array = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Array = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
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
        int64_t Minimum = INT64_MAX;
        if (Audio != NULL) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Array = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Array = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Array = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples - 1; Sample++) {
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
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                free(Audio->Samples.UInteger8);
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                free(Audio->Samples.SInteger8);
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                free(Audio->Samples.UInteger16);
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                free(Audio->Samples.SInteger16);
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                free(Audio->Samples.UInteger32);
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                free(Audio->Samples.SInteger32);
            }
            free(Audio);
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    typedef struct ImageContainer {
        union Pixels {
            uint16_t    ****UInteger16;
            uint8_t     ****UInteger8;
        } Pixels;
        Image_ChannelMask  *ChannelMask;
        uint64_t            Width;
        uint64_t            Height;
        uint64_t            BitDepth;
        uint64_t            NumChannels;
        uint64_t            NumViews;
        Image_Types         Type;
    } ImageContainer;
    
    ImageContainer *ImageContainer_Init(Image_Types Type, uint64_t BitDepth, uint8_t NumViews, uint64_t NumChannels, uint64_t Width, uint64_t Height) {
        ImageContainer *Image = NULL;
        if (BitDepth > 0 && NumChannels > 0 && Width > 0 && Height > 0) {
            Image = calloc(1, sizeof(ImageContainer));
            if (Image != NULL) {
                Image->Type                  = Type;
                Image->BitDepth              = BitDepth;
                Image->NumViews              = NumViews;
                Image->NumChannels           = NumChannels;
                Image->Width                 = Width;
                Image->Height                = Height;
                Image->ChannelMask           = calloc(NumChannels, sizeof(Image_ChannelMask));
                
                if (Type == ImageType_Integer8) {
                    Image->Pixels.UInteger8  = calloc(NumViews * NumChannels * Width * Height, sizeof(uint8_t));
                } else if (Type == ImageType_Integer16) {
                    Image->Pixels.UInteger16 = calloc(NumViews * NumChannels * Width * Height, sizeof(uint16_t));
                }
                if (Image->Pixels.UInteger8 == NULL && Image->Pixels.UInteger16 == NULL) {
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
        if (Image != NULL && Index < Image->NumChannels - 1) {
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
            Width      = Image->Width;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t ImageContainer_GetHeight(ImageContainer *Image) {
        uint64_t Height = 0ULL;
        if (Image != NULL) {
            Height      = Image->Height;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Height;
    }
    
    uint64_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        uint64_t BitDepth = 0ULL;
        if (Image != NULL) {
            BitDepth      = Image->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint64_t ImageContainer_GetNumChannels(ImageContainer *Image) {
        uint64_t NumChannels = 0ULL;
        if (Image != NULL) {
            if (Image->NumViews > 0) {
                NumChannels  = Image->NumChannels / Image->NumViews;
            } else {
                NumChannels  = Image->NumChannels;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint64_t Index                = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask View    = Mask & 0x7;
            Image_ChannelMask Channel = Mask - View;
            Index                     = Image->NumChannels;
            for (uint64_t Channels = 0ULL; Channels < Image->NumChannels - 1; Channels++) {
                if (Image->ChannelMask[Channels] == Channel) {
                    Index             = Channel;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    uint32_t ImageContainer_GetChannelMask(ImageContainer *Image) {
        uint32_t Mask     = 0;
        if (Image != NULL) {
            for (uint64_t Channel = 0ULL; Channel < Image->NumChannels - 1; Channel++) {
                Mask     |= Image->ChannelMask[Channel];
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Mask;
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
    
    uint64_t ImageContainer_GetViewsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        // So, Mask will contain say ImageMak_Red3D_L and we need to find the view that contains that mask
        uint64_t Index    = 0ULL;
        if (Image != NULL) {
            Index         = Image->NumViews;
            for (uint64_t ViewIndex = 0ULL; ViewIndex < Image->NumViews - 1; ViewIndex++) {
                if (Image->ChannelMask[ViewIndex] == (Mask & 0x7)) {
                    Index = ViewIndex;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    Image_Types ImageContainer_GetType(ImageContainer *Image) {
        Image_Types Type = ImageType_Unknown;
        if (Image != NULL) {
            Type         = Image->Type;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void ****ImageContainer_GetArray(ImageContainer *Image) {
        void ****ImageArray = NULL;
        if (Image != NULL) {
            if (Image->Type == ImageType_Integer8) {
                ImageArray = (void****) Image->Pixels.UInteger8;
            } else if (Image->Type == ImageType_Integer16) {
                ImageArray = (void****) Image->Pixels.UInteger16;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return ImageArray;
    }
    
    void ImageContainer_SetArray(ImageContainer *Image, void ****Array) {
        if (Image != NULL) {
            if (Image->Type == ImageType_Integer8) {
                Image->Pixels.UInteger8 = (uint8_t****)   Array;
            } else if (Image->Type == ImageType_Integer16) {
                Image->Pixels.UInteger16 = (uint16_t****) Array;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    int64_t ImageContainer_GetAverage(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        int64_t Average      = 0LL;
        if (Image != NULL) {
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ChannelMask);
            if (Channel < Image->NumChannels - 1) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            Average += Array[View][Width][Height][Channel];
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            Average += Array[View][Width][Height][Channel];
                        }
                    }
                }
                if (Average != 0) {
                    Average /= Image->Width * Image->Height;
                }
            } else {
                Log(Log_ERROR, __func__, U8("You tried getting the average from a nonexistant channel"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t ImageContainer_GetMax(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        int64_t Maximum      = INT64_MIN;
        if (Image != NULL) {
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ChannelMask);
            if (Channel < Image->NumChannels - 1) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            if (Array[View][Width][Height][Channel] > Maximum) {
                                Maximum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t ****Array = (uint16_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
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
    
    int64_t ImageContainer_GetMin(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        int64_t Minimum      = INT64_MAX;
        if (Image != NULL) {
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ChannelMask);
            if (Channel < Image->NumChannels - 1) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  ****Array = (uint8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            if (Array[View][Width][Height][Channel] < Minimum) {
                                Minimum = Array[View][Width][Height][Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t  ****Array = (uint16_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
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
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  ****Array = (uint8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews - 1; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height - 1; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels - 1; Channel++) {
                                        uint8_t TopPixel                        = Array[View][Width][TopLine][Channel];
                                        uint8_t BottomPixel                     = Array[View][Width][BottomLine][Channel];
                                        
                                        Array[View][Width][TopLine][Channel]    = BottomPixel;
                                        Array[View][Width][BottomLine][Channel] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews - 1; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height - 1; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels - 1; Channel++) {
                                        uint16_t TopPixel                       = Array[View][Width][TopLine][Channel];
                                        uint16_t BottomPixel                    = Array[View][Width][BottomLine][Channel];
                                        
                                        Array[View][Width][TopLine][Channel]    = BottomPixel;
                                        Array[View][Width][BottomLine][Channel] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (HorizontalFlip == Yes) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  ****Array = (uint8_t****)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews - 1; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels - 1; Channel++) {
                                        uint8_t LeftPixel                   = Array[View][Left][Height][Channel];
                                        uint8_t RightPixel                  = Array[View][Right][Height][Channel];
                                        
                                        Array[View][Left][Height][Channel]  = LeftPixel;
                                        Array[View][Right][Height][Channel] = RightPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < Image->NumViews - 1; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width - 1; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < Image->NumChannels - 1; Channel++) {
                                        uint16_t LeftPixel                  = Array[View][Left][Height][Channel];
                                        uint16_t RightPixel                 = Array[View][Right][Height][Channel];
                                        
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
    
    void ImageContainer_Resize(ImageContainer *Image, int64_t Top, int64_t Bottom, int64_t Left, int64_t Right) {
        if (Image != NULL) {
            uint64_t NumChannels = ImageContainer_GetNumChannels(Image);
            uint64_t NumViews    = ImageContainer_GetNumViews(Image);
            uint64_t Height      = ImageContainer_GetHeight(Image);
            uint64_t Width       = ImageContainer_GetWidth(Image);
            Image_Types Type     = ImageContainer_GetType(Image);
            
            
            // If top, bottom, left, or right is positive increase the canvas, if negative, decrease.
            if (Type == ImageType_Integer8) {
                uint8_t ****Array    = (uint8_t****) ImageContainer_GetArray(Image);
                if (Width > Top + Bottom) {
                    // Do something
                } else {
                    
                }
                
                if (Height > Left + Right) {
                    // Do something
                } else {
                }
                uint8_t ****NewArray = calloc((Width - (Top + Bottom)) * (Height - (Left + Right)) * NumChannels * NumViews, sizeof(uint8_t));
                if (NewArray != NULL) {
                    for (uint8_t View = 0; View < NumViews - 1; View++) {
                        for (int64_t W = Left; W < Right; W++) {
                            for (int64_t H = Top; H < Bottom; H++) {
                                for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                    NewArray[View][W][H][Channel] = Array[View][W + Left][H + Top][Channel];
                                }
                            }
                        }
                    }
                    ImageContainer_SetArray(Image, (void****) NewArray);
                    free(Array);
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate an array for the cropped image"));
                }
            } else if (Type == ImageType_Integer16) {
                uint16_t ****Array    = (uint16_t****) ImageContainer_GetArray(Image);
                if (Width > Top + Bottom) {
                    // Do something
                } else {
                    
                }
                
                if (Height > Left + Right) {
                    // Do something
                } else {
                }
                uint16_t ****NewArray = calloc((Width - (Top + Bottom)) * (Height - (Left + Right)) * NumChannels * NumViews, sizeof(uint16_t));
                if (NewArray != NULL) {
                    for (uint8_t View = 0; View < NumViews - 1; View++) {
                        for (int64_t W = Left; W < Right; W++) {
                            for (int64_t H = Top; H < Bottom; H++) {
                                for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                    NewArray[View][W][H][Channel] = Array[View][W + Left][H + Top][Channel];
                                }
                            }
                        }
                    }
                    ImageContainer_SetArray(Image, (void****) NewArray);
                    free(Array);
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate an array for the cropped image"));
                }
            } else if (Type == ImageType_Integer16) {
                int16_t ****Array    = (int16_t****) ImageContainer_GetArray(Image);
                
                int16_t ****NewArray = calloc((Width - (Top + Bottom)) * (Height - (Left + Right)) * NumChannels * NumViews, sizeof(int16_t));
                if (NewArray != NULL) {
                    for (uint8_t View = 0; View < NumViews - 1; View++) {
                        for (int64_t W = Left; W < Right; W++) {
                            for (int64_t H = Top; H < Bottom; H++) {
                                for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                    NewArray[View][W][H][Channel] = Array[View][W + Left][H + Top][Channel];
                                }
                            }
                        }
                    }
                    ImageContainer_SetArray(Image, (void****) NewArray);
                    free(Array);
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate an array for the cropped image"));
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        if (Image != NULL) {
            if (Image->Type == ImageType_Integer8) {
                free(Image->Pixels.UInteger8);
            } else if (Image->Type == ImageType_Integer16) {
                free(Image->Pixels.UInteger16);
            }
            free(Image);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    typedef struct ImageHistogram {
        union I_Histogram {
            uint16_t ***UInteger16;
            uint8_t  ***UInteger8;
        } I_Histogram;
        uint64_t        NumEntries;
        Image_Types     Type;
    } ImageHistogram;
    
    ImageHistogram *ImageHistogram_Init(ImageContainer *Image) {
        ImageHistogram *Histogram                    = NULL;
        if (Image != NULL) {
            Histogram                                = calloc(1, sizeof(Histogram));
            if (Histogram != NULL) {
                if (Image->Type == ImageType_Integer8) {
                    Histogram->I_Histogram.UInteger8 = calloc(Image->NumChannels, Image->NumViews * Image->Height * Image->Width);
                    if (Histogram->I_Histogram.UInteger8 != NULL) {
                        Histogram->Type              = Image->Type;
                        Histogram->NumEntries        = Image->Height * Image->Width;
                    } else {
                        free(Histogram->I_Histogram.UInteger8);
                        free(Histogram);
                        Log(Log_ERROR, __func__, U8("Couldn't allocate Histogram array"));
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    Histogram->I_Histogram.UInteger16 = calloc(Image->NumChannels, Image->NumViews * Image->Height * Image->Width);
                    if (Histogram->I_Histogram.UInteger16 != NULL) {
                        Histogram->Type              = Image->Type;
                        Histogram->NumEntries        = Image->Height * Image->Width;
                    } else {
                        free(Histogram->I_Histogram.UInteger16);
                        free(Histogram);
                        Log(Log_ERROR, __func__, U8("Couldn't allocate Histogram array"));
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void ***ImageHistogram_GetArray(ImageHistogram *Histogram) {
        void ***Array = NULL;
        if (Histogram != NULL) {
            if (Histogram->Type == ImageType_Integer8) {
                Array = (void***) Histogram->I_Histogram.UInteger8;
            } else if (Histogram->Type == ImageType_Integer16) {
                Array = (void***) Histogram->I_Histogram.UInteger16;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void ImageHistogram_SetArray(ImageHistogram *Histogram, void ***Array) {
        if (Histogram != NULL && Array != NULL) {
            if (Histogram->Type == ImageType_Integer8) {
                Histogram->I_Histogram.UInteger8  = (uint8_t***)  Array;
            } else if (Histogram->Type == ImageType_Integer16) {
                Histogram->I_Histogram.UInteger16 = (uint16_t***) Array;
            }
        } else if (Histogram == NULL) {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        } else if (Array == NULL) {
            Log(Log_ERROR, __func__, U8("Array Pointer is NULL"));
        }
    }
    
    ImageHistogram *ImageHistogram_GenerateHistogram(ImageContainer *Image) {
        ImageHistogram *Histogram       = NULL;
        if (Image != NULL) {
            Histogram                   = ImageHistogram_Init(Image);
            if (Histogram != NULL) {
                uint64_t NumViews       = ImageContainer_GetNumViews(Image);
                uint64_t Width          = ImageContainer_GetWidth(Image);
                uint64_t Height         = ImageContainer_GetHeight(Image);
                uint64_t NumChannels    = ImageContainer_GetNumChannels(Image);
                
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ****ImageArray  = (uint8_t****) Image->Pixels.UInteger8;
                    uint8_t  ***HistArray   = (uint8_t***)  Histogram->I_Histogram.UInteger8;
                    
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t C = 0ULL; C < NumChannels; C++) {
                                    uint8_t Sample              = ImageArray[View][W][H][C];
                                    HistArray[View][C][Sample] += 1;
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t ****ImageArray = (uint16_t****) Image->Pixels.UInteger16;
                    uint16_t  ***HistArray  = (uint16_t***)  Histogram->I_Histogram.UInteger16;
                    
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t C = 0ULL; C < NumChannels; C++) {
                                    uint16_t Sample             = ImageArray[View][W][H][C];
                                    HistArray[View][C][Sample] += 1;
                                }
                            }
                        }
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void ImageHistogram_Deinit(ImageHistogram *Histogram) {
        if (Histogram != NULL) {
            if (Histogram->Type == ImageType_Integer8) {
                free(Histogram->I_Histogram.UInteger8);
            } else if (Histogram->Type == ImageType_Integer16) {
                free(Histogram->I_Histogram.UInteger16);
            }
            free(Histogram);
        } else {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
