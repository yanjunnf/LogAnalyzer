// NewComponent.cpp : implementation file
//

#include "stdafx.h"
#include "NewSourcePathDlg.h"
#include "afxdialogex.h"
#include "CommonHeader.h"


// CNewSourcePathDlg dialog

IMPLEMENT_DYNAMIC(CNewSourcePathDlg, CDialogEx)

    CNewSourcePathDlg::CNewSourcePathDlg(const std::vector<CString>& enabledColumns, CWnd* pParent /*=NULL*/)
    : CDialogEx(CNewSourcePathDlg::IDD, pParent), m_bEnable(TRUE)
{
    m_enabledColumns = enabledColumns;
    m_strSourceCodePath = _T("");
    m_strContian = _T("");
}

CNewSourcePathDlg::~CNewSourcePathDlg()
{
}

void CNewSourcePathDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_ENABLE, m_bEnable);
    DDX_Text(pDX, IDC_EDIT_SOURCECODE_PATH, m_strSourceCodePath);
    DDV_MaxChars(pDX, m_strSourceCodePath, 256);
    DDX_Control(pDX, IDC_COMBO_ENABLED_COLUMNS, m_comboColumns);
    DDX_Text(pDX, IDC_EDIT_CONTIAN_STRING, m_strContian);
}

// CNewSourcePathDlg message handlers
BEGIN_MESSAGE_MAP(CNewSourcePathDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CNewSourcePathDlg::OnBnClickedOk)
    ON_BN_CLICKED(ID_BUTTON_BRO, &CNewSourcePathDlg::OnBnClickedButtonBro)
END_MESSAGE_MAP()


void CNewSourcePathDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    if (m_strContian.IsEmpty())
    {
        ERROR_MESSAGEBOX("The field 'Contian what' cannot be empty.");
        return;
    }
    if (m_strSourceCodePath.IsEmpty())
    {
        ERROR_MESSAGEBOX("The source code path cannot be empty.");
        return;
    }
    CString strItem;
    m_comboColumns.GetLBText(m_comboColumns.GetCurSel(), strItem);
    int nPos = strItem.Find(":");
    m_strSelectedColumn = strItem.Left(nPos);
    CDialogEx::OnOK();
}


void CNewSourcePathDlg::OnBnClickedButtonBro()
{
    UpdateData(TRUE);
    TCHAR szPath[MAX_PATH];
    CString str;
    ZeroMemory(szPath, sizeof(szPath));
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "Get Source Code path:";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);

    if (lp && SHGetPathFromIDList(lp, szPath))
    {
        m_strSourceCodePath = szPath;
        UpdateData(FALSE);
    }
}

void CNewSourcePathDlg::SetEnabledColumns()
{
    for (int i = 0; i < (int)m_enabledColumns.size(); ++i)
        m_comboColumns.AddString(m_enabledColumns.at(i));
    m_comboColumns.SetCurSel(0);
}

BOOL CNewSourcePathDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetEnabledColumns();

    return TRUE;
}
