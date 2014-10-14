/*==================================================================*\
|                EXIP - Embeddable EXI Processor in C                |
|--------------------------------------------------------------------|
|          This work is licensed under BSD 3-Clause License          |
|  The full license terms and conditions are located in LICENSE.txt  |
\===================================================================*/

/**
 * @file procTypes.h
 * @brief Common structure types used throughout the project
 *
 * @date Jul 7, 2010
 * @author Rumen Kyusakov
 * @version 0.5
 * @par[Revision] $Id: procTypes.h 328 2013-10-30 16:00:10Z kjussakov $
 */

#ifndef PROCTYPES_H_
#define PROCTYPES_H_

#include "errorHandle.h"
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "exipConfig.h"
#include <limits.h>

enum boolean
{
	FALSE = 0,
	TRUE  = 1
};

typedef enum boolean boolean;

#ifndef NULL
#define NULL ((void *)0)
#endif

/** An item in a Generic Stack structure */
struct stackNode
{
	void* item;
	struct stackNode* nextInStack;
};

typedef struct stackNode GenericStack;

/**
 * @def REVERSE_BIT_POSITION(p)
 * Given a bit position inside an octet (0-7)
 * returns the reversed position index.
 */
#define REVERSE_BIT_POSITION(p) (7 - p)

/**
 * @name EXI options related macros
 * Example usage:
 * @code
 *   	SET_ALIGNMENT(enumOpt, BYTE_ALIGNMENT)
 * @endcode
 */

/**@{*/
#define BIT_PACKED      0x00 // 0b00000000
#define BYTE_ALIGNMENT  0x40 // 0b01000000
#define PRE_COMPRESSION 0x80 // 0b10000000
#define ALIGNMENT       0xc0 // 0b11000000

#define COMPRESSION     0x01 // 0b00000001
#define STRICT          0x02 // 0b00000010
#define FRAGMENT        0x04 // 0b00000100
#define SELF_CONTAINED  0x08 // 0b00001000

#define GET_ALIGNMENT(p)       ((p) & ALIGNMENT)
#define WITH_COMPRESSION(p)    (((p) & COMPRESSION) != 0)
#define WITH_STRICT(p)         (((p) & STRICT) != 0)
#define WITH_FRAGMENT(p)       (((p) & FRAGMENT) != 0)
#define WITH_SELF_CONTAINED(p) (((p) & SELF_CONTAINED) != 0)

#define SET_ALIGNMENT(p, align_const) ((p) = (p) | align_const)
#define SET_COMPRESSION(p)            ((p) = (p) | COMPRESSION)
#define SET_STRICT(p)                 ((p) = (p) | STRICT)
#define SET_FRAGMENT(p)               ((p) = (p) | FRAGMENT)
#define SET_SELF_CONTAINED(p)         ((p) = (p) | SELF_CONTAINED)

/**@}*/

/**
 * @name Handling of SchemaID header field
 *
 * SchemaID option modes:
 * @def SCHEMA_ID_ABSENT
 * 		default,  no statement is made about the schema information
 * @def SCHEMA_ID_SET
 * 		some sting identification of the schema is given
 * @def SCHEMA_ID_NIL
 * 		no schema information is used for processing the EXI body (i.e. a schema-less EXI stream)
 * @def SCHEMA_ID_EMPTY
 * 		no user defined schema information is used for processing the EXI body; however, the built-in XML schema types are available for use in the EXI body
 *
 * @see http://www.w3.org/TR/2011/REC-exi-20110310/#key-schemaIdOption
 */
enum SchemaIdMode
{
	SCHEMA_ID_ABSENT = 0,
	SCHEMA_ID_SET    = 1,
	SCHEMA_ID_NIL    = 2,
	SCHEMA_ID_EMPTY  = 3
};

typedef enum SchemaIdMode SchemaIdMode;

/**
 *	@name Fidelity options handling
 *
 *	Example usage:
 *	@code
 *	  SET_PRESERVED(preserve, PRESERVE_PREFIXES)
 *	@endcode
 *
 *	@see http://www.w3.org/TR/2011/REC-exi-20110310/#fidelityOptions
 **/

/**@{*/
#define PRESERVE_COMMENTS  0x01 // 0b00000001
#define PRESERVE_PIS       0x02 // 0b00000010
#define PRESERVE_DTD       0x04 // 0b00000100
#define PRESERVE_PREFIXES  0x08 // 0b00001000
#define PRESERVE_LEXVALUES 0x10 // 0b00010000

#define IS_PRESERVED(p, mask) (((p) & (mask)) != 0)
#define SET_PRESERVED(p, preserve_const) ((p) = (p) | (preserve_const))
/**@}*/

/**
 * The maximum allowed prefixes per namespace.
 * If there is a possibility that a document defines more than 4 prefixes per namespace
 * i.e. weird XML, MAXIMUM_NUMBER_OF_PREFIXES_PER_URI should be increased
 * @note This will require many changes - for example statically generated grammars from XML schemas needs to be rebuilt
 */
#ifndef MAXIMUM_NUMBER_OF_PREFIXES_PER_URI
# define MAXIMUM_NUMBER_OF_PREFIXES_PER_URI 4
#endif

/**
 * Fractional seconds = value * 10^-(offset+1) seconds;
 * Example:
 * offset = 4
 * value = 123
 * Fractional seconds = 123×10^−5 = 0.00123 seconds = 1.23 milliseconds
 * @note (offset+1) must be greater or equal than the number of digits in value
 */
struct fractionalSecs
{
	unsigned char offset;
	unsigned int value;
};

typedef struct fractionalSecs FractionalSecs;

/** DateTime type in EXIP.
 * Contains a standard "struct tm" combined with
 * fractional seconds and presence mask. */
