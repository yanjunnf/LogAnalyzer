#include "stdafx.h"
#include "Tools.h"
#include "TraceLogger.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/String.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/RegularExpression.h"
#include "Poco/Environment.h"
#include "Poco/DirectoryIterator.h"
#include <windows.h>
#include <set>

/*
void CComponent::InsertLog(const Poco::SharedPtr<CTraceLog> log)
{
    m_vectTraceLogs.push_back(log);
}

void CComponent::getLogTypeCount(int& nTrace, int& nNotification, int& nError, int& nInformation, int& nDebug, int& nCritical, int& nWarning, int& nFatal)const
{
    for (int i = 0; i < m_vectTraceLogs.size(); ++i)
    {
        if (m_vectTraceLogs.at(i)->m_strLevel == "E")
            ++nError;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "I")
            ++nInformation;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "T")
            ++nTrace;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "N")
            ++nNotification;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "D")
            ++nDebug;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "W")
            ++nWarning;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "F")
            ++nFatal;
        else if (m_vectTraceLogs.at(i)->m_strLevel == "C")
            ++nCritical;
    }
}

void CComponent::RemoveAllTraces()
{
    m_vectTraceLogs.clear();
}
*/

void CTools::SplitString(const tstring& source, const tstring& split, std::vector<tstring>& result)
{
    if (!source.empty())
    {
        result.clear();
        int nStartPos = 0;
        int nEndPos = source.find(split, nStartPos);
        while (nEndPos != tstring::npos)
        {
            tstring strPath = source.substr(nStartPos, nEndPos - nStartPos);
            if (!strPath.empty())
            {
                //strPath = CTools::ReplaceEnvVar(strPath);
                result.push_back(strPath);
            }
            nStartPos = nEndPos + 1;
            nEndPos = source.find(split, nStartPos);
        }
        tstring strLast = source.substr(nStartPos, source.length());
        if (!strLast.empty())
        {
            //strLast = CTools::ReplaceEnvVar(strLast);
            result.push_back(strLast);
        }
    }
}

tstring CTools::ReplaceEnvVar(const tstring& strEnv)
{
    tstring strResult = strEnv;
    Poco::RegularExpression re("(.*){(.*)}(.*)");
    std::vector<tstring> stringAry;
    re.split(strEnv, stringAry);
    if (stringAry.size() == 4)
    {
        if (!stringAry.at(2).empty())
        {
            tstring strEnvVar = Poco::Environment::get(stringAry.at(2));
            if (!strEnvVar.empty())
            {
                strResult = stringAry.at(1);
                strResult += strEnvVar;
                strResult += stringAry.at(3);
            }
            else
                gTraceLogger.getLogger().information(tstring("CTools::ReplaceEnvVar() - Invalid environment variable : ") + stringAry.at(2));
        }
    }
    return strResult;
}

void CTools::IteratorDir(const tstring& path, const std::vector<tstring>& filters, std::vector<tstring>& files)
{
    try
    {
        Poco::DirectoryIterator it(path);
        Poco::DirectoryIterator end;
        while (it != end)
        {
            if (it->isDirectory())
                CTools::IteratorDir(it->path(), filters, files);
            else
            {
                tstring strFile = it->path();
                for (int i = 0; i < filters.size(); ++i)
                {
                    if (strFile.find(filters[i]) != tstring::npos)
                    {
                        files.push_back(it->path());
                        break;
                    }
                }
            }
            ++it;
        }
    }
    catch (const Poco::PathNotFoundException& exc)
    {
        gTraceLogger.getLogger().information(tstring("CTools::IteratorDir() - Exception:  Poco::PathNotFoundException : ") + exc.what());
    }
    catch (const Poco::PathSyntaxException& exc)
    {
        gTraceLogger.getLogger().information(tstring("CTools::IteratorDir() - Exception:  Poco::PathSyntaxException : ") + exc.what());
    }
}


