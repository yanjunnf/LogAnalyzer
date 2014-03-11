#pragma once

#include "resource.h"
#include <map>
#include "CommonHeader.h"
#include "ReadLog.h"
#include "Tools.h"
#include "Poco\SharedPtr.h"

class CFindUnusedTraceID : public CDialogEx
{
    DECLARE_DYNAMIC(CFindUnusedTraceID)

public:
    CFindUnusedTraceID(Poco::SharedPtr<CReadLog> pReadLog, CWnd* pParent = NULL);   // standard constructor
    virtual ~CFindUnusedTraceID();

    // Dialog Data
    enum { IDD = IDD_DIALOG_FIND_UNUSED_TRACE_ID };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

protected:
    virtual BOOL OnInitDialog();
    CListCtrl m_lstTrace;
    CString m_strMessageFile;
    CComboBox m_filterList;
    CMenu m_popupMenu;
    Poco::SharedPtr<CReadLog> m_pReadLog;
    //Key:messageID
    //Value:source file path
    std::map<tstring, tstring> m_messageIDMap;
    CComboBox m_comboSourcePath;

public:
    afx_msg void OnBnClickedButtonOpenMessageFile();
    afx_msg void OnBnClickedButtonOpenSourceCodeFolder();
    afx_msg void OnBnClickedBtnSearch();
    afx_msg void OnQuickOpensourcefileForSpecTraceId();
    afx_msg void OnRclickListFindUnusedIdResult(NMHDR *pNMHDR, LRESULT *pResult);
};
