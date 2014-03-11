#include "stdafx.h"
#include "ReadLog.h"
#include "Tools.h"
#include "TraceLogger.h"
#include "PythonFunctionExecuter.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/String.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/RegularExpression.h"
#include "Poco/Environment.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/NumberParser.h"
#include <windows.h>
#include <set>

CReadLog::CReadLog(const HWND mainWnd, const tstring& strFileName) : m_bCheckHeaderFile(FALSE), m_hMainWnd(mainWnd), m_strFileName(strFileName)
{
    Poco::SharedPtr<CComponent> defaultComponent = new CComponent(OTHER_COMPONENT);
    m_componentMap[OTHER_COMPONENT] = defaultComponent;
    //ParseSettingDir();
    gTraceLogger.getLogger().information("Start to read configurations");
}

CReadLog::~CReadLog()
{
}

void CReadLog::ParseSettingDir()
{
    gTraceLogger.getLogger().information("Start to read configurations");
    //Read setting file
    TCHAR szPathTemp[MAX_PATH + 1];
    GetModuleFileNameA(NULL, szPathTemp, MAX_PATH);
    tstring strSettingDir(szPathTemp);
    int nPos = strSettingDir.rfind("\\");
    if (nPos != tstring::npos)
    {
        strSettingDir = strSettingDir.substr(0, nPos);
        strSettingDir += "\\Setting\\";

        Poco::File file(strSettingDir);
        if (file.exists() && file.isDirectory())
        {
            std::vector<tstring> filters;
            std::vector<tstring> files;
            filters.push_back(CONFFILE);
            CTools::IteratorDir(strSettingDir, filters, files);
            
            for (int i = 0; i < files.size(); ++i)
            {
                Poco::SharedPtr<CUISetting> uiSetting = new CUISetting(files.at(i));
                m_settingArray.push_back(uiSetting);
            }
        }

        if (m_settingArray.size() > 0)
            m_uiActiveSetting = m_settingArray.at(0);
    }
}

void CReadLog::run()
{
    ParseFile();
}

void CReadLog::ParseLine(const tstring& strLine, int nLine)
{
    //Date Time   Thread Level Trace_Num Component CPU Text
    Poco::RegularExpression re(m_uiActiveSetting->GetRegularExpression());
    std::vector<tstring> stringAry;
    re.split(strLine, stringAry);
    if (stringAry.size() > 1)
    {
        Poco::SharedPtr<CTraceLog> log = new CTraceLog;
        log->m_nLine = nLine;
        if (!log.isNull())
        {
            for (int i = 1; i < stringAry.size(); ++i)
            {
                tstring strText = Poco::trim(stringAry.at(i));
                if (!strText.empty())
                    log->m_traceArray.push_back(stringAry.at(i));
            }
            
            const std::vector<Poco::SharedPtr<CVirtualComponent>>& components = m_uiActiveSetting->GetAllComponent();
            if (components.size() == 0)
                m_componentMap[OTHER_COMPONENT]->InsertLog(log);

            BOOL bFind = FALSE;
            for (int i = 0; i < components.size(); ++i)
            {
                int nCol = components.at(i)->GetColumnIndex();
                //If the current component is disabled, don't show it on UI
                if (nCol < log->m_traceArray.size() && components.at(i)->GetEnable() && log->m_traceArray.at(nCol).find(components.at(i)->GetCondition()) != tstring::npos)
                {
                    if (m_componentMap.find(components.at(i)->GetComponentName()) != m_componentMap.end())
                    {
                        m_componentMap[components.at(i)->GetComponentName()]->InsertLog(log);
                    }
                    else
                    {
                        Poco::SharedPtr<CComponent> component = new CComponent(components.at(i)->GetComponentName());
                        component->InsertLog(log);
                        m_componentMap[components.at(i)->GetComponentName()] = component;
                    }
                    bFind = TRUE;
                    break;
                }
                
            }
            if (!bFind)
            {
                //Default component
                m_componentMap[OTHER_COMPONENT]->InsertLog(log);
            }
        }
    }
}

void CReadLog::StartParse(const tstring& strFile)
{
    if (m_uiActiveSetting.isNull())
    {
        gTraceLogger.getLogger().error("UI setting is incorrect.");
        return;
    }
    if (!strFile.empty())
    {
        m_thread = new Poco::Thread;
        m_strFileName = strFile;
        m_thread->start(*this);
    }
    else
        gTraceLogger.getLogger().error("ERROR : The log file is empty.");
}

int CReadLog::GetCurProgress()
{
    if (0 == m_nFileSize)
        return 100;
    else
        return (int)(m_nUnReadSize*100/m_nFileSize);
}

void CReadLog::ParseFile()
{
    try
    {
        FILE* fp = NULL;
        tstring strFileName;
#ifdef _UNICODE
        //tstring<-->std::wstring
        Poco::UnicodeConverter::toUTF8(m_strFileName.c_str(), strFileName);
#else
        //tstring<-->tstring
        strFileName = m_strFileName;
#endif
        Poco::File file(strFileName);
        if (file.exists())
        {
            Poco::FileInputStream fileStream(strFileName);
            tstring strLineText;
            if (fileStream.good())
            {
                m_nFileSize = file.getSize();
                fileStream.seekg(0, std::ios_base::end);
                m_nFileSize = fileStream.tellg();
                fileStream.seekg(0, std::ios_base::beg);
                int nLine = 1;
                while (!fileStream.eof())
                {
                    std::getline(fileStream, strLineText);
                    m_nUnReadSize = fileStream.tellg();
                    if (!strLineText.empty())
                        ParseLine(strLineText, nLine++);
                }
                m_nUnReadSize = m_nFileSize;
                fileStream.close();
                ::PostMessage(m_hMainWnd, WM_UPDATECOMPONENTS, NULL, NULL);
            }
        }
        else
            gTraceLogger.getLogger().information(tstring("CReadLog::ParseFile - File is invalid : ") + strFileName);
    }
    catch (std::exception& ex)
    {
        gTraceLogger.getLogger().critical(tstring("CReadLog::ParseFile - ") + ex.what());
        throw ex;
    }
#ifdef RELEASE  
    catch (...)
    {
        gTraceLogger.getLogger().critical("CReadLog::ParseFile - Unknow Exception");
        throw;
    }
#endif
}