void CFilesTraceSet::ParseFile(const tstring& strFileName)
{
    std::map<tstring, std::vector<Poco::SharedPtr<STrace>>>::iterator ite = m_fileTraceMap.find(strFileName);
    if (ite == m_fileTraceMap.end())
    {
        Poco::FileInputStream fileStream(strFileName);
        tstring strLineText;
        if (fileStream.good())
        {
            int nLine = 1;
            std::vector<tstring> traceIDSet;
            //Trace log length = 7
            Poco::RegularExpression re("([a-z0-9A-Z]{3}-\\d{3})");
            std::vector<Poco::SharedPtr<STrace>> traceVect;
            while (!fileStream.eof())
            {   
                std::getline(fileStream, strLineText);
                if (!strLineText.empty())
                {   
                    re.split(strLineText, traceIDSet);
                    if (traceIDSet.size() == 2)
                    {
                        Poco::SharedPtr<STrace> traceObj = new STrace;
                        if (!traceObj.isNull())
                        {
                            traceObj->m_strTrace = traceIDSet.at(1);
                            traceObj->m_nLine = nLine;
                            traceVect.push_back(traceObj);
                        }
                    }
                }
                ++nLine;
            }
            fileStream.close();
            m_fileTraceMap[strFileName] = traceVect;
        }
        else
            gTraceLogger.getLogger().information(tstring("CFilesTraceSet::ParseFile - File is invalid : ") + strFileName);
    }
}

//Return the line value
int CFilesTraceSet::ValidateTraceLog(const tstring& strFileName, const tstring& strTrace)
{
    std::map<tstring, std::vector<Poco::SharedPtr<STrace>>>::iterator ite = m_fileTraceMap.find(strFileName);
    if (ite != m_fileTraceMap.end())
    {
        for (int i = 0; i < ite->second.size(); ++i)
        {
            if (ite->second.at(i)->m_strTrace == strTrace)
                return ite->second.at(i)->m_nLine;
        }
    }
    return 0;
}

BOOL CFilesTraceSet::IsFileExist(const tstring& strFileName)
{
    return m_fileTraceMap.find(strFileName) != m_fileTraceMap.end();
}

void CTools::FindAvailableTraces(const tstring& path, const tstring& strTraceHeader, 
                                   std::map<tstring, int>& alreadyUsed, std::vector<tstring>& availableTraces, BOOL bCheckHeaderFile)
{
    if (!path.empty())
    {
        std::vector<tstring> pathSet;
        CTools::SplitString(path, ";", pathSet);
        std::set<int> usedIDSet;
        //Search file content
        for (int i = 0; i < pathSet.size(); ++i)
        {
            std::vector<tstring> filters;
            std::vector<tstring> files;
            filters.push_back(CPPSOURCEFILE);
            if (bCheckHeaderFile)
                filters.push_back(HEADERFILE);
            usedIDSet.clear();
            tstring realPath = CTools::ReplaceEnvVar(pathSet.at(i));
            CTools::IteratorDir(realPath, filters, files);
            //Parse file
            for (int j = 0; j < files.size(); ++j)
            {
                Poco::FileInputStream fileStream(files.at(j));
                tstring strLineText;
                if (fileStream.good())
                {
                    int nLine = 1;
                    std::vector<tstring> traceIDSet;
                    //Trace log length = 7
                    Poco::RegularExpression re("([a-z0-9A-Z]{3}-(\\d{3}))");
                    std::vector<Poco::SharedPtr<STrace>> traceVect;
                    while (!fileStream.eof())
                    {   
                        std::getline(fileStream, strLineText);
                        if (!strLineText.empty())
                        {   
                            re.split(strLineText, traceIDSet);
                            if (traceIDSet.size() == 3)
                            {
                                if (traceIDSet.at(1).find(Poco::toUpper(strTraceHeader)) != tstring::npos)
                                {
                                    if (alreadyUsed.find(traceIDSet.at(1)) != alreadyUsed.end())
                                        alreadyUsed[traceIDSet.at(1)] += 1;
                                    else
                                        alreadyUsed[traceIDSet.at(1)] = 1;
                                    usedIDSet.insert(atoi(traceIDSet.at(2).c_str()));
                                }
                            }
                        }
                        ++nLine;
                    }
                    fileStream.close();
                }
            }
            //Get unused traces
            for (int i = 0; i <= 999; ++i)
            {
                if (usedIDSet.find(i) == usedIDSet.end())
                {
                    TCHAR buff[16];
                    sprintf(buff, "-%03d", i);
                    availableTraces.push_back(strTraceHeader + buff);
                }
            }
        }
    }
    else
        gTraceLogger.getLogger().information(tstring("CTools::FindAvailableTraces - Path is invalid : ") + path);
}

