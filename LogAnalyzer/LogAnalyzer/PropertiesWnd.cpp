#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "LogAnalyzer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define IDC_COMBO_ITEMS 10000+1024

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
    ON_CBN_SELCHANGE(IDC_COMBO_ITEMS, OnSelchangeComboSettings)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    
	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, IDC_COMBO_ITEMS))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

    m_wndObjectCombo.AddString(_T("Log File"));
    m_wndObjectCombo.AddString(_T("Component"));
    m_wndObjectCombo.AddString(_T("Setting"));
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
}

void CPropertiesWnd::OnProperties2()
{
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
    m_wndPropList.SetVSDotNetLook();
    m_wndPropList.MarkModifiedProperties();
    /*
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
    pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
    */
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}

void CPropertiesWnd::UpdateLogFileInformation(const Poco::SharedPtr<SLogFileProperty> logFilePro)
{
    m_logFileProperty = logFilePro;
    CPropertiesWnd::UpdatePropertyPanel();
}

void CPropertiesWnd::UpdateComponentInformation(const Poco::SharedPtr<SComponentProperty> compPro)
{
    m_componentProperty = compPro;
    CPropertiesWnd::UpdatePropertyPanel();
}

void CPropertiesWnd::SetCurrentSelSetting(const Poco::SharedPtr<CUISetting> uiSetting)
{
    m_currentSelUISetting = uiSetting;
}
void CPropertiesWnd::UpdatePropertyPanel()
{
    int nSel = m_wndObjectCombo.GetCurSel();
    m_wndPropList.RemoveAll();
    //Log File
    if (nSel == 0)
    {
        if (!m_logFileProperty.isNull())
        {
            CMFCPropertyGridProperty* basicInformation = new CMFCPropertyGridProperty(_T("Basic Information"));
            //File path
            CMFCPropertyGridProperty* pFilePathPro = new CMFCPropertyGridProperty(_T("Path"), 
                m_logFileProperty->m_strFileName.c_str(), _T(""));
            pFilePathPro->AllowEdit(FALSE);
            basicInformation->AddSubItem(pFilePathPro);
            //File Size
            char buff[64];
            sprintf(buff, "%d KB", m_logFileProperty->m_nFileSize/1024);
            CMFCPropertyGridProperty* pFileSizePro = new CMFCPropertyGridProperty(_T("Size"), 
                buff, _T(""));
            pFilePathPro->AllowEdit(FALSE);
            basicInformation->AddSubItem(pFileSizePro);

            m_wndPropList.AddProperty(basicInformation);
        }
    }
    //Component
    else if (nSel == 1)
    {
        if (!m_componentProperty.isNull())
        {
            CMFCPropertyGridProperty* basicInformation = new CMFCPropertyGridProperty(_T("Basic Information"));
            //Component Name
            CMFCPropertyGridProperty* pCompName = new CMFCPropertyGridProperty(_T("Name"), 
                m_componentProperty->m_strComponentName.c_str(), _T(""));
            pCompName->AllowEdit(FALSE);
            basicInformation->AddSubItem(pCompName);
            //Trace Log
            char buff[64];
            sprintf(buff, "%d", m_componentProperty->m_nLogTraceCount);
            CMFCPropertyGridProperty* pCompTraceCount = new CMFCPropertyGridProperty(_T("Trace Count"), 
                buff, _T(""));
            pCompTraceCount->AllowEdit(FALSE);
            basicInformation->AddSubItem(pCompTraceCount);

            m_wndPropList.AddProperty(basicInformation);
        }
    }
    //Setting
    else if (nSel == 2)
    {
        if (!m_currentSelUISetting.isNull())
        {
            CMFCPropertyGridProperty* basicInformation = new CMFCPropertyGridProperty(_T("Basic Information"));
            {
                //Name
                CMFCPropertyGridProperty* proName = new CMFCPropertyGridProperty(_T("Name"), 
                    m_currentSelUISetting->getName().c_str(), _T(""));
                proName->AllowEdit(FALSE);
                basicInformation->AddSubItem(proName);

                //Regular expression
                CMFCPropertyGridProperty* proRE = new CMFCPropertyGridProperty(_T("Regular Expression"), 
                    m_currentSelUISetting->GetRegularExpression().c_str(), _T(""));
                proRE->AllowEdit(FALSE);
                basicInformation->AddSubItem(proRE);
            }
            m_wndPropList.AddProperty(basicInformation);

            //Components
            CMFCPropertyGridProperty* components = new CMFCPropertyGridProperty(_T("Components"));
            {
                const std::vector<Poco::SharedPtr<CVirtualComponent>>& componentDefs = m_currentSelUISetting->GetAllComponent();
                for (int i = 0; i < componentDefs.size(); ++i)
                {
                    CMFCPropertyGridProperty* component = new CMFCPropertyGridProperty(_T("Component"));
                    {
                        //Name
                        CMFCPropertyGridProperty* proName = new CMFCPropertyGridProperty(_T("Name"), 
                            componentDefs.at(i)->GetComponentName().c_str(), 
                            componentDefs.at(i)->GetEnable() ? _T("This component is enabled.") : _T("This compoent is disabled."));
                        proName->AllowEdit(FALSE);
                        component->AddSubItem(proName);
                        //Keyword
                        CMFCPropertyGridProperty* keyWord = new CMFCPropertyGridProperty(_T("Name"), 
                            componentDefs.at(i)->GetCondition().c_str(), 
                            componentDefs.at(i)->GetEnable() ? _T("This component is enabled") : _T("This compoent is disable."));
                        keyWord->AllowEdit(FALSE);
                        component->AddSubItem(keyWord);
                    }
                    components->AddSubItem(component);
                }
            }
            m_wndPropList.AddProperty(components);

            //Paths
            CMFCPropertyGridProperty* pathes = new CMFCPropertyGridProperty(_T("Source Code path"));
            {
                const std::vector<Poco::SharedPtr<CSourceCoePath>>& pathDefs = m_currentSelUISetting->GetAllSourceCodePath();
                for (int i = 0; i < pathDefs.size(); ++i)
                {
                    CMFCPropertyGridProperty* path = new CMFCPropertyGridProperty(_T("Path"));
                    {
                        //Name
                        CMFCPropertyGridProperty* proName = new CMFCPropertyGridProperty(_T("Condition"), 
                            pathDefs.at(i)->m_strCondition.c_str(), 
                            pathDefs.at(i)->m_bEnable ? _T("This path definition is enabled.") : _T("This path defition is disabled."));
                        proName->AllowEdit(FALSE);
                        path->AddSubItem(proName);
                        //Keyword
                        CMFCPropertyGridProperty* keyWord = new CMFCPropertyGridProperty(_T("Path"), 
                            pathDefs.at(i)->m_strSourceCodePath.c_str(), 
                            pathDefs.at(i)->m_bEnable ? _T("This path definition is enabled.") : _T("This path definition is disabled."));
                        keyWord->AllowEdit(FALSE);
                        path->AddSubItem(keyWord);
                    }
                    pathes->AddSubItem(path);
                }
            }
            m_wndPropList.AddProperty(pathes);
        }
    }
}

void CPropertiesWnd::OnSelchangeComboSettings()
{
    UpdatePropertyPanel();
}