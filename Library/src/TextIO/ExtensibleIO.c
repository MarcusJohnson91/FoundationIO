#include "../../include/TextIO/ExtensibleIO.h"               /* Included for our declarations */
#include "../../include/AsynchronousIO.h"                /* Included for file handling */
#include "../../include/FileIO.h"                        /* Included for file handling */
#include "../../include/TextIO/StringIO.h"               /* Included for GetNumDigits */
#include "../../include/TextIO/Private/TextIOTables.h"   /* Included for Unicode tables */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifndef TokenString
#define TokenString(StartQuotedTokenString,StopQuotedTokenString) PlatformIO_Concat(TokenPrefix, StartQuotedTokenString) PlatformIO_Concat(TokenPrefix, StopQuotedTokenString)
#endif /* TokenString */

    typedef struct XMLSyntaxPair8 {
        const char8_t *Head;
        const char8_t *Tail;
        const uint8_t  HeadSize;
        const uint8_t  TailSize;
    } XMLSyntaxPair8;

    const XMLSyntaxPair8 XMLTokens8 = {
        [0] = {.Head = u8"<![CDATA[", .HeadSize = 9, .Tail = u8"]]>", .TailSize = 3}, // CDATA
        [1] = {.Head = u8"<!--", .HeadSize = 4, .Tail = u8"-->", .TailSize = 3}, // Comment
        [2] = {.Head = u8" <?", .HeadSize = 2, .Tail = u8"?>", .TailSize = 2}, // Instruction
        [3] = {.Head = u8"<", .HeadSize = 1, .Tail = u8"/>", .TailSize = 2}, // Self-Closing Tag
        [4] = {.Head = u8"</", .HeadSize = 2, .Tail = u8">",  .TailSize = 1}, // Closing Tag
        [5] = {.Head = u8"<", .HeadSize = 1, .Tail = u8">", .TailSize = 1}, // Open Tag
    } XMLTokens8;
    
    typedef struct XMLSyntaxPair16 {
        const char16_t *Head;
        const char16_t *Tail;
        const uint8_t  HeadSize;
        const uint8_t  TailSize;
    } XMLSyntaxPair16;

    const XMLSyntaxPair16 XMLTokens16 = {
        [0] = {.Head = u"<![CDATA[", .HeadSize = 9, .Tail = u"]]>", .TailSize = 3}, // CDATA
        [1] = {.Head = u"<!--", .HeadSize = 4, .Tail = u"-->", .TailSize = 3}, // Comment
        [2] = {.Head = u" <?", .HeadSize = 2, .Tail = u"?>", .TailSize = 2}, // Instruction
        [3] = {.Head = u"<", .HeadSize = 1, .Tail = u"/>", .TailSize = 2}, // Self-Closing Tag
        [4] = {.Head = u"</", .HeadSize = 2, .Tail = u">",  .TailSize = 1}, // Closing Tag
        [5] = {.Head = u"<", .HeadSize = 1, .Tail = u">", .TailSize = 1}, // Open Tag
    } XMLTokens16;
    
    typedef struct XMLSyntaxPair32 {
        const char32_t *Head;
        const char32_t *Tail;
        const uint8_t  HeadSize;
        const uint8_t  TailSize;
    } XMLSyntaxPair32;

    const XMLSyntaxPair32 XMLTokens32 = {
        [0] = {.Head = U"<![CDATA[", .HeadSize = 9, .Tail = U"]]>", .TailSize = 3}, // CDATA
        [1] = {.Head = U"<!--", .HeadSize = 4, .Tail = U"-->", .TailSize = 3}, // Comment
        [2] = {.Head = U"<?", .HeadSize = 2, .Tail = U"?>", .TailSize = 2}, // Instruction
        [3] = {.Head = U"<", .HeadSize = 1, .Tail = U"/>", .TailSize = 2}, // Self-Closing Tag
        [4] = {.Head = U"</", .HeadSize = 2, .Tail = U">",  .TailSize = 1}, // Closing Tag
        [5] = {.Head = U"<", .HeadSize = 1, .Tail = U">", .TailSize = 1}, // Open Tag
    } XMLTokens32;

    typedef struct XMLToken {
        void                   *StartTag;
        void                   *StopTag;
    } XMLToken;

    typedef struct XMLTokens {
        TextIO_StringTypes StringType;
        XMLToken           Tokens[];
    } XMLTokens;

    const XMLTokens XMLTokens8 = {
        .StringType = StringType_UTF8,
        .Tokens     = {
            [Enum2Index(XMLTokenType_Instruction)] = {
                .StartTag = "<?",
                .StopTag  = "?>",
            },
            [Enum2Index(XMLTokenType_Comment)] = {
                .StartTag = "<--",
                .StopTag  = "-->",
            },
            [Enum2Index(XMLTokenType_Tag_Open)] = {
                .StartTag = "<",
                .StopTag  = ">",
            },

        },
    };
    
    /*
    ok, so there's a bunch of open and close tag pairs, and then we need to generate a UTF-8, 16, and 32 variants.
    
    so we can write macros to do that by just taking the prefix as an argument.
    
    something like TokenPair(OpenToken, CloseToken, Prefix) Prefix##OpenToken, Prefix##CloseToken
    
    yeah that seems like a good idea generally.
    */ 

    const XMLTokens8[XMLTokenType_NumTokens] = {
        [Enum2Index(XMLTokenType_Instruction)] = {
        TokenPair("<?", "?>", u8)
        [Enum2Index(XMLTokenType_Comment)] = TokenPair("<--", "-->", u8)
    };

    const UTF8 *Tokens8[] = {
#ifndef TokenPrefix
#define TokenPrefix u8
#endif /* TokenPrefix */
        TokenString("<?", "?>"),         // Processing Instruction
        TokenString("<--", "-->"),       // Comment Tag
        TokenString("<", ">"),           // Open Tag
        TokenString("</", ">"),          // Close Tag
        TokenString("<", "/>"),          // Self-Closing Tag
        TokenString("=\"", ">"),         // Attribute Tag
        TokenString("&", ";"),           // Named Reference
        TokenString("&#", ";"),          // Decimal Reference
        TokenString("&#x", ";"),         // Hexadecimal Reference
        TokenString("<![CDATA[", "]]>"), // CDATA
        TokenString("xmlns=\"", "\""),   // Implicit Namespace
        TokenString("xmlns:", "\""),     // Expicit Namespace
    };

    const UTF16 *Tokens16[] = {
#ifndef TokenPrefix
#define TokenPrefix u
#endif /* TokenPrefix */
        TokenString("<?", "?>"),         // Processing Instruction
        TokenString("<--", "-->"),       // Comment Tag
        TokenString("<", ">"),           // Open Tag
        TokenString("</", ">"),          // Close Tag
        TokenString("<", "/>"),          // Self-Closing Tag
        TokenString("=\"", ">"),         // Attribute Tag
        TokenString("&", ";"),           // Named Reference
        TokenString("&#", ";"),          // Decimal Reference
        TokenString("&#x", ";"),         // Hexadecimal Reference
        TokenString("<![CDATA[", "]]>"), // CDATA
        TokenString("xmlns=\"", "\""),   // Implicit Namespace
        TokenString("xmlns:", "\""),     // Expicit Namespace
    };

    const UTF32 *Tokens32[] = {
#ifndef TokenPrefix
#define TokenPrefix U
#endif /* TokenPrefix */
        TokenString("<?", "?>"),         // Processing Instruction
        TokenString("<--", "-->"),       // Comment Tag
        TokenString("<", ">"),           // Open Tag
        TokenString("</", ">"),          // Close Tag
        TokenString("<", "/>"),          // Self-Closing Tag
        TokenString("=\"", ">"),         // Attribute Tag
        TokenString("&", ";"),           // Named Reference
        TokenString("&#", ";"),          // Decimal Reference
        TokenString("&#x", ";"),         // Hexadecimal Reference
        TokenString("<![CDATA[", "]]>"), // CDATA
        TokenString("xmlns=\"", "\""),   // Implicit Namespace
        TokenString("xmlns:", "\""),     // Expicit Namespace
    };

    /*
     Crreate Tokens for all types:
     define a TokenString macro which calls a function called TokenType
     for UTF8 TokenType prefixes u8
     for UTF16 TokenType prefixes u
     for UTF32 TokenType prefixes U
     */

    /*
     static const PlatformIO_Immutable(UTF8 **) StringSet[] = {
     &XMLInstruction_Open8,
     &XMLInstruction_Close8,
     };

     static const PlatformIO_Immutable(UTF8 **) *XMLTokens8[] = {
     [PlatformIO_Enum2Index(XMLTokenType_Instruction)] = UTF8StringSet(UTF8String("<?"), UTF8String("?>")),
     [PlatformIO_Enum2Index(XMLTokenType_Comment)] = {
     UTF8String("<!--"),
     UTF8String("-->"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Tag_Open)] = {
     UTF8String("<"),
     UTF8String(">"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Tag_Close)] = {
     UTF8String("</"),
     UTF8String(">"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Tag_Self)] = {
     UTF8String("<"),
     UTF8String("/>"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Attribute)] = {
     UTF8String("=\""),
     UTF8String("\""),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Ref_Name)] = {
     UTF8String("&"),
     UTF8String(";"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Ref_Dec)] = {
     UTF8String("&#"),
     UTF8String(";"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_Ref_Hex)] = {
     UTF8String("&#x"),
     UTF8String(";"),
     },
     [PlatformIO_Enum2Index(XMLTokenType_CDATA)] = {
     UTF8String("<![CDATA["),
     UTF8String("]]>"),
     },
     };
     */

    /*
     XML Syntax Kinds:

     Must have a root element

     Terminology: Element=Tag=Node

     Instruction: Start="<?" End="?>"

     Comment: Start="<!--" End="-->"

     Attribute: Start='="' End='"'

     References {

     Named: Start="&" End=";"

     Decimal: Start="&#" End=";"

     Hexadecimal: Start="&#x" End=";"

     }

     Element {

     Open Tag: Start="<" End=">"

     Close Tag: Start="</" End=">"

     Self-Closing Tag: Start="<" End="/>"

     }
     */

    /*
     XMLTokens: UTF-8, UTF-16, UTF-32

     All generated from a macro that creates 3 different arrays
     */

    /*
     Maybe we should have a data structure for each start and end token?
     */
    /*
     XMLToken(Instruction, "<?", "?>");

     typedef enum XMLTokens = {
     XMLTokenType_Comment = 0,
     } XMLTokens;

     static const UTF8 **XMLTokens8[] = {
     #ifndef XMLTokenType
     #define XMLTokenType StringType_UTF8
     #elif defined(XMLTokenType)
     #undef XMLTokenType
     #define XMLTokenType StringType_UTF8
     #endif
     XMLToken(XMLTokenType_Comment, "<!--", "-->"),
     };

     static const UTF16 **XMLTokens16[] = {
     #ifndef XMLTokenType
     #define XMLTokenType StringType_UTF16
     #elif defined(XMLTokenType)
     #undef XMLTokenType
     #define XMLTokenType StringType_UTF16
     #endif
     XMLToken(XMLTokenType_Comment, "<!--", "-->"),
     };

     static const UTF32 **XMLTokens32[] = {
     #ifndef XMLTokenType
     #define XMLTokenType StringType_UTF32
     #elif defined(XMLTokenType)
     #undef XMLTokenType
     #define XMLTokenType StringType_UTF32
     #endif
     XMLToken(XMLTokenType_Comment, "<!--", "-->"),
     };
     */

    static const UTF8 *XMLTokens8[] = {
        UTF8String("<?xml"),
        UTF8String("?>"),
        UTF8String("xmlns"),
        UTF8String("<!--"), // Comment Start
        UTF8String("-->"), // Comment End
        UTF8String("<"), // Opening tag
        UTF8String("</"), // Closing Tag start
        UTF8String("/>"), // Empty tag end
        UTF8String("&#"), // Decimal entity
        UTF8String("&#x"), // Hex entity
    };
    
    typedef struct UTF32Tokens {
        const UTF32 *ProcS;
        const UTF32 *ProcE;
    } UTF32Tokens;
    
    UTF32Tokens Tokens32 = {
        /*
         FoundationIO_String took as the first parameter the value in order to get it's type, and the second parameter was the value to assign
         The intent was to allow the compiler to detect the type needed, and using _Generic, create the actual string constant with the correct type.

         but with that, I could theoretically create the Refract feature, which would assign a value seperately from where it's declared

         like, FoundationIO_Refract(Where2Assign, What2Assign)
         */
        .ProcS = U"<?",
        .ProcE = U"?>",
        /*
         HOLY FUCK!!! I MIGHT JUST BE ABLE TO BUILD MY REGISTRATION SYSTEM AFTER ALL!!!!
         */
    };
    
    /*
     Reading an XML Document:
     
     Step 1: Open the file the document is contained in.
     
     Step 2: Identify the encoding before reading the declaration, 8/16/32 bit character type stuff
     
     Step 3: Read the declaration to understand the XML version, namespaces, root nodes, etc
     
     Step 4: Have the library user set up the type of parsing we're doing, we can do both PULL Parsing, if there's just certain bits of info they want to extract, or we can build a whole DOM tree.
     
     Step 5: Have the user start the parsing/DOM building process themselves.
     */
    
    /*
     XML Format:
     
     Each XML Node contains a number of attributes,
     
     So, we need to get the size of each XML node
     */
    
    /*
     When Parsing:
     <! = CDATA or Comment
     <? = Declaration
     <  = Element
     */
    
    /*
     Entities are basically a mapping from one representation, so we need a string splice, and the actual replacement data
     */
    
    typedef struct XMLDeclaration {
        XMLVersions Version;
        TextIO_StringTypes   Encoding;
        bool                 Standalone;
    } XMLDeclaration;
    
    typedef struct XMLNode {
        TextIO_Slice  Slice;
        XMLTokenTypes NodeType;
    } XMLNode;
    
    typedef struct XMLNamespace {
        TextIO_Slice         Name; // How do we handle the Default namespace? the slice end would equal the slice start
        TextIO_Slice         Identifier; // the URL part
    } XMLNamespace;
    
    /*!
     @variable DocOffset DocOffset in the Document, counted in CodeUnits
     */
    typedef struct XMLDocument {
        AsyncIOStream          *Stream;
        void                   *Document; // the actual XML file loaded as a giant string
        XMLNode       *Nodes;
        XMLNamespace  *Namespaces;
        size_t                  NumNodes;
        size_t                  NumNamespaces;
        size_t                  NodeOffset;
        size_t                  DocSize;
        size_t                  DocOffset;
        XMLDeclaration Declaration;
        TextIO_StringTypes      StringType;
    } XMLDocument;
    
    /*
     So, yeah we should try and use the actual document as the text, and just create pointers and offsets to it...
     
     XMLDocument needs to be a buffer containing the document, but how can we load in and out various pieces of data? like if it's 1 gig for the document, and we only have 16MB of ram to work with?
     */

    /*
     Let's do it in UTF-8, UTF-16, and UTF-32 sections
     */

    /* UTF-8 */
    static UTF8 NextCharacter_UTF8(XMLDocument *Doc) {
        UTF8 Character  = 0;
        Doc->DocOffset += 1;
        UTF8 *Document8 = Doc->Document;
        Character       = Document8[Doc->DocOffset];
        return Character;
    }

    XMLDocument *XMLDocument_Init8_FromPath(UTF8 *DocumentPath, size_t MaxDocumentSize, size_t MaxNodes) {
        XMLDocument *Doc = calloc(1, sizeof(XMLDocument));
        Doc->Stream               = AsyncIOStream_Init();
        AsyncIOStream_OpenPathUTF8(Doc->Stream, DocumentPath, FileMode_Read);
        if (MaxDocumentSize <= 0) {
            int64_t DocSize       = AsyncIOStream_GetSize(Doc->Stream);
            Doc->Document         = calloc(1, DocSize);
            if (MaxNodes > 5) {
                Doc->Nodes        = calloc(MaxNodes, sizeof(XMLNode));
            } else {
                Doc->Nodes        = calloc(DocSize / 5, sizeof(XMLNode));
            }
        } else {
            Doc->Document         = calloc(1, MaxDocumentSize);
        }
        uint8_t CharSize          = FileIO_GetEncodingSize(Doc->Stream);
        if (CharSize == 1) {
            Doc->StringType       = StringType_UTF8;
        } else if (CharSize == 2) {
            Doc->StringType       = StringType_UTF16;
        } else if (CharSize == 4) {
            Doc->StringType       = StringType_UTF32;
        }
        return Doc;
    }

    static bool CheckToken8(XMLDocument *Doc, StringIO_WhitespaceTypes WhitespaceType, UTF8 *Token) {
        /*
         We need to update the DocOffset

         So, how do we do that? like, we need to check for tokens in whatever encoding, if the tokens are found we need to know the start and the length of the found token

         then we need to update the DocOffset to the end of the token

         then we need to return a bool value so we know if the token was found
         */
        bool TokenFound     = false;
        UTF8 *Document      = Doc->Document;
        int64_t TokenStart  = UTF8_Find(&Document[Doc->DocOffset], Doc->DocSize - Doc->DocOffset, Token, WhitespaceType);
        if (TokenStart != -1) {
            TokenFound      = true;
            Doc->DocOffset += TokenStart;
        }
        return TokenStart;
    }

    static void Tokenizer8(XMLDocument *Doc) {
        uint64_t Offset = Doc->DocOffset;
        UTF8 *Document  = Doc->Document;
        while (Offset < Doc->DocSize) {
            UTF32 CodePoint = UTF8_DecodeCodePoint(&Document[Offset]);
            uint64_t CodePointSize = UTF8_GetCodePointSizeInCodeUnits(Document[Offset]);
            while (UTF32_IsCodePointInTable(LineBreakTable, LineBreakTableSize, CodePoint)) {
                Doc->DocOffset += CodePointSize;
            }
            /* So, we're skipping all linebreaks in search of tokens; What do we do now? */
            // Check the codepoint to see how it starts
            switch (CodePoint) {
                case '<': {
                    CodePoint     = UTF8_DecodeCodePoint(&Document[Offset + CodePointSize]);
                    CodePointSize = UTF8_GetCodePointSizeInCodeUnits(Document[Offset + CodePointSize]);
                    if (CodePoint == '?') {
                ParseInstruction(Doc);
                    } else if (CodePoint == '!') {
                        CodePoint     = UTF8_DecodeCodePoint(&Document[Offset + CodePointSize]);
                        CodePointSize = UTF8_GetCodePointSizeInCodeUnits(Document[Offset + CodePointSize]);
                        if (CodePoint == '-') {
                    ParseComment(Doc);
                        } else if (CodePoint == '[') {
                        ParseCDATA(Doc);
                        }
                    } else {
                        // Tag
                        ParseTag(Doc);
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }

    static void ParseInstruction8(XMLDocument *Doc) {
        UTF8 *Document = Doc->Document;

        size_t DeclOffset1 = UTF8_Find(&Document[Doc->DocOffset], Doc->DocSize - Doc->DocOffset, "<?", WhitespaceType_Insignificant); // Offset of "<?"
                                                                                                                                      // Offset of "xml"

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '?' && Document[Doc->DocOffset + 2] == 'x' && Document[Doc->DocOffset + 3] == 'm' && Document[Doc->DocOffset + 4] == 'l') {
            while (Document[Doc->DocOffset] != '?' && Document[Doc->DocOffset] != '>') {
                // Check for version, standalone, and encoding
                size_t EncodingOffset   = UTF8_Find(Document, Doc->DocSize, "encoding", WhitespaceType_Insignificant);
                size_t VersionOffset    = UTF8_Find(Document, Doc->DocSize, "version", WhitespaceType_Insignificant);
                size_t StandaloneOffset = UTF8_Find(Document, Doc->DocSize, "standalone", WhitespaceType_Insignificant);

                if (EncodingOffset != -1) {
                    // Skip the equals and quote
                    // Parse the encoding field
                } else if (VersionOffset != -1) {
                    // Parse the Version field
                } else if (StandaloneOffset != -1) {
                    // Parse the Standalone field
                }
            }
        }
    }

    static void ParseCDATA8(XMLDocument *Doc) {
        // <![CDATA[Content]]>
        UTF8 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '!' && Document[Doc->DocOffset + 2] == '[' &&
            Document[Doc->DocOffset + 3] == 'C' && Document[Doc->DocOffset + 4] == 'D' && Document[Doc->DocOffset + 5] == 'A' &&
            Document[Doc->DocOffset + 6] == 'T' && Document[Doc->DocOffset + 7] == 'A' && Document[Doc->DocOffset + 8] == '[') {
            size_t CDATAStart = Doc->DocOffset + 9;
            size_t CDATASize  = 0;
            while (Document[CDATAStart] != ']' && Document[CDATAStart + 1] != ']' && Document[CDATAStart + 2] != '>') {
                CDATASize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType = XMLTokenType_CDATA;
            Doc->Nodes[Doc->NumNodes + 1].Slice    = TextIO_Slice_Init(CDATAStart, CDATAStart + CDATASize);
        }
    }

    static void ParseReference8(XMLDocument *Doc) {
        UTF8 *Document = Doc->Document;

        if (Document[Doc->DocOffset] == '#') { // Some kind of number, either decimal or hex
            Doc->DocOffset += 1;
            if (Document[Doc->DocOffset] >= '0' && Document[Doc->DocOffset] <= '9') { // Decimal
                                                                                      // Keep reading decimal digits until they end
                uint8_t NumDigits = UTF8_GetNumDigits(Base_Integer | Base_Radix10, Document);
                Doc->DocOffset += NumDigits;
                // Extract the digits and create a node, so it can be represented
            } else if (Document[Doc->DocOffset] == 'x') { // Hex
                                                          // keep reading hex digits until they end
                uint8_t NumDigits = UTF8_GetNumDigits(Base_Integer | Base_Radix16 | Base_Uppercase | Base_Lowercase, Document);
            }
        } else if (Document[Doc->DocOffset] == 'a'  && Document[Doc->DocOffset + 1] == 'p'  && Document[Doc->DocOffset + 2] == 'o' && Document[Doc->DocOffset + 2] == 's') { // apos
                                                                                                                                                                             // Replace with '\''
        } else if (Document[Doc->DocOffset] == 'q'  && Document[Doc->DocOffset + 1] == 'u'  && Document[Doc->DocOffset + 2] == 'o'  && Document[Doc->DocOffset + 3] == 't') { // quot
                                                                                                                                                                              // Replace with '\"'
        } else if (Document[Doc->DocOffset] == 'a'  && Document[Doc->DocOffset + 1] == 'm'  && Document[Doc->DocOffset + 2] == 'p') {
            // Replace with '&'
        } else if (Document[Doc->DocOffset] == 'l' && Document[Doc->DocOffset + 1] == 't') {
            // Replace with '<'
        } else if (Document[Doc->DocOffset] == 'g' && Document[Doc->DocOffset + 1] == 't') {
            // Replace with '>'
        }
    }

    static void ParseComment8(XMLDocument *Doc) {
        // Starts with '<!--' ends with '-->'
        UTF8 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '!' && Document[Doc->DocOffset + 2] == '-' &&
            Document[Doc->DocOffset + 3] == '-') {
            size_t CommentStart = Doc->DocOffset + 4;
            size_t CommentSize  = 0;
            while (Document[CommentStart] != '-' && Document[CommentStart + 1] != '-' && Document[CommentStart + 2] != '>') {
                CommentSize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType         = XMLTokenType_Comment;
            Doc->Namespaces[Doc->NumNamespaces].Identifier = TextIO_Slice_Init(CommentStart, CommentStart + CommentSize);
        }
    }

    static void ParseNamespaceID8(XMLDocument *Doc) {
        UTF8 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '=' && Document[Doc->DocOffset + 1] == '\"') {
            size_t IDStart = Doc->DocOffset + 1;
            size_t IDSize  = 0;
            while (Document[IDStart] != '\"') {
                IDSize += 1;
            }
            Doc->DocOffset += IDSize + 1;
            Doc->Namespaces[Doc->NumNamespaces].Identifier = TextIO_Slice_Init(IDStart, IDStart + IDSize);
        }
    }

    static void ParseNamespaceDeclaration8(XMLDocument *Doc) {
        UTF8 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == 'x' && Document[Doc->DocOffset + 1] == 'm' && Document[Doc->DocOffset + 2] == 'l' && Document[Doc->DocOffset + 3] == 'n' && Document[Doc->DocOffset + 4] == 's') {
            if (Document[Doc->DocOffset + 5] == ':') { // Explicit namespace
                size_t NamespaceNameStart = Doc->DocOffset + 6;
                size_t NamespaceNameSize  = 0;
                while (Document[NamespaceNameStart] != '=') {
                    NamespaceNameSize += 1;
                }
                Doc->Nodes[Doc->NumNodes + 1].NodeType               = XMLTokenType_Namespace;
                Doc->Nodes[Doc->NumNodes + 1].Slice                  = TextIO_Slice_Init(NamespaceNameStart, NamespaceNameStart + NamespaceNameSize);
                ParseNamespaceID(Doc);
            } else if (Document[Doc->DocOffset + 5] == '=') { // Implicit namespace
                Doc->Namespaces[Doc->NumNamespaces].Identifier       = TextIO_Slice_Init(Doc->DocOffset + 6, Doc->DocOffset + 6);
                ParseNamespaceID(Doc);
            }
        }
    }
    /* UTF-8 */

    /* UTF-16 */
    static UTF16 NextCharacter_UTF16(XMLDocument *Doc) { // Make sure the document is in bounds
        UTF16 Character   = 0;
        Doc->DocOffset   += 1;
        UTF16 *Document16 = Doc->Document;
        Character         = Document16[Doc->DocOffset];
        return Character;
    }

    static UTF16 NextNonWhitespaceCharacter_UTF32(XMLDocument *Doc) {
        UTF16 Character     = NextCharacter_UTF16(Doc);
        while (UTF32_IsWhitespace(Character)) {
            Doc->DocOffset += 1;
            Character       = NextCharacter_UTF32(Doc);
        }
        return Character;
    }

    static UTF16 NextCharacter_UTF16(XMLDocument *Doc) {
        UTF16 Character   = 0;
        Doc->DocOffset   += 1;
        UTF16 *Document16 = Doc->Document;
        Character         = Document16[Doc->DocOffset];
        return Character;
    }

    XMLDocument *XMLDocument_Init16_FromPath(UTF16 *DocumentPath, size_t MaxDocumentSize, size_t MaxNodes) {
      XMLDocument *Doc = calloc(1, sizeof(XMLDocument));
        Doc->Stream               = AsyncIOStream_Init();
        AsyncIOStream_OpenPathUTF16(Doc->Stream, DocumentPath, FileMode_Read);
        if (MaxDocumentSize <= 0) {
            int64_t DocSize       = AsyncIOStream_GetSize(Doc->Stream);
            Doc->Document         = calloc(1, DocSize);
            if (MaxNodes > 5) {
                Doc->Nodes        = calloc(MaxNodes, sizeof(XMLNode));
            } else {
                Doc->Nodes        = calloc(DocSize / 5, sizeof(XMLNode));
            }
        } else {
            Doc->Document         = calloc(1, MaxDocumentSize);
        }
        uint8_t CharSize          = FileIO_GetEncodingSize(Doc->Stream);
        if (CharSize == 1) {
            Doc->StringType       = StringType_UTF8;
        } else if (CharSize == 2) {
            Doc->StringType       = StringType_UTF16;
        } else if (CharSize == 4) {
            Doc->StringType       = StringType_UTF32;
        }
        return Doc;
    }

    static void ParseInstruction16(XMLDocument *Doc) {
        UTF16 *Document = Doc->Document;

        size_t DeclOffset1 = UTF16_Find(&Document[Doc->DocOffset], Doc->DocSize - Doc->DocOffset, "<?", WhitespaceType_Insignificant); // Offset of "<?"
                                                                                                                                       // Offset of "xml"

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '?' && Document[Doc->DocOffset + 2] == 'x' && Document[Doc->DocOffset + 3] == 'm' && Document[Doc->DocOffset + 4] == 'l') {
            while (Document[Doc->DocOffset] != '?' && Document[Doc->DocOffset] != '>') {
                // Check for version, standalone, and encoding
                size_t EncodingOffset   = UTF16_Find(Document, Doc->DocSize, "encoding", WhitespaceType_Insignificant);
                size_t VersionOffset    = UTF16_Find(Document, Doc->DocSize, "version", WhitespaceType_Insignificant);
                size_t StandaloneOffset = UTF16_Find(Document, Doc->DocSize, "standalone", WhitespaceType_Insignificant);

                if (EncodingOffset != -1) {
                    // Skip the equals and quote
                    // Parse the encoding field
                } else if (VersionOffset != -1) {
                    // Parse the Version field
                } else if (StandaloneOffset != -1) {
                    // Parse the Standalone field
                }
            }
        }
    }

    static void ParseReference16(XMLDocument *Doc) {
        UTF16 *Document = Doc->Document;

        if (Document[Doc->DocOffset] == '#') { // Some kind of number, either decimal or hex
            Doc->DocOffset += 1;
            if (Document[Doc->DocOffset] >= '0' && Document[Doc->DocOffset] <= '9') { // Decimal
                                                                                      // Keep reading decimal digits until they end
                uint8_t NumDigits = UTF16_GetNumDigits(Base_Integer | Base_Radix10, Document);
                Doc->DocOffset += NumDigits;
                // Extract the digits and create a node, so it can be represented
            } else if (Document[Doc->DocOffset] == 'x') { // Hex
                                                          // keep reading hex digits until they end
                uint8_t NumDigits = UTF16_GetNumDigits(Base_Integer | Base_Radix16 | Base_Uppercase | Base_Lowercase, Document);
            }
        } else if (Document[Doc->DocOffset] == 'a'  && Document[Doc->DocOffset + 1] == 'p'  && Document[Doc->DocOffset + 2] == 'o' && Document[Doc->DocOffset + 2] == 's') { // apos
                                                                                                                                                                             // Replace with '\''
        } else if (Document[Doc->DocOffset] == 'q'  && Document[Doc->DocOffset + 1] == 'u'  && Document[Doc->DocOffset + 2] == 'o'  && Document[Doc->DocOffset + 3] == 't') { // quot
                                                                                                                                                                              // Replace with '\"'
        } else if (Document[Doc->DocOffset] == 'a'  && Document[Doc->DocOffset + 1] == 'm'  && Document[Doc->DocOffset + 2] == 'p') {
            // Replace with '&'
        } else if (Document[Doc->DocOffset] == 'l' && Document[Doc->DocOffset + 1] == 't') {
            // Replace with '<'
        } else if (Document[Doc->DocOffset] == 'g' && Document[Doc->DocOffset + 1] == 't') {
            // Replace with '>'
        }
    }

    static void ParseCDATA16(XMLDocument *Doc) {
        // <![CDATA[Content]]>
        UTF16 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '!' && Document[Doc->DocOffset + 2] == '[' &&
            Document[Doc->DocOffset + 3] == 'C' && Document[Doc->DocOffset + 4] == 'D' && Document[Doc->DocOffset + 5] == 'A' &&
            Document[Doc->DocOffset + 6] == 'T' && Document[Doc->DocOffset + 7] == 'A' && Document[Doc->DocOffset + 8] == '[') {
            size_t CDATAStart = Doc->DocOffset + 9;
            size_t CDATASize  = 0;
            while (Document[CDATAStart] != ']' && Document[CDATAStart + 1] != ']' && Document[CDATAStart + 2] != '>') {
                CDATASize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType = XMLTokenType_CDATA;
            Doc->Nodes[Doc->NumNodes + 1].Slice    = TextIO_Slice_Init(CDATAStart, CDATAStart + CDATASize);
        }
    }

    static void ParseComment16(XMLDocument *Doc) {
        UTF16 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '!' && Document[Doc->DocOffset + 2] == '-' &&
            Document[Doc->DocOffset + 3] == '-') {
            size_t CommentStart = Doc->DocOffset + 4;
            size_t CommentSize  = 0;
            while (Document[CommentStart] != '-' && Document[CommentStart + 1] != '-' && Document[CommentStart + 2] != '>') {
                CommentSize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType         = XMLTokenType_Comment;
            Doc->Namespaces[Doc->NumNamespaces].Identifier = TextIO_Slice_Init(CommentStart, CommentStart + CommentSize);
        }
    }

    static void ParseNamespaceID16(XMLDocument *Doc) {
        UTF16 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '=' && Document[Doc->DocOffset + 1] == '\"') {
            size_t IDStart = Doc->DocOffset + 1;
            size_t IDSize  = 0;
            while (Document[IDStart] != '\"') {
                IDSize += 1;
            }
            Doc->DocOffset += IDSize + 1;
            Doc->Namespaces[Doc->NumNamespaces].Identifier = TextIO_Slice_Init(IDStart, IDStart + IDSize);
        }
    }

    static void ParseNamespaceDeclaration16(XMLDocument *Doc) {
        UTF16 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 5] == ':') { // Explicit namespace
            size_t NamespaceNameStart = Doc->DocOffset + 6;
            size_t NamespaceNameSize  = 0;
            while (Document[NamespaceNameStart] != '=') {
                NamespaceNameSize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType               = XMLTokenType_Namespace;
            Doc->Nodes[Doc->NumNodes + 1].Slice                  = TextIO_Slice_Init(NamespaceNameStart, NamespaceNameStart + NamespaceNameSize);
            ParseNamespaceID(Doc);
        } else if (Document[Doc->DocOffset + 5] == '=') { // Implicit namespace
            Doc->Namespaces[Doc->NumNamespaces].Name = TextIO_Slice_Init(Doc->DocOffset + 6, Doc->DocOffset + 6);
            ParseNamespaceID(Doc);
        }
    }
    /* UTF-16 */

    /* UTF-32 */
    static UTF32 NextCharacter_UTF32(XMLDocument *Doc) { // Make sure the document is in bounds
        UTF32 Character   = 0;
        Doc->DocOffset   += 1;
        UTF32 *Document32 = Doc->Document;
        Character         = Document32[Doc->DocOffset];
        return Character;
    }

    static UTF32 NextNonWhitespaceCharacter_UTF8(XMLDocument *Doc) {
        UTF32 Character     = NextCharacter_UTF32(Doc);
        while (UTF32_IsWhitespace(Character)) {
            Doc->DocOffset += 1;
            Character       = NextCharacter_UTF32(Doc);
        }
        return Character;
    }

    XMLDocument *XMLDocument_Init32(UTF32 *DocumentPath, size_t MaxDocumentSize, size_t MaxNodes) {
        XMLDocument *Doc = calloc(1, sizeof(XMLDocument));
        Doc->Stream               = AsyncIOStream_Init();
        UTF16 *DocumentPath16     = UTF16_Encode(DocumentPath);
        AsyncIOStream_OpenPathUTF16(Doc->Stream, DocumentPath16, FileMode_Read);
        if (MaxDocumentSize <= 0) {
            int64_t DocSize       = AsyncIOStream_GetSize(Doc->Stream);
            Doc->Document         = calloc(1, DocSize);
            if (MaxNodes > 5) {
                Doc->Nodes        = calloc(MaxNodes, sizeof(XMLNode));
            } else {
                Doc->Nodes        = calloc(DocSize / 5, sizeof(XMLNode));
            }
        } else {
            Doc->Document         = calloc(1, MaxDocumentSize);
        }
        Doc->NumNamespaces        = 0;
        Doc->Namespaces           = calloc(1, sizeof(XMLNamespace));
        uint8_t CharSize          = FileIO_GetEncodingSize(Doc->Stream);
        if (CharSize == 1) {
            Doc->StringType       = StringType_UTF8;
        } else if (CharSize == 2) {
            Doc->StringType       = StringType_UTF16;
        } else if (CharSize == 4) {
            Doc->StringType       = StringType_UTF32;
        }
        return Doc;
    }

    static void ParseInstruction32(XMLDocument *Doc) {
        UTF32 *Document = Doc->Document;

        size_t DeclOffset1 = UTF32_Find(&Document[Doc->DocOffset], Doc->DocSize - Doc->DocOffset, "<?", WhitespaceType_Insignificant); // Offset of "<?"
                                                                                                                                       // Offset of "xml"

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '?' && Document[Doc->DocOffset + 2] == 'x' && Document[Doc->DocOffset + 3] == 'm' && Document[Doc->DocOffset + 4] == 'l') {
            while (Document[Doc->DocOffset] != '?' && Document[Doc->DocOffset] != '>') {
                // Check for version, standalone, and encoding
                size_t EncodingOffset   = UTF32_Find(Document, Doc->DocSize, "encoding", WhitespaceType_Insignificant);
                size_t VersionOffset    = UTF32_Find(Document, Doc->DocSize, "version", WhitespaceType_Insignificant);
                size_t StandaloneOffset = UTF32_Find(Document, Doc->DocSize, "standalone", WhitespaceType_Insignificant);

                if (EncodingOffset != -1) {
                    // Skip the equals and quote
                    // Parse the encoding field
                } else if (VersionOffset != -1) {
                    // Parse the Version field
                } else if (StandaloneOffset != -1) {
                    // Parse the Standalone field
                }
            }
        }
    }

    static bool CheckToken32(XMLDocument *Doc, StringIO_WhitespaceTypes WhitespaceType, UTF32 *Token) {
        /*
         We need to update the DocOffset

         So, how do we do that? like, we need to check for tokens in whatever encoding, if the tokens are found we need to know the start and the length of the found token

         then we need to update the DocOffset to the end of the token

         then we need to return a bool value so we know if the token was found
         */
        bool   TokenFound   = false;
        UTF32 *Document     = Doc->Document;
        int64_t TokenStart  = UTF32_Find(&Document[Doc->DocOffset], Doc->DocSize - Doc->DocOffset, Token, WhitespaceType);
        if (TokenStart != -1) {
            TokenFound      = true;
            Doc->DocOffset += TokenStart;
        }
        return TokenStart;
    }

    static void ParseReference32(XMLDocument *Doc) {
        UTF32 *Document = Doc->Document;

        if (Document[Doc->DocOffset] == '#') { // Some kind of number, either decimal or hex
            Doc->DocOffset += 1;
            if (Document[Doc->DocOffset] >= '0' && Document[Doc->DocOffset] <= '9') { // Decimal
                                                                                      // Keep reading decimal digits until they end
                uint8_t NumDigits = UTF32_GetNumDigits(Base_Integer | Base_Radix10, Document);
                Doc->DocOffset += NumDigits;
                // Extract the digits and create a node, so it can be represented
            } else if (Document[Doc->DocOffset] == 'x') { // Hex
                                                          // keep reading hex digits until they end
                uint8_t NumDigits = UTF32_GetNumDigits(Base_Integer | Base_Radix16 | Base_Uppercase | Base_Lowercase, Document);
            }
        } else if (Document[Doc->DocOffset] == 'a'  && Document[Doc->DocOffset + 1] == 'p'  && Document[Doc->DocOffset + 2] == 'o' && Document[Doc->DocOffset + 2] == 's') { // apos
                                                                                                                                                                             // Replace with '\''
        } else if (Document[Doc->DocOffset] == 'q'  && Document[Doc->DocOffset + 1] == 'u'  && Document[Doc->DocOffset + 2] == 'o'  && Document[Doc->DocOffset + 3] == 't') { // quot
                                                                                                                                                                              // Replace with '\"'
        } else if (Document[Doc->DocOffset] == 'a'  && Document[Doc->DocOffset + 1] == 'm'  && Document[Doc->DocOffset + 2] == 'p') {
            // Replace with '&'
        } else if (Document[Doc->DocOffset] == 'l' && Document[Doc->DocOffset + 1] == 't') {
            // Replace with '<'
        } else if (Document[Doc->DocOffset] == 'g' && Document[Doc->DocOffset + 1] == 't') {
            // Replace with '>'; How do we replace entities when the document is mostly gonna be string slices?
        }
    }

    static void ParseCDATA32(XMLDocument *Doc) {
        // <![CDATA[Content]]>

        UTF32 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '!' && Document[Doc->DocOffset + 2] == '[' &&
            Document[Doc->DocOffset + 3] == 'C' && Document[Doc->DocOffset + 4] == 'D' && Document[Doc->DocOffset + 5] == 'A' &&
            Document[Doc->DocOffset + 6] == 'T' && Document[Doc->DocOffset + 7] == 'A' && Document[Doc->DocOffset + 8] == '[') {
            size_t CDATAStart = Doc->DocOffset + 9;
            size_t CDATASize  = 0;
            while (Document[CDATAStart] != ']' && Document[CDATAStart + 1] != ']' && Document[CDATAStart + 2] != '>') {
                CDATASize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType = XMLTokenType_CDATA;
            Doc->Nodes[Doc->NumNodes + 1].Slice    = TextIO_Slice_Init(CDATAStart, CDATAStart + CDATASize);
        }
    }

    static void ParseComment32(XMLDocument *Doc) {
        UTF32 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '<' && Document[Doc->DocOffset + 1] == '!' && Document[Doc->DocOffset + 2] == '-' &&
            Document[Doc->DocOffset + 3] == '-') {
            size_t CommentStart = Doc->DocOffset + 4;
            size_t CommentSize  = 0;
            while (Document[CommentStart] != '-' && Document[CommentStart + 1] != '-' && Document[CommentStart + 2] != '>') {
                CommentSize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType               = XMLTokenType_Comment;
            Doc->Namespaces[Doc->NumNamespaces].Identifier = TextIO_Slice_Init(CommentStart, CommentStart + CommentSize);
        }
    }

    static void ParseNamespaceID32(XMLDocument *Doc) {
        UTF32 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 0] == '=' && Document[Doc->DocOffset + 1] == '\"') {
            size_t IDStart = Doc->DocOffset + 1;
            size_t IDSize  = 0;
            while (Document[IDStart] != '\"') {
                IDSize += 1;
            }
            Doc->DocOffset += IDSize + 1;
            Doc->Namespaces[Doc->NumNamespaces].Identifier = TextIO_Slice_Init(IDStart, IDStart + IDSize);
        }
    }

    static void ParseNamespaceDeclaration32(XMLDocument *Doc) {
        UTF32 *Document = Doc->Document;

        if (Document[Doc->DocOffset + 5] == ':') { // Explicit namespace
            size_t NamespaceNameStart = Doc->DocOffset + 6;
            size_t NamespaceNameSize  = 0;
            while (Document[NamespaceNameStart] != '=') {
                NamespaceNameSize += 1;
            }
            Doc->Nodes[Doc->NumNodes + 1].NodeType                = XMLTokenType_Namespace;
            Doc->Nodes[Doc->NumNodes + 1].Slice                   = TextIO_Slice_Init(NamespaceNameStart, NamespaceNameStart + NamespaceNameSize);
            ParseNamespaceID(Doc);
        } else if (Document[Doc->DocOffset + 5] == '=') { // Implicit namespace
            Doc->Namespaces[Doc->NumNamespaces].Name = TextIO_Slice_Init(Doc->DocOffset + 6, Doc->DocOffset + 6);
            ParseNamespaceID(Doc);
        }
    }
    /* UTF-32 */

    /*
     XMLDocument:
     
     <?xml version="1.0" encoding="utf-8"?>
     <Tests xmlns="http://www.adatum.com">
     <Test TestId="0001" TestType="CMD">
     <Name>Convert number to string</Name>
     <CommandLine>Examp1.EXE</CommandLine>
     <Input>1</Input>
     <Output>One</Output>
     </Test>
     <Test TestId="0002" TestType="CMD">
     <Name>Find succeeding characters</Name>
     <CommandLine>Examp2.EXE</CommandLine>
     <Input>abc</Input>
     <Output>def</Output>
     </Test>
     <Test TestId="0003" TestType="GUI">
     <Name>Convert multiple numbers to strings</Name>
     <CommandLine>Examp2.EXE /Verbose</CommandLine>
     <Input>123</Input>
     <Output>One Two Three</Output>
     </Test>
     <Test TestId="0004" TestType="GUI">
     <Name>Find correlated key</Name>
     <CommandLine>Examp3.EXE</CommandLine>
     <Input>a1</Input>
     <Output>b1</Output>
     </Test>
     <Test TestId="0005" TestType="GUI">
     <Name>Count characters</Name>
     <CommandLine>FinalExamp.EXE</CommandLine>
     <Input>This is a test</Input>
     <Output>14</Output>
     </Test>
     <Test TestId="0006" TestType="GUI">
     <Name>Another Test</Name>
     <CommandLine>Examp2.EXE</CommandLine>
     <Input>Test Input</Input>
     <Output>10</Output>
     </Test>
     </Tests>
     */
    
    /*
     So, we need to know what the next token is so we can handle it.
     
     Well hold up.
     
     We need to skip any whitespace, check if the token matches, and then change the DocOffset to past it.
     */

    /*
     Now we just need to work on the Tokenizer/Lexer

     Main issues:
     Tokens are multiple characters
     Whitespace sometimes needs to be skipped, especially if it's in between tokens
     so, while loop, if space or newline, increment

     Ok, so parsing means that we need to identify tokens, sooo what we need is two functions, one to identify tokens, and the next to parse the tokens
     */
    
