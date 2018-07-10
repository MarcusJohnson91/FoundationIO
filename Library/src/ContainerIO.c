#include "../include/ContainerIO.h"    /* Included for our declarations */
#include "../include/Log.h"            /* Included for error reporting */

#ifdef __cplusplus
extern "C" {
#endif
    
    
    typedef struct Container {
        union           Elements {
            uint64_t ***UInteger64;
            int64_t  ***SInteger64;
            uint32_t ***UInteger32;
            int32_t  ***SInteger32;
            uint16_t ***UInteger16;
            int16_t  ***SInteger16;
            uint8_t  ***UInteger8;
            int8_t   ***SInteger8;
        }               Elements;
        uint64_t        NumPlanes;
        uint64_t        Width;
        uint64_t        Height;
        ContainerTypes  ContainerType;
    } Container;
    
    Container *Container_Init(ContainerTypes ContainerType, uint64_t NumPlanes, uint64_t Width, uint64_t Height) {
        Container *Collection             = NULL;
        if (NumPlanes > 0 && Width > 0 && Height > 0) {
            Collection                    = calloc(1, sizeof(Container));
            if (Collection != NULL) {
                Collection->ContainerType = ContainerType;
                Collection->NumPlanes     = NumPlanes;
                Collection->Width         = Width;
                Collection->Height        = Height;
            } else {
                Log(Log_ERROR, __func__, U8("Couldn't allocate a new vector"));
            }
        } else if (NumPlanes == 0) {
            Log(Log_ERROR, __func__, U8("NumPlanes %lld does not make sense"), NumPlanes);
        } else if (Width == 0) {
            Log(Log_ERROR, __func__, U8("Width %lld does not make sense"), Width);
        } else if (Height == 0) {
            Log(Log_ERROR, __func__, U8("Height %lld does not make sense"), Height);
        }
        return Collection;
    }
    
    void Container_Attach(Container *Collection, void ***Array2Attach) {
        if (Collection != NULL && Array2Attach != NULL ) {
            if (Collection->ContainerType == UInteger64) {
                Collection->Elements.UInteger64 = (uint64_t) Array2Attach;
            } else if (Collection->ContainerType == SInteger64) {
                Collection->Elements.SInteger64 = (int64_t) Array2Attach;
            } else if (Collection->ContainerType == UInteger32) {
                Collection->Elements.UInteger32 = (uint32_t) Array2Attach;
            } else if (Collection->ContainerType == SInteger32) {
                Collection->Elements.SInteger32 = (int32_t) Array2Attach;
            } else if (Collection->ContainerType == UInteger16) {
                Collection->Elements.UInteger16 = (uint16_t) Array2Attach;
            } else if (Collection->ContainerType == SInteger16) {
                Collection->Elements.SInteger16 = (int16_t) Array2Attach;
            } else if (Collection->ContainerType == UInteger8) {
                Collection->Elements.UInteger8 = (uint8_t) Array2Attach;
            } else if (Collection->ContainerType == SInteger8) {
                Collection->Elements.SInteger8 = (int8_t) Array2Attach;
            }
        } else if (Collection == NULL) {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        } else if (Array2Attach == NULL) {
            Log(Log_ERROR, __func__, U8("Array Pointer is NULL"));
        }
    }
    
    uint8_t Container_GetElementSizeInBytes(Container *Collection) { // GetContainersElementSizeInBytes
        uint8_t ElementSize = 0;
        if (Collection != NULL) {
            ElementSize     = (Collection->ContainerType & 0xFF) / 8;
        }
        return ElementSize;
    }
    
    uint64_t Container_GetSize(Container *Collection) {
        uint64_t Size = 0;
        if (Collection != NULL) {
            Size      = Collection->NumPlanes * Collection->Width * Collection->Height;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Size;
    }
    
    uint64_t Container_GetNumPlanes(Container *Collection) {
        uint64_t NumPlanes = 0;
        if (Collection != NULL) {
            NumPlanes      = Collection->NumPlanes;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return NumPlanes;
    }
    
    void Container_SetWidth(Container *Collection, uint64_t Width) {
        if (Collection != NULL) {
            Collection->Width = Width;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
    }
    
    void Container_SetHeight(Container *Collection, uint64_t Height) {
        if (Collection != NULL) {
            Collection->Height = Height;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
    }
    
    uint64_t Container_GetWidth(Container *Collection) {
        uint64_t Width = 0ULL;
        if (Collection != NULL) {
            Width      = Collection->Width;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t Container_GetHeight(Container *Collection) {
        uint64_t Height = 0ULL;
        if (Collection != NULL) {
            Height      = Collection->Height;
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Height;
    }
    
    Container *Container_GenerateHistogram(Container *Collection) {
        Container *Histogram = NULL;
        if (Collection != NULL) {
            Histogram = Container_Init(Collection->ContainerType, Collection->NumPlanes, Collection->Width, Collection->Height);
            if (Histogram != NULL) {
                void ***Data = NULL;
                if (Collection->ContainerType == UInteger64) {
                    Data = calloc(18446744073709551615, sizeof(uint64_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == SInteger64) {
                    Data = calloc(18446744073709551615, sizeof(int64_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == UInteger32) {
                    Data = calloc(4294967296, sizeof(uint32_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == SInteger32) {
                    Data = calloc(4294967296, sizeof(int32_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == UInteger16) {
                    Data = calloc(65536, sizeof(uint16_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == SInteger16) {
                    Data = calloc(65536, sizeof(int16_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == UInteger8) {
                    Data = calloc(256, sizeof(uint8_t));
                    Container_Attach(Histogram, Data);
                } else if (Collection->ContainerType == SInteger8) {
                    Data = calloc(256, sizeof(int8_t));
                    Container_Attach(Histogram, Data);
                }
                // We need to use multi-threading here to return a container containing an array of the histogram of this array
                if (Histogram->Elements.UInteger64 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.UInteger64[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.SInteger64 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.SInteger64[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.UInteger32 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.UInteger32[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.SInteger32 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.SInteger32[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.UInteger16 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.UInteger16[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.SInteger16 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.SInteger16[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.UInteger8  != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.UInteger8[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else if (Histogram->Elements.SInteger8 != NULL) {
                    for (uint64_t Plane = 0; Plane < Collection->NumPlanes; Plane++) {
                        for (uint64_t Width = 0; Width < Collection->Width; Width++) {
                            for (uint64_t Height = 0; Height < Collection->Height; Height++) {
                                Histogram->Elements.SInteger8[Plane][Width][Height] += 1;
                            }
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("Histogram's Data Pointer is NULL"));
                }
            } else {
                Log(Log_ERROR, __func__, U8("Histogram Pointer is NULL"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Histogram;
    }
    
    int64_t Container_GetAverage(Container *Collection, uint64_t Plane2Average) {
        int64_t Average = 0;
        if (Collection != NULL && Plane2Average <= Collection->NumPlanes) {
            if (Collection->ContainerType == UInteger64) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.UInteger64[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == SInteger64) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.SInteger64[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == UInteger32) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.UInteger32[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == SInteger32) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.SInteger32[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == UInteger16) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.UInteger16[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == SInteger16) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.SInteger16[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == UInteger8) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.UInteger8[Plane2Average][X][Y];
                    }
                }
            } else if (Collection->ContainerType == SInteger8) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        Average += Collection->Elements.SInteger8[Plane2Average][X][Y];
                    }
                }
            }
            Average         /= (Collection->Width * Collection->Height);
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t Container_GetMax(Container *Collection, uint64_t Plane2GetMax) {
        int64_t Max = 0;
        if (Collection != NULL && Plane2GetMax <= Collection->NumPlanes) {
            if (Collection->ContainerType == UInteger64) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger64[Plane2GetMax][X][Y] < (uint64_t) Max) {
                            Max = Collection->Elements.UInteger64[Plane2GetMax][X][Y];
                        }
                    }
                    
                }
            } else if (Collection->ContainerType == SInteger64) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger64[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.SInteger64[Plane2GetMax][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == UInteger32) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger32[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.UInteger32[Plane2GetMax][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == SInteger32) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger32[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.SInteger32[Plane2GetMax][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == UInteger16) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger16[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.UInteger16[Plane2GetMax][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == SInteger16) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger16[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.SInteger16[Plane2GetMax][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == UInteger8) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger8[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.UInteger8[Plane2GetMax][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == SInteger8) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger8[Plane2GetMax][X][Y] > Max) {
                            Max = Collection->Elements.SInteger8[Plane2GetMax][X][Y];
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Max;
    }
    
    int64_t Container_GetMin(Container *Collection, uint64_t Plane2GetMin) {
        int64_t Min = 0;
        if (Collection != NULL && Plane2GetMin <= Collection->NumPlanes) {
            if (Collection->ContainerType == UInteger64) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger64[Plane2GetMin][X][Y] < (uint64_t) Min) {
                            Min = Collection->Elements.UInteger64[Plane2GetMin][X][Y];
                        }
                    }
                    
                }
            } else if (Collection->ContainerType == SInteger64) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger64[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.SInteger64[Plane2GetMin][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == UInteger32) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger32[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.UInteger32[Plane2GetMin][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == SInteger32) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger32[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.SInteger32[Plane2GetMin][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == UInteger16) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger16[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.UInteger16[Plane2GetMin][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == SInteger16) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger16[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.SInteger16[Plane2GetMin][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == UInteger8) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.UInteger8[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.UInteger8[Plane2GetMin][X][Y];
                        }
                    }
                }
            } else if (Collection->ContainerType == SInteger8) {
                for (uint64_t X = 0; X < Collection->Width; X++) {
                    for (uint64_t Y = 0; Y < Collection->Height; Y++) {
                        if (Collection->Elements.SInteger8[Plane2GetMin][X][Y] < Min) {
                            Min = Collection->Elements.SInteger8[Plane2GetMin][X][Y];
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
        return Min;
    }
    
    void Container_Resize(Container *Collection, uint64_t NewPlaneSize) {
        if (Collection != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("Container Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
