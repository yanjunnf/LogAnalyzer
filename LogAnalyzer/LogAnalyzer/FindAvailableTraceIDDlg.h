#pragma once

#include "CommonHeader.h"
#include "ReadLog.h"
#include <vector>
#include <map>

class CFindAvailableTraceIDDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CFindAvailableTraceIDDlg)

public:
    CFindAvailableTraceIDDlg(CWnd* pParent);   // standard constructor
    virtual ~CFindAvailableTraceIDDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_FIND_TRACE_ID };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

protected:
    CString m_strTraceHeader;
    CListCtrl m_listTraces;
    CComboBox m_cmboSourcePath;
    std::map<tstring, int> m_alreadyUsed;
    std::vector<tstring> m_availableTraces;

public:
    virtual BOOL OnInitDialog();

protected:
    virtual void OnOK();

public:
    afx_msg void OnClickedButtonBrowse();
    afx_msg void OnBnClickedBtnStart();
    afx_msg void OnBnClickedButtonExport();
};
