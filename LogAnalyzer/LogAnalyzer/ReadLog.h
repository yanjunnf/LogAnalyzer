#ifndef __READ_LOG_IMPL__
#define __READ_LOG_IMPL__

#include "CommonHeader.h"
#include "Settings.h"
#include "Poco/SharedPtr.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

#include <vector>
#include <map>

#define OTHER_COMPONENT "Other Component"

class CReadLog : public Poco::Runnable
{
public:
    CReadLog(const HWND mainWnd, const tstring& strFileName);
    ~CReadLog();
    virtual void run();
    tstring GetFileName()const{return m_strFileName;}
    void SetFileName(const tstring& strFileName){m_strFileName = strFileName;}
    int GetCurProgress();
    std::ios::streampos GetFileSize()const{return m_nFileSize;}
    void StartParse(const tstring& strFile);

    const std::map<tstring, Poco::SharedPtr<CComponent>>& GetComponents()const{return m_componentMap;}
    void ClearAllComponents()
    {
        m_componentMap.clear();
        Poco::SharedPtr<CComponent> defaultComponent = new CComponent(OTHER_COMPONENT);
        m_componentMap[OTHER_COMPONENT] = defaultComponent;
    }

    const Poco::SharedPtr<CUISetting> GetActiveUISetting()const{return m_uiActiveSetting;}
    const std::vector<Poco::SharedPtr<CUISetting>>& GetAllUISetting()const{return m_settingArray;}
    void SetActiveUISetting(const Poco::SharedPtr<CUISetting> uiSetting){m_uiActiveSetting = uiSetting;}
    void InsertUISetting(const Poco::SharedPtr<CUISetting> uiSetting){m_settingArray.push_back(uiSetting);}

    void GetAssocatedSourceFiles(Poco::SharedPtr<CTraceLog> traceLog);

private:
    void ParseSettingDir();
    void ParseFile();
    void ParseLine(const tstring& strLine, int nLine);

private:
    BOOL m_bCheckHeaderFile;
    HWND m_hMainWnd;
    std::ios::streampos m_nFileSize;
    std::ios::streampos m_nUnReadSize;
    Poco::SharedPtr<Poco::Thread> m_thread;
    tstring m_strFileName;

    std::vector<Poco::SharedPtr<CUISetting>> m_settingArray;
    Poco::SharedPtr<CUISetting> m_uiActiveSetting;
    std::map<tstring, Poco::SharedPtr<CComponent>> m_componentMap;
};



#endif //__READ_LOG_IMPL__