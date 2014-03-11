#include "stdafx.h"
#include "CommandLine.h"
#include "Tools.h"
#include "TraceLogger.h"
#include "XMLParse.h"
#include "Poco/FileStream.h"

#define COMMAND_LINE_HELP       "HELP"
#define FINDAVAILABLETRACEID    "-FindAvailableTraceId"
#define FINDTRACEID             "-FindTraceId"
#define FINDUNUSEDTRACEID       "-FindUnusedTraceId"
#define TraceHeader             "TraceHeader"
#define TraceID                 "TraceID"
#define ExportFile              "ExportFile"
#define SourcePath              "SourcePath"
#define MessageFile             "MessageFile"

/*
Support below command lines:
-FindAvailableTraceId TraceHeader=XXX SourcePath=XXX ExportFile=XXX
-FindTraceId TraceID=XXX SourcePath=XXX ExportFile=XXX
-FindUnusedTraceId MessageFile=XXX SourcePath=XXX ExportFile=XXX
*/

CCommandLine::CCommandLine(const std::vector<tstring>& args) : m_nOperationType(NON_AUTOMATION)
{
    Parse(args);
}

void CCommandLine::Parse(const std::vector<tstring>& args)
{
    try
    {
        if (args.size() == 0)
            return;

        for (unsigned int i = 0; i < args.size(); ++i)
        {
            tstring strParameter = args.at(i);
            if (i == 0)
            {
                if (strParameter.size() > 0 && strParameter[0] == '-')
                {
                    if (strParameter == FINDAVAILABLETRACEID)
                        m_nOperationType = FIND_AVAILABLE_TRACE_ID;
                    else if (strParameter == FINDTRACEID)
                        m_nOperationType = FIND_TRACE_ID;
                    else if (strParameter == FINDUNUSEDTRACEID)
                        m_nOperationType = FIND_UNUSED_TRACE_ID;
                }
                else
                    m_nOperationType = UNKNOWN_OPERATION;
            }
            else
            {
                std::vector<tstring> parameters;
                CTools::SplitString(args[i], "=", parameters);
                if (parameters.size() == 2)
                {
                    if (parameters.at(0) == TraceHeader)
                        m_strTraceHeader = parameters.at(1);
                    else if (parameters.at(0) == TraceID)
                        m_strTraceID = parameters.at(1);
                    else if (parameters.at(0) == ExportFile)
                        m_strExportFileName = parameters.at(1);
                    else if (parameters.at(0) == SourcePath)
                        m_strSourcePath = parameters.at(1);
                    else if (parameters.at(0) == MessageFile)
                        m_strMessageFile = parameters.at(1);
                }
                else
                {
                    m_strErrorMessage = "Error parameter:" + args[i];
                    CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Error parameter:") + m_strErrorMessage);
                    break;
                }
            }
        }

        BOOL bReady = TRUE;
        if (m_strSourcePath.empty())
        {
            bReady = FALSE;
            CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Source Path is empty"));
        }

        if (m_strExportFileName.empty())
        {
            bReady = FALSE;
            CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Exported file name is empty"));
        }

        if (m_nOperationType == FIND_AVAILABLE_TRACE_ID)
        {
            if (m_strTraceHeader.empty())
            {
                bReady = FALSE;
                CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Trace Header is empty"));
            }
        }
        else if (m_nOperationType == FIND_TRACE_ID)
        {
            if (m_strTraceID.empty())
            {
                bReady = FALSE;
                CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Trace ID is empty"));
            }
        }
        else if (m_nOperationType == FIND_UNUSED_TRACE_ID)
        {
            if (m_strMessageFile.empty())
            {
                bReady = FALSE;
                CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Message File is empty"));
            }
        }
        

        if (bReady)
            Execute();
    }
    catch (std::exception& exc)
    {
        CTraceLogger::getLogger().critical(tstring("CCommandLine::Parse - Exception:") + exc.what());
    }
}

