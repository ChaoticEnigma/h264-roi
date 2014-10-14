/*==================================================================*\
|                EXIP - Embeddable EXI Processor in C                |
|--------------------------------------------------------------------|
|          This work is licensed under BSD 3-Clause License          |
|  The full license terms and conditions are located in LICENSE.txt  |
\===================================================================*/

/**
 * @file contentHandler.h
 * @brief SAX-like interface for parsing the content of an EXI stream
 * The applications should register to this handlers with callback functions
 * invoked when the processor pass through the stream. This interface is lower level than SAX.
 * If you want to use SAX API you should wrap this interface.
 * @date Sep 7, 2010
 * @author Rumen Kyusakov
 * @version 0.5
 * @par[Revision] $Id: contentHandler.h 328 2013-10-30 16:00:10Z kjussakov $
 */

#ifndef CONTENTHANDLER_H_
#define CONTENTHANDLER_H_

#include "procTypes.h"

/**
 * Simple container for function pointers for document events.
 */
struct ContentHandler
{
	// For handling the meta-data (document structure)
	errorCode (*startDocument)(void* app_data);
	errorCode (*endDocument)(void* app_data);
	errorCode (*startElement)(QName qname, void* app_data);
	errorCode (*endElement)(void* app_data);
	errorCode (*attribute)(QName qname, void* app_data);

	// For handling the data
	errorCode (*intData)(Integer int_val, void* app_data);
	errorCode (*booleanData)(boolean bool_val, void* app_data);
	errorCode (*stringData)(const String str_val, void* app_data);
	errorCode (*floatData)(Float float_val, void* app_data);
	errorCode (*binaryData)(const char* binary_val, Index nbytes, void* app_data);
	errorCode (*dateTimeData)(EXIPDateTime dt_val, void* app_data);
	errorCode (*decimalData)(Decimal dec_val, void* app_data);
	errorCode (*listData)(EXITypeClass exiType, unsigned int itemCount, void* app_data);
	errorCode (*qnameData)(const QName qname, void* app_data); // xsi:type value only

	// Miscellaneous
	errorCode (*processingInstruction)(void* app_data); // TODO: define the parameters!
	errorCode (*namespaceDeclaration)(const String ns, const String prefix, boolean isLocalElementNS, void* app_data);

	// For error handling
	errorCode (*warning)(const errorCode code, const char* msg, void* app_data);
	errorCode (*error)(const errorCode code, const char* msg, void* app_data);
	errorCode (*fatalError)(const errorCode code, const char* msg, void* app_data);

	// EXI specific
	errorCode (*selfContained)(void* app_data);  // Used for indexing independent elements for random access
};

typedef struct ContentHandler ContentHandler;

/**
 * @brief Initialize the content handler before use
 * @param[in] handler fresh ContentHandler
 *
 */
void initContentHandler(ContentHandler* handler);

#endif /* CONTENTHANDLER_H_ */
