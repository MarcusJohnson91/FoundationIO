/*!
 @header          GUUID.h
 @author          Marcus Johnson
 @copyright       2021+
 @version         1.0.0
 @brief           This header contains code for manipulating GUIDs and UUIDs.
 */

#pragma once

#ifndef FoundationIO_GUUID_H
#define FoundationIO_GUUID_H

#include "AsynchronousIO.h"     /* Included for Asynchronous IO operations */
#include "TextIO/TextIOTypes.h" /* Included for Text types */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @enum         GUUIDTypes
     @constant     GUUIDType_Unspecified           The GUUID type is unknown, invalid.
     @constant     GUUIDType_GUIDString            The GUUID is a GUID string, aka LSByte first UUID with hyphens.
     @constant     GUUIDType_UUIDString            The GUUID is a UUID string, aka MSByte first UUID with hyphens.
     @constant     GUUIDType_BinaryGUID            The GUUID is a Binary GUID, aka LSByte first UUID without hyphens.
     @constant     GUUIDType_BinaryUUID            The GUUID is a Binary UUID, aka MSByte first UUID without hypthns.
     */
    typedef enum GUUIDTypes : uint8_t {
        GUUIDType_Unspecified           = 0,
        GUUIDType_GUIDString            = 1,
        GUUIDType_UUIDString            = 2,
        GUUIDType_BinaryGUID            = 3,
        GUUIDType_BinaryUUID            = 4,
    } GUUIDTypes;

    typedef enum GUUIDConstants : uint8_t {
        GUUIDString_Size                = 20,
        BinaryGUUID_Size                = 16,
    } GUUIDConstants;

    /*!
     @typedef      InsecurePRNG
     @abstract                                     Forward declaration from CryptographyIO.
     */
    typedef struct InsecurePRNG                    InsecurePRNG;

    /*!
     @typedef      BinaryGUUID
     @abstract                                     A binary GUUID is just the raw data without dashes.
     */
    typedef        const uint8_t                   BinaryGUUID[BinaryGUUID_Size];

    /*!
     @typedef      GUUIDString
     @abstract                                     A GUUID string is the GUUID with dashes and a NULL terminator
     */
    typedef        const uint8_t                   GUUIDString[GUUIDString_Size + TextIO_NULLTerminatorSize];

    /*!
     @abstract                                     Generates a random GUUID.
     @param        Insecure                        Pointer to InsecurePRNG.
     @return                                       Returns the generated GUUID.
     */
    uint8_t        *GUUID_Generate(InsecurePRNG *Insecure, GUUIDTypes GUUIDType);

    /*!
     @abstract                                     Compares GUUIDs for equivalence, GUUID1 and 2 HAVE to be the same type.
     @param        GUUID1                          Pointer to a GUUID to be compared.
     @param        GUUID2                          Pointer to a GUUID to be compared.
     @return                                       Returns Yes if GUUID1 and GUUID2 match, No otherwise.
     */
    bool           GUUID_Compare(GUUIDTypes GUUIDType, uint8_t *GUUID1, uint8_t *GUUID2);

    /*!
     @abstract                                     Compares GUUIDs for equivalence.
     @param        GUUID1                          Pointer to a GUUID to be compared.
     @param        GUUID2                          Pointer to a GUUID to be compared.
     @return                                       Returns true if GUUID1 and GUUID2 match, false otherwise.
     */
    bool           BinaryGUUID_Compare(BinaryGUUID GUUID1, BinaryGUUID GUUID2);

    /*!
     @abstract                                     Compares GUUIDs for equivalence.
     @param        GUUID1                          Pointer to a GUUID to be compared.
     @param        GUUID2                          Pointer to a GUUID to be compared.
     @return                                       Returns true if GUUID1 and GUUID2 match, false otherwise.
     */
    bool           GUUIDString_Compare(GUUIDString GUUID1, GUUIDString GUUID2);

    /*!
     @abstract                                     Converts a GUUID from one representation to another (String/Binary) || (UUID/GUID).
     @remark                                       The original GUUID is untouched, the user needs to deinit it.
     @param        GUUIDType_Input                 What type of GUUID are we converting from?
     @param        GUUIDType_Output                What type of GUUID are we converting to?
     @return                                       Returns the converted GUUID.
     */
    uint8_t        *GUUID_Convert(GUUIDTypes GUUIDType_Input, GUUIDTypes GUUIDType_Output, uint8_t *GUUID2Convert);

    /*!
     @abstract                                     Swaps the byte order of a BinaryGUUID or GUUIDString.
     @remark                                       The original GUUID is untouched, the user needs to deinit it.
     @param        GUUIDType                       Is this a GUUIDString or BinaryGUUID?
     @param        GUUID2Swap                      GUUID Pointer to swap.
     @return                                       Returns a pointer to a swapped GUUID.
     */
    uint8_t        *GUUID_Swap(GUUIDTypes GUUIDType, uint8_t *GUUID2Swap);

    /*!
     @abstract                                     Frees a GUUID.
     */
    void           GUUID_Deinit(uint8_t *GUUID);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */

#endif /* FoundationIO_GUUID_H */