struct EXIPDateTime
{
	/**
	 * As defined in time.h
	 * @note Decoding functions set negative values (INT_MIN) for the fields that are not available
	 */
	struct tm dateTime;
	FractionalSecs fSecs;
	int16_t TimeZone; // TZHours * 64 + TZMinutes

	/**
	 * Whether to included fractional seconds and timeZone information
	 * Use FRACT_PRESENCE and TZONE_PRESENCE masks
	 * and IS_PRESENT() macro
	 */
	uint8_t presenceMask;
};

typedef struct EXIPDateTime EXIPDateTime;

/**
 * @name Presence of fields in the EXIPDateTime type
 *
 * Example usage:
 * @code
 *   IS_PRESENT(presenceMask, FRACT_PRESENCE)
 * @endcode
 */
/**@{*/
#define TZONE_PRESENCE     0x01 // 0b00000001
#define FRACT_PRESENCE     0x02 // 0b00000010

#define IS_PRESENT(p, mask) (((p) & (mask)) != 0)
/**@}*/

#ifndef EXIP_UNSIGNED_INTEGER
# define EXIP_UNSIGNED_INTEGER uint64_t
#endif

typedef EXIP_UNSIGNED_INTEGER UnsignedInteger;

#ifndef EXIP_INTEGER
# define EXIP_INTEGER int64_t
#endif

typedef EXIP_INTEGER Integer;

/**
 * Represents base 10 (decimal) floating-point data.
 * The default Float representation in EXIP.
 * Maps directly to the EXI Float datatype.
 * Used for float and decimal data.
 *
 * @see http://www.w3.org/TR/2011/REC-exi-20110310/#encodingFloat
 */
struct EXIFloat
{
	int64_t mantissa;
	int16_t exponent;
};

#ifndef EXIP_FLOAT
# define EXIP_FLOAT struct EXIFloat
#endif

typedef EXIP_FLOAT Float;

/**
 * Used for the content handler interface for decimal values.
 * Application which require support for different type of decimal
 * representation (IEEE 754 or ISO/IEC/IEEE 60559:2011 standards) can
 * override this macro and re-define the decimal encoding/decoding
 * functions (not recommended). Instead:
 * On platforms supporting decimal floating types the conversion
 * between EXIP_FLOAT and _Decimal64 or _Decimal128 should be done
 * in the application code.
 *
 * @see http://gcc.gnu.org/onlinedocs/gcc/Decimal-Float.html#Decimal-Float
 * @see http://speleotrove.com/decimal/
 */
#ifndef EXIP_DECIMAL
# define EXIP_DECIMAL Float
#endif

typedef EXIP_DECIMAL Decimal;

#ifndef EXIP_INDEX
# define EXIP_INDEX size_t
#endif

typedef EXIP_INDEX Index;

#ifndef EXIP_INDEX_MAX
# define EXIP_INDEX_MAX SIZE_MAX
#endif

#define INDEX_MAX EXIP_INDEX_MAX

#ifndef EXIP_SMALL_INDEX
# define EXIP_SMALL_INDEX size_t
#endif

typedef EXIP_SMALL_INDEX SmallIndex;

#ifndef EXIP_SMALL_INDEX_MAX
# define EXIP_SMALL_INDEX_MAX SIZE_MAX
#endif

#define SMALL_INDEX_MAX EXIP_SMALL_INDEX_MAX

#ifndef EXIP_IMPLICIT_DATA_TYPE_CONVERSION
# define EXIP_IMPLICIT_DATA_TYPE_CONVERSION ON
#endif

/**
 * Defines the encoding used for characters.
 * It is dependent on the implementation of the stringManipulate.h functions
 * The default is ASCII characters (ASCII_stringManipulate.c)
 */
#ifndef CHAR_TYPE
# define CHAR_TYPE char
#endif

typedef CHAR_TYPE CharType;


#ifndef EXIP_STRTOLL
/** strtoll() function */
# define EXIP_STRTOLL strtoll
#endif

/**
 * Represents the length prefixed strings in EXIP
 */
struct StringType
{
	CharType* str;
	Index length;
};

typedef struct StringType String;

/**
 * Represent a fully qualified name
 */
struct QName {
	/**
	 * Pointer to a String value in the URI string table.
	 * @note It is not allowed to modify the string table content from this pointer.*/
	const String* uri;
	/**
	 * Pointer to a String value in the LN sting table.
	 * @note It is not allowed to modify the string table content from this pointer.
	 */
	const String* localName;
	/**
	 * Pointer to a String value in the Prefix string table if [Preserve prefixes] is TRUE and NULL otherwise.
	 * @note It is not allowed to modify the string table content from this pointer.
	 */
	const String* prefix;
};

typedef struct QName QName;

#define URI_MAX SMALL_INDEX_MAX
#define LN_MAX INDEX_MAX

/**
 * Position of a qname in the string tables
 */
struct QNameID {
	SmallIndex uriId;	// VOID == URI_MAX
	Index lnId;	// VOID == LN_MAX
};

typedef struct QNameID QNameID;

/**
 * @name Memory management definitions
 */
/**@{*/

/** @note Should not be bigger than SMALL_INDEX_MAX - 1 */
#define ALLOCATION_ARRAY_SIZE 100

/** An array of allocation pointers */
struct allocBlock {
	void* allocation[ALLOCATION_ARRAY_SIZE];
	struct allocBlock* nextBlock;
};

/** A list of allocation blocks.
 * Pass to initAllocList() before use. */
struct allocList {
	struct allocBlock* firstBlock;
	struct allocBlock* lastBlock;
	/** The next allocation slot in the allocation array of the lastBlock */
	SmallIndex currAllocSlot;
};

typedef struct allocList AllocList;

