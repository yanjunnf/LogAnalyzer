#pragma once


// CAsyncListCtrl
#include <vector>
#include <map>
#include "Poco\SharedPtr.h"
#include "Settings.h"

class CAsyncListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CAsyncListCtrl)

public:
    CAsyncListCtrl();
    virtual ~CAsyncListCtrl();

protected:
    int m_nPageSize;
    int m_nCurrentPage;
    std::vector<Poco::SharedPtr<CTraceLog>> m_traceList;
    std::vector<Poco::SharedPtr<CColumnSetting>> m_columnSetting;
    //
    std::map<int, int> columnMap;

public:
    void NextPage();
    void PreviousPage();
    void LastPage();
    void FirstPage();
    //Return the new page
    int GoToPage(int nPage);
    void SetPageSize(int nPageSize){if (m_nPageSize > 0)m_nPageSize = nPageSize;}
    int GetCurrentPage()const{return m_nCurrentPage;}

    int GetMaxPage()const{return m_traceList.size() / m_nPageSize;}
    int GetCount()const{return m_traceList.size();}

    void InsertTraceLog(const Poco::SharedPtr<CTraceLog> traceLog);
    BOOL DeleteAllTraceLog();
    void SetColumnSetting(const std::vector<Poco::SharedPtr<CColumnSetting>>& columnSetting){m_columnSetting = columnSetting;}

    //If nColumnIndex = -1, it means search all columns for each item.
    void SearchText(int nColumnIndex, const tstring& Text, BOOL bMatchCase = FALSE);

    int InsertNewColumn(int nCol, const LVCOLUMN* pColumn, int nRealColumnIndex);
    int GetRealColumnIndex(int nCol){return columnMap[nCol];}

protected:
    DECLARE_MESSAGE_MAP()

    void InsertContent(const Poco::SharedPtr<CTraceLog> traceLog);
    void ShowItems(int nCurrentPage);
};


