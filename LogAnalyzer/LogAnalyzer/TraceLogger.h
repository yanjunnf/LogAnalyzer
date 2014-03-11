#ifndef __TRACE_LOGGER__
#define __TRACE_LOGGER__

#include "CommonHeader.h"
#include <Poco/SharedPtr.h>
#include <Poco/SharedPtr.h>
#include <Poco/logger.h>

class CTraceLogger
{
public:
    CTraceLogger();
    ~CTraceLogger();
    Poco::Logger& getLogger();

protected:
    tstring m_strFileName;
};

extern CTraceLogger gTraceLogger;

#endif //__TRACE_LOGGER__