void CReadLog::GetAssocatedSourceFiles(Poco::SharedPtr<CTraceLog> traceLog)
{
    const std::vector<Poco::SharedPtr<CSourceCoePath>>& allPath = m_uiActiveSetting->GetAllSourceCodePath();
    for (int i = 0; i < allPath.size(); ++i)
    {
        if (allPath.at(i)->m_nColumnIndex < traceLog->m_traceArray.size())
        {
            tstring strText = traceLog->m_traceArray.at(allPath.at(i)->m_nColumnIndex);
            if (strText.find(allPath.at(i)->m_strCondition))
            {
                tstring strPathSet = allPath.at(i)->m_strSourceCodePath;
                if (!strPathSet.empty())
                {
                    std::vector<tstring> pathSet;
                    CTools::SplitString(strPathSet, ";", pathSet);
                    //Search file content
                    for (int i = 0; i < pathSet.size(); ++i)
                    {   
                        std::vector<tstring> filters;
                        std::vector<tstring> files;
                        filters.push_back(CPPSOURCEFILE);
                        filters.push_back(JAVAFILE);
                        if (m_bCheckHeaderFile)
                            filters.push_back(HEADERFILE);
                        tstring realPath = CTools::ReplaceEnvVar(pathSet.at(i));

                        //Parse file
                        if (m_uiActiveSetting->GetColumnIdentifierEnable())
                        {
                            CTools::IteratorDir(realPath, filters, files);
                            int nColumnIdentifier = m_uiActiveSetting->GetColumnIdentifier();
                            if (nColumnIdentifier >= 0 && nColumnIdentifier < traceLog->m_traceArray.size())
                            {
                                tstring strIdentifier = traceLog->m_traceArray.at(nColumnIdentifier);
                                if (!strIdentifier.empty())
                                {
                                    for (int j = 0; j < files.size(); ++j)
                                    {
                                        int nLine = CFindStringInFile::FindStringAndReturnLineNumber(files.at(j), strIdentifier);
                                        if (nLine >= 0)
                                        {
                                            traceLog->m_strSourceCodeFile = files.at(j);
                                            traceLog->m_nLineInSourceFile = nLine;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        //Execute python function
                        else if (m_uiActiveSetting->GetPythonModuleFlag())
                        {
                            Poco::SharedPtr<CPythonModule> pythonModule = m_uiActiveSetting->GetPythonModule();
                            if (!pythonModule.isNull())
                            {
                                if (pythonModule->GetFunctionName().empty())
                                {
                                    gTraceLogger.getLogger().error("The python module's definition is incorrect, cannot find function name");
                                    return;
                                }
                                char szPathTemp[MAX_PATH];
                                GetModuleFileNameA(NULL, szPathTemp, MAX_PATH);
                                tstring strEXEPath(szPathTemp);
                                int nPos = strEXEPath.rfind("\\");
                                if (nPos != tstring::npos)
                                {
                                    strEXEPath = strEXEPath.substr(0, nPos);
                                    strEXEPath += "\\";
                                    std::string strFileName("TemplateFile.py");
                                    strFileName = strEXEPath + strFileName;

                                    Poco::FileOutputStream outputStream(strFileName);
                                    tstring strLineText;

                                    if (outputStream.good())
                                    {
                                        for (int i = 0; i < pythonModule->GetSourceCode().size(); ++i)
                                            outputStream << pythonModule->GetSourceCode().at(i) << "\n";

                                        outputStream.close();
                                    }
                                    try
                                    {
                                        CPythonFunctionExecuter executer("TemplateFile", pythonModule->GetFunctionName());
                                        executer.addPath(strEXEPath);
                                        //The first parameter is source code path
                                        executer.PutArguments(realPath);
                                        //The following path are selected column
                                        for (int i = 0; i < pythonModule->GetArgColumns().size(); ++i)
                                        {
                                            int nIndex = pythonModule->GetArgColumns().at(i);
                                            executer.PutArguments(traceLog->m_traceArray.at(nIndex));
                                        }
                                        tstring strResult = executer.Execute();
                                        if (!strResult.empty())
                                        {
                                            std::vector<tstring> results;
                                            CTools::SplitString(strResult, ":", results);
                                            if (results.size() == 3)
                                            {
                                                traceLog->m_strSourceCodeFile = results.at(0) + ":" + results.at(1);
                                                traceLog->m_nLineInSourceFile = Poco::NumberParser::parse(results.at(2));
                                                break;
                                            }
                                            else
                                                gTraceLogger.getLogger().error("CReadLog::GetAssocatedSourceFiles : Failed to execute python function - " + strResult);
                                        }
                                        else
                                            gTraceLogger.getLogger().error("CReadLog::GetAssocatedSourceFiles : Failed to execute python function");
                                    }
                                    catch (const std::exception& e)
                                    {
                                        tstring strMsg("Failed to validate. Exception:");
                                        strMsg += e.what();
                                        ERROR_MESSAGEBOX(strMsg.c_str());
                                    }
                                    catch (...)
                                    {
                                        ERROR_MESSAGEBOX("Failed to validate. Unknown Exception.");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }   
    }
}
