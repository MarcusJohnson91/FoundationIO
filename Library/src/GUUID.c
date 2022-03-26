#include "../include/GUUID.h"             /* Included for our declarations */

#include "../include/AssertIO.h"          /* Included for Assertions */
#include "../include/BufferIO.h"          /* Included for BitBuffer */
#include "../include/CryptographyIO.h"    /* Included for InsecurePRNG_CreateInteger */
#include "../include/MathIO.h"            /* Included for Bits2Bytes */
#include "../include/TextIO/FormatIO.h"   /* Included for UTF32_Format */
#include "../include/TextIO/StringIO.h"   /* Included for StringIO's declarations */

#if   PlatformIO_Is(PlatformIO_TargetOS, PlatformIO_TargetOSIsPOSIX)
#include <dirent.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    uint8_t *GUUID_Generate(InsecurePRNG *Insecure, GUUIDTypes GUUIDType) {
        uint8_t *GUUID                   = 0;
        AssertIO(Insecure != NULL);
        AssertIO(GUUIDType != GUUIDType_Unspecified);

        uint64_t LowBits             = InsecurePRNG_CreateInteger(Insecure, 64);
        uint64_t HighBits            = InsecurePRNG_CreateInteger(Insecure, 64);
        if (GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) {
            GUUID                    = (uint8_t*) calloc(GUUIDString_Size, sizeof(uint8_t));
        } else if (GUUIDType == GUUIDType_BinaryGUID || GUUIDType == GUUIDType_BinaryUUID) {
            GUUID                    = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
        }
        AssertIO(GUUID != NULL);
        if (GUUIDType == GUUIDType_GUIDString || GUUIDType == GUUIDType_UUIDString) { // String
            for (uint8_t GUUIDByte = 0; GUUIDByte < GUUIDString_Size; GUUIDByte++) {
                if (GUUIDByte != 4 && GUUIDByte != 7 && GUUIDByte != 10) {
                    if (GUUIDByte < 8) {
                        uint8_t Byte     = (LowBits  & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                        GUUID[GUUIDByte] = Byte;
                    } else {
                        uint8_t Byte     = (HighBits & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                        GUUID[GUUIDByte] = Byte;
                    }
                }
            }
        } else if (GUUIDType == GUUIDType_BinaryGUID || GUUIDType == GUUIDType_BinaryUUID) { // Binary
            for (uint8_t GUUIDByte = 0; GUUIDByte < BinaryGUUID_Size; GUUIDByte++) {
                if (GUUIDByte < 8) {
                    uint8_t Byte     = (LowBits  & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                    GUUID[GUUIDByte] = Byte;
                } else {
                    uint8_t Byte     = (HighBits & (0xFF << (GUUIDByte * 8))) >> (GUUIDByte * 8);
                    GUUID[GUUIDByte] = Byte;
                }
            }
        }
        return GUUID;
    }
    
    bool GUUID_Compare(GUUIDTypes Type2Compare, const uint8_t *const GUUID1, const uint8_t *const GUUID2) {
        AssertIO(Type2Compare != GUUIDType_Unspecified);
        AssertIO(GUUID1 != NULL);
        AssertIO(GUUID2 != NULL);

        uint8_t GUUIDSize       = ((Type2Compare == GUUIDType_GUIDString || Type2Compare == GUUIDType_UUIDString) ? BinaryGUUID_Size : BinaryGUUID_Size);
        bool GUUIDsMatch        = Yes;

        for (uint8_t BinaryGUUIDByte = 0; BinaryGUUIDByte < GUUIDSize; BinaryGUUIDByte++) {
            if (GUUID1[BinaryGUUIDByte] != GUUID2[BinaryGUUIDByte]) {
                GUUIDsMatch = No;
            }
        }
        return GUUIDsMatch;
    }
    
    uint8_t *GUUID_Convert(GUUIDTypes InputType, GUUIDTypes OutputType, uint8_t *GUUID2Convert) {
        AssertIO(InputType != GUUIDType_Unspecified);
        AssertIO(OutputType != GUUIDType_Unspecified);
        AssertIO(InputType != OutputType);
        AssertIO(GUUID2Convert != NULL);

        uint8_t  Dash = '-';
        uint8_t  OutputGUUIDSize = ((OutputType == GUUIDType_GUIDString || OutputType == GUUIDType_UUIDString) ? GUUIDString_Size : BinaryGUUID_Size);
        uint8_t *ConvertedGUUID  = (uint8_t*) calloc(OutputGUUIDSize, sizeof(uint8_t));
        AssertIO(ConvertedGUUID != NULL);

        bool ByteOrderDiffers = (((InputType == GUUIDType_GUIDString && OutputType == GUUIDType_UUIDString) || (InputType == GUUIDType_UUIDString && OutputType == GUUIDType_GUIDString) || (InputType == GUUIDType_BinaryUUID && OutputType == GUUIDType_BinaryGUID) || (InputType == GUUIDType_BinaryGUID && OutputType == GUUIDType_BinaryUUID)) ? Yes : No);

        bool TypeDiffers      = (((InputType == GUUIDType_GUIDString && OutputType == GUUIDType_BinaryGUID) || (InputType == GUUIDType_BinaryGUID && OutputType == GUUIDType_GUIDString) || (InputType == GUUIDType_UUIDString && OutputType == GUUIDType_BinaryUUID) || (InputType == GUUIDType_BinaryUUID && OutputType == GUUIDType_UUIDString)) ? Yes : No);

        if (ByteOrderDiffers == Yes) {
            GUUID_Swap(InputType, GUUID2Convert);
        }

        if (TypeDiffers == Yes) {
            if ((InputType == GUUIDType_UUIDString || InputType == GUUIDType_GUIDString) && (OutputType == GUUIDType_BinaryUUID || OutputType == GUUIDType_BinaryGUID)) {
                for (uint8_t StringByte = 0; StringByte < BinaryGUUID_Size; StringByte++) {
                    for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUID_Size; BinaryByte++) {
                        if (GUUID2Convert[StringByte] != Dash) {
                            ConvertedGUUID[BinaryByte] = GUUID2Convert[StringByte];
                        }
                    }
                }
            } else if ((InputType == GUUIDType_BinaryUUID || InputType == GUUIDType_BinaryGUID) || (OutputType == GUUIDType_UUIDString || OutputType == GUUIDType_GUIDString)) {
                for (uint8_t BinaryByte = 0; BinaryByte < BinaryGUUID_Size; BinaryByte++) {
                    for (uint8_t StringByte = 0; StringByte < BinaryGUUID_Size; StringByte++) {
                        if (BinaryByte != 4 && BinaryByte != 7 && BinaryByte != 10 && BinaryByte != 13) {
                            ConvertedGUUID[StringByte]  = GUUID2Convert[BinaryByte];
                        } else {
                            ConvertedGUUID[StringByte]  = Dash;
                        }
                    }
                }
            }
        }
        return ConvertedGUUID;
    }
    
    uint8_t *GUUID_Swap(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap) {
        AssertIO(GUUIDType != GUUIDType_Unspecified);
        AssertIO(GUUID2Swap != NULL);

        uint8_t *SwappedGUUID = NULL;
        uint8_t Dash              = '-';
        if (GUUIDType == GUUIDType_UUIDString || GUUIDType == GUUIDType_GUIDString) {
            SwappedGUUID          = (uint8_t*) calloc(GUUIDString_Size, sizeof(uint8_t));
            AssertIO(SwappedGUUID != NULL);
            SwappedGUUID[0]   = GUUID2Swap[3];
            SwappedGUUID[1]   = GUUID2Swap[2];
            SwappedGUUID[2]   = GUUID2Swap[1];
            SwappedGUUID[3]   = GUUID2Swap[0];

            SwappedGUUID[4]   = Dash;

            SwappedGUUID[5]   = GUUID2Swap[6];
            SwappedGUUID[6]   = GUUID2Swap[5];

            SwappedGUUID[7]   = Dash;

            SwappedGUUID[8]   = GUUID2Swap[9];
            SwappedGUUID[9]   = GUUID2Swap[8];

            SwappedGUUID[10]  = Dash;

            SwappedGUUID[11]  = GUUID2Swap[12];
            SwappedGUUID[12]  = GUUID2Swap[11];

            SwappedGUUID[13]  = Dash;

            for (uint8_t EndBytes = 13; EndBytes < GUUIDString_Size; EndBytes++) {
                SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
            }
        } else if (GUUIDType == GUUIDType_BinaryUUID || GUUIDType == GUUIDType_BinaryGUID) {
            SwappedGUUID          = (uint8_t*) calloc(BinaryGUUID_Size, sizeof(uint8_t));
            AssertIO(SwappedGUUID != NULL);
            SwappedGUUID[0]   = GUUID2Swap[3];
            SwappedGUUID[1]   = GUUID2Swap[2];
            SwappedGUUID[2]   = GUUID2Swap[1];
            SwappedGUUID[3]   = GUUID2Swap[0];

            SwappedGUUID[4]   = GUUID2Swap[5];
            SwappedGUUID[5]   = GUUID2Swap[4];

            SwappedGUUID[6]   = GUUID2Swap[7];
            SwappedGUUID[7]   = GUUID2Swap[6];

            SwappedGUUID[8]   = GUUID2Swap[9];
            SwappedGUUID[9]   = GUUID2Swap[8];
            for (uint8_t EndBytes = 10; EndBytes < BinaryGUUID_Size; EndBytes++) {
                SwappedGUUID[EndBytes] = GUUID2Swap[EndBytes];
            }
        }
        return SwappedGUUID;
    }
    
    void GUUID_Deinit(uint8_t *GUUID) {
        AssertIO(GUUID != NULL);
        free(GUUID);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