/**
 * Meta-data of generic dynamic array.
 * A concrete dynamic array is defined as follow:
 * @code
 *   struct [ConcreteDynamicArray]
 *   {
 *     DynArray dynArray;
 *     [ArrayEntryType]* base;
 *     Index count;
 *   };
 * @endcode
 */
struct dynArray {
	/**
	 * The size of an entry in bytes
	 */
	size_t entrySize;

	/**
	 * The initial size of the dynamic array (in number of entries), 
	 * also the chunk of number of entries to be added each expansion time
	 */
	Index chunkEntries;

	/**
	 * The total number of entries in the array
	 */
	Index arrayEntries;
};

typedef struct dynArray DynArray;

/**@}*/ //End Memory management definitions


/**
 * @name Grammar Types
 */
/**@{*/

/**
 * @page grammars Representing the EXI grammars in EXIP
 *
 * For a sample grammar G:
 * @code
 *   NonTerminal_0 :
 * 					Terminal_1    NonTerminal_0	0
 * 					Terminal_2    NonTerminal_1	1.0
 * 					Terminal_3    NonTerminal_1	1.1.0
 *
 * 	 NonTerminal_1 :
 * 					Terminal_4    NonTerminal_1	0
 * 					Terminal_5    NonTerminal_1	1
 * 					EE
 * @endcode
 *
 * A grammar production is for example "NonTerminal_0 : Terminal_1    NonTerminal_0	0".
 * All grammar productions with the same left-hand side define
 * a grammar rule.
 * Therefore, the grammar G has 2 grammar rules and 6 grammar productions.
 * <p>
 *   A single grammar production is represented by the struct Production.
 *   The [Terminal] is represented by the event type, the type of the value
 *   content if any and a fully qualified name.
 *   The [NonTerminal] is a non-negative integer value.
 *   The last part of the production (the event code) is implicitly encoded
 *   as a position of the production in the grammar rule.
 * </p>
 *
 * @section e_codes Event codes representation
 * The event codes in a production are represented as follow: <br/>
 * For every grammar rule there is an array of grammar productions.
 * It contains the productions with event codes with length 1 (having only one part).
 * The last production in the production array has event code
 * 0, the production before it 1 etc. - that is they are
 * stored in reverse order
 */

/**
 * Event types that occur in an EXI stream. This is moved from the grammar module because 
 * of the added "Void" element.
 * 
 * <table>
 * <tr><th>Name</th><th>Notation</th><th>Value</th></tr>
 * <tr><td>Start Document</td><td>SD         </td><td> 0</td></tr>
 * <tr><td>End Document  </td><td>ED         </td><td>10</td></tr>
 * <tr><td>Start Element </td><td>SE( qname )</td><td>20</td></tr>
 * <tr><td>Start Element </td><td>SE( uri:* )</td><td>21</td></tr>
 * <tr><td>Start Element </td><td>SE( * )    </td><td>22</td></tr>
 * <tr><td>End Element   </td><td>EE         </td><td>30</td></tr>
 * <tr><td>Attribute     </td><td>AT( qname )</td><td>31</td></tr>
 * <tr><td>Attribute     </td><td>AT( uri:* )</td><td>32</td></tr>
 * <tr><td>Attribute     </td><td>AT( * )    </td><td>40</td></tr>
 * <tr><td>Characters    </td><td>CH         </td><td>50</td></tr>
 * <tr><td>Namespace Decl</td><td>NS         </td><td>60</td></tr>
 * <tr><td>Comment       </td><td>CM         </td><td>70</td></tr>
 * <tr><td>Processng Inst</td><td>PI         </td><td>80</td></tr>
 * <tr><td>DOCTYPE       </td><td>DT         </td><td>90</td></tr>
 * <tr><td>Entity Refrnce</td><td>ER         </td><td>100</td></tr>
 * <tr><td>Self-contained</td><td>SC         </td><td>110</td></tr>
 * <tr><td>Void          </td><td>--         </td><td>255</td></tr>
 * </table>
 */
enum EventType
{
	EVENT_SD       =   0,
	EVENT_ED       =  10,
	EVENT_AT_QNAME =  20,
	EVENT_AT_URI   =  21,
	EVENT_AT_ALL   =  22,
	EVENT_SE_QNAME =  30,
	EVENT_SE_URI   =  31,
	EVENT_SE_ALL   =  32,
	EVENT_EE       =  40,
	EVENT_CH       =  50,
	EVENT_NS       =  60,
	EVENT_CM       =  70,
	EVENT_PI       =  80,
	EVENT_DT       =  90,
	EVENT_ER       = 100,
	EVENT_SC       = 110,
	/** Indicates lack of Terminal symbol in proto-grammars */
	EVENT_VOID     = 255
};

typedef enum EventType EventType;

enum EventTypeClass
{
	EVENT_SD_CLASS   =  0,
	EVENT_ED_CLASS   =  1,
	EVENT_AT_CLASS   =  2,
	EVENT_SE_CLASS 	 =  3,
	EVENT_EE_CLASS   =  4,
	EVENT_CH_CLASS   =  5,
	EVENT_NS_CLASS   =  6,
	EVENT_CM_CLASS   =  7,
	EVENT_PI_CLASS   =  8,
	EVENT_DT_CLASS   =  9,
	EVENT_ER_CLASS   = 10,
	EVENT_SC_CLASS   = 11,
	EVENT_VOID_CLASS = 25
};

typedef enum EventTypeClass EventTypeClass;

#define GET_EVENT_CLASS(evnt) (evnt/10)

/**
 * This is the type of the "value" content of EXI events. It is used when schema is available.
 */
