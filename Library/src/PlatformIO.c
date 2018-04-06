#include <stdint.h>                    /* Included for u/intX_t */

#include "../include/Macros.h"         /* Included for our declarations */

#ifdef  __cplusplus
extern  "C" {
#endif
    
    void GetRuntimeByteBitOrder(void) {
        uint8_t  BitOrderNumber  = 0x8;
        if (BitOrderNumber == 8) {
            GlobalBitOrder = LSBitFirst;
        } else if (BitOrderNumber == 16) {
            GlobalBitOrder = MSBitFirst;
        }
        
        uint16_t ByteOrderNumber = 0xFF00;
        uint8_t *ByteOrder       = (uint8_t*)&ByteOrderNumber;
        if (ByteOrder[0] == 0) {
            GlobalByteOrder = LSByteFirst; // LittleEndian
        } else {
            GlobalByteOrder = MSByteFirst; // BigEndian
        }
    }
    
#ifdef  __cplusplus
}
#endif
