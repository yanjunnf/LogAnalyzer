#include "stdafx.h"
#include "ChooseColorDlg.h"
#include "afxdialogex.h"
#include <vector>

IMPLEMENT_DYNAMIC(CChooseColorDlg, CDialogEx)

    CChooseColorDlg::CChooseColorDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CChooseColorDlg::IDD, pParent)
{

    m_strCondition = _T("");
}

CChooseColorDlg::~CChooseColorDlg()
{
}

void CChooseColorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CONDITION_COLOR, m_listColor);
    DDX_Text(pDX, IDC_EDIT_CONDITION, m_strCondition);
    DDX_Control(pDX, IDC_MFCCOLORBUTTON_COLOR, m_colorContorl);
}


BEGIN_MESSAGE_MAP(CChooseColorDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &CChooseColorDlg::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CChooseColorDlg::OnBnClickedButtonDelete)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_CONDITION_COLOR, &CChooseColorDlg::OnCustomdrawListTrace)
    ON_BN_CLICKED(IDOK, &CChooseColorDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CChooseColorDlg message handlers


BOOL CChooseColorDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Condition";
    lv.cx = 200;
    m_listColor.InsertColumn(0, &lv);
    lv.cx = 100;
    lv.pszText = "Color";
    m_listColor.InsertColumn(1, &lv);
    m_listColor.SetExtendedStyle(m_listColor.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    if (m_colorMap.size() > 0)
    {
        int nIndex = 0;
        std::map<tstring, COLORREF>::iterator ite = m_colorMap.begin();
        while (ite != m_colorMap.end())
        {
            m_listColor.InsertItem(nIndex, ite->first.c_str());
            ++ite;
            ++nIndex;
        }
    }

    return TRUE;
}

void CChooseColorDlg::OnBnClickedButtonAdd()
{
    UpdateData(TRUE);
    if (m_strCondition.IsEmpty())
    {
        ERROR_MESSAGEBOX("The filed 'Contain what' cannot be empty.");
        return;
    }

    tstring strCondition = m_strCondition.GetBuffer(m_strCondition.GetLength());
    if (m_colorMap.find(strCondition) != m_colorMap.end())
    {
        ERROR_MESSAGEBOX("This condition has already existed.");
        return;
    }
    m_colorMap[strCondition] = m_colorContorl.GetColor();
    m_listColor.InsertItem(m_listColor.GetItemCount(), m_strCondition);
}

void CChooseColorDlg::OnBnClickedButtonDelete()
{
    POSITION pos = m_listColor.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        ERROR_MESSAGEBOX("Please select items you want to remove");
        return;
    }
    std::vector<int> selArray;
    while (pos != NULL)
    {
        int nSel = m_listColor.GetNextSelectedItem(pos);
        selArray.push_back(nSel);
    }
    std::vector<int>::reverse_iterator ite = selArray.rbegin();
    while (ite != selArray.rend())
    {
        CString strText = m_listColor.GetItemText(*ite, 0);
        std::map<tstring, COLORREF>::iterator iteColor = m_colorMap.find(strText.GetBuffer(strText.GetLength()));
        if (iteColor != m_colorMap.end())
            m_colorMap.erase(iteColor);
        m_listColor.DeleteItem(*ite);
        ++ite;
    }
}


//Draw different color for different trace level
void CChooseColorDlg::OnCustomdrawListTrace(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
    int nItem = lplvcd->nmcd.dwItemSpec;
    int nColumn = lplvcd->iSubItem;
    COLORREF bkColor = RGB(255, 255, 255);
    CString strText;
    switch (lplvcd->nmcd.dwDrawStage) 
    { 
    case CDDS_PREPAINT : 
        { 
            *pResult = CDRF_NOTIFYITEMDRAW; 
            return; 
        } 
    case CDDS_ITEMPREPAINT: 
        { 
            *pResult = CDRF_NOTIFYSUBITEMDRAW; 
            return; 
        } 
    case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
        { 
            switch (lplvcd->iSubItem)
            {
            case 1:
                bkColor = RGB(255, 255, 255);
                strText = m_listColor.GetItemText(nItem, 0);
                if (m_colorMap.find(strText.GetBuffer(strText.GetLength())) != m_colorMap.end())
                    bkColor =  m_colorMap[strText.GetBuffer(strText.GetLength())];

                lplvcd->clrTextBk = bkColor;
                break;
            default:
                lplvcd->clrTextBk = RGB(255, 255, 255);
                break;
            }
            *pResult = CDRF_DODEFAULT;  
            return; 
        }
    } 

    *pResult = 0;
}

void CChooseColorDlg::OnBnClickedOk()
{
    CDialogEx::OnOK();
}
