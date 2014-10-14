/*==================================================================*\
|                EXIP - Embeddable EXI Processor in C                |
|--------------------------------------------------------------------|
|          This work is licensed under BSD 3-Clause License          |
|  The full license terms and conditions are located in LICENSE.txt  |
\===================================================================*/

/**
 * @file stringManipulate.h
 * @brief String manipulation functions
 * @date Sep 3, 2010
 * @author Rumen Kyusakov
 * @version 0.5
 * @par[Revision] $Id: stringManipulate.h 328 2013-10-30 16:00:10Z kjussakov $
 */

#ifndef STRINGMANIPULATE_H_
#define STRINGMANIPULATE_H_

#include "procTypes.h"
#include "errorHandle.h"

/**
 * @brief Allocate a memory for a string with UCSchars number of UCS characters
 * It depends on the representation of the characters
 *
 * @param[in, out] str a pointer to the uninitialized string
 * @param[in] UCSchars the number of characters (as described by UCS [ISO/IEC 10646])
 * @return Error handling code
 */
errorCode allocateStringMemory(CharType** str, Index UCSchars);

/**
 * @brief Allocate a memory for a string with UCSchars number of UCS characters
 * It depends on the representation of the characters
 *
 * @param[in, out] str a pointer to the uninitialized string
 * @param[in] UCSchars the number of characters (as described by UCS [ISO/IEC 10646])
 * @param[in, out] memList A list storing the memory allocations
 * @return Error handling code
 */
errorCode allocateStringMemoryManaged(CharType** str, Index UCSchars, AllocList* memList);

/**
 * @brief Writes a UCS [ISO/IEC 10646] code point to a string
 * It depends on the representation of the characters.
 * The memory needed for str should be allocated before the invocation
 * of this function
 *
 * @param[in, out] str string to be written on
 * @param[in] code_point UCS [ISO/IEC 10646] code point
 * @param[in, out] writerPosition:
 * 					[in] the index of the next CharType to be written i.e.
 * 					str->str + *writerPosition is a pointer to the beginning
 * 					of the next character position where the character with Code Point code_point will be written.
 * 					[out] the next writerPosition used for any subsequent call to writeCharToString
 * @return Error handling code
 */
errorCode writeCharToString(String* str, uint32_t code_point, Index* writerPosition);

/**
 * @brief Returns the UCS [ISO/IEC 10646] code point at particular index from a String
 * @param[in] str string
 * @param[in, out] readerPosition:
 * 				[in] The position that marks the beginning of the character to be read
 * 				[out] The position of the next character to be passed to a subsequent calls to readCharFromString
 * @return UCS code point
 */
uint32_t readCharFromString(const String* str, Index* readerPosition);

/**
 * @brief Creates an empty string
 * @param[in, out] emptyStr empty string
 */
void getEmptyString(String* emptyStr);

/**
 * @brief Checks if an string is empty
 * @param[in] str string to check
 * @return FALSE if not empty, TRUE if empty
 */
boolean isStringEmpty(const String* str);

/**
 * @brief Transform a NULL terminated string of ASCII chars to String allocating memory for the CharType*.
 * @param[in] inStr ASCII stream
 * @param[in, out] outStr resulted string
 * @param[in, out] memList A list storing the memory allocations
 * @param[in] clone Boolean indicating if outStr should reuse the memory allocated for inStr if possible.
 * 					FALSE - if String implementation allows it - do not allocate new memory for the string
 * 					TRUE - always allocate fresh memory for outStr and copy inStr there
 * @return Error handling code
 */
errorCode asciiToString(const char* inStr, String* outStr, AllocList* memList, boolean clone);

/**
 * @brief Tests if two strings are equal
 * @param[in] str1 string to compare
 * @param[in] str2 string to compare
 * @return TRUE if the strings are equal, FALSE - otherwise
 */
boolean stringEqual(const String str1, const String str2);

