
#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputSummary.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputWarning.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputHistory.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_SUMMARY_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputSummary, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_WARNING_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputWarning, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_HISTORY_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputHistory, strTabName, (UINT)2);

	// Fill output tabs with some dummy text (nothing magic here)
	FillSummaryWindow();
	FillWarningWindow();
	FillHistoryWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillSummaryWindow()
{
	//m_wndOutputSummary.AddString(_T("Build output is being displayed here."));
	//m_wndOutputSummary.AddString(_T("The output is being displayed in rows of a list view"));
	//m_wndOutputSummary.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::FillWarningWindow()
{
	//m_wndOutputWarning.AddString(_T("Debug output is being displayed here."));
	//m_wndOutputWarning.AddString(_T("The output is being displayed in rows of a list view"));
	//m_wndOutputWarning.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::FillHistoryWindow()
{
	//m_wndOutputHistory.AddString(_T("Find output is being displayed here."));
	//m_wndOutputHistory.AddString(_T("The output is being displayed in rows of a list view"));
	//m_wndOutputHistory.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::UpdateFonts()
{
	//m_wndOutputSummary.SetFont(&afxGlobalData.fontRegular);
	//m_wndOutputWarning.SetFont(&afxGlobalData.fontRegular);
	//m_wndOutputHistory.SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("Copy output"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("Clear output"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();
	}
}

void COutputWnd::AddSummary(const CString& strSummary)
{
}

void COutputWnd::AddWarning(const CString& strWarning)
{
}

void COutputWnd::AddHistory(const CString& strHistory)
{
}