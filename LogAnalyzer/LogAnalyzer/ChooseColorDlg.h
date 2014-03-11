#pragma once
#include "resource.h"
#include "CommonHeader.h"
#include <map>

class CChooseColorDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CChooseColorDlg)

public:
    CChooseColorDlg(CWnd* pParent = NULL);
    virtual ~CChooseColorDlg();

    // Dialog Data
    enum { IDD = IDD_DIALOG_CHOOSE_COLOR };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()

protected:
    CListCtrl m_listColor;
    CString m_strCondition;
    CMFCColorButton m_colorContorl;
    std::map<tstring, COLORREF> m_colorMap;

public:
    std::map<tstring, COLORREF> GetColorMap()const{return m_colorMap;}
    void SetColorMap(const std::map<tstring, COLORREF>& colorMap){m_colorMap = colorMap;}
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButtonAdd();
    afx_msg void OnBnClickedButtonDelete();
    afx_msg void OnCustomdrawListTrace(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedOk();
};
