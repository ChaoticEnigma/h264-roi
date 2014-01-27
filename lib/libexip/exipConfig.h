/*==================================================================*\
|                EXIP - Embeddable EXI Processor in C                |
|--------------------------------------------------------------------|
|          This work is licensed under BSD 3-Clause License          |
|  The full license terms and conditions are located in LICENSE.txt  |
\===================================================================*/

/**
 * @file pc/exipConfig.h
 * @brief Configuration parameters of the EXIP library
 * To be defined per application
 *
 * @date Oct 13, 2010
 * @author Rumen Kyusakov
 * @version 0.5
 * @par[Revision] $Id: exipConfig.h 328 2013-10-30 16:00:10Z kjussakov $
 */

#include <stdlib.h>

#ifndef EXIPCONFIG_H_
#define EXIPCONFIG_H_

#define ON  1
#define OFF 0

/**
 * @def EXIP_DEBUG
 * 		Turn the debugging ON/OFF
 * @def EXIP_DEBUG_LEVEL
 * 		Sets the debugging level (INFO, WARNING or ERROR)
 *
 * @def DEBUG_STREAM_IO
 * 		Turns the debugging ON/OFF for the STREAM_IO module
 * @def DEBUG_COMMON
 * 		Turns the debugging ON/OFF for the COMMON module
 * @def DEBUG_CONTENT_IO
 * 		Turns the debugging ON/OFF for the CONTENT_IO module
 * @def DEBUG_GRAMMAR
 * 		Turns the debugging ON/OFF for the GRAMMAR module
 * @def DEBUG_GRAMMAR_GEN
 * 		Turns the debugging ON/OFF for the GRAMMAR_GEN module
 * @def DEBUG_STRING_TBLS
 * 		Turns the debugging ON/OFF for the STRING_TBLS module
 * @ref debugging */
#define EXIP_DEBUG  	   ON
#define EXIP_DEBUG_LEVEL INFO

#define DEBUG_STREAM_IO   OFF
#define DEBUG_COMMON      OFF
#define DEBUG_CONTENT_IO  OFF
#define DEBUG_GRAMMAR     OFF
#define DEBUG_GRAMMAR_GEN OFF
#define DEBUG_STRING_TBLS OFF

#if EXIP_DEBUG != ON
# define NDEBUG
#endif

#include <assert.h>

/**
 * @name mem_group Define the memory allocation functions and freeing functions
 *
 * @def EXIP_MALLOC
 * 		malloc function
 * @def EXIP_REALLOC
 * 		realloc function
 * @def EXIP_MFREE
 * 		free function
 */
#define EXIP_MALLOC malloc
#define EXIP_REALLOC realloc
#define EXIP_MFREE free

/** @def HASH_TABLE_USE
 * 		Whether to use hash table for value partition table when in encoding mode
 * 	@def INITIAL_HASH_TABLE_SIZE
 * 		The initial capacity of the hash tables
 * 	@def MAX_HASH_TABLE_SIZE
 * 		The maximum capacity of the hash tables
 */
#define HASH_TABLE_USE ON
#define INITIAL_HASH_TABLE_SIZE 6151
#define MAX_HASH_TABLE_SIZE 32000

/** Whether to use dynamic arrays */
#define DYN_ARRAY_USE ON

// NOTE: The GR_VOID_NON_TERMINAL should be set to the maximum 24 bit unsigned value in case the
// SMALL_INDEX_MAX is 32 bits or bigger
#define GR_VOID_NON_TERMINAL 0xFFFFFF

/**
 * Affects encoding only!
 * When an untyped value is expected in the EXI stream (encoded with
 * String according to the spec) passing a typed value diferent from String will require a
 * conversion. If EXIP_IMPLICIT_DATA_TYPE_CONVERSION is enabled the
 * EXIP library takes care of that. Otherwise, if disabled, the applications
 * need to make sure they always pass String typed data when String/untyped value is
 * expected in the EXI stream. For example, assume a schema-less stream
 * and a value for an element encoded with serialize.intData():
 * 1) When EXIP_IMPLICIT_DATA_TYPE_CONVERSION == OFF
 * serialize.intData() will return an error
 * 2) When EXIP_IMPLICIT_DATA_TYPE_CONVERSION == ON
 * serialize.intData() will first convert the int value to
 * string and then encode it as a string in the EXI stream */
#define EXIP_IMPLICIT_DATA_TYPE_CONVERSION ON

/**
 * Whether the EXIP library is conforming to the W3C EXI Profile specification.
 * EXI Profile helps reduce RAM and programming memory and can be used
 * only for schema mode processing. Use SCHEMA_ID_EMPTY schemaId switch to enable
 * schema mode for otherwise schema-less streams.
 * The value of OFF disable the EXI Profile mode i.e. full featured EXI processing.
 * Set to ON to enable EXI Profile default (most restrictive) mode of operation where:
 * - maximumNumberOfBuiltInElementGrammars parameter value is set to 0
 * - maximumNumberOfBuiltInProductions parameter value is set to 0
 * - localValuePartitions parameter value is set to 0.
 *
 * Note that encoding/decoding the Profile parameters in the header is not supported
 * and most likely will never be.
 * Use only out-of-band communication of the EXI Profile default mode!
 *
 * @see http://www.w3.org/TR/exi-profile/
 */
#define EXI_PROFILE_DEFAULT OFF

#if EXI_PROFILE_DEFAULT
# define VALUE_CROSSTABLE_USE  OFF
# define BUILD_IN_GRAMMARS_USE OFF
#else
// Configure here which of the EXI features/components to be included in the compilation:

/* Whether to implement the local value string table.
 * NOTE: EXI streams that are not encoded using this option cannot be decoded correctly
 * and will return an error. The opposite is true however - a stream encoded with no
 * local value tables is valid EXI stream can be decoded with full-fledged EXI processor.
 * Disabling the local values indexing is used in EXI Profile
 * and can possibly found use in other application profiles of the EXI spec. */
#define VALUE_CROSSTABLE_USE  ON

/* Whether to enable the use of build-in grammars.
 * NOTE: Only useful for schema STRICT mode processing without schema extension points
 * or so called wildcard elements and attributes. */
#define BUILD_IN_GRAMMARS_USE ON
#endif



#endif /* EXIPCONFIG_H_ */