enum EXIType
{
	VALUE_TYPE_NONE             =   0,
	VALUE_TYPE_STRING           =  10,
	VALUE_TYPE_FLOAT            =  20,
	VALUE_TYPE_DECIMAL          =  30,
	VALUE_TYPE_DATE_TIME        =  40,
	/** Used for xs:gYear type*/
	VALUE_TYPE_YEAR             =  41,
	/** Used for xs:gYearMonth and xs:date types */
	VALUE_TYPE_DATE             =  42,
	/** Used for xs:gMonth, xs:gMonthDay and xs:gDay types */
	VALUE_TYPE_MONTH            =  43,
	/** Used for xs:time type */
	VALUE_TYPE_TIME             =  44,
	VALUE_TYPE_BOOLEAN          =  50,
	VALUE_TYPE_BINARY           =  60,
	VALUE_TYPE_LIST             =  70,
	VALUE_TYPE_QNAME            =  80,
	VALUE_TYPE_INTEGER          =  90,
	VALUE_TYPE_SMALL_INTEGER    =  91,
	VALUE_TYPE_NON_NEGATIVE_INT =  92,
	VALUE_TYPE_UNTYPED          = 255
};

typedef enum EXIType EXIType;

/**
 * This is the type of the "value" content of EXI events. It is used when schema is available.
 */
enum EXITypeClass
{
	VALUE_TYPE_NONE_CLASS              =   0,
	VALUE_TYPE_STRING_CLASS            =   1,
	VALUE_TYPE_FLOAT_CLASS             =   2,
	VALUE_TYPE_DECIMAL_CLASS           =   3,
	VALUE_TYPE_DATE_TIME_CLASS         =   4,
	VALUE_TYPE_BOOLEAN_CLASS           =   5,
	VALUE_TYPE_BINARY_CLASS            =   6,
	VALUE_TYPE_LIST_CLASS              =   7,
	VALUE_TYPE_QNAME_CLASS             =   8,
	VALUE_TYPE_INTEGER_CLASS           =   9,
	VALUE_TYPE_UNTYPED_CLASS           =  25
};

typedef enum EXITypeClass EXITypeClass;

#define GET_VALUE_TYPE_CLASS(value_type) (value_type/10)

#define PROD_CONTENT_MASK 0xFFFFFF // 0b00000000111111111111111111111111

#define GET_PROD_EXI_EVENT(content) (content>>24)
#define GET_PROD_EXI_EVENT_CLASS(content) GET_EVENT_CLASS((content>>24))
#define SET_PROD_EXI_EVENT(content, eventType) (content = (content & PROD_CONTENT_MASK) | (((uint32_t) eventType)<<24))
#define GET_PROD_NON_TERM(content) (content & PROD_CONTENT_MASK)
#define SET_PROD_NON_TERM(content, nt) (content = (content & ~PROD_CONTENT_MASK) | (nt & PROD_CONTENT_MASK))

/** Specifies the right-hand side for a production rule.
 * The left-hand side is implicitly defined by the
 * parent grammar rule. */
struct Production
{
	/** The production content consists of two parts:
	 * - 8-bits EXI event type, like SD, CH, or EE (most significant 8 bits)
	 * - 24-bits unique identifier of right-hand side non-terminal (least significant 24 bits)
	 * Use GET_PROD_EXI_EVENT(), SET_PROD_EXI_EVENT(),
	 * */
	uint32_t content;

	/**
	 * Index of the grammar for the production event, if applicable.
	 * <ul>
	 *   <li> AT, CH: index of simple type, in EXIPSchema's simpleTypeTable </li>
	 *   <li> SE(qname): index of EXI grammar, in EXIPSchema's grammarTable.
	 *   Processes the content of the SE(qname) event.</li>
	 *   <li> Otherwise: INDEX_MAX </li>
	 * </ul>
	 */
	Index typeId;
	
	/**
	 * Identifies the terminal portion, the element/attribute for SE(qname), 
     * SE(uri:*), AT(qname) or AT(uri:*).
	 */
	QNameID qnameId;
};

typedef struct Production Production;

struct EventCode
{
	Index part[3];
	Index bits[3];
	unsigned char length;
};

typedef struct EventCode EventCode;

/**
 * @name Built-in Grammars non-terminals
 */
/**@{*/
/**
 * Used to indicate that the production does not have NON_TERMINAL on the right-hand side
 */
#ifndef GR_VOID_NON_TERMINAL
# define GR_VOID_NON_TERMINAL SMALL_INDEX_MAX
#endif

#define GR_DOC_CONTENT       0
#define GR_DOC_END           1

#define GR_START_TAG_CONTENT 0
#define GR_ELEMENT_CONTENT   1
#define GR_CONTENT_2 (GR_VOID_NON_TERMINAL-1)

#define GR_FRAGMENT_CONTENT  0
/**@}*/

/** 
 * A container for productions for a particular left-hand side non-terminal.
 */
struct GrammarRule
{
	/** Array of grammar productions with event code length 1 */
    Production* production;

    /** The number of productions */
    Index pCount;

    /** Meta information for the grammar rule:
     * - most significant 15 bits contain the number of AT(qname)[schema-typed value]  productions
     * - least significant bit is used to: (1) indicate whether the rule has an EE production
     * in case of Schema grammar OR (2) whether the rule contain AT(xsi:type) production in
     * case of Build-in element grammar */
    uint16_t meta;
};

typedef struct GrammarRule GrammarRule;

#define RULE_CONTAIN_EE_OR_XSI_TYPE_MASK       0x01 // 0b0000000000000001

#define RULE_CONTAIN_EE(meta) ((meta & RULE_CONTAIN_EE_OR_XSI_TYPE_MASK) != 0)
#define RULE_SET_CONTAIN_EE(meta) (meta = meta | RULE_CONTAIN_EE_OR_XSI_TYPE_MASK)
#define RULE_CONTAIN_XSI_TYPE(meta) ((meta & RULE_CONTAIN_EE_OR_XSI_TYPE_MASK) != 0)
#define RULE_SET_CONTAIN_XSI_TYPE(meta) (meta = meta | RULE_CONTAIN_EE_OR_XSI_TYPE_MASK)