void CCommandLine::Execute()
{
    if (m_nOperationType == FIND_AVAILABLE_TRACE_ID)
    {
        std::map<tstring, int> alreadyUsed;
        std::vector<tstring> availableTraces;
        CTools::FindAvailableTraces(m_strSourcePath, m_strTraceID, alreadyUsed, availableTraces, FALSE);

        //Export the result to file
        Poco::FileOutputStream fileStream(m_strExportFileName);
        if (fileStream.good())
        {
            //Write already used trace IDs
            fileStream << "Trace Header: '" << m_strTraceHeader << "'"<< "\n";
            fileStream << "Already Used Trace IDs:" << "\n";
            std::map<tstring, int>::iterator ite = alreadyUsed.begin();
            char buff[32];
            while (ite != alreadyUsed.end())
            {
                fileStream << "  " << ite->first << ":" << itoa(ite->second, buff, 10) << "\n";
                ++ite;
            }
            fileStream << "Available Trace IDs:" << "\n";
            for (int i = 0; i < availableTraces.size(); ++i)
            {
                fileStream << "  " << availableTraces.at(i) << "\n";
                ++ite;
            }
            fileStream.close();
        }
    }
    else if (m_nOperationType == FIND_TRACE_ID)
    {
        std::map<tstring, int> traceIDMap;
        CTools::FindASpecTraceID(m_strSourcePath, m_strTraceID, traceIDMap, FALSE);
        
        //Export the result to file
        Poco::FileOutputStream fileStream(m_strExportFileName);
        char buff[32];
        if (fileStream.good())
        {
            //Write already used trace IDs
            fileStream << "Trace ID '" << m_strTraceID << "'"<< "\n";
            std::map<tstring, int>::iterator ite = traceIDMap.begin();
            while (ite != traceIDMap.end())
            {
                fileStream << "  " << ite->first << ":" << itoa(ite->second, buff, 10) << "\n";
                ++ite;
            }
            fileStream.close();
        }
    }
    else if (m_nOperationType == FIND_UNUSED_TRACE_ID)
    {
        try
        {
            XMLParser parser;
            parser.StartXML(m_strMessageFile);
            std::map<tstring, tstring> messageIDMap;
            const std::vector<tstring>& msgIDs = parser.getMessageIDs();
            if (msgIDs.size() > 0)
            {
                //Remove the odd IDs
                messageIDMap.clear();
                std::vector<tstring> filters;
                std::vector<tstring> files;
                filters.push_back(CPPSOURCEFILE);
                try
                {
                    m_strSourcePath = CTools::ReplaceEnvVar(m_strSourcePath);
                    CTools::IteratorDir(m_strSourcePath, filters, files);
                }
                catch (const std::exception& e)
                {
                    CTraceLogger::getLogger().information(tstring("CCommandLine::Execute() - Exception when iterate dir") + e.what());
                    return;
                }

                for (int i = 0; i < files.size(); ++i)
                {
                    CFindStringInFile findString(files.at(i));
                    for (int j = 0; j < msgIDs.size(); ++j)
                    {
                        if (messageIDMap.find(msgIDs.at(j)) == messageIDMap.end())
                        {
                            if (findString.FindString(msgIDs.at(j)))
                                messageIDMap[msgIDs.at(j)] = files.at(i);
                        }
                    }
                }

                if (messageIDMap.size() > 0)
                {
                    //Export the result to file
                    Poco::FileOutputStream fileStream(m_strExportFileName);
                    if (fileStream.good())
                    {
                        //Write already used trace IDs
                        std::map<tstring, tstring>::iterator ite = messageIDMap.begin();
                        while (ite != messageIDMap.end())
                        {
                            fileStream << ite->first << " : " << ite->second << "\n";
                            ++ite;
                        }
                        fileStream.close();
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            CTraceLogger::getLogger().information(tstring("CCommandLine::Execute() - Exception when execute operation 'FIND_UNUSED_TRACE_ID'") + e.what());
        }
    }
}