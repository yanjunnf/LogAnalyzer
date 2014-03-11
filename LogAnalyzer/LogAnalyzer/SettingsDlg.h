#pragma once

#include "Poco/SharedPtr.h"
#include "Readlog.h"
#include "Settings.h"
#include <map>
#include "afxmenubutton.h"
#include "afxwin.h"

class CSettingsDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSettingsDlg)

public:
    CSettingsDlg(std::vector<Poco::SharedPtr<CUISetting>> uiSettingArray, CWnd* pParent = NULL);
    virtual ~CSettingsDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_SETTINGS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkListPath(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnKillfocusEditEdtbox();

    void CSettingsDlg::InitializeData(int nSel);
    afx_msg void OnClickedButtonBrowse();
    BOOL SaveData();

protected:
    int m_nRow, m_nCol;
    CString m_notePadPath;
    CString m_strTraceExample;
    CString m_strOriginalText;
    CListCtrl m_pathList;
    CEdit m_edtPath;
    CListCtrl m_listColumes;
    CListCtrl m_listComponent;
    Poco::SharedPtr<CUISetting> m_uiSetting;
    std::vector<Poco::SharedPtr<CUISetting>> m_uiSettingArray;

    std::map<int, std::map<tstring, COLORREF>> m_colorMap;

    BOOL m_bPathListSelected;
    BOOL m_bModified;

public:
    afx_msg void OnClickedCheckSaveTraces();
    afx_msg void OnBnClickedButtonParse();
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnBnClickedButtonRemove();
    afx_msg void OnBnClickedButtonParseRe();
    afx_msg void OnDblclkListColumes(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonAddComponent();
    afx_msg void OnBnClickedButtonRemoveDeleteComponent();
    afx_msg void OnBnClickedButtonAddComponentCheck();
    afx_msg void OnBnClickedButtonAddSourcePathCheck();
    afx_msg void OnBnClickedButtonOk();
    afx_msg void OnBnClickedMfcmenubuttonAction();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnClickedCheckIdentifier();
    afx_msg void OnSelchangeComboSettings();
    afx_msg void OnBnClickedEditPythonCode();
    afx_msg void OnClickedCheckPython();

    BOOL GetModifyFlag()const{return m_bModified;}
    std::vector<Poco::SharedPtr<CUISetting>> GetUISettings()const {return m_uiSettingArray;}

protected:
    CMFCMenuButton m_menuButton;
    CMenu m_menu;
    CString m_strRegularExpression;
    CComboBox m_comboIdentifier;
    CComboBox m_uiSettings;
    BOOL m_bCheckIdentifier;
    BOOL m_bCheckPython;
    std::vector<CString> m_pythonSourceCodeArray;
    CString m_strFunctionName;
    CListBox m_listParameter;
};
