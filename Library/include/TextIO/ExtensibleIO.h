/*!
 @header      ExtensibleIO.h
 @author      Marcus Johnson
 @copyright   2021+
 @version     1.0.0
 @brief       This header contains types, data structures, and functions related to reading and writing XML documents.
 */

#include "../PlatformIO.h"  /* Included for Platform Independence macros */
#include "TextIOTypes.h"    /* Included for UTFX types */

#pragma once

#ifndef FoundationIO_TextIO_ExtensibleIO_H
#define FoundationIO_TextIO_ExtensibleIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @enum                XMLTokenTypes
     @constant            XMLTokenType_Unspecified                   Invalid/Default value
     @constant            XMLTokenType_Instruction                   <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
     @constant            XMLTokenType_Comment                       <!-- Comment Content -->
     @constant            XMLTokenType_Tag_Open                      <namespace:element>; Open tag
     @constant            XMLTokenType_Tag_Close                     </namespace:element>; Close tag
     @constant            XMLTokenType_Tag_Self                      <namespace:element/>; Self closing tag
     @constant            XMLTokenType_Attribute                     <Namespace:element attribute="Attribute"/>
     @constant            XMLTokenType_Ref_Name                      '&lt;' = '<', '&gt;' = '>', '&apos;' = '\'', '&quot;' = '\"', '&amp;' = '&'
     @constant            XMLTokenType_Ref_Dec                       '&#xx' = xx in decimal, any number of digits, may have leading zeros.
     @constant            XMLTokenType_Ref_Hex                       '&#xyy' = yy in hexadecimal, any number of digits, may have leading 0's, may use either case
     @constant            XMLTokenType_CDATA                         <![CDATA[Content]]>
     @constant            XMLTokenType_Namespace                     Explicit: xmlns:Namespace="URL"; Implicit: xmlns="URL";
     @constant            XMLTokenType_Text                          <Namespace:element>Text</Namespace:element>
     */
    typedef enum XMLTokenTypes {
                          XMLTokenType_Unspecified                   = 0,
                          XMLTokenType_Instruction                   = 1,
                          XMLTokenType_Comment                       = 2,
                          XMLTokenType_Tag_Open                      = 3,
                          XMLTokenType_Tag_Close                     = 4,
                          XMLTokenType_Tag_Self                      = 5,
                          XMLTokenType_Attribute                     = 6,
                          XMLTokenType_Ref_Name                      = 7,
                          XMLTokenType_Ref_Dec                       = 8,
                          XMLTokenType_Ref_Hex                       = 9,
                          XMLTokenType_CDATA                         = 10,
                          XMLTokenType_Namespace                     = 11,
                          XMLTokenType_Text                          = 12,
                          XMLTokenType_NumTokens                     = XMLTokenType_Text,
    } XMLTokenTypes;
    
    /*!
     @enum                XMLVersions
     @constant            XMLVersion_Unspecified                 Invalid/Default value
     @constant            XMLVersion_1_0                         1.0
     @constant            XMLVersion_1_1                         1.1
     */
    typedef enum XMLVersions {
                          XMLVersion_Unspecified                 = 0,
                          XMLVersion_1_0                         = 1,
                          XMLVersion_1_1                         = 2,
    } XMLVersions;
    
    typedef struct XMLDocument XMLDocument;
    
    XMLDocument *XMLDocument_Init8_FromPath(UTF8 *DocumentPath, size_t MaxDocumentSize, size_t MaxNodes);
    
    XMLDocument *XMLDocument_Init16_FromPath(UTF16 *DocumentPath, size_t MaxDocumentSize, size_t MaxNodes);
    
    XMLDocument *XMLDocument_Init32_FromPath(UTF32 *DocumentPath, size_t MaxDocumentSize, size_t MaxNodes);
    
#define XMLDocument_Init(DocumentPath, MaxDocumentSize, MaxNodes) _Generic((DocumentPath), UTF8*:XMLDocument_Init8_FromPath, UTF8[]:XMLDocument_Init8_FromPath, UTF16*:XMLDocument_Init16_FromPath, UTF16[]:XMLDocument_Init16_FromPath, UTF32*:XMLDocument_Init32_FromPath, UTF32[]:XMLDocument_Init32_FromPath)(DocumentPath, MaxDocumentSize, MaxNodes)
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* FoundationIO_TextIO_ExtensibleIO_H */