#define RULE_SET_AT_COUNT(meta, ac) (meta = meta | (ac<<1))
#define RULE_GET_AT_COUNT(meta) (meta>>1)

#if BUILD_IN_GRAMMARS_USE
	/**
	 * Extension to the GrammarRule. In the DynGrammarRule the production array is a dynamic array.
	 * The dynamic GrammarRule is used for Built-in Element Grammar and Built-in Fragment Grammar
	 */
	struct DynGrammarRule
	{
		/** Array of grammar productions with event code length 1 */
		Production* production;

		/** The number of productions */
		Index pCount;

		/** Meta information for the grammar rule:
		 * - most significant 10 bits contain the number of AT(qname)[schema-typed value]  productions
		 * - least significant 5 bits contain the number of bits for productions event codes */
		uint16_t meta;

	   /** Additional field */
	   Index prodDim; // The size of the productions Dynamic production array /allocated space for Productions in it/
	};
	typedef struct DynGrammarRule DynGrammarRule;
#endif

/**
 * @name EXIGrammar properties handling
 */
/**@{*/
#define IS_NILLABLE(p) 			        ((p & GR_PROP_NILLABLE) != 0)
#define IS_BUILT_IN_ELEM(p) 	        ((p & GR_PROP_BUILT_IN_ELEMENT) != 0)
#define IS_SCHEMA(p) 			        ((p & GR_PROP_SCHEMA_INFORMED) != 0)
#define IS_DOCUMENT(p) 			        ((p & GR_PROP_DOCUMENT) != 0)
#define IS_FRAGMENT(p) 			        ((p & GR_PROP_FRAGMENT) != 0)
#define HAS_NAMED_SUB_TYPE_OR_UNION(p) 	((p & GR_PROP_NAMED_SUB_TYPE_OR_UNION) != 0)
#define HAS_CONTENT2(p) 				((p & GR_PROP_HAS_CONTENT2) != 0)

#define SET_NILLABLE_GR(p)    	            ((p) = (p) | GR_PROP_NILLABLE)
#define SET_BUILT_IN_ELEM_GR(p)             ((p) = (p) | GR_PROP_BUILT_IN_ELEMENT)
#define SET_SCHEMA_GR(p)    		        ((p) = (p) | GR_PROP_SCHEMA_INFORMED)
#define SET_DOCUMENT_GR(p)    		        ((p) = (p) | GR_PROP_DOCUMENT)
#define SET_FRAGMENT_GR(p)    		        ((p) = (p) | GR_PROP_FRAGMENT)
#define SET_NAMED_SUB_TYPE_OR_UNION(p) 		((p) = (p) | GR_PROP_NAMED_SUB_TYPE_OR_UNION)
#define SET_HAS_CONTENT2(p)  				((p) = (p) | GR_PROP_HAS_CONTENT2)

#define GR_PROP_BUILT_IN_ELEMENT         0x1000000 // 0b00000001000000000000000000000000
#define GR_PROP_SCHEMA_INFORMED          0x2000000 // 0b00000010000000000000000000000000
#define GR_PROP_DOCUMENT                 0x4000000 // 0b00000100000000000000000000000000
#define GR_PROP_FRAGMENT                 0x8000000 // 0b00001000000000000000000000000000
#define GR_PROP_NILLABLE                0x10000000 // 0b00010000000000000000000000000000
#define GR_PROP_NAMED_SUB_TYPE_OR_UNION 0x20000000 // 0b00100000000000000000000000000000
/* There is a content2 rule if there are AT productions
 * that point to the content grammar rule OR the content index is 0. */
#define GR_PROP_HAS_CONTENT2            0x40000000 // 0b01000000000000000000000000000000

#define GR_CONTENT_INDEX_MASK 0xFFFFFF // 0b00000000111111111111111111111111

#define SET_CONTENT_INDEX(props, indx) (props = (props & ~GR_CONTENT_INDEX_MASK) | (indx & GR_CONTENT_INDEX_MASK))
#define GET_CONTENT_INDEX(props) (props & GR_CONTENT_INDEX_MASK)

/**@}*/

/**
 * An EXI regular grammar, organized as a collection of containers for production rules, 
 * and grouped by each rule's left-hand side non-terminal.
 */
struct EXIGrammar
{
	/** 
     * Rule container array, where the index of a container is its left-hand side non-terminal ID.
     */
	GrammarRule* rule;

	/**
	 * Bitmap of properties and the value for the grammar contentIndex (the index of the first rule that does not contain attribute content.).
	 * The most significant 8 bits are the properties and the least significant
	 * 24 bits is the value for the contentIndex.
	 * Use parenthesized macro to retrieve, or the "SET" form to update.
	 * <ul>
	 *   <li> nillable (IS_NILLABLE(p)) </li>
	 *   <li> built-in grammar (IS_BUILT_IN_ELEM(p)) </li>
	 *   <li> schema-informed grammar (IS_SCHEMA(p)) </li>
	 *   <li> does type of this grammar have named sub-types or is it union? (HAS_NAMED_SUB_TYPE_OR_UNION(p)) </li>
	 * </ul>
	 */
	uint32_t props;

	/** Size of the rule array */
	SmallIndex count;
};

typedef struct EXIGrammar EXIGrammar;

/**
 * Used for the processing grammar stack. */
struct GrammarStackNode
{
	EXIGrammar* grammar;

	/**
	 * Current (left-hand side) non-terminal ID that identifies the current grammar rule.
	 * Defines the context/processor state.
	 */
	SmallIndex currNonTermID;
	/** The qname of the current element being parsed/serialized */
	QNameID currQNameID;
	struct GrammarStackNode* nextInStack;
};

