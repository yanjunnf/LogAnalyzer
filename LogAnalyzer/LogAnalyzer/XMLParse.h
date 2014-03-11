#ifndef __XML_PARSE__
#define __XML_PARSE__
#include "CommonHeader.h"
#include "TraceLogger.h"
#include "Poco\SAX\Attributes.h"
#include "Poco\Sax\ContentHandler.h"
#include "Poco\Sax\SAXParser.h"

#include <vector>

#define ELEMENT_LOCAL_NAME_STRING  "String"
#define ELEMENT_LOCAL_NAME_MESSAGE "Message"
#define ATTRIBUTE_LOCAL_NAME       "Id"

class MessageIDHandler : public Poco::XML::ContentHandler
{
public:
    MessageIDHandler(){}
    ~MessageIDHandler(){}
    void setDocumentLocator(const Poco::XML::Locator* loc)
    {
    }

    void startDocument()
    {
    }

    void endDocument()
    {
    }

    void startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes)
    {
        for (int i = 0; i < attributes.getLength(); ++i)
        {
            std::string strLocalName = attributes.getLocalName(i);
            if ((localName == ELEMENT_LOCAL_NAME_STRING || localName == ELEMENT_LOCAL_NAME_MESSAGE) &&
                strLocalName == ATTRIBUTE_LOCAL_NAME)
            {
                m_messageIDArray.push_back(attributes.getValue(i));
            }
        }
    }

    void endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname)
    {
    }

    virtual void characters(const Poco::XML::XMLChar ch[], int start, int length)
    {
    }

    void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length)
    {
    }

    void processingInstruction(const Poco::XML::XMLString& target, const Poco::XML::XMLString& data)
    {
    }

    void startPrefixMapping(const Poco::XML::XMLString& prefix, const Poco::XML::XMLString& uri)
    {
    }

    void endPrefixMapping(const Poco::XML::XMLString& prefix)
    {
    }

    void skippedEntity(const Poco::XML::XMLString& name)
    {
    }
    std::vector<tstring> m_messageIDArray;
};

class XMLParser
{
public:
    XMLParser(){};
    ~XMLParser(){};
    void StartXML(const tstring& strFileName);
    void StartProperty(const tstring& strFileName);
    const std::vector<tstring>& getMessageIDs()const{return m_messageIDHandler.m_messageIDArray;}
protected:
    MessageIDHandler m_messageIDHandler;
    Poco::XML::SAXParser m_saxParser;
};


#endif //__XML_PARSE__