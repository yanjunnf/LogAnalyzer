#ifndef __PROPERTY_INFORMATION__
#define __PROPERTY_INFORMATION__

#include "CommonHeader.h"

struct SLogFileProperty
{
    tstring m_strFileName;
    Poco::UInt64 m_nFileSize;
};

struct SComponentProperty
{
    tstring m_strComponentName;
    int m_nLogTraceCount;
};

#endif //__PROPERTY_INFORMATION__