typedef struct GrammarStackNode EXIGrammarStack;

/**@}*/ // End Grammar Types


/** @name String Table Types */
/**@{*/

struct ValueEntry {
#if VALUE_CROSSTABLE_USE
	struct
	{
		QNameID forQNameId;
		Index vxEntryId;
	} locValuePartition; // A reference to the VxEntry for that global value
#endif
	String valueStr;
};

typedef struct ValueEntry ValueEntry;

struct ValueTable {
#if DYN_ARRAY_USE == ON
	DynArray dynArray;
#endif
	ValueEntry* value;
	Index count;
#if HASH_TABLE_USE
	/**
	 * Hashtable for fast look-up of global values in the table.
	 * Only used when:
	 * serializing &&
	 * valuePartitionCapacity > 50  &&   //for small table full-scan will work better
	 * valueMaxLength > 0 // this is essentially equal to valuePartitionCapacity == 0
	 *
	 */
	struct hashtable* hashTbl;
#endif
	/** @see http://www.w3.org/TR/2011/REC-exi-20110310/#key-globalID */
	Index globalId;
};

typedef struct ValueTable ValueTable;

#if VALUE_CROSSTABLE_USE
	struct VxEntry {
		Index globalId;
	};

	typedef struct VxEntry VxEntry;

	struct VxTable {
	#if DYN_ARRAY_USE == ON
		DynArray dynArray;
	#endif
		VxEntry* vx;
		Index count;
	};

	typedef struct VxTable VxTable;
#endif

struct PfxTable {
	/** The number of entries */
	SmallIndex count;
	String pfxStr[MAXIMUM_NUMBER_OF_PREFIXES_PER_URI];
};

typedef struct PfxTable PfxTable;

struct LnEntry {
#if VALUE_CROSSTABLE_USE
	VxTable* vxTable;
#endif
	String lnStr;
	/** Global element grammar with uriStr:lnStr qname.
	 *  Either Index of a global element grammar in the SchemaGrammarTable OR
	 *  INDEX_MAX if a global element grammar with that qname does not exist
	 */
	Index elemGrammar;
	/** Global type grammar with uriStr:lnStr qname.
	 *  Either Index of a global type grammar in the SchemaGrammarTable OR
	 *  INDEX_MAX if a global type grammar with that qname does not exist
	 */
	Index typeGrammar;
};

typedef struct LnEntry LnEntry;

struct LnTable {
#if DYN_ARRAY_USE == ON
	DynArray dynArray;
#endif
	LnEntry* ln;
	Index count;
};

typedef struct LnTable LnTable;

struct UriEntry {
	LnTable lnTable;
	PfxTable* pfxTable;
	String uriStr;
};

typedef struct UriEntry UriEntry;

struct UriTable {
#if DYN_ARRAY_USE == ON
	DynArray dynArray;
#endif
	UriEntry* uri;
	SmallIndex count;
};

typedef struct UriTable UriTable;

/**@}*/ // End String Table Types

/** @name Constraining Facets IDs
 * Used for fine-grained schema validation */
/**@{*/
#define TYPE_FACET_LENGTH               0x0001 // 0b0000000000000001
#define TYPE_FACET_MIN_LENGTH           0x0002 // 0b0000000000000010
#define TYPE_FACET_MAX_LENGTH           0x0004 // 0b0000000000000100
#define TYPE_FACET_PATTERN              0x0008 // 0b0000000000001000
#define TYPE_FACET_ENUMERATION          0x0010 // 0b0000000000010000
#define TYPE_FACET_WHITE_SPACE          0x0020 // 0b0000000000100000
#define TYPE_FACET_MAX_INCLUSIVE        0x0040 // 0b0000000001000000
#define TYPE_FACET_MAX_EXCLUSIVE        0x0080 // 0b0000000010000000
#define TYPE_FACET_MIN_EXCLUSIVE        0x0100 // 0b0000000100000000
#define TYPE_FACET_MIN_INCLUSIVE        0x0200 // 0b0000001000000000
#define TYPE_FACET_TOTAL_DIGITS         0x0400 // 0b0000010000000000
#define TYPE_FACET_FRACTION_DIGITS      0x0800 // 0b0000100000000000
#define TYPE_FACET_NAMED_SUBTYPE_UNION  0x1000 // 0b0001000000000000
/**@}*/

#define ST_CONTENT_MASK 0xFFFFFF // 0b00000000111111111111111111111111

#define GET_EXI_TYPE(content) (content>>24)
#define SET_EXI_TYPE(content, et) (content = (content & ST_CONTENT_MASK) | (((uint32_t) et)<<24))
#define HAS_TYPE_FACET(content, facet) ((content & facet) != 0)
#define SET_TYPE_FACET(content, facet) (content = (content | facet))
#define REMOVE_TYPE_FACET(content, facet) (content = (content & ~facet))

/**
 * Attributes of a schema simple type, including EXI datatype for the simple content. 
 */
struct SimpleType {
	/** The content of the simple type consists of two parts:
	 * - 8-bits exiType such as VALUE_TYPE_STRING , VALUE_TYPE_FLOAT, VALUE_TYPE_DECIMAL etc. (most significant 8 bits)
	 * - 24-bits facetPresenceMask - the least significant 24 bits
	 */
	uint32_t content;
	/**
	 * either length or (totalDigits and fractionDigits) or typeId, when exiType is LIST (they are mutually exclusive)
	 * The totalDigits (if available) are encoded in the most significant 16 bits as unsigned integer
	 * and fractionDigits (if available) are encoded in the least significant 16 bits as unsigned integer
	 */
	uint32_t length;

