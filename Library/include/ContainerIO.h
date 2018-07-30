#include "Macros.h"

#pragma  once

#ifndef  FoundationIO_ContainerIO_H
#define  FoundationIO_ContainerIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header    ContainerIO.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2018+
     @version   2.0.0
     @brief     This header contains code for containers (type generic arrays)
     */
    
    typedef enum Audio_ChannelMask {
        Audio_UnknownChannel = 0,
        LeftChannel          = 1,
        CenterChannel        = 2,
        RightChannel         = 3,
        LFEChannel           = 4,
        SurroundLeftChannel  = 5,
        SurroundRightChannel = 6,
        RearLeftChannel      = 7,
        RearRightChannel     = 8,
    } Audio_ChannelMask;
    
    typedef enum Image_Views {
        UnknownView  = 0,
        View2D       = 1,
        LeftEyeView  = 2,
        RightEyeView = 3,
    } Image_Views;
    
    typedef enum Image_ChannelMask {
        Image_UnknownChannel =  0,
        GrayscaleChannel     =  1,
        RedChannel           =  2,
        Green1Channel        =  3,
        BlueChannel          =  4,
        Green2Channel        =  5,
        AlphaChannel         =  6,
        LumaChannel          =  7,
        Chroma1Channel       =  8,
        Chroma2Channel       =  9,
        Chroma3Channel       = 10,
    } Image_ChannelMask;
    
    typedef struct    AudioContainer AudioContainer;
    
    typedef struct    ImageContainer ImageContainer;
    
    /*!
     @abstract                       "Initalizes an empty AudioContainer".
     @param           IsUnsigned     "Are the samples signed or not"?
     @param           BitDepth       "The number of actual bits to store a sample, e.g. 24 bit audio has a bit depth of 24, not 32".
     @param           NumChannels    "The number of channels in the audio".
     @param           NumSamples     "NumSamples is the number of channel independent samples, e.g. X samples is BitDepth * NumChnnels * X".
     */
    AudioContainer   *AudioContainer_Init(bool IsUnsigned, uint8_t BitDepth, uint8_t NumChannels, uint64_t NumSamples);
    
    /*!
     @abstract                       "Sets the channel type for each channel index".
     @param           Audio          "A pointer to the instance of an AudioContainer".
     @param           Index          "Which index does the channel mask apply to"?
     @param           ChannelMask    "What is the channel mask"?
     */
    void              AudioContainer_SetChannelMask(AudioContainer *Audio, uint8_t Index, Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                       "Returns the number of bits required to represent a audio sample".
     @param           Audio          "A pointer to an instance of an AudioContainer".
     */
    uint8_t           AudioContainer_GetBitDepth(AudioContainer *Audio);
    
    /*!
     @abstract                       "Returns the number of audio channels".
     @param           Audio          "A pointer to an instance of an AudioContainer".
     */
    uint8_t           AudioContainer_GetNumChannels(AudioContainer *Audio);
    
    /*!
     @abstract                       "Returns the number of channel-agnostic audio samples stored in the container".
     @param           Audio          "A pointer to an instance of an AudioContainer".
     */
    uint64_t          AudioContainer_GetNumSamples(AudioContainer *Audio);
    
    /*!
     @abstract                       "Sets the channel type for each channel index".
     @param           Audio          "A pointer to the instance of an AudioContainer".
     @param           Index          "Which index does the channel mask apply to"?
     */
    Audio_ChannelMask AudioContainer_GetChannelMask(AudioContainer *Audio, uint8_t Index);
    
    /*!
     @abstract                       "Returns the average of the samples in the buffer".
     @param           Audio          "A pointer to an instance of an AudioContainer".
     @param           Channel        "Which channel should we get average"?
     */
    int64_t           AudioContainer_GetAverage(AudioContainer *Audio, Audio_ChannelMask Channel);
    
    /*!
     @abstract                       "Returns the highest valued sample in the buffer".
     @param           Audio          "A pointer to an instance of an AudioContainer".
     @param           Channel        "Which channel should we get the highest value from"?
     */
    int64_t           AudioContainer_GetMax(AudioContainer *Audio, Audio_ChannelMask Channel);
    
    /*!
     @abstract                       "Returns the lowest valued sample in the buffer".
     @param           Audio          "A pointer to an instance of an AudioContainer".
     @param           Channel        "Which channel should we get the lowest value from"?
     */
    int64_t           AudioContainer_GetMin(AudioContainer *Audio, Audio_ChannelMask Channel);
    
    /*!
     @abstract                       "Deinitalizes an AudioContainer, and any samples stored within it".
     @param           Audio          "A pointer to the instance of an AudioContainer to be deinitalized".
     */
    void              AudioContainer_Deinit(AudioContainer *Audio);
    
    /*!
     @abstract                       "Initalizes an empty ImageContainer".
     @param           Is3D           "Is there a left and right eye view for this image"?
     @param           IsUnsigned     "Are the samples signed or not"?
     @param           BitDepth       "The number of actual bits to store a sample, e.g. 14 bit image has a bit depth of 14, not 16".
     @param           NumChannels    "The number of channels in the image".
     @param           Width          "The number of pixels making up one row".
     @param           Height         "The number of pixels making up one column".
     */
    ImageContainer   *ImageContainer_Init(bool Is3D, bool IsUnsigned, uint8_t BitDepth, uint8_t NumChannels, uint64_t Width, uint64_t Height);
    
    /*!
     @abstract                       "Sets the channel type for each channel index".
     @param           Image          "A pointer to the instance of an ImageContainer to get the min from".
     @param           Index          "Which index does the channel mask apply to"?
     @param           ChannelMask    "What is the channel mask"?
     */
    void              ImageContainer_SetChannelMask(ImageContainer *Image, uint8_t Index, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                       "Returns the left or right eye for a 3D image, or for a 2D image, view 0".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     */
    uint8_t           ImageContainer_GetView(ImageContainer *Image);
    
    /*!
     @abstract                       "Returns the index channel X is contained in".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     @param           ChannelMask    "Which channel (not index) should we get the index from"?
     */
    uint8_t           ImageContainer_GetChannel(ImageContainer *Image, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                       "Returns if the ImageContainer is 3D or not".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     */
    bool              ImageContainer_Is3D(ImageContainer *Image);
    
    /*!
     @abstract                       "Returns the number of bits needed to represent the image in this container".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     */
    uint8_t           ImageContainer_GetBitDepth(ImageContainer *Image);
    
    /*!
     @abstract                       "Returns the number of channels contained in this image".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     */
    uint8_t           ImageContainer_GetNumChannels(ImageContainer *Image);
    
    /*!
     @abstract                       "Returns the number of pixels in one row of this image".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     */
    uint64_t          ImageContainer_GetWidth(ImageContainer *Image);
    
    /*!
     @abstract                       "Returns the number of pixels in one column of this image".
     @param           Image          "A pointer to the instance of an ImageContainer to get the view from".
     */
    uint64_t          ImageContainer_GetHeight(ImageContainer *Image);
    
    /*!
     @abstract                       "Sets the channel type for each channel index".
     @param           Image          "A pointer to the instance of an ImageContainer to get the min from".
     @param           Index          "Which index does the channel mask apply to"?
     */
    Image_ChannelMask ImageContainer_GetChannelMask(ImageContainer *Image, uint8_t Index);
    
    /*!
     @abstract                       "Returns the average value (rounded) of the pixels in this image".
     @param           Image          "A pointer to the instance of an ImageContainer to get the average from".
     @param           Channel        "Which channel (not index) should we get the average from"?
     */
    int64_t           ImageContainer_GetAverage(ImageContainer *Image, Image_Views View, Image_ChannelMask Channel);
    
    /*!
     @abstract                       "Returns the highest value pixel in this image".
     @param           Image          "A pointer to the instance of an ImageContainer to get the max from".
     @param           Channel        "Which channel (not index) should we get the max value pixel from"?
     */
    int64_t           ImageContainer_GetMax(ImageContainer *Image, Image_ChannelMask Channel);
    
    /*!
     @abstract                       "Returns the lowest value pixel in this image".
     @param           Image          "A pointer to the instance of an ImageContainer to get the min from".
     @param           Channel        "Which channel (not index) should we get the min value pixel from"?
     */
    int64_t           ImageContainer_GetMin(ImageContainer *Image, Image_Views View, Image_ChannelMask Channel);
    
    /*!
     @abstract                       "Deinitalizes the ImageContainer pointed to".
     @param           Image          "A pointer to the instance of an ImageContainer to deinitalize".
     */
    void              ImageContainer_Deinit(ImageContainer *Image);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
