#include "stdafx.h"
#ifndef SHARED_HANDLERS
#include "LogAnalyzer.h"
#endif
#include "ReadLog.h"

#include "LogAnalyzerDoc.h"
#include "AnalyseProgressDlg.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLogAnalyzerDoc, CDocument)

BEGIN_MESSAGE_MAP(CLogAnalyzerDoc, CDocument)
END_MESSAGE_MAP()

CLogAnalyzerDoc::CLogAnalyzerDoc() : m_logger(NULL)
{
	m_logger = new CReadLog(0, "");
}

CLogAnalyzerDoc::~CLogAnalyzerDoc()
{
}

BOOL CLogAnalyzerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

void CLogAnalyzerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CLogAnalyzerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CLogAnalyzerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CLogAnalyzerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CLogAnalyzerDoc diagnostics

#ifdef _DEBUG
void CLogAnalyzerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLogAnalyzerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CLogAnalyzerDoc::SetReadLog(Poco::SharedPtr<CReadLog> readLog)
{
    m_logger = readLog;
}

Poco::SharedPtr<CReadLog> CLogAnalyzerDoc::GetReadLog()const
{
    return m_logger;
}

void CLogAnalyzerDoc::SetDocIdentifier(const CString& strIdentifier)
{
    m_strIdentifier = strIdentifier;
}