	/**
	 * either maxInclusive or maxExclusive or maxLength or
	 * when exiType is LIST and TYPE_FACET_LENGTH is set then the
	 * TYPE_FACET_LENGTH of the list as length indicates the
	 * itemType/typeID of the elements(they are mutually exclusive) */
	int64_t max;
	/**
	 * either minInclusive or minExclusive or minLength (they are mutually exclusive) */
	int64_t min;
};

typedef struct SimpleType SimpleType;

/** Holds all simple types defined for particular EXI processing (build-in + schema-defined) */
struct SimpleTypeTable {
#if DYN_ARRAY_USE == ON
	DynArray dynArray;
#endif
	SimpleType* sType;
	Index count;
};

typedef struct SimpleTypeTable SimpleTypeTable;

/** @name Simple types IDs */
/**@{*/
#define SIMPLE_TYPE_ENTITIES              0
#define SIMPLE_TYPE_ENTITY                1
#define SIMPLE_TYPE_ID                    2
#define SIMPLE_TYPE_IDREF                 3
#define SIMPLE_TYPE_IDREFS                4
#define SIMPLE_TYPE_NCNAME                5
#define SIMPLE_TYPE_NMTOKEN               6
#define SIMPLE_TYPE_NMTOKENS              7
#define SIMPLE_TYPE_NOTATION              8
#define SIMPLE_TYPE_NAME                  9
#define SIMPLE_TYPE_QNAME                10
#define SIMPLE_TYPE_ANY_SIMPLE_TYPE      11
#define SIMPLE_TYPE_ANY_TYPE             12
#define SIMPLE_TYPE_ANY_URI              13
#define SIMPLE_TYPE_BASE64_BINARY        14
#define SIMPLE_TYPE_BOOLEAN              15
#define SIMPLE_TYPE_BYTE                 16
#define SIMPLE_TYPE_DATE                 17
#define SIMPLE_TYPE_DATE_TIME            18
#define SIMPLE_TYPE_DECIMAL              19
#define SIMPLE_TYPE_DOUBLE               20
#define SIMPLE_TYPE_DURATION             21
#define SIMPLE_TYPE_FLOAT                22
#define SIMPLE_TYPE_GDAY                 23
#define SIMPLE_TYPE_GMONTH               24
#define SIMPLE_TYPE_GMONTH_DAY           25
#define SIMPLE_TYPE_GYEAR                26
#define SIMPLE_TYPE_GYEAR_MONTH          27
#define SIMPLE_TYPE_HEX_BINARY           28
#define SIMPLE_TYPE_INT                  29
#define SIMPLE_TYPE_INTEGER              30
#define SIMPLE_TYPE_LANGUAGE             31
#define SIMPLE_TYPE_LONG                 32
#define SIMPLE_TYPE_NEGATIVE_INTEGER     33
#define SIMPLE_TYPE_NON_NEGATIVE_INTEGER 34
#define SIMPLE_TYPE_NON_POSITIVE_INTEGER 35
#define SIMPLE_TYPE_NORMALIZED_STRING    36
#define SIMPLE_TYPE_POSITIVE_INTEGER     37
#define SIMPLE_TYPE_SHORT                38
#define SIMPLE_TYPE_STRING               39
#define SIMPLE_TYPE_TIME                 40
#define SIMPLE_TYPE_TOKEN                41
#define SIMPLE_TYPE_UNSIGNED_BYTE        42
#define SIMPLE_TYPE_UNSIGNED_INT         43
#define SIMPLE_TYPE_UNSIGNED_LONG        44
#define SIMPLE_TYPE_UNSIGNED_SHORT       45

#define SIMPLE_TYPE_COUNT                46
/**@}*/

#define XML_NAMESPACE_ID                  1

#define XML_SCHEMA_INSTANCE_ID            2
#define XML_SCHEMA_INSTANCE_NIL_ID        0
#define XML_SCHEMA_INSTANCE_TYPE_ID       1

#define XML_SCHEMA_NAMESPACE_ID           3

/** Holds all EXI grammars defined for particular EXI processing (build-in + schema-defined) */
struct SchemaGrammarTable {
#if DYN_ARRAY_USE == ON
	DynArray dynArray;
#endif
	EXIGrammar* grammar;
	Index count;
};

typedef struct SchemaGrammarTable SchemaGrammarTable;

#if EXI_PROFILE_DEFAULT
// The index of the EXI Profile stub grammar
#  define EXI_PROFILE_STUB_GRAMMAR_INDX (INDEX_MAX-1)
#endif

/**
 * Stores the enum values for a particular simple type */
struct enumDefinition
{
	/** Index of the simple type grammar in the simpleTypeTable */
	Index typeId;
	/** Array of enum values */
	void *values;
	/** The number or enum values*/
	SmallIndex count;
};

typedef struct enumDefinition EnumDefinition;

/** All the enumerations defined in the schema.
 * The entries are sorted by qnameid */
struct EnumTable {
#if DYN_ARRAY_USE == ON
	DynArray dynArray;
#endif
	EnumDefinition* enumDef;
	Index count;
};

typedef struct EnumTable EnumTable;

/**
 * EXIP representation of XML Schema.
 * @todo If the simple types are included in the grammarTable's EXIGrammar structure,
 * the simpleTypeTable and grammarTable can be merged.
 */
struct EXIPSchema
{
	/**
	 * Keep track of allocations within this structure when the
	 * schema object is dynamically created.
	 */
	AllocList memList;

	/**
	 * Initial string tables
	 */
	UriTable uriTable;

	/**
	 * The document/fragment grammar for that schema instance.
	 */
	EXIGrammar docGrammar;

	/**
	 * Array of simple types for this schema
	 */
	SimpleTypeTable	simpleTypeTable;

	/**
	 * Array of all the grammars in the schema.
	 */
	SchemaGrammarTable grammarTable;

