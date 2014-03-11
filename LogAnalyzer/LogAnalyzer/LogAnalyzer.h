
// LogAnalyzer.h : main header file for the LogAnalyzer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "Poco\SharedPtr.h"
#include "Settings.h"
#include <vector>
// CLogAnalyzerApp:
// See LogAnalyzer.cpp for the implementation of this class
//

class CLogAnalyzerApp : public CWinAppEx
{
public:
	CLogAnalyzerApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
    afx_msg void OnFileOpenEx();
	DECLARE_MESSAGE_MAP()

protected:
    std::vector<Poco::SharedPtr<CUISetting>> m_uiSettingArray;
    void ReadSettings();

public:
    std::vector<Poco::SharedPtr<CUISetting>> GetUISettings()const {return m_uiSettingArray;}
    void SetUISettings(std::vector<Poco::SharedPtr<CUISetting>> uiSettingArray);
    Poco::SharedPtr<CUISetting> GetUISetting(const tstring& strName);
};

extern CLogAnalyzerApp theApp;
