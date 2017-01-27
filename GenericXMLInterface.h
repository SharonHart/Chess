#ifndef GENERIC_XML_INTERFACE_H
#define GENERIC_XML_INTERFACE_H

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include "CommonUtils.h"

typedef struct
{
	char* name;
	char* value;
} XML_ELEMENT;

/**
 * XMLInit:
 */
void XMLInit(void);

/**
 * XMLStartDocumentReader:
 * Note:		XMLEndDocumentRead must be called on returned pointer to free allocated data
 * @filename:	the input filename
 * @pReader:	the output parameter pointer to an xmlTextWriterPtr type to be passed on to other methods
 */
BOOL XMLStartDocumentReader(const char *filename, xmlTextReaderPtr* pReader);

/**
 * XMLReadElement:
 * returns BOOL whether there are more elements
 * Note: 	It's the caller's responsibility to free the element fields assigned
 * 			The caller should do this by calling XMLFreeElement on the element
 * @reader:		the reader pointer returned by XMLStartDocumentReader
 * @pElement:	the output parameter pointer to an XML_ELEMENT type whose fields are to be assigned
 */
BOOL XMLReadElement(xmlTextReaderPtr reader, XML_ELEMENT* pElement);

void XMLFreeElement(XML_ELEMENT* pElement);

/**
 * XMLEndDocumentRead:
 * Frees data allocated by XMLStartDocumentReader
 * @pReader:	the returned pointer by XMLStartDocumentReader
 */
void XMLEndDocumentRead(xmlTextReaderPtr reader);

/**
 * XMLStartDocumentWriter:
 * @filename:	the output filename
 * @pWriter:	the output parameter pointer to an xmlTextWriterPtr type to be passed on to other methods
 */
BOOL XMLStartDocumentWriter(const char *filename, xmlTextWriterPtr* pWriter);

/**
 * XMLStartElement:
 */
void XMLStartElement(xmlTextWriterPtr writer, const char* elementName);
/**
 * XMLWriteFormatElementContent:
 */
void XMLWriteFormatElementContent(xmlTextWriterPtr writer, const char* elementName, const char* elementContent);
/**
 * XMLEndElement:
 */
void XMLEndElement(xmlTextWriterPtr writer);
/**
 * XMLEndDocumentWrite:
 * @writer:	xmlTextWriterPtr returned from XMLStartDocumentWriter
 */
void XMLEndDocumentWrite(xmlTextWriterPtr writer);

void XMLTerminate(void);

#endif