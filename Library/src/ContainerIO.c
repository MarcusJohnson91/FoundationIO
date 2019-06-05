#include "../include/Macros.h"         /* Included for FoundationIO's macros */
#include "../include/ContainerIO.h"    /* Included for our declarations */

#include "../include/Log.h"            /* Included for error reporting */
#include "../include/Math.h"           /* Included for Absolute, Max/Min */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct AudioContainer {
        void             **Samples;
        Audio_ChannelMask *ChannelMap; // So basically it's type is AudioChannelMask and each index contains the enum matching the channel at that index in the array
        uint64_t           NumSamples;
        uint64_t           SampleRate;
        Audio_ChannelMask  ChannelMask;
        uint8_t            BitDepth;
        Audio_Types        Type;
    } AudioContainer;
    
    AudioContainer *AudioContainer_Init(Audio_Types Type, Audio_ChannelMask ChannelMask, uint64_t SampleRate, uint64_t NumSamples) {
        AudioContainer *Audio       = NULL;
        if (NumSamples > 0) {
            Audio                   = calloc(1, sizeof(AudioContainer));
            if (Audio != NULL) {
                uint8_t NumChannels = AudioMask_GetNumChannels(ChannelMask);
                void **Array        = calloc(NumChannels * NumSamples, Type / 4); // !!! DO NOT CHANGE AUDIO_TYPES WITHOUT CHANGING THE SIZE FIELD HERE
                if (Array != NULL) {
                    Audio->Samples  = Array;
                } else {
                    AudioContainer_Deinit(Audio);
                    Log(Log_ERROR, __func__, U8("Couldn't allocate the audio array"));
                }
                
                Audio->ChannelMask  = ChannelMask;
                Audio->SampleRate   = SampleRate;
                Audio->NumSamples   = NumSamples;
            } else {
                AudioContainer_Deinit(Audio);
                Log(Log_ERROR, __func__, U8("Couldn't allocate the AudioContainer"));
            }
        } else if (NumSamples == 0) {
            Log(Log_ERROR, __func__, U8("NumSamples %llu is invalid"), NumSamples);
        }
        return Audio;
    }
    
    void AudioContainer_SetChannelMap(AudioContainer *Audio, uint64_t Channel, Audio_ChannelMask ChannelMask) {
        if (Audio != NULL) {
            uint8_t NumChannels            = AudioMask_GetNumChannels(ChannelMask);
            if (NumChannels == 1) {
                Audio->ChannelMap[Channel] = ChannelMask;
            } else {
                Log(Log_ERROR, __func__, U8("ChannelMask must contain only 1 channel"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    uint8_t AudioContainer_GetBitDepth(AudioContainer *Audio) {
        uint8_t BitDepth = 0ULL;
        if (Audio != NULL) {
            BitDepth     = Audio->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint64_t AudioContainer_GetNumChannels(AudioContainer *Audio) {
        uint64_t NumChannels       = 0ULL;
        if (Audio != NULL) {
            Audio_ChannelMask Mask = Audio->ChannelMask;
            NumChannels            = AudioMask_GetNumChannels(Mask);
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t AudioMask_GetNumChannels(Audio_ChannelMask ChannelMask) {
        uint64_t NumChannels           = 0ULL;
        Audio_ChannelMask ChannelMask2 = ChannelMask;
        do {
            if ((ChannelMask2 & 1) == 1) {
                NumChannels           += 1;
            }
            ChannelMask2             >>= 1;
        } while (ChannelMask > 0);
        return NumChannels;
    }
    
    uint64_t AudioContainer_GetNumSamples(AudioContainer *Audio) {
        uint64_t NumSamples = 0ULL;
        if (Audio != NULL) {
            NumSamples      = Audio->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return NumSamples;
    }
    
    uint8_t AudioContainer_GetChannelsIndex(AudioContainer *Audio, Audio_ChannelMask ChannelMask) {
        uint8_t ChannelMap      = 0;
        if (Audio != NULL) {
            uint8_t NumChannels = AudioContainer_GetNumChannels(Audio);
            ChannelMap          = NumChannels; // In case the mask is not present
            for (uint8_t Channel = 0; Channel < NumChannels - 1; Channel++) {
                if (Audio->ChannelMap[Channel] == ChannelMask) {
                    ChannelMap  = Channel;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return ChannelMap;
    }
    
    Audio_Types AudioContainer_GetType(AudioContainer *Audio) {
        Audio_Types Type = AudioType_Unknown;
        if (Audio != NULL) {
            Type         = Audio->Type;
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void **AudioContainer_GetArray(AudioContainer *Audio) {
        void **AudioArray = NULL;
        if (Audio != NULL) {
            AudioArray    = Audio->Samples;
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
        if (Audio != NULL) {
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
            free(Audio->Samples);
            free(Audio->ChannelMap);
            free(Audio);
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    /*
     AudioObject: 4096 Samples, 24 bits per sample, MONO (all objects are mono), and then there's like effects and direction and shit.
     AudioDirection:
     
     btw should this be called AudioVector because it's an actual vector.
     
     DIRECTION
     */
    
    typedef struct AudioVector {
        void       *Samples;
        uint64_t    NumSamples;
        uint64_t    SampleRate;
        uint64_t    DirectionOffset;
        uint64_t   *Direction;
        uint64_t    NumDirections;
        Audio_Types Type;
    } AudioVector;
    
    typedef struct AudioContainer3D {
        uint64_t     NumObjects;
        AudioVector *Objects;
    } AudioContainer3D;
    
    AudioContainer3D *AudioContainer3D_Init(uint64_t NumObjects) {
        AudioContainer3D *Container       = NULL;
        if (NumObjects > 0) {
            Container                     = calloc(1, sizeof(AudioContainer3D));
            if (Container != NULL) {
                Container->Objects        = calloc(NumObjects, sizeof(AudioVector));
                if (Container->Objects != NULL) {
                    Container->NumObjects = NumObjects;
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't allocate %lld AudioObjects"), NumObjects);
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate AudioContainer3D"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("NumObjects %lld is invalid"), NumObjects);
        }
        return Container;
    }
    
    void AudioContainer3D_SetVector(AudioContainer3D *Container, AudioVector *Vector, uint64_t Index) {
        if (Container != NULL && Vector != NULL && Index < Container->NumObjects) {
            Container->Objects[Index - 1] = *Vector;
        } else if (Container == NULL) {
            Log(Log_ERROR, __func__, U8("AudioContainer3D Pointer is NULL"));
        } else if (Vector == NULL) {
            Log(Log_ERROR, __func__, U8("AudioVector Pointer is NULL"));
        } else if (Index >= Container->NumObjects) {
            Log(Log_ERROR, __func__, U8("Index %llu is larger than %llu"), Index, Container->NumObjects);
        }
    }
    
    typedef struct AudioHistogram {
        void          **Array; // Channel, Sample
        uint64_t        NumEntries;
        Audio_Types     Type;
    } AudioHistogram;
    
    AudioHistogram *AudioHistogram_Init(AudioContainer *Audio) {
        AudioHistogram *Histogram         = NULL;
        if (Audio != NULL) {
            // Create a histogram for each sample value in each channel
            Histogram                     = calloc(1, sizeof(AudioHistogram));
            uint64_t NumChannels          = AudioContainer_GetNumChannels(Audio);
            if (Histogram != NULL) {
                uint64_t NumValues        = Exponentiate(2, Audio->BitDepth);
                if (Audio->BitDepth <= 8) {
                    Histogram->Array      = calloc(NumValues * NumChannels, sizeof(uint8_t));
                } else if (Audio->BitDepth <= 16) {
                    Histogram->Array      = calloc(NumValues * NumChannels, sizeof(uint16_t));
                } else if (Audio->BitDepth <= 32) {
                    Histogram->Array      = calloc(NumValues * NumChannels, sizeof(uint32_t));
                }
                
                if (Histogram->Array != NULL) {
                    Histogram->Type       = Audio->Type;
                    Histogram->NumEntries = NumValues;
                } else {
                    AudioHistogram_Deinit(Histogram);
                    Log(Log_ERROR, __func__, U8("Couldn't allocate AudioHistogram Array"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate AudioHistogram"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void *AudioHistogram_GetArray(AudioHistogram *Histogram) {
        void *Array = NULL;
        if (Histogram != NULL) {
            Array   = Histogram->Array;
        } else {
            Log(Log_ERROR, __func__, U8("AudioHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void AudioHistogram_SetArray(AudioHistogram *Histogram, void *Array) {
        if (Histogram != NULL) {
            Histogram->Array = Array;
        } else {
            Log(Log_ERROR, __func__, U8("AudioHistogram Pointer is NULL"));
        }
    }
    
    AudioHistogram *AudioHistogram_GenerateHistogram(AudioContainer *Audio) {
        AudioHistogram *Histogram                                = NULL;
        if (Audio != NULL) {
            Histogram                                            = AudioHistogram_Init(Audio);
            if (Histogram != NULL) {
                uint8_t  NumChannels                             = AudioContainer_GetNumChannels(Audio);
                
                if (Histogram->Type == AudioType_Integer8) {
                    uint8_t *SampleArray                         = (uint8_t*) Audio->Samples;
                    uint8_t *HistArray                           = (uint8_t*) Histogram->Array;
                    for (uint64_t C = 0ULL; C < NumChannels - 1; C++) {
                        for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                            uint8_t Sample                       = SampleArray[C * S];
                            HistArray[Sample]                   += 1;
                        }
                    }
                } else if (Histogram->Type == AudioType_Integer16) {
                    uint16_t *SampleArray                        = (uint16_t*) Audio->Samples;
                    uint16_t *HistArray                          = (uint16_t*) Histogram->Array;
                    
                    for (uint64_t C = 0ULL; C < NumChannels - 1; C++) {
                        for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                            uint16_t Sample                      = SampleArray[C * S];
                            HistArray[Sample]                   += 1;
                        }
                    }
                } else if (Histogram->Type == AudioType_Integer32) {
                    uint32_t *SampleArray                        = (uint32_t*) Audio->Samples;
                    uint32_t *HistArray                          = (uint32_t*) Histogram->Array;
                    
                    for (uint64_t C = 0ULL; C < NumChannels - 1; C++) {
                        for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                            uint32_t Sample                      = SampleArray[C * S];
                            HistArray[Sample]                   += 1;
                        }
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate AudioHistogram"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void AudioHistogram_Sort(AudioHistogram *Histogram, bool SortAscending) {
        if (Histogram != NULL) {
            uint64_t NumCores = FoundationIO_GetNumCPUCores(); // Now divide Histogram->NumEntries by NumCores (how do we handle rounding?)
            if (SortAscending == Yes) { // Top to bottom
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t  *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint8_t) Max(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint16_t) Max(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                    uint32_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint32_t) Max(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                    int8_t  *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int8_t) Max(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                    int16_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int16_t) Max(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                    int32_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int32_t) Max(Audio[Element - 1], Audio[Element]);
                        }
                    }
                }
            } else { // Bottom to top
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t  *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint8_t) Min(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint16_t) Min(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                    uint32_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint32_t) Min(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                    int8_t  *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int8_t) Min(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                    int16_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int16_t) Min(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                    int32_t *Audio = AudioHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int32_t) Min(Audio[Element - 1], Audio[Element]);
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        }
    }
    
    void AudioHistogram_Deinit(AudioHistogram *Histogram) {
        free(Histogram->Array);
        free(Histogram);
    }
    
    typedef struct ImageContainer {
        void              *Pixels;
        Image_ChannelMask *ChannelMap;
        uint64_t           Width;
        uint64_t           Height;
        Image_ChannelMask  ChannelMask;
        uint8_t            BitDepth;
        Image_Types        Type;
    } ImageContainer;
    
    ImageContainer *ImageContainer_Init(Image_Types Type, Image_ChannelMask ChannelMask, uint64_t Width, uint64_t Height) {
        ImageContainer *Image = NULL;
        if (Width > 0 && Height > 0) {
            Image = calloc(1, sizeof(ImageContainer));
            if (Image != NULL) {
                uint8_t           NumViews    = ImageMask_GetNumViews(ChannelMask);
                uint8_t           NumChannels = ImageMask_GetNumChannels(ChannelMask);
                void *Array                   = calloc(NumViews * NumChannels * Width * Height, Type); // !!!DO NOT CHANGE IMAGE_TYPES WITHOUT CHANGING THE SIZE FIELD HERE
                if (Array != NULL) {
                    Image->Pixels             = Array;
                    Image->ChannelMap         = calloc(NumViews * NumChannels, sizeof(Image_ChannelMask));
                    if (Image->ChannelMap != NULL) {
                        Image->Type           = Type;
                        Image->Width          = Width;
                        Image->Height         = Height;
                    } else {
                        ImageContainer_Deinit(Image);
                        Log(Log_ERROR, __func__, U8("Couldn't allocate channel map"));
                    }
                } else {
                    ImageContainer_Deinit(Image);
                    Log(Log_ERROR, __func__, U8("Couldn't allocate pixel array"));
                }
            } else {
                ImageContainer_Deinit(Image);
                Log(Log_ERROR, __func__, U8("Couldn't allocate the ImageContainer"));
            }
        } else if (Width == 0) {
            Log(Log_ERROR, __func__, U8("Width %llu is invalid"), Width);
        } else if (Height == 0) {
            Log(Log_ERROR, __func__, U8("Height %llu is invalid"), Height);
        }
        return Image;
    }
    
    void ImageContainer_SetChannelMap(ImageContainer *Image, uint64_t Index, Image_ChannelMask ChannelMask) {
        if (Image != NULL) {
            uint8_t NumViews             = ImageMask_GetNumViews(ChannelMask);
            uint8_t NumChannels          = ImageMask_GetNumChannels(ChannelMask);
            if (NumViews == 1 && NumChannels == 1) {
                Image->ChannelMap[Index] = ChannelMask;
            } else {
                Log(Log_ERROR, __func__, U8("ChannelMask must contain exactly 1 view and 1 channel"));
            }
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
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
    
    uint8_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        uint8_t BitDepth = 0ULL;
        if (Image != NULL) {
            BitDepth     = Image->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint8_t ImageMask_GetNumViews(Image_ChannelMask ChannelMask) {
        uint8_t NumViews            = 0;
        Image_ChannelMask JustViews = ChannelMask & (ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R);
        do {
            if ((JustViews & 1) == 1) {
                NumViews           += 1;
            }
            JustViews             >>= 1;
        } while (JustViews > 0);
        return NumViews;
    }
    
    uint64_t ImageMask_GetNumChannels(Image_ChannelMask ChannelMask) {
        uint64_t NumChannels           = 0ULL;
        Image_ChannelMask JustChannels = ChannelMask - (ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R);
        do {
            if ((JustChannels & 1) == 1) {
                NumChannels           += 1;
            }
            JustChannels             >>= 1;
        } while (JustChannels > 0);
        return NumChannels;
    }
    
    uint64_t ImageContainer_GetNumChannels(ImageContainer *Image) {
        uint64_t NumChannels       = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask Mask = Image->ChannelMask;
            NumChannels            = ImageMask_GetNumChannels(Mask);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint64_t Index                = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask Channel = Mask - (ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R);
            uint8_t NumViews          = ImageMask_GetNumViews(Mask);
            uint8_t NumChannels       = ImageMask_GetNumChannels(Mask);
            Index                     = NumChannels; // in case the channel isn't present, return this which is an invalid index
            if (NumViews == 1 && NumChannels == 1) {
                for (uint64_t Channels = 0ULL; Channels < NumChannels - 1; Channels++) {
                    if (Image->ChannelMap[Channels] == Channel) {
                        Index         = Channels;
                    }
                }
            } else if (NumViews != 1) {
                Log(Log_ERROR, __func__, U8("Mask must contain 1 and only 1 view"));
            } else if (NumChannels != 1) {
                Log(Log_ERROR, __func__, U8("Mask must contain 1 and only 1 channel"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    Image_ChannelMask ImageContainer_GetChannelMask(ImageContainer *Image) {
        Image_ChannelMask Mask    = 0;
        if (Image != NULL) {
            Mask                  = Image->ChannelMask;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Mask;
    }
    
    uint8_t ImageContainer_GetNumViews(ImageContainer *Image) {
        uint8_t NumViews           = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask Mask = Image->ChannelMask;
            NumViews               = ImageMask_GetNumViews(Mask);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return NumViews;
    }
    
    uint64_t ImageContainer_GetViewsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint64_t Index          = 0ULL;
        if (Image != NULL) {
            uint8_t NumViews    = ImageContainer_GetNumViews(Image);
            uint8_t NumChannels = ImageContainer_GetNumChannels(Image);
            Index               = NumChannels;
            for (uint64_t ViewIndex = 0ULL; ViewIndex < NumViews - 1; ViewIndex++) {
                if (Image->ChannelMap[ViewIndex] == (Mask & 0x7)) {
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
    
    void *ImageContainer_GetArray(ImageContainer *Image) {
        void *ImageArray = NULL;
        if (Image != NULL) {
            ImageArray   = Image->Pixels;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return ImageArray;
    }
    
    void ImageContainer_SetArray(ImageContainer *Image, void *Array) {
        if (Image != NULL) {
            Image->Pixels = Array;
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    int64_t ImageContainer_GetAverage(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        int64_t Average      = 0LL;
        if (Image != NULL) {
            uint64_t Channel = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View    = ImageContainer_GetViewsIndex(Image, ChannelMask);
            uint64_t NumChannels = ImageMask_GetNumChannels(ChannelMask);
            if (Channel < NumChannels - 1) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t *Array = (uint8_t*) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            Average += Array[View * Width * Height * Channel];
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *Array = (uint16_t*) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            Average += Array[View * Width * Height * Channel];
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
            if (Channel < ImageMask_GetNumChannels(ChannelMask) - 1) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t *Array = (uint8_t*) ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            if (Array[View * Width * Height * Channel] > Maximum) {
                                Maximum = Array[View * Width * Height * Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *Array = (uint16_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            if (Array[View * Width * Height * Channel] > Maximum) {
                                Maximum = Array[View * Width * Height * Channel];
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
            if (Channel < ImageMask_GetNumChannels(ChannelMask) - 1) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  *Array = (uint8_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            if (Array[View * Width * Height * Channel] < Minimum) {
                                Minimum = Array[View * Width * Height * Channel];
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t  *Array = (uint16_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                        for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                            if (Array[View * Width * Height * Channel] < Minimum) {
                                Minimum = Array[View * Width * Height * Channel];
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
            uint8_t NumChannels = ImageContainer_GetNumChannels(Image);
            uint8_t NumViews    = ImageContainer_GetNumViews(Image);
            if (VerticalFlip == Yes) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  *Array = (uint8_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews - 1; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height - 1; TopLine++) {
                                for (uint64_t BottomLine = Image->Height - 1; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                        uint8_t TopPixel                        = Array[View * Width * TopLine * Channel];
                                        uint8_t BottomPixel                     = Array[View * Width * BottomLine * Channel];
                                        
                                        Array[View * Width * TopLine * Channel]    = BottomPixel;
                                        Array[View * Width * BottomLine * Channel] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *Array = (uint16_t*) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews - 1; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width - 1; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height - 1; TopLine++) {
                                for (uint64_t BottomLine = Image->Height - 1; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                        uint16_t TopPixel                          = Array[View * Width * TopLine * Channel];
                                        uint16_t BottomPixel                       = Array[View * Width * BottomLine * Channel];
                                        
                                        Array[View * Width * TopLine * Channel]    = BottomPixel;
                                        Array[View * Width * BottomLine * Channel] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (HorizontalFlip == Yes) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  *Array = (uint8_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 1ULL; View <= NumViews - 1; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width - 1; Left++) {
                            for (uint64_t Right = Image->Width - 1; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                        uint8_t LeftPixel                   = Array[View * Left * Height * Channel];
                                        uint8_t RightPixel                  = Array[View * Right * Height * Channel];
                                        
                                        Array[View * Left * Height * Channel]  = RightPixel;
                                        Array[View * Right * Height * Channel] = LeftPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *Array = (uint16_t*) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews - 1; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width - 1; Left++) {
                            for (uint64_t Right = Image->Width - 1; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height - 1; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                        uint16_t LeftPixel                   = Array[View * Left * Height * Channel];
                                        uint16_t RightPixel                  = Array[View * Right * Height * Channel];
                                        
                                        Array[View * Left * Height * Channel]  = RightPixel;
                                        Array[View * Right * Height * Channel] = LeftPixel;
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
    
    void ImageContainer_Resize(ImageContainer *Image, int64_t Left, int64_t Right, int64_t Top, int64_t Bottom) {
        if (Image != NULL) {
            uint8_t  NumChannels = ImageContainer_GetNumChannels(Image);
            uint8_t  NumViews    = ImageContainer_GetNumViews(Image);
            uint64_t Height      = ImageContainer_GetHeight(Image);
            uint64_t Width       = ImageContainer_GetWidth(Image);
            Image_Types Type     = ImageContainer_GetType(Image);
            
            int64_t NewWidth     = 0;
            int64_t NewHeight    = 0;
            
            int64_t LeftOffset   = AbsoluteI(Left);
            int64_t RightOffset  = Width + Right;
            int64_t TopOffset    = AbsoluteI(Top);
            int64_t BottomOffset = Height + Bottom;
            
            void *New            = calloc(NumViews * NewWidth * NewHeight * NumChannels, Type);
            if (New != NULL) {
                if (Type == ImageType_Integer8) {
                    uint8_t *Array    = (uint8_t*) ImageContainer_GetArray(Image);
                    uint8_t *NewArray = (uint8_t*) New;
                    if (Array != NULL) {
                        for (uint8_t View = 0; View < NumViews - 1; View++) {
                            for (int64_t W = LeftOffset; W < RightOffset; W++) {
                                for (int64_t H = TopOffset; H < BottomOffset; H++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels - 1; Channel++) {
                                        NewArray[View * (W - LeftOffset) * (H - TopOffset) * Channel] = Array[View * W * H * Channel];
                                    }
                                }
                            }
                        }
                        ImageContainer_SetArray(Image, (void*) NewArray);
                        free(Array);
                    } else {
                        Log(Log_ERROR, __func__, U8("Couldn't allocate an array for the cropped image"));
                    }
                } else if (Type == ImageType_Integer16) {
                    uint16_t *Array    = (uint16_t*) ImageContainer_GetArray(Image);
                    uint16_t *NewArray = (uint16_t*) New;
                    if (Array != NULL) {
                        for (uint8_t View = 1; View <= NumViews; View++) {
                            for (int64_t W = LeftOffset; W < RightOffset; W++) {
                                for (int64_t H = TopOffset; H < BottomOffset; H++) {
                                    for (uint64_t Channel = 1ULL; Channel <= NumChannels; Channel++) {
                                        NewArray[View * (W - LeftOffset) * (H - TopOffset) * Channel] = Array[View * W * H * Channel];
                                    }
                                }
                            }
                        }
                        ImageContainer_SetArray(Image, (void*) NewArray);
                        free(Array);
                    } else {
                        Log(Log_ERROR, __func__, U8("Couldn't allocate an array for the cropped image"));
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate resized array"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    ImageContainer *ImageContainer_Compare(ImageContainer *Reference, ImageContainer *Compare) {
        ImageContainer *Difference     = NULL;
        if (Reference != NULL && Compare != NULL) {
            Image_Types RefType        = ImageContainer_GetType(Reference);
            Image_Types ComType        = ImageContainer_GetType(Compare);
            uint8_t     RefNumViews    = ImageContainer_GetNumViews(Reference);
            uint8_t     ComNumViews    = ImageContainer_GetNumViews(Compare);
            uint8_t     RefNumChannels = ImageContainer_GetNumChannels(Reference);
            uint8_t     ComNumChannels = ImageContainer_GetNumChannels(Compare);
            uint64_t    RefHeight      = ImageContainer_GetHeight(Reference);
            uint64_t    ComHeight      = ImageContainer_GetHeight(Compare);
            uint64_t    RefWidth       = ImageContainer_GetWidth(Reference);
            uint64_t    ComWidth       = ImageContainer_GetWidth(Compare);
            if (RefType == ComType && RefNumViews == ComNumViews && RefNumChannels == ComNumChannels && RefHeight == ComHeight && RefWidth == ComWidth) {
                Difference          = ImageContainer_Init(RefType, ImageContainer_GetChannelMask(Reference), RefWidth, RefHeight);
                if (Difference != NULL) {
                    if (RefType == ImageType_Integer8 && ComType == ImageType_Integer8) {
                        uint8_t *RefArray = ImageContainer_GetArray(Reference);
                        uint8_t *ComArray = ImageContainer_GetArray(Compare);
                        uint8_t *DifArray = ImageContainer_GetArray(Difference);
                        for (uint8_t View = 0; View < RefNumViews - 1; View++) {
                            for (uint64_t Width = 0ULL; Width < RefWidth - 1; Width++) {
                                for (uint64_t Height = 0ULL; Height < RefHeight - 1; Height++) {
                                    for (uint8_t Channel = 0; Channel < RefNumChannels - 1; Channel++) {
                                        DifArray[View * Width * Height * Channel] = (RefArray[View * Width * Height * Channel] - ComArray[View * Width * Height * Channel]) % 256;
                                    }
                                }
                            }
                        }
                    } else if (RefType == ImageType_Integer16 && ComType == ImageType_Integer16) {
                        uint16_t *RefArray = ImageContainer_GetArray(Reference);
                        uint16_t *ComArray = ImageContainer_GetArray(Compare);
                        uint16_t *DifArray = ImageContainer_GetArray(Difference);
                        for (uint8_t View = 0; View < RefNumViews - 1; View++) {
                            for (uint64_t Width = 0ULL; Width < RefWidth - 1; Width++) {
                                for (uint64_t Height = 0ULL; Height < RefHeight - 1; Height++) {
                                    for (uint8_t Channel = 0; Channel < RefNumChannels - 1; Channel++) {
                                        DifArray[View * Width * Height * Channel] = (RefArray[View * Width * Height * Channel] - ComArray[View * Width * Height * Channel]) % 65536;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Couldn't Allocate Difference"));
                }
            } else if (RefType != ComType) {
                Log(Log_ERROR, __func__, U8("Reference Type %d does not match Compare Type %d"), RefType, ComType);
            } else if (RefNumViews != ComNumViews) {
                Log(Log_ERROR, __func__, U8("Reference NumViews %d does not match Compare NumViews %d"), RefNumViews, ComNumViews);
            } else if (RefNumChannels != ComNumChannels) {
                Log(Log_ERROR, __func__, U8("Reference NumChannels %d does not match Compare NumChannels %d"), RefNumChannels, ComNumChannels);
            } else if (RefHeight != ComHeight) {
                Log(Log_ERROR, __func__, U8("Reference Height %lld does not match Compare Height %lld"), RefHeight, ComHeight);
            } else if (RefWidth != ComWidth) {
                Log(Log_ERROR, __func__, U8("Reference Width %lld does not match Compare Width %lld"), RefWidth, ComWidth);
            }
        } else if (Reference == NULL) {
            Log(Log_ERROR, __func__, U8("Reference Pointer is NULL"));
        } else if (Compare == NULL) {
            Log(Log_ERROR, __func__, U8("Compare Pointer is NULL"));
        }
        return Difference;
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        if (Image != NULL) {
            free(Image->Pixels);
            free(Image->ChannelMap);
            free(Image);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    typedef struct ImageHistogram {
        void           *Array;
        uint64_t        NumEntries;
        Image_Types     Type;
    } ImageHistogram;
    
    ImageHistogram *ImageHistogram_Init(ImageContainer *Image) {
        ImageHistogram *Histogram                    = NULL;
        if (Image != NULL) {
            Histogram                                = calloc(1, sizeof(Histogram));
            if (Histogram != NULL) {
                uint8_t NumViews                     = ImageContainer_GetNumViews(Image);
                uint8_t NumChannels                  = ImageContainer_GetNumChannels(Image);
                if (Image->Type == ImageType_Integer8) {
                    uint8_t *HistogramArray          = calloc(256 * NumViews * NumChannels, sizeof(uint8_t));
                    
                    Histogram->Array                 = HistogramArray;
                    if (HistogramArray != NULL) {
                        Histogram->Type              = Image->Type;
                        Histogram->NumEntries        = NumViews * NumChannels;
                    } else {
                        ImageHistogram_Deinit(Histogram);
                        Log(Log_ERROR, __func__, U8("Couldn't allocate Histogram array"));
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *HistogramArray         = calloc(65536 * NumViews * NumChannels, sizeof(uint16_t));
                    
                    Histogram->Array                 = HistogramArray;
                    
                    if (HistogramArray != NULL) {
                        Histogram->Type              = Image->Type;
                        Histogram->NumEntries        = NumViews * NumChannels * 65536;
                    } else {
                        ImageHistogram_Deinit(Histogram);
                        Log(Log_ERROR, __func__, U8("Couldn't allocate Histogram array"));
                    }
                }
            } else {
                ImageHistogram_Deinit(Histogram);
                Log(Log_ERROR, __func__, U8("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void *ImageHistogram_GetArray(ImageHistogram *Histogram) {
        void *Array = NULL;
        if (Histogram != NULL) {
            if (Histogram->Type == ImageType_Integer8) {
                Array = Histogram->Array;
            } else if (Histogram->Type == ImageType_Integer16) {
                Array = Histogram->Array;
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void ImageHistogram_SetArray(ImageHistogram *Histogram, void *Array) {
        if (Histogram != NULL && Array != NULL) {
            Histogram->Array = Array;
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
                uint8_t  NumViews                                = ImageContainer_GetNumViews(Image);
                uint8_t  NumChannels                             = ImageContainer_GetNumChannels(Image);
                uint64_t Width                                   = ImageContainer_GetWidth(Image);
                uint64_t Height                                  = ImageContainer_GetHeight(Image);
                
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t *ImageArray                          = (uint8_t*) Image->Pixels;
                    uint8_t *HistArray                           = (uint8_t*) Histogram->Array;
                    
                    for (uint64_t View = 0ULL; View < NumViews - 1; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t C = 0ULL; C < NumChannels - 1; C++) {
                                    uint8_t Sample               = ImageArray[View * W * H * C];
                                    HistArray[Sample]           += 1;
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t *ImageArray                         = (uint16_t*) Image->Pixels;
                    uint16_t *HistArray                          = (uint16_t*) Histogram->Array;
                    
                    for (uint64_t View = 0ULL; View < NumViews - 1; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t C = 0ULL; C < NumChannels - 1; C++) {
                                    uint16_t Sample              = ImageArray[View * W * H * C];
                                    HistArray[Sample]           += 1;
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
    
    void ImageHistogram_Sort(ImageHistogram *Histogram, bool SortAscending) {
        if (Histogram != NULL) { // Our sorting algorithm will be stable, so values that are tied, will not be moved
            uint64_t NumCores = FoundationIO_GetNumCPUCores();
            if (SortAscending == Yes) { // Top to bottom
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t  *Image = ImageHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Image[Element - 1] = (uint8_t) Max(Image[Element - 1], Image[Element]);
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t *Image = ImageHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Image[Element - 1] = (uint16_t) Max(Image[Element - 1], Image[Element]);
                        }
                    }
                }
            } else { // Bottom to top
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t  *Image = ImageHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Image[Element - 1] = (uint8_t) Min(Image[Element - 1], Image[Element]);
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t *Image = ImageHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Image[Element - 1] = (uint16_t) Min(Image[Element - 1], Image[Element]);
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        }
    }
    
    void ImageHistogram_Deinit(ImageHistogram *Histogram) {
        if (Histogram != NULL) {
            free(Histogram->Array);
            free(Histogram);
        } else {
            Log(Log_ERROR, __func__, U8("ImageHistogram Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