	/**
	 * The number of schema-informed grammars in grammarTable
	 */
	Index staticGrCount;

	EnumTable enumTable;
};

typedef struct EXIPSchema EXIPSchema;

struct StreamContext
{
	/**
	 * Current position in the buffer - bytewise
	 */
	Index bufferIndx;

	/**
	 * Value between 0 and 7; shows the current position within the current byte.
	 * 7 is the least significant bit position in the byte.
	 */
	unsigned char bitPointer;

	/** The qname of the current attribute */
	QNameID currAttr;

	/** Non-zero if expecting attribute data or list items */
	unsigned int expectATData;

	/** TRUE if the current grammar rule must be processed as EmptyType grammar */
	boolean isNilType;

	/** Value type of the expected attribute */
	Index attrTypeId;
};

typedef struct StreamContext StreamContext;

/**
 * Representation of an Input/Output Stream
 */
struct ioStream
{
	/**
	 * When parsing: A function pointer used to fill the EXI buffer when emptied by reading from "stream" "size" number of bytes
	 * When serializing: A function pointer used to write "size" number of bytes of the buffer to the stream
	 * Return the number of bytes read/write
	 */
	size_t (*readWriteToStream)(void* buf, size_t size, void* stream);
	/**
	 * The input stream to be passed to the readInput function pointer
	 */
	void* stream;
};

typedef struct ioStream IOStream;

struct DatatypeRepresentationMap
{
	void* TODO; //TODO: fill in the information for this structure
};

typedef struct DatatypeRepresentationMap DatatypeRepresentationMap;

struct EXIOptions
{
	/**
	 * Use the macros GET_ALIGNMENT(p), WITH_COMPRESSION(p), WITH_STRICT,
	 * WITH_FRAGMENT(p), WITH_SELF_CONTAINED(p) to extract the options:
	 * alignment, compression, strict, fragment and selfContained
	 *
	 * @see options_defs
	 */
	unsigned char enumOpt;

	/**
	 * Specifies whether comments, pis, etc. are preserved - bit mask of booleans
	 * Use IS_PRESERVED macro to retrieve the values different preserve options
	 */
	unsigned char preserve;

	/** schemaID mode, default SCHEMA_ID_ABSENT */
	SchemaIdMode schemaIDMode;

	/**
	 * Identify the schema information, if any, used to encode the body.
	 * It the schemaID field is absent or empty, then schemaID is
	 * an empty string. Use schemaIDMode to check/set the exact schemaID mode
	 * of operation
	 */
	String schemaID;

	/**
	 * Specify alternate datatype representations for typed values in the EXI body
	 */
	DatatypeRepresentationMap* drMap;

	/**
	 *  Specifies the block size used for EXI compression
	 */
	uint32_t blockSize;

	/**
	 * Specifies the maximum string length of value content items to be considered for addition to the string table.
	 * INDEX_MAX - unbounded
	 */
	Index valueMaxLength;

	/**
	 * Specifies the total capacity of value partitions in a string table
	 * INDEX_MAX - unbounded
	 */
	Index valuePartitionCapacity;

	/**
	 * User defined meta-data may be added
	 */
	void* user_defined_data;
};

typedef struct EXIOptions EXIOptions;

/**
 * Represents an EXI header
 */
struct EXIheader
{
	/**
	 * Boolean value - FALSE for lack of EXI cookie, otherwise TRUE
	 */
	boolean has_cookie;

	/**
	 * Boolean value - FALSE for lack of EXI Options, otherwise TRUE
	 */
	boolean has_options;

	/** Boolean value - TRUE preview version, FALSE final version */
	boolean is_preview_version;

	/**
	 * EXI stream version
	 */
	int16_t version_number;

	EXIOptions opts;
};

typedef struct EXIheader EXIheader;

struct BinaryBuffer
{
	/**
	 * Read/write memory buffer
	 */
	char* buf;

	/**
	 * The size of the buffer
	 */
	Index bufLen;

	/**
	 * The size of the data stored in the buffer - number of bytes
	 */
	Index bufContent;

	/**
	 * Input/Output Stream used to fill/flush the buffer during processing
	 */
	IOStream ioStrm;
};

typedef struct BinaryBuffer BinaryBuffer;

/**
 * Represents an EXI stream
 */
struct EXIStream
{
	BinaryBuffer buffer;

	/**
	 * EXI Header - the most important field is the EXI Options. They control the
	 * parsing and serialization of the stream.
	 */
	EXIheader header;

	/** Holds the current state of the stream. */
	StreamContext context;

	/**
	 * The value string table
	 */
	ValueTable valueTable;

	/**
	 * The grammar stack used during processing
	 */
	EXIGrammarStack* gStack;

	/**
	 * Stores the information of all the allocated memory for that stream,
	 * except the global sting values that are stored in the ValueTable
	 */
	AllocList memList;

	/**
	 * Schema information for that stream.
	 * It contains the string tables and possibly schema-informed EXI grammars.
	 */
	EXIPSchema* schema;
};

typedef struct EXIStream EXIStream;


/**********************Function definitions************************/

/**
 * @brief Set the EXI options to their default values
 *
 * @param[in, out] opts EXI options structure
 */
void makeDefaultOpts(EXIOptions* opts);

/**
 * @brief Check if the EXI options are set correctly
 *
 * @param[in] opts EXI options structure
 * @returns EXIP_OK if the values are correct, otherwise EXIP_HEADER_OPTIONS_MISMATCH
 */
errorCode checkOptionValues(EXIOptions* opts);

errorCode pushOnStack(GenericStack** stack, void* item);

void popFromStack(GenericStack** stack, void** item);

int compareEnumDefs(const void* enum1, const void* enum2);

#endif /* PROCTYPES_H_ */
