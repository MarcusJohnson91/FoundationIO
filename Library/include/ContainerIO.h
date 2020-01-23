/*!
 @header              ContainerIO.h
 @author              Marcus Johnson
 @copyright           2018+
 @version             1.1.0
 @brief               This header contains code for containers (type generic arrays)
 */

#include "../include/Macros.h"

#pragma  once

#ifndef  FoundationIO_ContainerIO_H
#define  FoundationIO_ContainerIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @enum                  AudioChannelMask
     @abstract                                              "Defines the Audio_Types values, OR-able".
     @constant              AudioMask_Unknown               "Invalid AudioMask, exists solely to tell when it hasn't been set".
     @constant              AudioMask_FrontLeft             "The channel's location is the front left".
     @constant              AudioMask_FrontRight            "The channel's location is the front right".
     @constant              AudioMask_FrontCenter           "The channel's location is the front center".
     @constant              AudioMask_LFE                   "The channel's contains the Low Frequency Effects for subwoofers".
     @constant              AudioMask_SurroundLeft          "The channel's location is the side left".
     @constant              AudioMask_SurroundRight         "The channel's location is the side right".
     @constant              AudioMask_SurroundCenter        "The channel's location is the side center".
     @constant              AudioMask_RearLeft              "The channel's location is the rear left".
     @constant              AudioMask_RearRight             "The channel's location is the rear right".
     @constant              AudioMask_RearCenter            "The channel's location is the rear center".
     @constant              AudioMask_FrontCenterLeft       "The channel's location is the front center left".
     @constant              AudioMask_FrontCenterRight      "The channel's location is the front center right".
     @constant              AudioMask_TopCenter             "The channel's location is the top center".
     @constant              AudioMask_TopFrontLeft          "The channel's location is the top front left".
     @constant              AudioMask_TopFrontRight         "The channel's location is the top front right".
     @constant              AudioMask_TopFrontCenter        "The channel's location is the top front center".
     @constant              AudioMask_TopRearLeft           "The channel's location is the top rear left".
     @constant              AudioMask_TopRearRight          "The channel's location is the top rear right".
     @constant              AudioMask_TopRearCenter         "The channel's location is the top rear center".
     @constant              AudioMask_StereoLeft            "RF64 Extension, Stereo Downmix, Left".
     @constant              AudioMask_StereoRight           "RF64 Extension, Stereo Downmix, Right".
     */
    typedef enum AudioChannelMask {
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
    } AudioChannelMask;
    
    /*!
     @enum                  Audio_Types
     @abstract                                              "Defines the Audio_Types values, OR-able".
     @constant              AudioType_Unknown               "Invalid AudioType, exists solely to tell when it hasn't been set".
     @constant              AudioType_Unsigned              "The samples are unsigned".
     @constant              AudioType_Signed                "The samples are signed".
     @constant              AudioType_Integer8              "The samples values are between 0 and 255        for Unsigned, -128        and 127        for Signed".
     @constant              AudioType_Integer16             "The samples values are between 0 and 65535      for Unsigned, -32768      and 32767      for Signed".
     @constant              AudioType_Integer32             "The samples values are between 0 and 4294967295 for Unsigned, -2147483648 and 2147483647 for Signed".
     */
    typedef enum Audio_Types {
                            AudioType_Unknown               = 0,
                            AudioType_Unsigned              = 1,
                            AudioType_Signed                = 2,
                            AudioType_Integer8              = 4,
                            AudioType_Integer16             = 8,
                            AudioType_Integer32             = 16,
    } Audio_Types;
    
    /*!
     @enum                  Image_ChannelMask
     @abstract                                              "Defines the ChannelMask values".
     @constant              ImageMask_Unknown               "Invalid ImageMask, exists solely to tell when it hasn't been set".
     @constant              ImageMask_2D                    "The image has two dimensions".
     @constant              ImageMask_Luma                  "The channel contains the Luminance aka Brightness information, also used for single channel images".
     @constant              ImageMask_Chroma1               "The channel contains the Chrominance aka Color information".
     @constant              ImageMask_Chroma2               "The channel contains the Chrominance aka Color information".
     @constant              ImageMask_Chroma3               "The channel contains the Chrominance aka Color information".
     @constant              ImageMask_Alpha                 "The channel contains the Alpha aka transparency information".
     @constant              ImageMask_3D_L                  "The image has three dimensions, this one is the left".
     @constant              ImageMask_3D_R                  "The image has three dimensions, this one is the right".
     @constant              ImageMask_Red                   "The channel contains the Red    color information".
     @constant              ImageMask_Green                 "The channel contains the Green  color information".
     @constant              ImageMask_Blue                  "The channel contains the Blue   color information".
     @constant              ImageMask_Green2                "The channel contains the Green2 color information, for Bayer filtered images".
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
     @enum                  Image_Types
     @abstract                                              "Defines the type of image".
     @constant              ImageType_Unknown               "Invalid ImageType, exists solely to tell when it hasn't been set".
     @constant              ImageType_Integer8              "The pixels are unsigned 8  bit integers".
     @constant              ImageType_Integer16             "The pixels are unsigned 16 bit integers".
     */
    typedef enum Image_Types {
                            ImageType_Unknown               = 0,
                            ImageType_Integer8              = 1,
                            ImageType_Integer16             = 2,
    } Image_Types;
    
    /*!
     @enum                  FlipTypes
     @abstract                                              "Defines the type of flipping".
     @constant              FlipType_Unknown                "Invalid ImageType, exists solely to tell when it hasn't been set".
     @constant              FlipType_Vertical               "Flip the imge vertically, up and down".
     @constant              FlipType_Horizontal             "Flip the image horizontally, side to side".
     @constant              FlipType_VerticalAndHorizontal  "Flip the image in both ways".
     */
    typedef enum FlipTypes {
                            FlipType_Unknown                = 0,
                            FlipType_Vertical               = 1,
                            FlipType_Horizontal             = 2,
                            FlipType_VerticalAndHorizontal  = 3,
    } FlipTypes;
    
    /*!
     @enum                  SortTypes
     @abstract                                              "Defines the type of sorting".
     @constant              SortType_Unknown                "Invalid SortType, exists solely to tell when it hasn't been set".
     @constant              SortType_Ascending              "Index 0 contains the most common value".
     @constant              SortType_Descending             "Index 0 contains the least common value".
     */
    typedef enum SortTypes {
                            SortType_Unknown                = 0,
                            SortType_Ascending              = 1,
                            SortType_Descending             = 2,
    } SortTypes;
    
    typedef struct          Audio2DContainer     Audio2DContainer;
    
    typedef struct          AudioChannelMap      AudioChannelMap;
    
    typedef struct          Audio2DHistogram     Audio2DHistogram;
    
    typedef struct          Audio3DContainer     Audio3DContainer;
    
    typedef struct          AudioVector          AudioVector;
    
    typedef struct          AudioVectorHistogram AudioVectorHistogram;
    
    typedef struct          ImageContainer       ImageContainer;
    
    typedef struct          ImageChannelMap      ImageChannelMap;
    
    typedef struct          ImageHistogram       ImageHistogram;
    
    /*!
     @abstract                             "Creates an empty Audio2DContainer".
     @param                 Type           "A type from Audio_Types".
     @param                 ChannelMap     "Array of ChannelMasks, one array entry for each channel".
     @param                 SampleRate     "The number of samples in one second of audio".
     @param                 NumSamples     "NumSamples is the number of channel independent samples, e.g. X samples is BitDepth * NumChannels * X".
     */
    Audio2DContainer       *Audio2DContainer_Init(Audio_Types Type, AudioChannelMap *ChannelMap, uint64_t SampleRate, uint64_t NumSamples);
    
    /*!
     @abstract                             "Gets a pointer to the ChannelMap".
     @param                 Audio          "Audio2DContainer Pointer".
     @return                               "Returns a pointer to the ChannelMap".
     */
    AudioChannelMap        *Audio2DContainer_GetChannelMap(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of audio channels".
     @param                 Audio          "Audio2DContainer Pointer".
     */
    uint64_t                Audio2DContainer_GetNumChannels(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Returns the ChannelMask for Index".
     @param                 ChannelMap     "Audio2DContainer Pointer".
     @param                 Index          "The channel index to get the mask for".
     */
    AudioChannelMask        AudioChannelMap_GetMask(AudioChannelMap *ChannelMap, uint64_t Index);
    
    /*!
     @abstract                             "Returns the number of channel-agnostic samples in one second".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    uint64_t                Audio2DContainer_GetSampleRate(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of bits required to represent a audio sample".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    uint8_t                 Audio2DContainer_GetBitDepth(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of channel-agnostic audio samples stored in the container".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    uint64_t                Audio2DContainer_GetNumSamples(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Gets the type of the array contained by the Audio2DContainer".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    Audio_Types             Audio2DContainer_GetType(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Gets a pointer to the array of samples".
     @remark                               "You need to cast the pointer to the correct type you got from Audio2DContainer_GetType".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    void                  **Audio2DContainer_GetArray(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Returns the average of the samples in the buffer".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     @param                 Channel        "Which index should we get average"?
     */
    int64_t                 Audio2DContainer_GetAverage(Audio2DContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Returns the highest valued sample in the buffer".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     @param                 Channel        "Which index should we get the highest value from"?
     */
    int64_t                 Audio2DContainer_GetMax(Audio2DContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Returns the lowest valued sample in the buffer".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     @param                 Channel        "Which index should we get the lowest value from"?
     */
    int64_t                 Audio2DContainer_GetMin(Audio2DContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Securely erases an Audio2DContainer".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    void                    Audio2DContainer_Erase(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Deletes an Audio2DContainer, and any samples stored within it".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     */
    void                    Audio2DContainer_Deinit(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Creates a Audio2DHistogram".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    Audio2DHistogram       *Audio2DHistogram_Init(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Gets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an Audio2DHistogram in question".
     @return                               "Returns a pointer to the histogram data".
     */
    void                   *Audio2DHistogram_GetArray(Audio2DHistogram *Histogram);
    
    /*!
     @abstract                             "Sets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an Audio2DHistogram in question".
     @param                 Array          "A pointer to the histogram data".
     */
    void                    Audio2DHistogram_SetArray(Audio2DHistogram *Histogram, void *Array);
    
    /*!
     @abstract                             "Generates a histogram from an Audio2DContainer".
     @param                 Audio          "A pointer to the instance of an Audio2DContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    Audio2DHistogram       *Audio2DHistogram_Generate(Audio2DContainer *Audio);
    
    /*!
     @abstract                             "Sorts a histogram".
     @param                 Histogram      "A pointer to the Audio2DHistogram in question".
     @param                 Sort           "The kind of sorting to do".
     */
    void                    Audio2DHistogram_Sort(Audio2DHistogram *Histogram, SortTypes Sort);
    
    /*!
     @abstract                             "Securely erases an Audio2DContainer".
     @param                 Histogram      "A pointer to the instance of an Audio2DContainer in question".
     */
    void                    Audio2DHistogram_Erase(Audio2DHistogram *Histogram);
    
    /*!
     @abstract                             "frees a Audio2DHistogram".
     @param                 Histogram      "A pointer to the Audio2DHistogram to deinitalize".
     */
    void                    Audio2DHistogram_Deinit(Audio2DHistogram *Histogram);
    
    /*!
     @abstract                             "Creates a AudioChannelMap".
     @param                 NumChannels    "The number of channels".
     @return                               "Returns the newly created ChannelMap".
     */
    AudioChannelMap        *AudioChannelMap_Init(uint64_t NumChannels);
    
    /*!
     @abstract                             "Adds a AudioChannelMask at the specified Index to the ChannelMap".
     @param                 ChannelMap     "The number of channels".
     @param                 Index          "The index in the ChannelMap to add the mask".
     @param                 Mask           "The ChannelMask for the index".
     */
    void                    AudioChannelMap_AddMask(AudioChannelMap *ChannelMap, uint64_t Index, AudioChannelMask Mask);
    
    /*!
     @abstract                             "Deletes a ChannelMap".
     @param                 ChannelMap     "The AudioChannelMap to delete".
     */
    void                    AudioChannelMap_Deinit(AudioChannelMap *ChannelMap);
    
    /*!
     @abstract                             "Creates an empty Audio3DContainer".
     @return                               "Returns a pointer to an Audio3DContainer".
     */
    Audio3DContainer       *Audio3DContainer_Init(void);
    
    /*!
     @abstract                             "Adds a AudioVector to an Audio3DContainer".
     @param                 Container      "The Audio3DContainer to add the AudioVector to".
     @param                 Vector         "The AudioVector to add".
     @param                 Index          "The order of the AudioVector".
     */
    void                    Audio3DContainer_SetVector(Audio3DContainer *Container, AudioVector *Vector, uint64_t Index);
    
    /*!
     @abstract                             "Gets a pointer to an AudioVector".
     @param                 Container      "The Audio3DContainer to get the AudioVector from".
     @param                 Index          "The AudioVector to get".
     */
    AudioVector            *Audio3DContainer_GetVector(Audio3DContainer *Container, uint64_t Index);
    
    /*!
     @abstract                             "Securely erases an Audio3DContainer".
     @param                 Container      "A pointer to the instance of the Audio3DContainer in question".
     */
    void                    Audio3DContainer_Erase(Audio3DContainer *Container);
    
    /*!
     @abstract                             "frees a Audio3DContainer".
     @param                 Container      "A pointer to the Audio3DContainer to deinitalize".
     */
    void                    Audio3DContainer_Deinit(Audio3DContainer *Container);
    
    /*!
     @abstract                             "Creates an empty AudioVector".
     */
    AudioVector            *AudioVector_Init(void);
    
    /*!
     @abstract                             "Gets a Pointer to an AudioVector".
     @param                 Vector         "A pointer to the Audio3DContainer to deinitalize".
     */
    void                   *AudioVector_GetArray(AudioVector *Vector);
    
    /*!
     @abstract                             "Sets a Pointer to the samples for an AudioVector".
     @param                 Vector         "A pointer to the AudioVector to set".
     @param                 Array          "A pointer to the sample data".
     */
    void                    AudioVector_SetArray(AudioVector *Vector, void *Array);
    
    /*!
     @abstract                             "Zeros an AudioVector".
     @param                 Vector         "The AudioVector to zero".
     */
    void                    AudioVector_Erase(AudioVector *Vector);
    
    /*!
     @abstract                             "Destroys an AudioVector".
     @param                 Vector         "The AudioVector to destroy".
     */
    void                    AudioVector_Deinit(AudioVector *Vector);
    
    /*!
     @abstract                             "Creates an empty AudioVectorHistogram".
     @param                 Vector         "The AudioVector to create the histogram from".
     */
    AudioVectorHistogram   *AudioVectorHistogram_Init(AudioVector *Vector);
    
    /*!
     @abstract                             "Gets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an AudioVectorHistogram in question".
     @return                               "Returns a pointer to the histogram data".
     */
    void                   *AudioVectorHistogram_GetArray(AudioVectorHistogram *Histogram);
    
    /*!
     @abstract                             "Sets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an AudioVectorHistogram in question".
     @param                 Array          "A pointer to the histogram data".
     */
    void                    AudioVectorHistogram_SetArray(AudioVectorHistogram *Histogram, void *Array);
    
    /*!
     @abstract                             "Generates a histogram from an AudioVector".
     @param                 Vector         "A pointer to the instance of an AudioVector in question".
     @return                               "Returns the newly created Histogram".
     */
    AudioVectorHistogram   *AudioVectorHistogram_Generate(AudioVector *Vector);
    
    /*!
     @abstract                             "Sorts a histogram".
     @param                 Histogram      "A pointer to the AudioVectorHistogram in question".
     @param                 Sort           "The kind of sorting to do".
     */
    void                    AudioVectorHistogram_Sort(AudioVectorHistogram *Histogram, SortTypes Sort);
    
    /*!
     @abstract                             "Zeros an AudioVectorHistogram".
     @param                 Histogram      "The AudioVector to zero".
     */
    void                    AudioVectorHistogram_Erase(AudioVectorHistogram *Histogram);
    
    /*!
     @abstract                             "Destroys an AudioVectorHistogram".
     @param                 Histogram      "The AudioVectorHistogram to destroy".
     */
    void                    AudioVectorHistogram_Deinit(AudioVectorHistogram *Histogram);
    
    /*!
     @abstract                             "Creates an empty ImageChannelMap".
     @param                 NumViews       "The number of views in the Image".
     @param                 NumChannels    "The number of channels in the Image".
     */
    ImageChannelMap        *ImageChannelMap_Init(uint8_t NumViews, uint8_t NumChannels);
    
    /*!
     @abstract                             "Gets the number of views in the ChannelMap".
     @param                 ChannelMap     "The ChannelMap to read".
     */
    uint8_t                 ImageChannelMap_GetNumViews(ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                             "Gets the number of channels in the ChannelMap".
     @param                 ChannelMap     "The ChannelMap to read".
     */
    uint8_t                 ImageChannelMap_GetNumChannels(ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                             "Destroys an AudioVectorHistogram".
     @param                 ChannelMap     "The ChannelMap to destroy".
     */
    void                    ImageChannelMap_Deinit(ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                             "Creates an empty ImageContainer".
     @remark                               "All channels in an image must have the same bit depth, padding will be added if necessary".
     @param                 Type           "The type of array to create".
     @param                 ChannelMap     "The channel map".
     @param                 Width          "The number of pixels making up one row".
     @param                 Height         "The number of pixels making up one column".
     */
    ImageContainer         *ImageContainer_Init(Image_Types Type, ImageChannelMap *ChannelMap, uint64_t Width, uint64_t Height);
    
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
     @abstract                             "Gets the channel mask".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    ImageChannelMap        *ImageContainer_GetChannelMap(ImageContainer *Image);
    
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
    void                ****ImageContainer_GetArray(ImageContainer *Image);
    
    /*!
     @abstract                             "Sets a pointer to the array of pixels".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Array          "A pointer to the new array".
     */
    void                    ImageContainer_SetArray(ImageContainer *Image, void ****Array);
    
    /*!
     @abstract                             "Returns the average value (rounded) of the pixels in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 View           "Which view should we get the minimum from"?
     @param                 Channel        "Which channel should we get the minimum from"?
     */
    uint64_t                ImageContainer_GetAverage(ImageContainer *Image, uint8_t View, uint8_t Channel);
    
    /*!
     @abstract                             "Returns the highest value pixel in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 View           "Which view should we get the minimum from"?
     @param                 Channel        "Which channel should we get the minimum from"?
     */
    uint64_t                ImageContainer_GetMax(ImageContainer *Image, uint8_t View, uint8_t Channel);
    
    /*!
     @abstract                             "Returns the lowest value pixel in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 View           "Which view should we get the minimum from"?
     @param                 Channel        "Which channel should we get the minimum from"?
     */
    uint64_t                ImageContainer_GetMin(ImageContainer *Image, uint8_t View, uint8_t Channel);
    
    /*!
     @abstract                             "Rotates an Image, either Vertically, Horizontally, or both".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 FlipType       "The type of flipping to use".
     */
    void                    ImageContainer_Flip(ImageContainer *Image, FlipTypes FlipType);
    
    /*!
     @abstract                             "Resizes an Image".
     @remark                               "0 means keep the same, -1 means remove one, 1 means add one".
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
     @abstract                             "Securely erases an ImageContainer".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    void                    ImageContainer_Erase(ImageContainer *Image);
    
    /*!
     @abstract                             "Deletes the ImageContainer pointed to".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    void                    ImageContainer_Deinit(ImageContainer *Image);
    
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
    void                 ***ImageHistogram_GetArray(ImageHistogram *Histogram);
    
    /*!
     @abstract                             "Sets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an ImageHistogram in question".
     @param                 Array          "A pointer to the histogram data".
     */
    void                    ImageHistogram_SetArray(ImageHistogram *Histogram, void ***Array);
    
    /*!
     @abstract                             "Generates a histogram from an ImageContainer".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    ImageHistogram         *ImageHistogram_Generate(ImageContainer *Image);
    
    /*!
     @abstract                             "Sorts a histogram".
     @param                 Histogram      "A pointer to the ImageHistogram in question".
     @param                 Sort           "The kind of sorting to do".
     */
    void                    ImageHistogram_Sort(ImageHistogram *Histogram, SortTypes Sort);
    
    /*!
     @abstract                             "Securely erases an ImageHistogram".
     @param                 Histogram      "A pointer to the instance of an ImageContainer in question".
     */
    void                    ImageHistogram_Erase(ImageHistogram *Histogram);
    
    /*!
     @abstract                             "frees a ImageHistogram".
     @param                 Histogram      "A pointer to the ImageHistogram to deinitalize".
     */
    void                    ImageHistogram_Deinit(ImageHistogram *Histogram);
    
    /*!
     @abstract                             "Gets a pointer to an ImageChannelMap from an ImageHistogram".
     @param                 Histogram      "A pointer to the ImageHistogram to deinitalize".
     @return                               "Returns a pointer to the ImageChannelMap".
     */
    ImageChannelMap        *ImageHistogram_GetChannelMap(ImageHistogram *Histogram);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
