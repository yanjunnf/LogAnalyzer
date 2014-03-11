#pragma once
#include "resource.h"
#include "Settings.h"
#include <vector>

class CNewComponentDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CNewComponentDlg)

public:
    CNewComponentDlg(const std::vector<CString>& enabledColumns, CWnd* pParent = NULL);   // standard constructor
    virtual ~CNewComponentDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_NEW_COMPONENT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

protected:
    void SetEnabledColumns();

protected:
    CComboBox m_comboColumns;
    CString m_strComponentName;
    CString m_strCondition;
    CString m_strSelectedItem;
    BOOL m_bEnable;
    std::vector<CString> m_enabledColumns;

public:
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    CString GetComponentName()const{return m_strComponentName;}
    CString GetCondition()const{return m_strCondition;}
    BOOL GetEnable()const{return m_bEnable;}
    CString GetSelectedColumn()const{return m_strSelectedItem;}
};
