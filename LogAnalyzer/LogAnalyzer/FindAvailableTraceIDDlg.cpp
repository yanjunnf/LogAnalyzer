#include "stdafx.h"
#include "LogAnalyzer.h"
#include "FindAvailableTraceIDDlg.h"
#include "afxdialogex.h"
#include "TraceLogger.h"
#include "Tools.h"
#include "ReadLog.h"
#include "Poco/FileStream.h"

// CFindAvailableTraceIDDlg dialog

IMPLEMENT_DYNAMIC(CFindAvailableTraceIDDlg, CDialogEx)

CFindAvailableTraceIDDlg::CFindAvailableTraceIDDlg(CWnd* pParent)
	: CDialogEx(CFindAvailableTraceIDDlg::IDD, pParent)
{
    m_strTraceHeader = _T("");
}

CFindAvailableTraceIDDlg::~CFindAvailableTraceIDDlg()
{
}

void CFindAvailableTraceIDDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_TRACE_HEADER, m_strTraceHeader);
    DDV_MaxChars(pDX, m_strTraceHeader, 3);
    DDX_Control(pDX, IDC_LIST_TRACES, m_listTraces);
    DDX_Control(pDX, IDC_COMBO_SOURCE_PATH, m_cmboSourcePath);
}


BEGIN_MESSAGE_MAP(CFindAvailableTraceIDDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFindAvailableTraceIDDlg::OnClickedButtonBrowse)
    ON_BN_CLICKED(ID_BTN_START, &CFindAvailableTraceIDDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CFindAvailableTraceIDDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()

BOOL CFindAvailableTraceIDDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    
    //Init list control
    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Already Used";
    lv.cx = 200;
    m_listTraces.InsertColumn(0, &lv);

    lv.cx = 200;
    lv.pszText = "Available Trace ID";
    m_listTraces.InsertColumn(1, &lv);
    m_listTraces.SetExtendedStyle(m_listTraces.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);

    return TRUE;
}


void CFindAvailableTraceIDDlg::OnOK()
{
    UpdateData(true);
    CDialogEx::OnOK();
}


void CFindAvailableTraceIDDlg::OnClickedButtonBrowse()
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
        m_cmboSourcePath.InsertString(0, szPath);
        m_cmboSourcePath.SetCurSel(0);
    }
}


void CFindAvailableTraceIDDlg::OnBnClickedBtnStart()
{
    UpdateData(TRUE);
    
    CString strSourcePath;
    if (m_cmboSourcePath.GetCurSel() > 0)
        m_cmboSourcePath.GetLBText(m_cmboSourcePath.GetCurSel(), strSourcePath);
    else
        m_cmboSourcePath.GetWindowText(strSourcePath);
    if (strSourcePath.IsEmpty())
    {
        ERROR_MESSAGEBOX("The Source Path cannot be empty.");
        return;
    }
    if (m_strTraceHeader.IsEmpty())
    {
        ERROR_MESSAGEBOX("The Trace ID header cannot be empty.");
        return;
    }

    m_listTraces.DeleteAllItems();
    m_alreadyUsed.clear();
    m_availableTraces.clear();
    
    HCURSOR holdcursor,hwaitcursor;
    hwaitcursor = LoadCursor(NULL,IDC_WAIT);
    holdcursor = ::SetCursor(hwaitcursor);

    try
    {
        CTools::FindAvailableTraces(tstring(strSourcePath.GetBuffer(strSourcePath.GetLength())), 
            tstring(m_strTraceHeader.GetBuffer(m_strTraceHeader.GetLength())), m_alreadyUsed, m_availableTraces);
    }
    catch (...)
    {
        gTraceLogger.getLogger().information("CFindAvailableTraceIDDlg::OnBnClickedBtnStart() - Unknown Exception");
    }

    ::SetCursor(holdcursor);
    //Fill data into list control
    if (m_alreadyUsed.size() > m_availableTraces.size())
    {
        int nIndex = 0;
        TCHAR buff[16];
        std::map<tstring, int>::iterator iteUsed = m_alreadyUsed.begin();
        while (iteUsed != m_alreadyUsed.end())
        {
            tstring strText = iteUsed->first;
            strText += " : ";
            strText += itoa(iteUsed->second, buff, 10);
            m_listTraces.InsertItem(nIndex, strText.c_str());
            ++iteUsed;
            ++nIndex;
        }
        nIndex = 0;

        std::vector<tstring>::iterator iteUnUsed = m_availableTraces.begin();
        while (iteUnUsed != m_availableTraces.end())
        {
            m_listTraces.SetItemText(nIndex, 1, iteUnUsed->c_str());
            ++iteUnUsed;
            ++nIndex;
        }
    }
    else
    {
        int nIndex = 0;
        TCHAR buff[16];
        std::vector<tstring>::iterator iteUnUsed = m_availableTraces.begin();
        while (iteUnUsed != m_availableTraces.end())
        {
            m_listTraces.InsertItem(nIndex, "");
            m_listTraces.SetItemText(nIndex, 1, iteUnUsed->c_str());
            ++iteUnUsed;
            ++nIndex;
        }

        nIndex = 0;
        std::map<tstring, int>::iterator iteUsed = m_alreadyUsed.begin();
        while (iteUsed != m_alreadyUsed.end())
        {
            tstring strText = iteUsed->first;
            strText += " : ";
            strText += itoa(iteUsed->second, buff, 10);
            m_listTraces.SetItemText(nIndex, 0, strText.c_str());
            ++iteUsed;
            ++nIndex;
        }
    }
    
    GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);
}


void CFindAvailableTraceIDDlg::OnBnClickedButtonExport()
{
    tstring strExportResult("C:\\TraceExportResult.txt");
    Poco::FileOutputStream fileStream(strExportResult);
    tstring strLineText;
    if (fileStream.good())
    {
        TCHAR buff[16];
        fileStream << "Already Used Trace ID: \n";
        std::map<tstring, int>::iterator iteUsed = m_alreadyUsed.begin();
        while (iteUsed != m_alreadyUsed.end())
        {
            tstring strText = iteUsed->first;
            strText += " : ";
            strText += itoa(iteUsed->second, buff, 10);
            fileStream << " " << strText << "\n";
            ++iteUsed;
        }

        fileStream << "Unused Trace ID: \n";
        std::vector<tstring>::iterator iteUnUsed = m_availableTraces.begin();
        while (iteUnUsed != m_availableTraces.end())
        {
            fileStream << " " << *iteUnUsed << "\n";
            ++iteUnUsed;
        }
        fileStream.close();
        ERROR_MESSAGEBOX("Export successfully. File location : C:\\TraceExportResult.txt");
    }
}
