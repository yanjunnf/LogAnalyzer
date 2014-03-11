#include "stdafx.h"
#include "NewComponentDlg.h"
#include "afxdialogex.h"
#include "ReadLog.h"

IMPLEMENT_DYNAMIC(CNewComponentDlg, CDialogEx)

    CNewComponentDlg::CNewComponentDlg(const std::vector<CString>& enabledColumns, CWnd* pParent /*=NULL*/)
    : CDialogEx(CNewComponentDlg::IDD, pParent), m_bEnable(TRUE)
{
    m_enabledColumns = enabledColumns;
    m_strComponentName = _T("");
    m_strCondition = _T("");
}

CNewComponentDlg::~CNewComponentDlg()
{
}

void CNewComponentDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_ENABLED_COLUMNS, m_comboColumns);
    DDX_Text(pDX, IDC_EDIT_COMP_NAME, m_strComponentName);
    DDX_Text(pDX, IDC_EDIT_CONTAIN_STRING, m_strCondition);
    DDX_Check(pDX, IDC_CHECK_ENABLE, m_bEnable);
}


BEGIN_MESSAGE_MAP(CNewComponentDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CNewComponentDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewComponentDlg message handlers

void CNewComponentDlg::SetEnabledColumns()
{
    for (int i = 0; i < (int)m_enabledColumns.size(); ++i)
        m_comboColumns.AddString(m_enabledColumns.at(i));
    m_comboColumns.SetCurSel(0);
}

void CNewComponentDlg::OnBnClickedOk()
{
    UpdateData(TRUE);
    if (m_strComponentName.IsEmpty())
    {
        MessageBox("The component name cannot be empty.", "ERROR");
        return;
    }

    if (m_strCondition.IsEmpty())
    {
        MessageBox("The filed 'Contian what' cannot be empty.", "ERROR");
        return;
    }

    m_comboColumns.GetLBText(m_comboColumns.GetCurSel(), m_strSelectedItem);
    int nPos = m_strSelectedItem.Find(":");
    m_strSelectedItem = m_strSelectedItem.Left(nPos);
    CDialogEx::OnOK();
}


BOOL CNewComponentDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetEnabledColumns();
    return TRUE;
}
