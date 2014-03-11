#include "stdafx.h"
#include "LogAnalyzer.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "NewSourcePathDlg.h"
#include "NewComponentDlg.h"
#include "PythonMethodEditDlg.h"
#include "ChooseColorDlg.h"
#include "Poco/RegularExpression.h"
#include "Poco/String.h"
#include "Poco/NumberParser.h"

#define COLUMN_NAME "Column"

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialogEx)

CSettingsDlg::CSettingsDlg(std::vector<Poco::SharedPtr<CUISetting>> uiSettingArray, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingsDlg::IDD, pParent), m_uiSettingArray(uiSettingArray), m_nRow(0), m_nCol(0)
    , m_notePadPath(_T(""))
    , m_strRegularExpression(_T(""))
    , m_bCheckIdentifier(FALSE)
    , m_bModified(FALSE)
    , m_bCheckPython(FALSE)
{   
    m_notePadPath = _T("");
    m_strTraceExample = _T("");
    m_strFunctionName = _T("");
}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PATH, m_pathList);
    DDX_Control(pDX, IDC_EDIT_EDTBOX, m_edtPath);
    DDX_Control(pDX, IDC_LIST_COLUMES, m_listColumes);
    DDX_Text(pDX, IDC_EDIT_NOTEPADD_PATH, m_notePadPath);
    DDV_MaxChars(pDX, m_notePadPath, 128);
    DDX_Text(pDX, IDC_EDIT_TRACE_EXAMPLE, m_strTraceExample);
    DDX_Control(pDX, IDC_LIST_COMPONENT, m_listComponent);
    DDX_Control(pDX, IDC_MFCMENUBUTTON_ACTION, m_menuButton);
    DDX_Text(pDX, IDC_EDIT_REGULAR_EXPRESSION, m_strRegularExpression);
    DDX_Control(pDX, IDC_COMBO_IDENTIFIER, m_comboIdentifier);
    DDX_Check(pDX, IDC_CHECK_IDENTIFIER, m_bCheckIdentifier);
    DDX_Control(pDX, IDC_COMBO_UI_SETTINGS, m_uiSettings);
    DDX_Check(pDX, IDC_CHECK_PYTHON, m_bCheckPython);
    DDX_Text(pDX, IDC_EDIT_FUN_NAME, m_strFunctionName);
    DDX_Control(pDX, IDC_LIST_PARAMETER, m_listParameter);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialogEx)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PATH, &CSettingsDlg::OnDblclkListPath)
    ON_EN_KILLFOCUS(IDC_EDIT_EDTBOX, &CSettingsDlg::OnKillfocusEditEdtbox)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CSettingsDlg::OnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_CHECK_SAVE_TRACES, &CSettingsDlg::OnClickedCheckSaveTraces)
    ON_BN_CLICKED(IDC_BUTTON_ADD, &CSettingsDlg::OnBnClickedButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CSettingsDlg::OnBnClickedButtonRemove)
    ON_BN_CLICKED(IDC_BUTTON_PARSE_RE, &CSettingsDlg::OnBnClickedButtonParseRe)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_COLUMES, &CSettingsDlg::OnDblclkListColumes)
    ON_BN_CLICKED(IDC_BUTTON_ADD_COMPONENT, &CSettingsDlg::OnBnClickedButtonAddComponent)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE_DELETE_COMPONENT, &CSettingsDlg::OnBnClickedButtonRemoveDeleteComponent)
	ON_BN_CLICKED(IDC_BUTTON_ADD_COMPONENT_CHECK, &CSettingsDlg::OnBnClickedButtonAddComponentCheck)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SOURCE_PATH_CHECK, &CSettingsDlg::OnBnClickedButtonAddSourcePathCheck)
    ON_BN_CLICKED(ID_BUTTON_OK, &CSettingsDlg::OnBnClickedButtonOk)
    ON_BN_CLICKED(IDC_MFCMENUBUTTON_ACTION, &CSettingsDlg::OnBnClickedMfcmenubuttonAction)
    ON_BN_CLICKED(IDC_CHECK_IDENTIFIER, &CSettingsDlg::OnClickedCheckIdentifier)
    ON_CBN_SELCHANGE(IDC_COMBO_UI_SETTINGS, &CSettingsDlg::OnSelchangeComboSettings)
    ON_BN_CLICKED(IDC_BUTTON_PYTHON_EDIT, &CSettingsDlg::OnBnClickedEditPythonCode)
    ON_BN_CLICKED(IDC_CHECK_PYTHON, &CSettingsDlg::OnClickedCheckPython)
END_MESSAGE_MAP()