void CTools::FindASpecTraceID(const tstring& path, const tstring& strTraceID, std::map<tstring, int>& traceIDMap, BOOL bCheckHeaderFile/* = FALSE*/)
{
    if (!path.empty())
    {
        std::vector<tstring> pathSet;
        CTools::SplitString(path, ";", pathSet);
        //Search file content
        for (int i = 0; i < pathSet.size(); ++i)
        {
            std::vector<tstring> filters;
            std::vector<tstring> files;
            filters.push_back(CPPSOURCEFILE);
            if (bCheckHeaderFile)
                filters.push_back(HEADERFILE);
            tstring realPath = CTools::ReplaceEnvVar(pathSet.at(i));
            CTools::IteratorDir(realPath, filters, files);
            //Parse file
            for (int i = 0; i < files.size(); ++i)
            {
                Poco::FileInputStream fileStream(files.at(i));
                tstring strLineText;
                if (fileStream.good())
                {
                    int nLine = 1;
                    std::vector<tstring> traceIDSet;
                    //Trace log length = 7
                    Poco::RegularExpression re("([a-z0-9A-Z]{3}-\\d{3})");
                    std::vector<Poco::SharedPtr<STrace>> traceVect;
                    while (!fileStream.eof())
                    {   
                        std::getline(fileStream, strLineText);
                        if (!strLineText.empty())
                        {   
                            re.split(strLineText, traceIDSet);
                            if (traceIDSet.size() == 2)
                            {
                                if (traceIDSet.at(1) == Poco::toUpper(strTraceID))
                                    traceIDMap[files.at(i)] = nLine;
                            }
                        }
                        ++nLine;
                    }
                    fileStream.close();
                }
            }
        }
    }
    else
        gTraceLogger.getLogger().information(tstring("CTools::FindASpecTraceID - Path is invalid : ") + path);
}

//////////////////////////////////////////////////////////////////////////
//CFindStringInFile
void CFindStringInFile::ReadFile()
{
    if (!m_strFileName.empty())
    {
        m_strContent.clear();
        tstring strContent;
        Poco::FileInputStream fileStream(m_strFileName);
        tstring strLineText;
        if (fileStream.good())
        {
            while (!fileStream.eof())
            {
                std::getline(fileStream, strLineText);

                if (strLineText.empty())
                    continue;
                if (strLineText.find("#define") == 0)
                    continue;
                if (strLineText.find("#include") == 0)
                    continue;
                if (strLineText.find("using") == 0)
                    continue;
                if (strLineText.find("namespace") == 0)
                    continue;
                if (strLineText.find("void") == 0)
                    continue;
                if (strLineText.find("return") == 0)
                    continue;

                //If start with double slash, ignore this line
                int nPos = strLineText.find("//");
                if (nPos == tstring::npos)
                    strContent += strLineText;
                else if (nPos > 0)
                    strContent += strLineText.substr(0, nPos);
            }
            fileStream.close();
        }

        //Remove all comments, start with "/*", end with "*/"
        if (!strContent.empty())
        {
            int nStartPos = 0;
            while (TRUE)
            {
                //Find "/*"
                int nPos = strContent.find("/*", nStartPos);
                if (nPos == tstring::npos)
                {
                    m_strContent += strContent.substr(nStartPos);
                    break;
                }
                m_strContent += strContent.substr(nStartPos, nPos);
                nStartPos = nPos;

                //Find "*/"
                nPos = strContent.find("*/", nPos);
                if (nPos == tstring::npos)
                    break;
                nStartPos = nPos + 2;
            };
        }
    }
}

BOOL CFindStringInFile::FindString(const tstring& strValue)
{
    return m_strContent.find(strValue) != tstring::npos;
}

int CFindStringInFile::FindStringAndReturnLineNumber(const tstring& strFileName, const tstring& strValue)
{
    int nLine = 1;
    BOOL bFind = FALSE;
    Poco::FileInputStream fileStream(strFileName);
    tstring strLineText;
    if (fileStream.good())
    {
        std::vector<tstring> traceIDSet;
        //Trace log length = 7
        while (!fileStream.eof())
        {   
            std::getline(fileStream, strLineText);
            if (!strLineText.empty())
            {   
                if (strLineText.find(strValue) != tstring::npos)
                {
                    bFind = TRUE;
                    break;
                }
            }
            ++nLine;
        }
        fileStream.close();
    }
    else
        gTraceLogger.getLogger().information(tstring("CFindStringInFile::FindStringAndReturnLineNumber - File is invalid : ") + strFileName);

    return bFind ? nLine : -1;
}
//////////////////////////////////////////////////////////////////////////