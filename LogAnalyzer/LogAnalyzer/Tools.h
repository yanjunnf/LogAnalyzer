#ifndef __READ_LOG__
#define __READ_LOG__

#include "CommonHeader.h"
#include "Poco/SharedPtr.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

#include <vector>
#include <map>

/*
class CTraceLog
{
public:
    CTraceLog() : m_nLine(0){}
    //Log Date
    tstring m_strDate;
    //Log time
    tstring m_strTime;
    //The current thread id
    tstring m_strThreadId;
    //Log priority
    tstring m_strLevel;
    //Log number
    tstring m_strLogNumber;
    //CPU utilization
    tstring m_strCPUUtilization;
    //Log message text
    tstring m_strMessageText;
    //The related source file
    tstring m_strSourceCodeFile;
    //The related message file
    tstring m_strMessageFile;
    //The line number in source file
    int m_nLine;
};
*/

struct STrace
{
    tstring m_strTrace;
    int m_nLine;
};

class CFilesTraceSet
{
public:
    CFilesTraceSet(){};
    ~CFilesTraceSet(){m_fileTraceMap.clear();}
    void ParseFile(const tstring& strFileName);
    BOOL ValidateTraceLog(const tstring& strFileName, const tstring& strTrace);
    BOOL IsFileExist(const tstring& strFileName);
protected:
    //Key : file full path
    //Value : All traces(Trace ID and Line) in this file
    std::map<tstring, std::vector<Poco::SharedPtr<STrace>>> m_fileTraceMap;

};

//1. Need to support Java/C++ file
//2. Need to filter the comment line
class CFindStringInFile
{
public:
    CFindStringInFile(const tstring& strFileName) : m_strFileName(strFileName){ReadFile();};
    BOOL FindString(const tstring& strValue);
    static int FindStringAndReturnLineNumber(const tstring& strFileName, const tstring& strValue);

protected:
    void ReadFile();

protected:
    tstring m_strContent;
    tstring m_strFileName;
};

class CTools
{
public:
    //std::map<tstring, int>
    //  key:trace id
    //  value:Line value
    static void FindASpecTraceID(const tstring& path, const tstring& strTraceID, std::map<tstring, int>& traceIDMap, BOOL bCheckHeaderFile = FALSE);

    //std::map<tstring, int> :
    //  key:trace id
    //  value:used times
    static void FindAvailableTraces(const tstring& path, const tstring& strTraceHeader, std::map<tstring, int>& alreadyUsed, std::vector<tstring>& availableTraces, BOOL bCheckHeaderFile = FALSE);
    //Split string using a split string
    static void SplitString(const tstring& source, const tstring& split, std::vector<tstring>& result);
    //Replace ENV variable
    static tstring ReplaceEnvVar(const tstring& strEnv);
    //Go through all files for a folder
    static void IteratorDir(const tstring& path, const std::vector<tstring>& filters, std::vector<tstring>& files);
};

#endif //__READ_LOG__