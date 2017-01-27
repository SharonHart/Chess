// un-comment to enable
#ifdef _DEBUG
//#define DEBUG_XML
//#define DEBUG_MEMORY
//#define DEBUG_MEMORY_FREED
#endif

#ifdef __linux__ 
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#include "GenericXMLInterface.h"
#include "CommonUtils.h"

#define ENCODING_UTF "UTF-8"

#if !defined(LIBXML_WRITER_ENABLED) || !defined(LIBXML_OUTPUT_ENABLED)
#error LIBXML write/out not enabled
#endif

#ifndef LIBXML_READER_ENABLED
#error LIBXML_READER_ENABLED not defined
#endif

void XMLInit(void)
{
	/*
	* This initializes the library and checks potential ABI mismatches
	* between the version it was compiled for and the actual shared library used.
	*/
	LIBXML_TEST_VERSION;
}

BOOL XMLStartDocumentReader(const char *filename, xmlTextReaderPtr* pReader)
{
	xmlTextReaderPtr reader;
	//FUNCTION_DEBUG_TRACE;
	reader = xmlReaderForFile(filename, NULL, 0);
	if (NULL == reader) {
		PRINT_ERROR("Unable to open %s", filename);
		return false;
	}
	VERBOSE_PRINT("xmlReaderForFile returned reader=%p", (void*)reader);
	*pReader = reader;
	return true;	
}

BOOL XMLReadNode(xmlTextReaderPtr reader)
{
	int ret;
#ifdef DEBUG_XML
	int xmlNodeType;
	xmlChar *name;
#endif
	ret = xmlTextReaderRead(reader);

	if (ret < 0)
	{	// error
		PRINT_ERROR("failed to parse");
		return false;
	}	
	else if (0 == ret)
	{	// no more nodes to read
		return false;
	}
#ifdef DEBUG_XML	
	name = xmlTextReaderName(reader);
	xmlNodeType = xmlTextReaderNodeType(reader);
	VERBOSE_PRINT("name=%s, type=%d", name, xmlNodeType);
	xmlFree(name);
#endif	
	return true;
}

BOOL XMLReadElement(xmlTextReaderPtr reader, XML_ELEMENT* pElement)
{
	int xmlNodeType;
	xmlChar *name, *value;
	BOOL elementHasBeenFound = false;
	BOOL textHasBeenFound = false;

	// initialize
	pElement->name = NULL;
	pElement->value = NULL;

	while (!elementHasBeenFound)
	{	
		while (!elementHasBeenFound)
		{
			// read first/next node
			if (false == XMLReadNode(reader))
			{
				return false;
			}

			xmlNodeType = xmlTextReaderNodeType(reader);

			// 1st node was read successfully
			if (XML_READER_TYPE_ELEMENT != xmlNodeType)
			{
				// not an element
				//DEBUG_PRINT("skipping on non element");
				continue;
			}
			else
			{
				// element found, break while to continue to next node
				elementHasBeenFound = true;
				break;
			}
		} // while !elementHasBeenFound

		while (!textHasBeenFound)
		{
			xmlNodeType = xmlTextReaderNodeType(reader);
			
			if (XML_READER_TYPE_ELEMENT != xmlNodeType)
			{
				elementHasBeenFound = false;
#ifdef DEBUG_XML				
				VERBOSE_PRINT("unexpectedly encountered non element, type=%d", xmlNodeType);
#endif				
				break;
			}
			
			name = xmlTextReaderName(reader);
#ifdef DEBUG_XML			
			VERBOSE_PRINT("found element: name=%s, type=%d", name, xmlNodeType);
#endif			
			assert(NULL != name);
			
			// read next node
			if (false == XMLReadNode(reader))
			{
				xmlFree(name);
				return false;
			}
			
			// 2nd node was read successfully
			
			// verify its the text value of the previous element
			xmlNodeType = xmlTextReaderNodeType(reader);
			if (XML_READER_TYPE_TEXT  == xmlNodeType)
			{
				value = xmlTextReaderValue(reader);
				pElement->name = (char*)name;
				pElement->value = (char*)value;
#ifdef DEBUG_XML			
				VERBOSE_PRINT("found element: name=%s, value=%s", name, value);
#endif	
				textHasBeenFound = true;
				return true;			
			}
			else
			{
				xmlFree(name);
			}
			
		} // while !textHasBeenFound
		
	} // while !elementHasBeenFound
	
	DEBUG_PRINT("not supposed to get here: %s %d", __FILE__, __LINE__);
	return false;

}	// XMLReadElement