// CSettingsDlg message handlers


BOOL CSettingsDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    //Init list control
    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Enable";
    lv.cx = 50;
    m_pathList.InsertColumn(0, &lv);
    lv.cx = 100;
    lv.pszText = "Column Index";
    m_pathList.InsertColumn(1, &lv);
    lv.cx = 100;
    lv.pszText = "Contain what";
    m_pathList.InsertColumn(2, &lv);
    lv.cx = 300;
    lv.pszText = "Path";
    m_pathList.InsertColumn(3, &lv);
    m_pathList.SetExtendedStyle(m_pathList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);


    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Enable";
    lv.cx = 50;
    m_listColumes.InsertColumn(0, &lv);
    lv.pszText = "Index";
    lv.cx = 40;
    m_listColumes.InsertColumn(1, &lv);
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Parse results";
    lv.cx = 280;
    m_listColumes.InsertColumn(2, &lv);
    lv.cx = 100;
    lv.pszText = "Column Name";
    m_listColumes.InsertColumn(3, &lv);
    lv.cx = 80;
    lv.pszText = "Color";
    m_listColumes.InsertColumn(4, &lv);
    m_listColumes.SetExtendedStyle(m_listColumes.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

    lv.cx = 50;
    lv.pszText = "Enable";
    m_listComponent.InsertColumn(0, &lv);
	lv.cx = 100;
	lv.pszText = "Column Index";
	m_listComponent.InsertColumn(1, &lv);
    lv.cx = 150;
    lv.pszText = "Component Name";
    m_listComponent.InsertColumn(2, &lv);
    lv.cx = 250;
    lv.pszText = "Contain what";
    m_listComponent.InsertColumn(3, &lv);
    m_listComponent.SetExtendedStyle(m_listComponent.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
    //Edit box
    m_edtPath.ShowWindow(SW_HIDE);

    //Set menu
    m_menu.LoadMenuA(IDR_MENU_ACTION);
    m_menuButton.m_hMenu = m_menu.GetSubMenu(0)->GetSafeHmenu();
    m_menuButton.SizeToContent();
    m_menuButton.m_bOSMenu = FALSE;

    //m_cmboRegularExpression.AddString("(\\S*) (\\S*)(\\s+)(T[0-9]+) (.) (.*-[0-9]{3}) (.{4}) ([0-9\\.]+) (.*)");

    /*
    if (!m_pReadLog.isNull())
    {
        m_notePadPath = m_pReadLog->GetNotePadPath().c_str();
        m_bSaveTraces = m_pReadLog->GetEnableSaveTraces();
        m_strSaveTracePath = m_pReadLog->GetSaveTracePath().c_str();
        m_bCheckHeaderFile = m_pReadLog->GetCheckHeaderFile();
        if (m_bSaveTraces)
            GetDlgItem(IDC_EDIT_SAVE_TRACE_PATH)->EnableWindow(TRUE);
        else
            GetDlgItem(IDC_EDIT_SAVE_TRACE_PATH)->EnableWindow(FALSE);
        UpdateData(FALSE);
    }
    */
    
    m_bPathListSelected = TRUE;

    m_uiSettings.Clear();
    for (int i = 0; i < m_uiSettingArray.size(); ++i)
        m_uiSettings.AddString(m_uiSettingArray.at(i)->getName().c_str());

    m_uiSettings.SetCurSel(0);
    InitializeData(0);

    return TRUE;
}


void CSettingsDlg::OnDblclkListPath(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    if (pNMItemActivate->iSubItem == 3)
    {
        Poco::SharedPtr<CEdit> pEdit = new CEdit();
        //
        m_nRow = pNMItemActivate->iItem;
        m_nCol = pNMItemActivate->iSubItem;
        CRect cRectItem;
        CRect cRectList;
        m_pathList.GetSubItemRect( pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_LABEL, cRectItem ); 
        m_pathList.GetWindowRect(&cRectList);
        ScreenToClient(&cRectList);
        cRectItem.left   += cRectList.left + 2;
        cRectItem.top    += cRectList.top;
        cRectItem.right  += cRectList.left + 3;
        cRectItem.bottom += cRectList.top + 2;

        TCHAR ach[512] = {0};
        m_pathList.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, ach, 512);
        m_edtPath.SetWindowTextA(ach);
        m_edtPath.ShowWindow(SW_SHOW);
        m_edtPath.MoveWindow(&cRectItem); 
        m_edtPath.SetFocus();
        m_edtPath.CreateSolidCaret(1, cRectItem.Height() - 5);
        m_edtPath.ShowCaret(); 
        m_edtPath.SetSel( -1 );
        m_edtPath.SetSel(0, strlen(ach));
        m_strOriginalText = ach;
        m_bPathListSelected = TRUE;
    }
    
    *pResult = 0;
}


