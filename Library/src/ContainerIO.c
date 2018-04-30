#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */
#include <stdlib.h>                    /* Included for calloc, and free */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct Container {
        void          *Elements;
        uint64_t       NumPlanes;
        uint64_t       NumElements;
        ContainerTypes ContainerType;
    } Container;
    
    Container *Container_Init(const uint64_t NumPlanes, const uint64_t NumElements, ContainerTypes ContainerType) {
        Container *Container                 = NULL;
        if (NumPlanes > 0 && NumElements > 0) {
            Container                        = calloc(1, sizeof(Container));
            if (Container != NULL) {
                Container->NumPlanes         = NumPlanes;
                Container->NumElements       = NumElements;
                Container->ContainerType     = ContainerType;
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate a new vector"));
            }
        } else if (NumPlanes == 0) {
            Log(Log_ERROR, __func__, U8("NumPlanes %lld does not make sense"), NumPlanes);
        } else if (NumElements == 0) {
            Log(Log_ERROR, __func__, U8("NumElemenets %lld does not make sense"), NumElements);
        }
        return Container;
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
    
#ifdef __cplusplus
}
#endif