void XMLFreeElement(XML_ELEMENT* pElement)
{
	FUNCTION_DEBUG_TRACE;
	xmlFree(pElement->name);
	xmlFree(pElement->value);	
}

void XMLEndDocumentRead(xmlTextReaderPtr reader)
{
	VERBOSE_PRINT("xmlFreeTextReader(reader=%p)", (void*)reader);
	xmlFreeTextReader(reader);
}

BOOL XMLStartDocumentWriter(const char *filename, xmlTextWriterPtr* pWriter)
{
	int rc;
	xmlTextWriterPtr writer;

	/* Create a new XmlWriter for filename, with no compression. */
	writer = xmlNewTextWriterFilename(filename, 0);
	if (writer == NULL) {
		PRINT_ERROR("Error creating the xml writer");
		return false;
	}

	/* Start the document 
	* use the xml default for the version, encoding
	* and the default for the standalone declaration.
	*/
	rc = xmlTextWriterStartDocument(writer, NULL, ENCODING_UTF, NULL);
	if (rc < 0) {
		PRINT_ERROR("Error at xmlTextWriterStartDocument");
		return false;
	}
	VERBOSE_PRINT("writer=%p", (void*)writer);
	*pWriter = writer;
	return true;
}


void XMLStartElement(xmlTextWriterPtr writer, const char* elementName)
{
	int rc;
	rc = xmlTextWriterStartElement(writer, BAD_CAST elementName);
	if (rc < 0) {
		PRINT_ERROR("Error at xmlTextWriterStartElement");
		return;
	}	
}

void XMLWriteFormatElementContent(xmlTextWriterPtr writer, const char* elementName, const char* elementContent)
{
	int rc;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST elementName, "%s", elementContent);
	if (rc < 0) {
		PRINT_ERROR("Error at xmlTextWriterWriteFormatElement");
		return;
	}
}

void XMLEndElement(xmlTextWriterPtr writer)
{
	int rc;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) {
		PRINT_ERROR
			("Error at xmlTextWriterEndElement");
		return;
	}	
}

void XMLEndDocumentWrite(xmlTextWriterPtr writer)
{
	int rc;
	VERBOSE_PRINT("writer=%p", (void*)writer);
	rc = xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	if (rc < 0) {
		PRINT_ERROR("Error at xmlTextWriterEndDocument");
	}
}

void XMLTerminate(void)
{
	FUNCTION_DEBUG_TRACE;
	/*
	* Cleanup function for the XML library.
	*/
	xmlCleanupParser();
	/*
	* this is to debug memory for regression tests
	*/
	xmlMemoryDump();
}

#ifdef DEBUG_XML

/**
 * processNode:
 * @reader: the xmlReader
 *
 * Dump information about the current node
 */
static void
processNode(xmlTextReaderPtr reader) {
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
	name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("DumpXMLFile: %d type=%d name=%s empty=%d hasValue=%d value=", 
	    xmlTextReaderDepth(reader),
	    xmlTextReaderNodeType(reader),
	    name,
	    xmlTextReaderIsEmptyElement(reader),
	    xmlTextReaderHasValue(reader));
    if (value == NULL)
	printf("\n");
    else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
	    printf(" %s\n", value);
    }
}


void DumpXMLFile(const char *filename)
{
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
}
#endif // DEBUG


#endif // LINUX