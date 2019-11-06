#include <stdint.h>

#pragma once

#ifndef FoundationIO_Constants_H
#define FoundationIO_Constants_H

#ifdef __cplusplus
extern "C" {
#endif

#define BitMaskTableSize 8

	static const uint8_t BitMaskTable[BitMaskTableSize] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

#ifdef __cplusplus
}
#endif

#endif /* FoundationIO_Constants_H */
