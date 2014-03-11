#include "stdafx.h"
#include "LogAnalyzer.h"
#include "FindSpecTraceIDDlg.h"
#include "afxdialogex.h"
#include "ReadLog.h"
#include "Tools.h"
#include "Poco/Process.h"

// CFindSpecTraceIDDlg dialog

IMPLEMENT_DYNAMIC(CFindSpecTraceIDDlg, CDialogEx)

CFindSpecTraceIDDlg::CFindSpecTraceIDDlg(CWnd* pParent)
	: CDialogEx(CFindSpecTraceIDDlg::IDD, pParent)
{
    m_strTraceID = _T("");
}

CFindSpecTraceIDDlg::~CFindSpecTraceIDDlg()
{
}

void CFindSpecTraceIDDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_TRACE_ID, m_strTraceID);
    DDV_MaxChars(pDX, m_strTraceID, 7);
    DDX_Control(pDX, IDC_LIST_TRACE_IDS, m_listTraces);
    DDX_Control(pDX, IDC_COMBO_SOURCE_PATH, m_cmboSourcePath);
}


BEGIN_MESSAGE_MAP(CFindSpecTraceIDDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFindSpecTraceIDDlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_START, &CFindSpecTraceIDDlg::OnBnClickedButtonStart)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_TRACE_IDS, &CFindSpecTraceIDDlg::OnRclickListTraceIds)
    ON_COMMAND(ID_OPEN_OPENSOURCEFILE, &CFindSpecTraceIDDlg::OnQuickOpensourcefileForSpecTraceId)
END_MESSAGE_MAP()



BOOL CFindSpecTraceIDDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_popupMenu.LoadMenuA(IDR_MENU_OPEN_SOURCE_FILE);
    //Init list control
    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "File Name";
    lv.cx = 300;
    m_listTraces.InsertColumn(0, &lv);

    lv.cx = 100;
    lv.pszText = "Line";
    m_listTraces.InsertColumn(1, &lv);
    m_listTraces.SetExtendedStyle(m_listTraces.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    return TRUE;
}


void CFindSpecTraceIDDlg::OnBnClickedButtonBrowse()
{
    UpdateData(TRUE);
    TCHAR szPath[MAX_PATH];
    CString str;
    ZeroMemory(szPath, sizeof(szPath));
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "Save Trace";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);

    if(lp && SHGetPathFromIDList(lp, szPath))
    {
        //m_strSourcePath = szPath;
        m_cmboSourcePath.InsertString(0, szPath);
        m_cmboSourcePath.SetCurSel(0);
    }
}

void CFindSpecTraceIDDlg::OnBnClickedButtonStart()
{
    UpdateData(TRUE);
    CString strSourcePath;
    int nCurSel = m_cmboSourcePath.GetCurSel();
    if (nCurSel >= 0)
        m_cmboSourcePath.GetLBText(m_cmboSourcePath.GetCurSel(), strSourcePath);
    else
        m_cmboSourcePath.GetWindowTextA(strSourcePath);

    if (strSourcePath.IsEmpty())
    {
        ERROR_MESSAGEBOX("The Source Path cannot be empty.");
        return;
    }
    if (m_strTraceID.IsEmpty())
    {
        ERROR_MESSAGEBOX("The Trace ID cannot be empty.");
        return;
    }

    m_listTraces.DeleteAllItems();

    HCURSOR holdcursor, hwaitcursor;
    hwaitcursor = LoadCursor(NULL,IDC_WAIT);
    holdcursor = ::SetCursor(hwaitcursor);

    std::map<tstring, int> traceIDMap;
    CTools::FindASpecTraceID(tstring(strSourcePath.GetBuffer(strSourcePath.GetLength())), 
        tstring(m_strTraceID.GetBuffer(m_strTraceID.GetLength())), traceIDMap);
    ::SetCursor(holdcursor);
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
    if (traceIDMap.size() > 0)
    {
        std::map<tstring, int>::iterator ite = traceIDMap.begin();
        int nLine = 0;
        TCHAR buff[16];
        while (ite != traceIDMap.end())
        {
            itoa(ite->second, buff, 10);
            m_listTraces.InsertItem(nLine, ite->first.c_str());
            m_listTraces.SetItemText(nLine, 1, buff);
            ++nLine;
            ++ite;
        }
    }
    else
    {
        CString strMessage = "Cannot find the trace id '";
        strMessage += m_strTraceID;
        strMessage += "'";
        strMessage += "from path '";
        strMessage += strSourcePath;
        strMessage += "'";
        ERROR_MESSAGEBOX(strMessage);
    }
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
}

void CFindSpecTraceIDDlg::OnRclickListTraceIds(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    if (m_listTraces.GetItemCount() > 0 && m_listTraces.GetSelectedCount() > 0)
    {
        // Popup menu
        CMenu* pMenu = NULL;
        pMenu = m_popupMenu.GetSubMenu(0);
        if (pMenu != NULL)
        {
            CPoint point(pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y);
            m_listTraces.ClientToScreen(&point);
            pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
        }
    }

    *pResult = 0;
}

void CFindSpecTraceIDDlg::OnQuickOpensourcefileForSpecTraceId()
{
    /*
    POSITION pt = m_listTraces.GetFirstSelectedItemPosition();
    int nSelIndex = m_listTraces.GetNextSelectedItem(pt);
    if (nSelIndex >= 0)
    {
        if (!m_pReadLog->GetActiveUISetting().isNull())
        {
            std::vector<tstring> args;
            tstring strCommandLine = m_pReadLog->GetActiveUISetting()->GetNodepadPath();
            CString strItemText(strCommandLine.c_str());
            strItemText += " -n";
            strItemText += m_listTraces.GetItemText(nSelIndex, 1);
            strItemText += " \"";
            strItemText += m_listTraces.GetItemText(nSelIndex, 0);
            strItemText += " \"";
            Poco::Process::launch(tstring(strItemText.GetBuffer(strItemText.GetLength())), args);
        }
    }
    */
}