/**
 * @brief Compare two strings lexicographically
 * @param[in] str1 string to compare
 * @param[in] str2 string to compare
 * @return 0 when the strings are equal; negative int when str1<str2; positive when  str1>str2
 */
int stringCompare(const String str1, const String str2);

/**
 * @brief Checks if a String string and ASCII string are equal
 * @param[in] str1 string to compare
 * @param[in] str2 null terminated string to compare
 * @return TRUE if the strings are equal, FALSE - otherwise
 */
boolean stringEqualToAscii(const String str1, const char* str2);

/**
 * @brief Makes a copy of the string in a new location
 * @param[in] src the string to be copied
 * @param[in] newStr will point to the newly allocated memory with scr->CharType* copied there
 * @return Error handling code
 */
errorCode cloneString(const String* src, String* newStr);

/**
 * @brief Makes a copy of the string in a new location
 * @param[in] src the string to be copied
 * @param[in] newStr will point to the newly allocated memory with scr->CharType* copied there
 * @param[in, out] memList A list storing the memory allocations
 * @return Error handling code
 */
errorCode cloneStringManaged(const String* src, String* newStr, AllocList* memList);

/**
 * @brief Returns the index of the first occurrence of a character in a string
 * @param[in] src the source string
 * @param[in] sCh a character that is searched
 * @return the index of first occurrence; INDEX_MAX if not found
 */
Index getIndexOfChar(const String* src, CharType sCh);

/**
 * @brief Converts a string to int representation. Only 10-base representation
 * @param[in] src the source string
 * @param[out] number the int representation of the string
 * @return Error handling code
 */
errorCode stringToInteger(const String* src, int* number);

/**
 * @brief Converts a string to int64 representation. Only 10-base representation
 * @param[in] src the source string
 * @param[out] number the int64 representation of the string
 * @return Error handling code
 */
errorCode stringToInt64(const String* src, int64_t* number);

#if EXIP_IMPLICIT_DATA_TYPE_CONVERSION

/**
 * @brief Converts a integer to string representation.
 * Uses allocateStringMemory() so make sure to free the
 * memory allocated for the string manually!
 *
 * @param[in] number the source integer
 * @param[out] outStr the resulting string representation
 * @return Error handling code
 */
errorCode integerToString(Integer number, String* outStr);

/**
 * @brief Converts a boolean to string representation.
 * Uses allocateStringMemory() so make sure to free the
 * memory allocated for the string manually!
 *
 * @param[in] b TRUE/FALSE
 * @param[out] outStr the resulting string representation
 * @return Error handling code
 */
errorCode booleanToString(boolean b, String* outStr);

/**
 * @brief Converts a float to string representation.
 * Uses allocateStringMemory() so make sure to free the
 * memory allocated for the string manually!
 *
 * @param[in] f float in EXI format (base 10)
 * @param[out] outStr the resulting string representation
 * @return Error handling code
 */
errorCode floatToString(Float f, String* outStr);

/**
 * @brief Converts a decimal to string representation.
 * Uses allocateStringMemory() so make sure to free the
 * memory allocated for the string manually!
 *
 * @param[in] d the source decimal
 * @param[out] outStr the resulting string representation
 * @return Error handling code
 */
errorCode decimalToString(Decimal d, String* outStr);

/**
 * @brief Converts a dateTime value to string representation.
 * Uses allocateStringMemory() so make sure to free the
 * memory allocated for the string manually!
 *
 * @param[in] dt the source dtValue
 * @param[out] outStr the resulting string representation
 * @return Error handling code
 */
errorCode dateTimeToString(EXIPDateTime dt, String* outStr);

#endif /* EXIP_IMPLICIT_DATA_TYPE_CONVERSION */


#if EXIP_DEBUG == ON

/**
 * @brief Prints out a String
 * Used for debugging purposes.
 * @param[in] inStr Input string to be printed
 */
void printString(const String* inStr);

#endif /* EXIP_DEBUG */

#endif /* STRINGMANIPULATE_H_ */