void CSettingsDlg::OnKillfocusEditEdtbox()
{
    if (m_edtPath.IsWindowVisible())
    {
        CString cstrText;
        m_edtPath.GetWindowText(cstrText);
        if (cstrText.IsEmpty())
            cstrText = m_strOriginalText;

        if (m_bPathListSelected)
            m_pathList.SetItemText(m_nRow, m_nCol, cstrText);
        else
            m_listColumes.SetItemText(m_nRow, m_nCol, cstrText);
        m_edtPath.ShowWindow(SW_HIDE);
    }
}

void CSettingsDlg::InitializeData(int nSel)
{
    if (nSel >= m_uiSettingArray.size())
        return;

    Poco::SharedPtr<CUISetting> uiSetting = m_uiSettingArray[nSel];
    if (!uiSetting.isNull())
    {
        m_strFunctionName.Empty();
        if (m_listParameter.GetCount() > 0)
            while (m_listParameter.DeleteString(0));
        //m_strSettingName = uiSetting->getName().c_str();
        m_strTraceExample = uiSetting->GetTraceExample().c_str();
        m_strRegularExpression = uiSetting->GetRegularExpression().c_str();
        m_pythonSourceCodeArray.clear();
        UpdateData(FALSE);
        //Do 'Parse' action
        OnBnClickedButtonParseRe();
        //Column
        const std::vector<Poco::SharedPtr<CColumnSetting>>& columns = uiSetting->GetAllColumns();
        if (columns.size() == m_listColumes.GetItemCount())
        {
            for (int i = 0; i < columns.size(); ++i)
            {
                m_listColumes.SetCheck(i, columns.at(i)->m_bEnable);
                m_listColumes.SetItemText(i, 3, columns.at(i)->m_strRealColumnName.c_str());
                if (columns.at(i)->m_columnColorArray.size() > 0)
                {
                    char buff[32];
                    std::map<tstring, COLORREF> colorMap;
                    for (int j = 0; j < columns.at(i)->m_columnColorArray.size(); ++j)
                        colorMap[columns.at(i)->m_columnColorArray.at(j)->m_strCondition] = columns.at(i)->m_columnColorArray.at(j)->m_nColor;
                    m_colorMap[i] = colorMap;
                    sprintf(buff, "%d Conditions", columns.at(i)->m_columnColorArray.size());
                    m_listColumes.SetItemText(i, 4, buff);
                }
            }
        }
        //Path
        const std::vector<Poco::SharedPtr<CSourceCoePath>>& pathes = uiSetting->GetAllSourceCodePath();
        for (int i = 0; i < pathes.size(); ++i)
        {
            char buff[32];
            m_pathList.InsertItem(i, "");
            sprintf(buff, "%d", pathes.at(i)->m_nColumnIndex);
            m_pathList.SetItemText(i, 1, buff);
            m_pathList.SetItemText(i, 2, pathes.at(i)->m_strCondition.c_str());
            m_pathList.SetItemText(i, 3, pathes.at(i)->m_strSourceCodePath.c_str());
            m_pathList.SetCheck(i, pathes.at(i)->m_bEnable);
        }
        //Component
        const std::vector<Poco::SharedPtr<CVirtualComponent>>& components = uiSetting->GetAllComponent();
        for (int i = 0; i < components.size(); ++i)
        {
            char buff[32];
            m_listComponent.InsertItem(i, "");
            sprintf(buff, "%d", components.at(i)->GetColumnIndex());
            m_listComponent.SetItemText(i, 1, buff);
            m_listComponent.SetItemText(i, 2, components.at(i)->GetComponentName().c_str());
            m_listComponent.SetItemText(i, 3, components.at(i)->GetCondition().c_str());
            m_listComponent.SetCheck(i, components.at(i)->GetEnable());
        }

        m_notePadPath = uiSetting->GetNodepadPath().c_str();
        if (uiSetting->GetColumnIdentifierEnable())
        {
            m_comboIdentifier.SetCurSel(uiSetting->GetColumnIdentifier());
            m_bCheckIdentifier = TRUE;
            GetDlgItem(IDC_COMBO_IDENTIFIER)->EnableWindow(TRUE);
        }
        else
        {
            m_bCheckIdentifier = FALSE;
            GetDlgItem(IDC_COMBO_IDENTIFIER)->EnableWindow(FALSE);
        }

        if (uiSetting->GetPythonModuleFlag())
        {
            m_bCheckPython = TRUE;
            Poco::SharedPtr<CPythonModule> pythonModule = uiSetting->GetPythonModule();
            if (!pythonModule.isNull())
            {
                char buff[32];
                //Function Name
                m_strFunctionName = pythonModule->GetFunctionName().c_str();
                //Column arguments
                if (m_listParameter.GetCount() > 0)
                    while (m_listParameter.DeleteString(0));
                for (int i = 0; i < pythonModule->GetArgColumns().size(); ++i)
                {
                    sprintf(buff, "%s%d", COLUMN_NAME, pythonModule->GetArgColumns().at(i));
                    m_listParameter.AddString(buff);
                }
                m_pythonSourceCodeArray.clear();
                for (int i = 0; i < pythonModule->GetSourceCode().size(); ++i)
                {
                    CString strItem = pythonModule->GetSourceCode().at(i).c_str();
                    strItem += "\n";
                    m_pythonSourceCodeArray.push_back(strItem);
                }
            }
            GetDlgItem(IDC_BUTTON_PYTHON_EDIT)->EnableWindow(TRUE);
        }
        else
        {
            m_bCheckPython = FALSE;
            GetDlgItem(IDC_BUTTON_PYTHON_EDIT)->EnableWindow(FALSE);
        }

        m_uiSettings.SetCurSel(m_uiSettings.FindString(0, uiSetting->getName().c_str()));
        m_uiSetting = uiSetting;

        UpdateData(FALSE);
    }
}

