/*!
 @header                ContainerIO.h
 @author                Marcus Johnson
 @copyright             2018+
 @version               1.0.0
 @brief                 This header contains code for containers (type generic arrays)
 */

#include <stdbool.h>
#include <stdint.h>

#pragma  once

#ifndef  FoundationIO_ContainerIO_H
#define  FoundationIO_ContainerIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     3D Audio API:
     
     AudioContainer contains X channel audio and metadata required like bit depth, sample rate, etc.
     
     AudioObject is simply an AudioContainer with more metadata about where the sound should be played, when, etc.
     
     that's it.
     */
    
    /*!
     @enum                      Audio_ChannelMask
     @abstract                                                  "Defines the Audio_Types values, OR-able".
     @constant                  AudioMask_Unknown               "Invalid AudioMask, exists solely to tell when it hasn't been set".
     @constant                  AudioMask_FrontLeft             "The channel's location is the front left".
     @constant                  AudioMask_FrontRight            "The channel's location is the front right".
     @constant                  AudioMask_FrontCenter           "The channel's location is the front center".
     @constant                  AudioMask_LFE                   "The channel's contains the Low Frequency Effects for subwoofers".
     @constant                  AudioMask_SurroundLeft          "The channel's location is the side left".
     @constant                  AudioMask_SurroundRight         "The channel's location is the side right".
     @constant                  AudioMask_SurroundCenter        "The channel's location is the side center".
     @constant                  AudioMask_RearLeft              "The channel's location is the rear left".
     @constant                  AudioMask_RearRight             "The channel's location is the rear right".
     @constant                  AudioMask_RearCenter            "The channel's location is the rear center".
     @constant                  AudioMask_FrontCenterLeft       "The channel's location is the front center left".
     @constant                  AudioMask_FrontCenterRight      "The channel's location is the front center right".
     @constant                  AudioMask_TopCenter             "The channel's location is the top center".
     @constant                  AudioMask_TopFrontLeft          "The channel's location is the top front left".
     @constant                  AudioMask_TopFrontRight         "The channel's location is the top front right".
     @constant                  AudioMask_TopFrontCenter        "The channel's location is the top front center".
     @constant                  AudioMask_TopRearLeft           "The channel's location is the top rear left".
     @constant                  AudioMask_TopRearRight          "The channel's location is the top rear right".
     @constant                  AudioMask_TopRearCenter         "The channel's location is the top rear center".
     @constant                  AudioMask_StereoLeft            "RF64 Extension, Stereo Downmix, Left".
     @constant                  AudioMask_StereoRight           "RF64 Extension, Stereo Downmix, Right".
     */
    typedef enum Audio_ChannelMask {
                                AudioMask_Unknown               = 0,
                                AudioMask_FrontLeft             = 1,
                                AudioMask_FrontRight            = 2,
                                AudioMask_FrontCenter           = 4,
                                AudioMask_LFE                   = 8,
                                AudioMask_SurroundLeft          = 16,
                                AudioMask_SurroundRight         = 32,
                                AudioMask_SurroundCenter        = 64,
                                AudioMask_RearLeft              = 128,
                                AudioMask_RearRight             = 256,
                                AudioMask_RearCenter            = 512,
                                AudioMask_FrontCenterLeft       = 1024,
                                AudioMask_FrontCenterRight      = 2048,
                                AudioMask_TopCenter             = 4096,
                                AudioMask_TopFrontLeft          = 8192,
                                AudioMask_TopFrontRight         = 16384,
                                AudioMask_TopFrontCenter        = 32768,
                                AudioMask_TopRearLeft           = 65536,
                                AudioMask_TopRearRight          = 131072,
                                AudioMask_TopRearCenter         = 262144,
                                AudioMask_StereoLeft            = 524288,
                                AudioMask_StereoRight           = 1048576,
    } Audio_ChannelMask;
    
    /*!
     @enum                      Audio_Types
     @abstract                                                  "Defines the Audio_Types values, OR-able".
     @constant                  AudioType_Unknown               "Invalid AudioType, exists solely to tell when it hasn't been set".
     @constant                  AudioType_Unsigned              "The samples are unsigned".
     @constant                  AudioType_Signed                "The samples are signed".
     @constant                  AudioType_Integer8              "The samples values are between 0 and 255        for Unsigned, -128        and 127        for Signed".
     @constant                  AudioType_Integer16             "The samples values are between 0 and 65535      for Unsigned, -32768      and 32767      for Signed".
     @constant                  AudioType_Integer32             "The samples values are between 0 and 4294967295 for Unsigned, -2147483648 and 2147483647 for Signed".
     */
    typedef enum Audio_Types {
                                AudioType_Unknown               = 0,
                                AudioType_Unsigned              = 1,
                                AudioType_Signed                = 2,
                                AudioType_Integer8              = 4,
                                AudioType_Integer16             = 8,
                                AudioType_Integer32             = 16,
    } Audio_Types;
    
    typedef struct          AudioContainer   AudioContainer;
    
    typedef struct          AudioLocation    AudioLocation;
    
    typedef struct          AudioObject      AudioObject;
    
    typedef struct          AudioContainer3D AudioContainer3D;
    
    /*!
     @abstract                             "Initalizes an empty AudioContainer".
     @param                 Type           "A type from AudioContinerTypes".
     @param                 ChannelMask    "The ChannelMask".
     @param                 SampleRate     "The number of samples in one second of audio".
     @param                 NumSamples     "NumSamples is the number of channel independent samples, e.g. X samples is BitDepth * NumChnnels * X".
     */
    AudioContainer         *AudioContainer_Init(Audio_Types Type, Audio_ChannelMask ChannelMask, uint64_t SampleRate, uint64_t NumSamples);
    
    /*!
     @abstract                             "Sets the channel type for each channel index".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Index          "Which index does the channel mask apply to"?
     @param                 ChannelMask    "What is the channel mask"?
     */
    void                    AudioContainer_SetChannelMap(AudioContainer *Audio, uint64_t Index, Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of audio channels".
     @param                 Audio          "AudioContainer Pointer".
     */
    uint8_t                 AudioContainer_GetNumChannels(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of audio channels".
     @param                 ChannelMask    "The channel mask".
     */
    uint8_t                 AudioMask_GetNumChannels(Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of audio channels".
     @param                 Audio          "AudioContainer Pointer".
     @param                 ChannelMask    "The channel mask".
     */
    uint8_t                 AudioContainer_GetIndex(AudioContainer *Audio, Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of channel-agnostic samples in one second".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint64_t                AudioContainer_GetSampleRate(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of bits required to represent a audio sample".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint8_t                 AudioContainer_GetBitDepth(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of channel-agnostic audio samples stored in the container".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint64_t          AudioContainer_GetNumSamples(AudioContainer *Audio);
    uint64_t                AudioContainer_GetNumSamples(AudioContainer *Audio);
    
    /*!
     @abstract                       "Sets the channel type for each channel index".
     @param           Audio          "A pointer to the instance of an AudioContainer".
     @param           Index          "Which index does the channel mask apply to"?
     @abstract                             "Gets the index that channel X is in".
     @remark                               "If the channel is not present in the stream, we return AudioMask_Unknown".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 ChannelMask    "The actual channel you're trying to get".
     */
    uint8_t                 AudioContainer_GetChannelsIndex(AudioContainer *Audio, Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Gets the type of the array contained by the AudioContainer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    Audio_Types             AudioContainer_GetType(AudioContainer *Audio);
    
    /*!
     @abstract                             "Gets a pointer to the array of samples".
     @remark                               "You need to cast the pointer to the correct type you got from AudioContainer_GetType".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    void                  **AudioContainer_GetArray(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the average of the samples in the buffer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Channel        "Which index should we get average"?
     */
    int64_t                 AudioContainer_GetAverage(AudioContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Returns the highest valued sample in the buffer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Channel        "Which index should we get the highest value from"?
     */
    int64_t                 AudioContainer_GetMax(AudioContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Returns the lowest valued sample in the buffer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Channel        "Which index should we get the lowest value from"?
     */
    int64_t                 AudioContainer_GetMin(AudioContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Deinitializes an AudioContainer, and any samples stored within it".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    void                    AudioContainer_Deinit(AudioContainer *Audio);
    
    typedef struct          AudioHistogram AudioHistogram;
    
    /*!
     @abstract                             "Creates a AudioHistogram".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    AudioHistogram         *AudioHistogram_Init(AudioContainer *Audio);
    
    /*!
     @abstract                             "Gets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an AudioHistogram in question".
     @return                               "Returns a pointer to the histogram data".
     */
    void                   *AudioHistogram_GetArray(AudioHistogram *Histogram);
    
    /*!
     @abstract                             "Sets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an AudioHistogram in question".
     @param                 Array          "A pointer to the histogram data".
     */
    void                    AudioHistogram_SetArray(AudioHistogram *Histogram, void *Array);
    
    /*!
     @abstract                             "Generates a histogram from an image".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    AudioHistogram         *AudioHistogram_GenerateHistogram(AudioContainer *Audio);
    
    /*!
     @abstract                             "Sorts a histogram".
     @param                 Histogram      "A pointer to the AudioHistogram in question".
     @param                 SortAscending  "If true, sorts so the most common index is at the top".
     */
    void                    AudioHistogram_Sort(AudioHistogram *Histogram, bool SortAscending);
    
    /*!
     @abstract                             "frees a AudioHistogram".
     @param                 Histogram      "A pointer to the AudioHistogram to deinitalize".
     */
    void                    AudioHistogram_Deinit(AudioHistogram *Histogram);
    
    /*!
     @enum                      Image_ChannelMask
     @abstract                                                  "Defines the ChannelMask values".
     @constant                  ImageMask_Unknown               "Invalid ImageMask, exists solely to tell when it hasn't been set".
     @constant                  ImageMask_2D                    "The image has two dimensions".
     @constant                  ImageMask_Luma                  "The channel contains the Luminance aka Brightness information, also used for single channel images".
     @constant                  ImageMask_Chroma1               "The channel contains the Chrominance aka Color information".
     @constant                  ImageMask_Chroma2               "The channel contains the Chrominance aka Color information".
     @constant                  ImageMask_Chroma3               "The channel contains the Chrominance aka Color information".
     @constant                  ImageMask_Alpha                 "The channel contains the Alpha aka transparency information".
     @constant                  ImageMask_3D_L                  "The image has three dimensions, this one is the left".
     @constant                  ImageMask_3D_R                  "The image has three dimensions, this one is the right".
     @constant                  ImageMask_Red                   "The channel contains the Red    color information".
     @constant                  ImageMask_Green                 "The channel contains the Green  color information".
     @constant                  ImageMask_Blue                  "The channel contains the Blue   color information".
     @constant                  ImageMask_Green2                "The channel contains the Green2 color information, for Bayer filtered images".
     */
    typedef enum Image_ChannelMask {
                                ImageMask_Unknown               = 0,
                                ImageMask_2D                    = 1,
                                ImageMask_Luma                  = 2,
                                ImageMask_Chroma1               = 4,
                                ImageMask_Chroma2               = 8,
                                ImageMask_Chroma3               = 16,
                                ImageMask_Alpha                 = 32,
                                ImageMask_3D_L                  = 64,
                                ImageMask_3D_R                  = 128,
                                ImageMask_Red                   = 256,
                                ImageMask_Green                 = 512,
                                ImageMask_Blue                  = 1024,
                                ImageMask_Green2                = 2048,
    } Image_ChannelMask;
    
    /*!
     @enum                      Image_Types
     @abstract                                                  "Defines the type of image".
     @constant                  ImageType_Unknown               "Invalid ImageType, exists solely to tell when it hasn't been set".
     @constant                  ImageType_Integer8              "The pixels are unsigned 8  bit integers".
     @constant                  ImageType_Integer16             "The pixels are unsigned 16 bit integers".
     */
    typedef enum Image_Types {
                                ImageType_Unknown               = 0,
                                ImageType_Integer8              = 1,
                                ImageType_Integer16             = 2,
    } Image_Types;
    
    typedef struct          ImageContainer ImageContainer;
    
    /*!
     @abstract                             "Initalizes an empty ImageContainer".
     @remark                               "All channels in an image must have the same bit depth, padding will be added if necessary".
     @param                 Type           "The type of array to create".
     @param                 ChannelMask    "The channel mask".
     @param                 Width          "The number of pixels making up one row".
     @param                 Height         "The number of pixels making up one column".
     */
    ImageContainer         *ImageContainer_Init(Image_Types Type, Image_ChannelMask ChannelMask, uint64_t Width, uint64_t Height);
    
    /*!
     @abstract                             "Sets the channel type for each channel index".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Index          "Which index does the channel mask apply to"?
     @param                 ChannelMask    "What is the channel mask"?
     */
    void                    ImageContainer_SetChannelMap(ImageContainer *Image, uint64_t Index, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of pixels in one row of this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetWidth(ImageContainer *Image);
    
    /*!
     @abstract                             "Returns the number of pixels in one column of this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetHeight(ImageContainer *Image);
    
    /*!
     @abstract                             "Returns the number of bits needed to represent the image in this container".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint8_t                 ImageContainer_GetBitDepth(ImageContainer *Image);
    
    /*!
     @abstract                             "Returns the number of views contained in this image".
     @param                 ChannelMask    "The channel mask".
     */
    uint8_t                 ImageMask_GetNumViews(Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of channels contained in this image".
     @remark                               "If there are multiple views, it only returns the number of channels per view".
     @param                 ChannelMask    "The channel mask".
     */
    uint8_t                 ImageMask_GetNumChannels(Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of channels contained in this image".
     @remark                               "If there are multiple views, it only returns the number of channels per view".
     @param                 Image          "ImageContaienr Pointer".
     */
    uint8_t                 ImageContainer_GetNumChannels(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets the channel index for the specified mask".
     @remark                               "If the mask is not present in the ImageContainer, we return the number of channels, which is outside the valid range".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Mask           "Which channel are you trying to access"?
     */
    uint64_t                ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask Mask);
    
    /*!
     @abstract                             "Gets the channel mask".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    Image_ChannelMask       ImageContainer_GetChannelMask(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets the number of views in an ImageContainer".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint8_t                 ImageContainer_GetNumViews(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets the views index for the specified mask".
     @remark                               "If the mask is not present in the ImageContainer, we return the number of views, which is outside the valid range".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Mask           "Which view are you trying to access"?
     */
    uint64_t                ImageContainer_GetViewsIndex(ImageContainer *Image, Image_ChannelMask Mask);
    
    /*!
     @abstract                             "Gets the type of the array contained by the ImageContainer".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    Image_Types             ImageContainer_GetType(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets a pointer to the array of pixels".
     @remark                               "You need to cast the pointer to the correct type you got from ImageType_GetType".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    void                   *ImageContainer_GetArray(ImageContainer *Image);
    
    /*!
     @abstract                             "Sets a pointer to the array of pixels".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Array          "A pointer to the new array".
     */
    void                    ImageContainer_SetArray(ImageContainer *Image, void *Array);
    
    /*!
     @abstract                             "Returns the average value (rounded) of the pixels in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ChannelMask    "Which channel should we get the average from"?
     */
    int64_t                 ImageContainer_GetAverage(ImageContainer *Image, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the highest value pixel in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ChannelMask    "Which channel should we get the maximum from"?
     */
    int64_t                 ImageContainer_GetMax(ImageContainer *Image, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the lowest value pixel in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ChannelMask    "Which channel should we get the minimum from"?
     */
    int64_t                 ImageContainer_GetMin(ImageContainer *Image, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Rotates an Image, either Vertically, Horizontally, or both".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 VerticalFlip   "Top becomes bottom and vice versa".
     @param                 HorizontalFlip "left becomes right and vice versa".
     */
    void                    ImageContainer_Flip(ImageContainer *Image, bool VerticalFlip, bool HorizontalFlip);
    
    /*!
     @abstract                             "Resizes an Image".
     @remark                               "0 means keep the same, -1 means remove one, 1 means add one".
     @remark                               "Left and Right are first because we use the Width, Height format".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Top            "The offset from the top of the image".
     @param                 Bottom         "The offset from the bottom of the image".
     @param                 Left           "The offset from the left of the image".
     @param                 Right          "The offset from the right of the image".
     */
    void                    ImageContainer_Resize(ImageContainer *Image, int64_t Left, int64_t Right, int64_t Top, int64_t Bottom);
    
    /*!
     @abstract                             "Compares two Images".
     @param                 Reference      "A pointer to the reference ImageContainer".
     @param                 Compare        "A pointer to the ImageContainer to compare to the Reference".
     @return                               "Returns an ImageContainer containing (Reference - Compare) % 2^BitDepth".
     */
    ImageContainer         *ImageContainer_Compare(ImageContainer *Reference, ImageContainer *Compare);
    
    /*!
     @abstract                             "Deinitializes the ImageContainer pointed to".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    void                    ImageContainer_Deinit(ImageContainer *Image);
    
    typedef struct          ImageHistogram ImageHistogram;
    
    /*!
     @abstract                             "Creates a ImageHistogram".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    ImageHistogram         *ImageHistogram_Init(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an ImageHistogram in question".
     @return                               "Returns a pointer to the histogram data".
     */
    void                   *ImageHistogram_GetArray(ImageHistogram *Histogram);
    
    /*!
     @abstract                             "Sets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an ImageHistogram in question".
     @param                 Array          "A pointer to the histogram data".
     */
    void                    ImageHistogram_SetArray(ImageHistogram *Histogram, void *Array);
    
    /*!
     @abstract                             "Generates a histogram from an image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    ImageHistogram         *ImageHistogram_GenerateHistogram(ImageContainer *Image);
    
    /*!
     @abstract                             "Sorts a histogram".
     @param                 Histogram      "A pointer to the ImageHistogram in question".
     @param                 SortAscending  "If true, sorts so the most common index is at the top".
     */
    void                    ImageHistogram_Sort(ImageHistogram *Histogram, bool SortAscending);
    
    /*!
     @abstract                             "frees a ImageHistogram".
     @param                 Histogram      "A pointer to the ImageHistogram to deinitalize".
     */
    void                    ImageHistogram_Deinit(ImageHistogram *Histogram);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
