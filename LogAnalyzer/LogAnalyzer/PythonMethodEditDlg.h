#pragma once
#include "resource.h"
#include "PythonRichEditor.h"
#include "Settings.h"
#include "Poco\SharedPtr.h"

class CPythonMethodEditDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CPythonMethodEditDlg)

public:
    CPythonMethodEditDlg(const CString& strFunName,
        const std::vector<CString>& columnValues, 
        const std::vector<CString>& sourceCodeArray, 
        const std::vector<int>& enableColumns, 
        std::vector<CString>& pathList,
        CWnd* pParent = NULL);
    virtual ~CPythonMethodEditDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_PYTHON };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

protected:
    CPythonRichEditor m_pythonEditor;
    CString m_strFunName;
    CString m_strResult;
    CListCtrl m_listArugments;
    std::vector<CString> m_sourceCodeArray;
    std::vector<CString> m_columnValues;
    std::vector<CString> m_pathArray;
    std::vector<int> m_enableColumns;
    CComboBox m_pathList;

public:
    CString GetFunctionName()const{return m_strFunName;}
    const std::vector<int>& GetEnableColumns()const{return m_enableColumns;}
    const std::vector<CString>& GetSourceCode()const{return m_sourceCodeArray;}
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButtonValidate();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