void CSettingsDlg::OnClickedButtonBrowse()
{
    CString strFile = _T("");
    CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.exe)|*.exe|All Files (*.*)|*.*||"), NULL);

    if (dlgFile.DoModal() == IDOK)
    {
        m_notePadPath = dlgFile.GetPathName();
        UpdateData(FALSE);
    }
}


void CSettingsDlg::OnClickedCheckSaveTraces()
{
    UpdateData(TRUE);
    if (m_notePadPath.IsEmpty())
    {
        ERROR_MESSAGEBOX("The NotePad++ Path cannot be empty.");
        return;
    }
}

void CSettingsDlg::OnBnClickedButtonAdd()
{
	std::vector<CString> columns;
	for (int i = 0; i < m_listColumes.GetItemCount(); ++i)
	{
		if (m_listColumes.GetCheck(i) == TRUE)
		{
			CString strItem;
			strItem.Format("%s:%s", m_listColumes.GetItemText(i, 1), m_listColumes.GetItemText(i, 3));
			columns.push_back(strItem);
		}
	}

    CNewSourcePathDlg dlg(columns);
    if (dlg.DoModal() == IDOK)
    {
        int nLine = m_pathList.GetItemCount();
        m_pathList.InsertItem(nLine, "");
        m_pathList.SetItemText(nLine, 1, dlg.GetSelectedColumnIndex());
        m_pathList.SetItemText(nLine, 2, dlg.GetContianWhat());
        m_pathList.SetItemText(nLine, 3, dlg.GetSourceCodePath());
        m_pathList.SetCheck(nLine, dlg.GetEnable());
    }
}

void CSettingsDlg::OnBnClickedButtonRemove()
{
    POSITION pos = m_pathList.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        ERROR_MESSAGEBOX("Please select items you want to remove");
        return;
    }
    std::vector<int> selArray;
    while (pos != NULL)
    {
        int nSel = m_pathList.GetNextSelectedItem(pos);
        selArray.push_back(nSel);
    }
    std::vector<int>::reverse_iterator ite = selArray.rbegin();
    while (ite != selArray.rend())
    {
        m_pathList.DeleteItem(*ite);
        ++ite;
    }
}

void CSettingsDlg::OnBnClickedButtonParseRe()
{
    UpdateData(TRUE);
    if (m_strTraceExample.IsEmpty())
    {
        ERROR_MESSAGEBOX("The Trace Example cannot be empty.");
        return;
    }

    if (m_strRegularExpression.IsEmpty())
    {
		ERROR_MESSAGEBOX("The Regular expression cannot be empty.");
        return;
    }
    //Remove the data about column identifier
    if (m_comboIdentifier.GetCount() > 0)
        while (m_comboIdentifier.DeleteString(0));

    tstring strTraceExample, strRegularExpression;
    strTraceExample = m_strTraceExample.GetBuffer(m_strTraceExample.GetLength());
    strRegularExpression = m_strRegularExpression.GetBuffer(m_strRegularExpression.GetLength());

    Poco::RegularExpression re(strRegularExpression);
    std::vector<tstring> stringAry;
    re.split(strTraceExample, stringAry);
    m_listColumes.DeleteAllItems();
    m_colorMap.clear();
    int nIndex = 0;
    for (int i = 1; i < stringAry.size(); ++i)
    {
        if (!Poco::trim(stringAry.at(i)).empty())
        {
            char buffer[32];
            m_listColumes.InsertItem(nIndex, "");
            m_listColumes.SetItemText(nIndex, 1, itoa(nIndex, buffer, 10));
            m_listColumes.SetItemText(nIndex, 2, stringAry.at(i).c_str());
            sprintf(buffer, "Column%d", nIndex);
            m_listColumes.SetItemText(nIndex, 3, buffer);
            m_listColumes.SetCheck(nIndex, TRUE);
            m_comboIdentifier.AddString(buffer);
            ++nIndex;
        }
    }
}


