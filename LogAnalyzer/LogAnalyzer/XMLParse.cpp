#include "stdafx.h"
#include "XMLParse.h"
#include "Poco\SAX\LexicalHandler.h"
#include "Poco\XML\XMLException.h"
void XMLParser::StartXML(const tstring& strPath)
{
    try
    {
        char buffer[32];
        m_saxParser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, true);
        m_saxParser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACE_PREFIXES, true);
        //m_saxParser.setProperty(Poco::XML::XMLReader::PROPERTY_LEXICAL_HANDLER, static_cast<Poco::XML::LexicalHandler*>(&m_messageIDHandler));
        m_saxParser.setContentHandler(&m_messageIDHandler);
        m_saxParser.parse(strPath);
        gTraceLogger.getLogger().information("XMLParser::Start File:" + strPath + ". ID count:" + tstring(itoa(m_messageIDHandler.m_messageIDArray.size(), buffer, 10)));
    }
    catch (const Poco::XML::XMLException& e)
    {
        gTraceLogger.getLogger().information("XMLParser::Start - Exception:" + e.displayText());
    }
    catch (const std::exception& e)
    {
        gTraceLogger.getLogger().information("XMLParser::Start - Exception:" + tstring(e.what()));
    }
}