#pragma once

#include "ReadLog.h"

class CFindSpecTraceIDDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CFindSpecTraceIDDlg)

public:
    CFindSpecTraceIDDlg(CWnd* pParent);   // standard constructor
    virtual ~CFindSpecTraceIDDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_FIND_SPEC_TRACE_ID };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

protected:
    CString m_strTraceID;
    CListCtrl m_listTraces;
    CComboBox m_cmboSourcePath;
    CMenu m_popupMenu;

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnRclickListTraceIds(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnQuickOpensourcefileForSpecTraceId();
};
