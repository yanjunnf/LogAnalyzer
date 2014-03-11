
// LogAnalyzerView.h : interface of the CLogAnalyzerView class
//

#pragma once

#include "resource.h"
#include "AsyncListCtrl.h"
#include "ResizableFormView.h"
#include "Poco\SharedPtr.h"

class CReadLog;

class CLogAnalyzerView : public CResizableFormView
{
protected: // create from serialization only
	CLogAnalyzerView();
	DECLARE_DYNCREATE(CLogAnalyzerView)

public:
	enum{ IDD = IDD_LOGANALYZER_FORM };
	CLogAnalyzerDoc* GetDocument() const;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual ~CLogAnalyzerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    Poco::SharedPtr<CReadLog> m_logger;
    CAsyncListCtrl m_traceList;
    CComboBox m_comboColumns;
    CComboBox m_comboSettings;
    CString m_strSelectedMsg;
    CString m_strCurrentPage;
    //The current selected component
    CString m_strSelComponent;
    CString m_strFindContent;
    CString m_strTraceCount;
    BOOL m_bMatchCase;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnInitialUpdate();

	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

    COLORREF GetColor(int nItem, int nColumn);
    void ShowTraces(const CString& strComponent, BOOL bShowAll);
    void EnableControls();
    void RemoveItem();

public:
    afx_msg void OnBnClickedButtonNextPage();
    afx_msg void OnBnClickedButtonOpen();
    afx_msg void OnCustomdrawListTrace(NMHDR *pNMHDR, LRESULT *pResult);

    void ShowComponent(const CString& strComponent);
    void UpdateUISetting();

    afx_msg void OnBnClickedButtonFirstPage();
    afx_msg void OnBnClickedButtonPreviousPage();
    afx_msg void OnBnClickedButtonLastPage();
    afx_msg void OnClickListTraceIds(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg void OnToolGeneratereportEXCEL();
    afx_msg void OnEditOpensourcefile();
    afx_msg void OnEditLocatelogtrace();
    afx_msg void OnUpdateEditOpensourcefile(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditLocatelogtrace(CCmdUI *pCmdUI);
    afx_msg void OnBnClickedButtonFind();
    afx_msg void OnGeneratereportCsv();
    afx_msg void OnGeneratereportPdf();
    afx_msg void OnGeneratereportExcel();
    afx_msg void OnUpdateGeneratereportExcel(CCmdUI *pCmdUI);
};

