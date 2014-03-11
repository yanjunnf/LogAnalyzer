#pragma once
#include "Poco\Sharedptr.h"
class CReadLog;

class CLogAnalyzerDoc : public CDocument
{
protected:
	CLogAnalyzerDoc();
	DECLARE_DYNCREATE(CLogAnalyzerDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CLogAnalyzerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    void SetReadLog(Poco::SharedPtr<CReadLog> readLog);
    void SetDocIdentifier(const CString& strIdentifier);
    CString GetDocIdentifier()const {return m_strIdentifier;}
    Poco::SharedPtr<CReadLog> CLogAnalyzerDoc::GetReadLog()const;

protected:
    Poco::SharedPtr<CReadLog> m_logger;
    CString m_strIdentifier;

protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
