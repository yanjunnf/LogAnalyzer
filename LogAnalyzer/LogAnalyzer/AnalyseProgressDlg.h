#pragma once
#include "afxcmn.h"
#include "ReadLog.h"

class CReadLog;

class CAnalyseProgressDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CAnalyseProgressDlg)

public:
    CAnalyseProgressDlg(const Poco::SharedPtr<CReadLog> readLog, CWnd* pParent = NULL);   // standard constructor
    virtual ~CAnalyseProgressDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_PROGRESS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnNMCustomdrawProgressAnalyze(NMHDR *pNMHDR, LRESULT *pResult);
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
    Poco::SharedPtr<CReadLog> m_readLog;
    CProgressCtrl m_progressCtrl;
};
