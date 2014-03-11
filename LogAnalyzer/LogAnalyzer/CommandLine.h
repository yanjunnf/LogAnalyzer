#ifndef __CCOMMAND_LINE__
#define __CCOMMAND_LINE__

#include "CommonHeader.h"
#include <vector>

/*
-FindAvailableTraceId TraceID=PLY
*/

class CCommandLine
{
public:
    enum OPERATIONS
    {
        NON_AUTOMATION = -1,
        UNKNOWN_OPERATION = 0,
        FIND_AVAILABLE_TRACE_ID,
        FIND_TRACE_ID,
        FIND_UNUSED_TRACE_ID
    };

    CCommandLine(const std::vector<tstring>& args);

protected:
    BOOL m_bAutomationStyle;
    OPERATIONS m_nOperationType;
    tstring m_strPath;
    tstring m_strTraceHeader;
    tstring m_strTraceID;
    tstring m_strSourcePath;
    tstring m_strExportFileName;
    tstring m_strErrorMessage;
    tstring m_strMessageFile;

protected:
    void Parse(const std::vector<tstring>& args);
    void Execute();

public:
    OPERATIONS getOperationType()const{return m_nOperationType;}
};


#endif //__CCOMMAND_LINE__