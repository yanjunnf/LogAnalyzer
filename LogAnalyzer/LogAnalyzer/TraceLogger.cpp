#include "stdafx.h"
#include "tracelogger.h"
#include "CommonHeader.h"
#include <Poco/AutoPtr.h>
#include <Poco/FileChannel.h>
#include <Poco/Formatter.h>
#include <Poco/PatternFormatter.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>

CTraceLogger gTraceLogger;

CTraceLogger::CTraceLogger() : m_strFileName("LogAnalyzer.log")
{
}

Poco::Logger& CTraceLogger::getLogger()
{
    Poco::Logger& logger = Poco::Logger::root();
    if (logger.getChannel() == NULL)
    {
        Poco::AutoPtr<Poco::FileChannel> fileChannel = new Poco::FileChannel(m_strFileName);
        Poco::AutoPtr<Poco::PatternFormatter> formatter = new Poco::PatternFormatter("%Y-%m-%d %H:%M:%S.%i %t");
        formatter->setProperty(Poco::PatternFormatter::PROP_TIMES, "local");
        Poco::AutoPtr<Poco::FormattingChannel> fileFormattingChannel = new Poco::FormattingChannel(formatter.get(), fileChannel.get());
        logger.setChannel(fileFormattingChannel.get());
    }
    return logger;
}

CTraceLogger::~CTraceLogger()
{
    Poco::Logger& logger = Poco::Logger::root();
    if (logger.getChannel() != NULL)
    {
        Poco::FormattingChannel *channel = static_cast<Poco::FormattingChannel*>(logger.getChannel());
        channel->close();
        channel->setFormatter(NULL);
        channel->setChannel(NULL);
    }
    Poco::Logger::shutdown();
}