void CSettingsDlg::OnDblclkListColumes(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    if (pNMItemActivate->iSubItem == 3 && pNMItemActivate->iItem >= 0)
    {
        Poco::SharedPtr<CEdit> pEdit = new CEdit();
        m_nRow = pNMItemActivate->iItem;
        m_nCol = pNMItemActivate->iSubItem;
        CRect cRectItem;
        CRect cRectList;
        m_listColumes.GetSubItemRect( pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_LABEL, cRectItem ); 
        m_listColumes.GetWindowRect(&cRectList);
        ScreenToClient(&cRectList);
        cRectItem.left   += cRectList.left + 2;
        cRectItem.top    += cRectList.top;
        cRectItem.right  += cRectList.left + 3;
        cRectItem.bottom += cRectList.top + 2;

        TCHAR ach[512] = {0};
        m_listColumes.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, ach, 512);
        m_edtPath.SetWindowTextA(ach);
        m_edtPath.ShowWindow(SW_SHOW);
        m_edtPath.MoveWindow(&cRectItem); 
        m_edtPath.SetFocus();
        m_edtPath.CreateSolidCaret(1, cRectItem.Height() - 5);
        m_edtPath.ShowCaret(); 
        m_edtPath.SetSel( -1 );
        
        m_bPathListSelected = FALSE;
    }
    //Select color
    else if (pNMItemActivate->iSubItem == 4 && pNMItemActivate->iItem > 0)
    {
        CChooseColorDlg dlg;
        if (m_colorMap.find(pNMItemActivate->iItem) != m_colorMap.end())
            dlg.SetColorMap(m_colorMap[pNMItemActivate->iItem]);

        if (dlg.DoModal() == IDOK)
        {
            char buff[32];
            std::map<tstring, COLORREF> colorMap = dlg.GetColorMap();
            m_colorMap[pNMItemActivate->iItem] = colorMap;
            if (colorMap.size() > 0)
            {
                sprintf(buff, "%d Conditions", colorMap.size());
                m_listColumes.SetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, buff);
            }
            else
                m_listColumes.SetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem, "");
        }
    }

    *pResult = 0;
}

void CSettingsDlg::OnBnClickedButtonAddComponent()
{
	std::vector<CString> columns;
	for (int i = 0; i < m_listColumes.GetItemCount(); ++i)
	{
		if (m_listColumes.GetCheck(i) == TRUE)
		{
			CString strItem;
			strItem.Format("%s:%s", m_listColumes.GetItemText(i, 1), m_listColumes.GetItemText(i, 3));
			columns.push_back(strItem);
		}
	}

	CNewComponentDlg dlg(columns);
	if (dlg.DoModal() == IDOK)
    {
        int nIndex = m_listComponent.GetItemCount();
        m_listComponent.InsertItem(nIndex, "");
        m_listComponent.SetItemText(nIndex, 1, dlg.GetSelectedColumn());
        m_listComponent.SetItemText(nIndex, 2, dlg.GetComponentName());
        m_listComponent.SetItemText(nIndex, 3, dlg.GetCondition());
        m_listComponent.SetCheck(nIndex, dlg.GetEnable());
    }
}

void CSettingsDlg::OnBnClickedButtonRemoveDeleteComponent()
{
	POSITION pos = m_listComponent.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		ERROR_MESSAGEBOX("Please select items you want to remove.");
		return;
	}
	std::vector<int> selArray;
	while (pos != NULL)
	{
		int nSel = m_listComponent.GetNextSelectedItem(pos);
		selArray.push_back(nSel);
	}
	std::vector<int>::reverse_iterator ite = selArray.rbegin();
	while (ite != selArray.rend())
	{
		m_listComponent.DeleteItem(*ite);
		++ite;
	}
}


