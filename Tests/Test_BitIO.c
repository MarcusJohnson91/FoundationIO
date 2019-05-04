#include "../libFoundationIO/include/BitIO.h"

#include "../libFoundationIO/include/TestIO.h"
#include "../libFoundationIO/include/Log.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    int main(int argc, const char *argv[]) {
        // Well, we need to test all of the BitIO components, so let's set up a BitInput and BitOutput.
        
        // for testing sockets, we can use the loopback mechanism so only the test runner needs to be available.
        
        typedef enum Constants {
            WrittenValueSize = 64,
            WrittenValue     = 1,
        } Constants;
        
        BitInput  *BitI      = BitInput_Init();
        BitOutput *BitO      = BitOutput_Init();
        
        BitInput_UTF8_OpenFile(BitI, U8("/Users/Marcus/Desktop/TestImages/21.ppm"));
        BitOutput_UTF8_OpenFile(BitO, U8("/Users/Marcus/Desktop/TestImages/21.ppm"));
        
        uint64_t FileSize    = BitInput_GetFileSize(BitI);
        
        BitBuffer *BitB      = BitBuffer_Init(FileSize);
        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, WrittenValueSize, WrittenValue);
        uint64_t ReadValue   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, WrittenValueSize);
        
        if (ReadValue != WrittenValue) {
            Log(Log_TEST, __func__, U8("Written Value %u does not match the Read Value %llu"), WrittenValue, ReadValue);
        }

        return 0;
    }
    
#ifdef __cplusplus
}
#endif
