
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "Poco\SharedPtr.h"

struct SMsgParamter;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();
    virtual ~CMainFrame();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

public:
    CClassView* GetClassView(){return &m_wndClassView;}
    void SwitchDocument(const Poco::SharedPtr<SMsgParamter> messageParamter);
    void RemoveLogFileFromTreeView(const CString& strLogFile);

    //Update Log file Properties in property panel
    void SetLogFileProperty(const Poco::SharedPtr<SLogFileProperty> logFilePro);
    //Update component properties in property panel
    void SetComponentProperty(const Poco::SharedPtr<SComponentProperty> compPro);
    //Show UI settings in property panel
    void SetCurrentUISettingProperty(const Poco::SharedPtr<CUISetting> uiSetting);

    afx_msg void OnEditSettings();
    afx_msg void OnToolLogmonitoring();
    afx_msg void OnToolFindavailabeltraceid();
    afx_msg void OnToolSearchtraceid();
    afx_msg void OnButtonEditSettings();
};