void CSettingsDlg::OnBnClickedButtonAddComponentCheck()
{
	CString strText;
	GetDlgItem(IDC_BUTTON_ADD_COMPONENT_CHECK)->GetWindowTextA(strText);
	if (strText == "C")
	{
		GetDlgItem(IDC_BUTTON_ADD_COMPONENT_CHECK)->SetWindowTextA("U");
		for (int i = 0; i < m_listComponent.GetItemCount(); ++i)
			m_listComponent.SetCheck(i, FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADD_COMPONENT_CHECK)->SetWindowTextA("C");
		for (int i = 0; i < m_listComponent.GetItemCount(); ++i)
			m_listComponent.SetCheck(i, TRUE);
	}
}


void CSettingsDlg::OnBnClickedButtonAddSourcePathCheck()
{
	CString strText;
	GetDlgItem(IDC_BUTTON_ADD_SOURCE_PATH_CHECK)->GetWindowTextA(strText);
	if (strText == "C")
	{
		GetDlgItem(IDC_BUTTON_ADD_SOURCE_PATH_CHECK)->SetWindowTextA("U");
		for (int i = 0; i < m_pathList.GetItemCount(); ++i)
			m_pathList.SetCheck(i, TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADD_SOURCE_PATH_CHECK)->SetWindowTextA("C");
		for (int i = 0; i < m_pathList.GetItemCount(); ++i)
			m_pathList.SetCheck(i, FALSE);
	}
}

void CSettingsDlg::OnBnClickedButtonOk()
{
    if (SaveData())
        CDialogEx::OnOK();
}

