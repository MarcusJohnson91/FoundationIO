#include "Macros.h"

#pragma  once

#ifndef  FoundationIO_ContainerIO_H
#define  FoundationIO_ContainerIO_H

#ifdef   __cplusplus
extern   "C" {
#endif
    
    /*!
     @header    ContainerIO.h
     @author    Marcus Johnson aka BumbleBritches57
     @copyright 2018+
     @version   1.0.0
     @brief     This header contains code for containers (type generic arrays)
     */
    
    /*!
     @enum                      ContainerTypes
     @constant                  UInteger8Array                  "Array of Unsigned Integers,  8 bits".
     @constant                  UInteger16Array                 "Array of Unsigned Integers, 16 bits".
     @constant                  UInteger32Array                 "Array of Unsigned Integers, 32 bits".
     @constant                  UInteger64Array                 "Array of Unsigned Integers, 64 bits".
     @constant                  SInteger8Array                  "Array of Signed Integers,    8 bits".
     @constant                  SInteger16Array                 "Array of Signed Integers,   16 bits".
     @constant                  SInteger32Array                 "Array of Signed Integers,   32 bits".
     @constant                  SInteger64Array                 "Array of Signed Integers,   64 bits".
     @constant                  Decimal16Array                  "Array of Floating Point,    16 bits".
     @constant                  Decimal32Array                  "Array of Floating Point,    32 bits".
     @constant                  Decimal64Array                  "Array of Floating Point,    64 bits".
     @constant                  Decimal128Array                 "Array of Floating Point,   128 bits".
     */
    typedef enum ContainerTypes {
                                UInteger8Array                  = 264,
                                UInteger16Array                 = 272,
                                UInteger32Array                 = 288,
                                UInteger64Array                 = 320,
                                SInteger8Array                  = 520,
                                SInteger16Array                 = 528,
                                SInteger32Array                 = 544,
                                SInteger64Array                 = 576,
                                Decimal16Array                  = 1040,
                                Decimal32Array                  = 1056,
                                Decimal64Array                  = 1088,
                                Decimal128Array                 = 1152,
    } ContainerTypes;
    
    typedef struct Container Container;
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    NumPlanes     "The number of dimensions in the array, for example a 4D array has 4 planes".
     @param    NumElements   "The number of elements per plane, a 4D array with 1024 elements per plane has 1024 elements, not 4096".
     @param    ContainerType "The type of the array containing the elements".
     */
    Container *Container_Init(const uint64_t NumPlanes, const uint64_t NumElements, ContainerTypes ContainerType);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     @param    Array2Attach  "A pointer to an array to attach to the container".
     */
    void       Container_Attach(Container *Container, void *Array2Attach);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     */
    uint8_t    Container_GetElementSizeInBytes(Container *Container);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     */
    uint64_t   Container_GetNumElements(Container *Container);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     */
    uint64_t   Container_GetNumPlanes(Container *Container);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     @param    Width         "The width to set".
     */
    void       Container_SetWidth(Container *Container, uint64_t Width);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     @param    Height        "The height to set".
     */
    void       Container_SetHeight(Container *Container, uint64_t Height);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     */
    uint64_t   Container_GetWidth(Container *Container);
    
    /*!
     @abstract               "Initalizes an empty container".
     @param    Container     "The container to access".
     */
    uint64_t   Container_GetHeight(Container *Container);
    
#ifdef   __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
