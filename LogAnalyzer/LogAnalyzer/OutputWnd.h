
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList window

class COutputList : public CListBox
{
// Construction
public:
	COutputList();

// Implementation
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// Construction
public:
	COutputWnd();

	void UpdateFonts();

// Attributes
protected:
	CMFCTabCtrl	m_wndTabs;

    COutputList m_wndOutputSummary;
    COutputList m_wndOutputWarning;
    COutputList m_wndOutputHistory;

protected:
	void FillSummaryWindow();
	void FillWarningWindow();
	void FillHistoryWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	virtual ~COutputWnd();
    void AddSummary(const CString& strSummary);
    void AddWarning(const CString& strWarning);
    void AddHistory(const CString& strHistory);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