#define CheckToken(XMLDocument, WhitespaceType, Token2Check) _Generic((Token2Check), signed char*:CheckToken8, unsigned char*:CheckToken8, signed short*:CheckToken16, unsigned short*:CheckToken16, signed int*:CheckToken32, unsigned int*:CheckToken32)(XMLDocument, WhitespaceType, Token2Check)
    
    /*
     PeekToken = Read the token without moving the Offset, which means we need a list of tokens...
     EatToken  = Read the token and move the Offset.
     */
    
    /*
     
     Sooo XML tokens
     
     We need an enum to handle the identity of the token types.
     
     and then we need the actual values and we gotta support UTF-8, UTF-16, and UTF-32

     Stages:

     Read the document, searching through text comparing tokens
     */

    void DOM(XMLDocument *Doc) {
        /*
         Ok, so here we create a DOM based on the document, aka a tree of tags for the whole document
         */
    }

    void Pull(XMLDocument *Doc) {
        /*
         This is the pull parser interface, basically we just create a node list for each XML element
         */
    }

    /*
     In other words, the DOM interface could be built above the pull parser interface...

     So:

     DOM -> Pull -> Tokenize
     */

    void Tokenizer8(XMLDocument *Doc) {
        UTF8 Character = Doc->Document[Doc->DocOffset];
        if (Character == '<') { // Open
            Character = NextCharacter_UTF8(Doc);
            if (Character == '!') {
                // Could be CDATA
                ParseCDATA(Doc);
            } else if (Character == '?') {
                // Instuction?
                ParseInstruction(Doc);
            } else if (Character == '/') {
                // Start of a close tag
            } else {
                // Just an open tag
            }
        } else if (Character == '?') {
            // Probably the end of an instruction, check for '>'
            Character = NextCharacter_UTF8(Doc);
        }

        /*
         The tokenizer just reads each token, DOM/pull interface is above this
         */

        /*
         Ok, so we have a list of token-strings we need to read, like <, <?, <![, <!--,
         */

    }

    void Tokenizer(XMLDocument *Doc) {
        switch (Doc->StringType) {
            case StringType_UTF8:
                Tokenizer8(Doc);
                break;

            case StringType_UTF16:
                Tokenizer16(Doc);
                break;

            case StringType_UTF32:
                Tokenizer32(Doc);
                break;
                
            case StringType_Unspecified:
            default:
            AssertIO("Unknown Encoding");
                break;
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
