#pragma once
#include "resource.h"
#include <vector>
// CNewSourcePath dialog

class CNewSourcePathDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CNewSourcePathDlg)

public:
    CNewSourcePathDlg(const std::vector<CString>& enabledColumns, CWnd* pParent = NULL);   // standard constructor
    virtual ~CNewSourcePathDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_NEW_SOURCE_PATH};

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()

protected:
    BOOL m_bEnable;
    CString m_strSourceCodePath;
    CComboBox m_comboColumns;
    CString m_strContian;
    std::vector<CString> m_enabledColumns;
    CString m_strSelectedColumn;
protected:
    void SetEnabledColumns();

public:
    afx_msg void OnBnClickedOk();
    CString GetSelectedColumnIndex()const{return m_strSelectedColumn;};
    CString GetContianWhat()const{return m_strContian;}
    CString GetSourceCodePath()const{return m_strSourceCodePath;}
    BOOL GetEnable()const{return m_bEnable;}
    afx_msg void OnBnClickedButtonBro();    
    virtual BOOL OnInitDialog();
};