BOOL CSettingsDlg::SaveData()
{
    //Save Item
    UpdateData(TRUE);
    CString strName;
    if (m_uiSettings.GetCurSel() >= 0)
        m_uiSettings.GetLBText(m_uiSettings.GetCurSel(), strName);
    else
        m_uiSettings.GetWindowText(strName);

    if (strName.IsEmpty())
    {
        ERROR_MESSAGEBOX("The setting name cannot be empty.");
        return FALSE;
    }

    if (m_strTraceExample.IsEmpty())
    {
        ERROR_MESSAGEBOX("The trace example cannot be empty.");
        return FALSE;
    }

    if (m_strRegularExpression.IsEmpty())
    {
        ERROR_MESSAGEBOX("The regular expression cannot be empty.");
        return FALSE;
    }

    //if (!m_pReadLog->GetFileName().empty() && AfxMessageBox("The setting has been changed, you need to parse the file again, Do you want to continue?", MB_OKCANCEL) == IDCANCEL)
    //    return FALSE;

    int nSel = m_uiSettings.GetCurSel();
    Poco::SharedPtr<CUISetting> uiSetting = m_uiSettingArray.at(nSel);
    if (uiSetting.isNull())
        uiSetting = new CUISetting();

    if (!uiSetting.isNull())
    {
        m_bModified = TRUE;
        uiSetting->SetModifyFlag(TRUE);
        //
        uiSetting->SetName(strName.GetBuffer(strName.GetLength()));
        uiSetting->SetTraceExample(m_strTraceExample.GetBuffer(m_strTraceExample.GetLength()));
        uiSetting->SetRegularExpression(m_strRegularExpression.GetBuffer(m_strRegularExpression.GetLength()));

        //Column
        uiSetting->ClearAllColumns();
        CString strItem;
        for (int i = 0; i < m_listColumes.GetItemCount(); ++i)
        {
            Poco::SharedPtr<CColumnSetting> column = new CColumnSetting;
            column->m_bEnable = m_listColumes.GetCheck(i);
            strItem = m_listColumes.GetItemText(i, 3);
            column->m_strRealColumnName = strItem.GetBuffer(strItem.GetLength());
            column->m_nColumnIndex = i;

            std::map<int, std::map<tstring, COLORREF>>::iterator ite = m_colorMap.find(i);
            if (ite != m_colorMap.end())
            {
                if (ite->second.size() > 0)
                {
                    std::map<tstring, COLORREF>::iterator subIte = ite->second.begin();
                    while (subIte != ite->second.end())
                    {
                        Poco::SharedPtr<CColumnColor> color = new CColumnColor;
                        color->m_nColor = subIte->second;
                        color->m_strCondition = subIte->first;
                        column->m_columnColorArray.push_back(color);
                        ++subIte;
                    }
                }
            }
            uiSetting->InsertColumn(column);
        }
        //Component
        uiSetting->ClearAllComponent();
        for (int i = 0; i < m_listComponent.GetItemCount(); ++i)
        {   
            BOOL bEnable = m_listComponent.GetCheck(i);
            strItem = m_listComponent.GetItemText(i, 1);
            int nColumnIndex = Poco::NumberParser::parse(strItem.GetBuffer(strItem.GetLength()));

            strItem = m_listComponent.GetItemText(i, 2);
            tstring strComponent = strItem.GetBuffer(strItem.GetLength());

            strItem = m_listComponent.GetItemText(i, 3);
            tstring strCondition = strItem.GetBuffer(strItem.GetLength());

            Poco::SharedPtr<CVirtualComponent> component = new CVirtualComponent(strComponent, nColumnIndex, strCondition, bEnable);
            uiSetting->InsertComponent(component);
        }
        //Path
        uiSetting->ClearAllSourcePath();
        for (int i = 0; i < m_pathList.GetItemCount(); ++i)
        {
            Poco::SharedPtr<CSourceCoePath> sourcePath = new CSourceCoePath;
            sourcePath->m_bEnable = m_pathList.GetCheck(i);
            sourcePath->m_nColumnIndex = 1;

            strItem = m_pathList.GetItemText(i, 2);
            sourcePath->m_strCondition = strItem.GetBuffer(strItem.GetLength());

            strItem = m_pathList.GetItemText(i, 3);
            sourcePath->m_strSourceCodePath = strItem.GetBuffer(strItem.GetLength());

            uiSetting->InsertSourcePath(sourcePath);
        }
        uiSetting->SetColumnIdentifierEnable(m_bCheckIdentifier);

        if (m_bCheckIdentifier)
            uiSetting->SetColumnIdentifier(m_comboIdentifier.GetCurSel());

        uiSetting->SetPythonModuleFlag(m_bPathListSelected);
        if (m_bPathListSelected)
        {
            Poco::SharedPtr<CPythonModule> pythonModule = uiSetting->GetPythonModule();
            if (!pythonModule.isNull())
            {
                //Name
                pythonModule->SetFunctionName(m_strFunctionName.GetBuffer(m_strFunctionName.GetLength()));
                //Column
                CString strItem;
                std::vector<int> columnParameter;
                for (int i = 0; i < m_listParameter.GetCount(); ++i)
                {
                    m_listParameter.GetText(i, strItem);
                    CString strNumber = strItem.Mid(strlen(COLUMN_NAME), strItem.GetLength() - strlen(COLUMN_NAME));
                    int nValue = Poco::NumberParser::parse(strNumber.GetBuffer(strNumber.GetLength()));
                    columnParameter.push_back(nValue);
                }
                pythonModule->SetArgColumns(columnParameter);
                //Python code
                pythonModule->ClearSourceCode();
                char ch1 = 13;
                char ch2 = 10;
                for (int i = 0; i < m_pythonSourceCodeArray.size(); ++i)
                {
                    CString strLine = m_pythonSourceCodeArray.at(i);
                    strLine.Remove(ch1);
                    strLine.Remove(ch2);
                    
                    pythonModule->InsertSourceCodeLine(strLine.GetBuffer(strLine.GetLength()));
                }
            }
        }

        uiSetting->SetNodepadPath(m_notePadPath.GetBuffer(m_notePadPath.GetLength()));
        uiSetting->SaveSetting();

        //If this is a new setting
        if (m_uiSettings.FindString(0, strName) < 0)
            m_uiSettingArray.push_back(uiSetting);
    }
    return TRUE;
}


void CSettingsDlg::OnBnClickedMfcmenubuttonAction()
{
    if (m_menuButton.m_nMenuResult == ID_ACTION_OPEN)
    {
        if (m_uiSetting->IsModify())
        {
            if (AfxMessageBox("The setting has been changed, you need to parse the file again, Do you want to continue?", MB_OKCANCEL) == IDOK)
                m_uiSetting->SaveSetting();

        }

        CString strFile = _T("");
        CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.conf)|*.conf|All Files (*.*)|*.*||"), NULL);

        if (dlgFile.DoModal() == IDOK)
        {
            m_notePadPath = dlgFile.GetPathName();
            UpdateData(FALSE);
            try
            {
                Poco::SharedPtr<CUISetting> uiSetting = new CUISetting(m_notePadPath.GetBuffer(m_notePadPath.GetLength()));
                /*
                m_pReadLog->InsertUISetting(uiSetting);
                m_pReadLog->SetActiveUISetting(uiSetting);
                m_listColumes.DeleteAllItems();
                m_listComponent.DeleteAllItems();
                m_pathList.DeleteAllItems();
                m_colorMap.clear();
                m_bCheckIdentifier = FALSE;
                m_comboIdentifier.SetCurSel(0);
                InitializeData();
                */
            }
            catch(const std::exception& e)
            {
                ERROR_MESSAGEBOX(e.what());
            }
        }
    }
    else if (m_menuButton.m_nMenuResult == ID_ACTION_NEW)
    {
        if (m_uiSetting->IsModify())
        {
            if (AfxMessageBox("The setting has been changed, you need to parse the file again, Do you want to continue?", MB_OKCANCEL) == IDOK)
                m_uiSetting->SaveSetting();

        }
        m_listColumes.DeleteAllItems();
        m_listComponent.DeleteAllItems();
        m_pathList.DeleteAllItems();
        m_strRegularExpression.Empty();
        m_strTraceExample.Empty();
        //m_strSettingName.Empty();
        m_uiSettings.SetWindowTextA("");
        m_notePadPath.Empty();
        m_bCheckIdentifier = FALSE;
        UpdateData(FALSE);
        GetDlgItem(IDC_EDIT_SETTING_NAME)->SetFocus();
    }
}


