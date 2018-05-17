#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */

#ifdef __cplusplus
extern "C" {
#endif
    
    
    /* Features I need:
     Support for multi-dimensional arrays of any type.
     Doing various operations on those arrays, like getting the average, min, and max values from the array.
     I don't think I'll need setting or extracting, I should be able to treat it like a normal array for that, tho I'd need to know it's type so maybe having it's own functions would be best?
     
     What exactly are my use cases for this?
     
     libPCM & libModernPNG could use it to reduce the amount of memory needed at runtime, which would require runtime type info, so macros are a no-go.
     How would they use a generic array?
     
     libPCM would return a pointer to a generic array containing the array's size, type, and actual array to another function.
     
     libModernPNG would do much the same.
     
     So, who's job would it be? like, we have an image, of varying type, what do we do with it?
     
     Also, how do we do SIMD operations on a container? do we just extract a pointer from a container?
     
     */
    typedef struct Container {
        void          *Elements;
        uint64_t       NumPlanes;
        uint64_t       NumElements;
        uint64_t       Width;
        uint64_t       Height;
        ContainerTypes ContainerType;
    } Container;
    
    Container *Container_Init(const uint64_t NumPlanes, const uint64_t NumElements, ContainerTypes ContainerType) {
        Container *NewContainer             = NULL;
        if (NumPlanes > 0 && NumElements > 0) {
            NewContainer                    = calloc(1, sizeof(Container));
            if (NewContainer != NULL) {
                NewContainer->NumPlanes     = NumPlanes;
                NewContainer->NumElements   = NumElements;
                NewContainer->ContainerType = ContainerType;
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate a new vector"));
            }
        } else if (NumPlanes == 0) {
            Log(Log_ERROR, __func__, U8("NumPlanes %lld does not make sense"), NumPlanes);
        } else if (NumElements == 0) {
            Log(Log_ERROR, __func__, U8("NumElemenets %lld does not make sense"), NumElements);
        }
        return NewContainer;
    }
    
    void Container_Attach(Container *Container, void *Array2Attach) {
        if (Container != NULL && Array2Attach != NULL ) {
            Container->Elements = Array2Attach;
        } else if (Container == NULL) {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        } else if (Array2Attach == NULL) {
            Log(Log_ERROR, __func__, U8("Array2Attach Pointer is NULL"));
        }
    }
    
    uint8_t Container_GetElementSizeInBytes(Container *Container) { // GetContainersElementSizeInBytes
        uint8_t ElementSize = 0;
        if (Container != NULL) {
            ElementSize     = (Container->ContainerType & 0xFF) / 8;
        }
        return ElementSize;
    }
    
    uint64_t Container_GetNumElements(Container *Container) {
        uint64_t NumElements = 0;
        if (Container != NULL) {
            NumElements      = Container->NumElements;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return NumElements;
    }
    
    uint64_t Container_GetNumPlanes(Container *Container) {
        uint64_t NumPlanes = 0;
        if (Container != NULL) {
            NumPlanes      = Container->NumPlanes;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return NumPlanes;
    }
    
    void Container_SetWidth(Container *Container, uint64_t Width) {
        if (Container != NULL) {
            Container->Width = Width;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
    }
    
    void Container_SetHeight(Container *Container, uint64_t Height) {
        if (Container != NULL) {
            Container->Height = Height;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
    }
    
    uint64_t Container_GetWidth(Container *Container) {
        uint64_t Width = 0ULL;
        if (Container != NULL) {
            Width      = Container->Width;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t Container_GetHeight(Container *Container) {
        uint64_t Height = 0ULL;
        if (Container != NULL) {
            Height      = Container->Height;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Height;
    }
    
#ifdef __cplusplus
}
#endif
