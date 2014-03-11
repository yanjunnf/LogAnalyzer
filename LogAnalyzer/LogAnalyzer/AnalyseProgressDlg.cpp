// AnalyseProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogAnalyzer.h"
#include "AnalyseProgressDlg.h"
#include "afxdialogex.h"
#include "CommonHeader.h"


// CAnalyseProgressDlg dialog

IMPLEMENT_DYNAMIC(CAnalyseProgressDlg, CDialogEx)

CAnalyseProgressDlg::CAnalyseProgressDlg(const Poco::SharedPtr<CReadLog> readLog, CWnd* pParent /*=NULL*/)
    : m_readLog(readLog), CDialogEx(CAnalyseProgressDlg::IDD, pParent)
{

}

CAnalyseProgressDlg::~CAnalyseProgressDlg()
{
}

void CAnalyseProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_ANALYZE, m_progressCtrl);
}


BEGIN_MESSAGE_MAP(CAnalyseProgressDlg, CDialogEx)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS_ANALYZE, &CAnalyseProgressDlg::OnNMCustomdrawProgressAnalyze)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CAnalyseProgressDlg message handlers


void CAnalyseProgressDlg::OnNMCustomdrawProgressAnalyze(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    *pResult = 0;
}


BOOL CAnalyseProgressDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_progressCtrl.SetRange(0, 100);
    m_progressCtrl.SetStep(1);
    m_progressCtrl.SetPos(0);
    ::SetTimer(GetSafeHwnd(), 1, 10, NULL);

    return TRUE;
}


void CAnalyseProgressDlg::OnCancel()
{
    //CDialogEx::OnCancel();
}

void CAnalyseProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (!m_readLog.isNull())
    {
        int nPos = m_readLog->GetCurProgress();
        m_progressCtrl.SetPos(nPos);
    }
    if (m_progressCtrl.GetPos() == 100)
    {
        ::PostMessageA(GetParent()->GetSafeHwnd(), WM_GENERATEANALYSERESULT, NULL, NULL);
        CDialogEx::OnCancel();
    }
    CDialogEx::OnTimer(nIDEvent);
}