#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */

#ifdef __cplusplus
extern "C" {
#endif
    
    
    typedef struct Container {
        void          *Elements;
        uint64_t       NumPlanes;
        uint64_t       NumElements;
        uint64_t       Width;
        uint64_t       Height;
        ContainerTypes ContainerType;
    } Container;
    
    Container *Container_Init(const uint64_t NumPlanes, const uint64_t NumElementsPerPlane, ContainerTypes ContainerType) {
        Container *NewContainer             = NULL;
        if (NumPlanes > 0 && NumElementsPerPlane > 0) {
            NewContainer                    = calloc(1, sizeof(Container));
            if (NewContainer != NULL) {
                NewContainer->NumPlanes     = NumPlanes;
                NewContainer->NumElements   = NumElementsPerPlane;
                NewContainer->ContainerType = ContainerType;
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate a new vector"));
            }
        } else if (NumPlanes == 0) {
            Log(Log_ERROR, __func__, U8("NumPlanes %lld does not make sense"), NumPlanes);
        } else if (NumElementsPerPlane == 0) {
            Log(Log_ERROR, __func__, U8("NumElementsPerPlane %lld does not make sense"), NumElements);
        }
        return NewContainer;
    }
    
    void Container_Attach(Container *Container, void *Array2Attach) {
        if (Container != NULL && Array2Attach != NULL ) {
            Container->Elements = Array2Attach;
        } else if (Container == NULL) {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        } else if (Array2Attach == NULL) {
            Log(Log_ERROR, __func__, U8("Array Pointer is NULL"));
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
