#pragma once
#include "Poco\SharedPtr.h"
#include "PropertyInformation.h"

class CUISetting;

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
public:
	CPropertiesWnd();
	void AdjustLayout();
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
    virtual ~CPropertiesWnd();

    //Operation
    void UpdateLogFileInformation(const Poco::SharedPtr<SLogFileProperty> logFilePro);
    void UpdateComponentInformation(const Poco::SharedPtr<SComponentProperty> compPro);
    void SetCurrentSelSetting(const Poco::SharedPtr<CUISetting> uiSetting);

    void UpdatePropertyPanel();

protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;
    Poco::SharedPtr<SComponentProperty> m_componentProperty;
    Poco::SharedPtr<SLogFileProperty> m_logFileProperty;
    Poco::SharedPtr<CUISetting> m_currentSelUISetting;
    
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnExpandAllProperties();
    afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
    afx_msg void OnSortProperties();
    afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
    afx_msg void OnProperties1();
    afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
    afx_msg void OnProperties2();
    afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    afx_msg void OnSelchangeComboSettings();

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};