BOOL CSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )     
    {
        switch(pMsg->wParam)  
        {  
        case VK_RETURN:
            {
                if (m_edtPath.IsWindowVisible())
                    OnKillfocusEditEdtbox();
            }
            return true;
        }

    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSettingsDlg::OnClickedCheckIdentifier()
{
    UpdateData(TRUE);
    if (m_bCheckIdentifier)
        GetDlgItem(IDC_COMBO_IDENTIFIER)->EnableWindow(TRUE);
    else
        GetDlgItem(IDC_COMBO_IDENTIFIER)->EnableWindow(FALSE);
}

void CSettingsDlg::OnSelchangeComboSettings()
{
    if (m_uiSetting->IsModify() && AfxMessageBox("Do you want to save the current UI setting before switch to another one?", MB_OKCANCEL) == IDOK)
    {
        m_uiSetting->SetModifyFlag(TRUE);
        m_uiSetting->SaveSetting();
    }
    m_bModified = TRUE;
    CString strName;
    m_uiSettings.GetLBText(m_uiSettings.GetCurSel(), strName);
    if (!strName.IsEmpty())
    {
        tstring strSettingName = strName.GetBuffer(strName.GetLength());
        for (int i = 0; i < m_uiSettingArray.size(); ++i)
        {
            if (strSettingName == m_uiSettingArray.at(i)->getName())
            {
                m_listColumes.DeleteAllItems();
                m_listComponent.DeleteAllItems();
                m_pathList.DeleteAllItems();
                m_strRegularExpression.Empty();
                m_strTraceExample.Empty();
                m_notePadPath.Empty();
                m_bCheckIdentifier = FALSE;
                UpdateData(FALSE);
                InitializeData(i);
            }
        }
    }
}

void CSettingsDlg::OnBnClickedEditPythonCode()
{
    UpdateData(TRUE);
    std::vector<CString> columns;
    std::vector<int> enableColumns;
    char buff[32];
    for (int i = 0; i < m_listColumes.GetItemCount(); ++i)
    {
        sprintf(buff, "%s%d", COLUMN_NAME, i);
        if (m_listParameter.FindString(0, buff) >=0 )
            enableColumns.push_back(i);
        columns.push_back(m_listColumes.GetItemText(i, 2));
    }

    //Get path list
    std::vector<CString> pathList;
    for (int i = 0; i < m_pathList.GetItemCount(); ++i)
    {
        CString strItem = m_pathList.GetItemText(i, 3);
        pathList.push_back(strItem);
    }

    if (pathList.size() > 0)
    {
        CPythonMethodEditDlg dlg(m_strFunctionName, columns, m_pythonSourceCodeArray, enableColumns, pathList);
        if (dlg.DoModal() == IDOK)
        {
            m_strFunctionName = dlg.GetFunctionName();
            m_pythonSourceCodeArray = dlg.GetSourceCode();
            if (m_listParameter.GetCount() > 0)
                while (m_listParameter.DeleteString(0));
            for (int i = 0; i < dlg.GetEnableColumns().size(); ++i)
            {
                sprintf(buff, "%s%d", COLUMN_NAME, dlg.GetEnableColumns().at(i));
                m_listParameter.AddString(buff);
            }
            UpdateData(FALSE);
        }
    }
    else
        ERROR_MESSAGEBOX("The Path list cannot be empty.");
}


void CSettingsDlg::OnClickedCheckPython()
{
    UpdateData(TRUE);
    if (m_bCheckPython)
        GetDlgItem(IDC_BUTTON_PYTHON_EDIT)->EnableWindow(TRUE);
    else
        GetDlgItem(IDC_BUTTON_PYTHON_EDIT)->EnableWindow(FALSE);
}
