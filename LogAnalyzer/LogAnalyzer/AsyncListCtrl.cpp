// AsyncListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AsyncListCtrl.h"
#include "Poco/String.h"

// CAsyncListCtrl

IMPLEMENT_DYNAMIC(CAsyncListCtrl, CListCtrl)

CAsyncListCtrl::CAsyncListCtrl() : m_nCurrentPage(1), m_nPageSize(50)
{

}

CAsyncListCtrl::~CAsyncListCtrl()
{
}


BEGIN_MESSAGE_MAP(CAsyncListCtrl, CListCtrl)
END_MESSAGE_MAP()


void CAsyncListCtrl::NextPage()
{
    if (m_nCurrentPage < m_traceList.size()/m_nPageSize)
    {
        ShowItems(++m_nCurrentPage);
    }
}
void CAsyncListCtrl::PreviousPage()
{
    if (m_nCurrentPage > 1)
    {
        ShowItems(--m_nCurrentPage);
    }
}
void CAsyncListCtrl::LastPage()
{
    if (m_nCurrentPage < m_traceList.size()/m_nPageSize)
    {
        m_nCurrentPage = m_traceList.size()/m_nPageSize;
        ShowItems(m_nCurrentPage);
    }
}
void CAsyncListCtrl::FirstPage()
{
    if (m_nCurrentPage != 0)
    {
        m_nCurrentPage = 1;
        ShowItems(m_nCurrentPage);
    }
}

void CAsyncListCtrl::InsertTraceLog(const Poco::SharedPtr<CTraceLog> traceLog)
{
    if (!traceLog.isNull())
    {
        m_traceList.push_back(traceLog);
        if (this->GetItemCount() < m_nPageSize)
            InsertContent(traceLog);
    }
}

BOOL CAsyncListCtrl::DeleteAllTraceLog()
{
    m_traceList.clear();
    m_nCurrentPage = 1;
    return DeleteAllItems();
}

void CAsyncListCtrl::ShowItems(int nCurrentPage)
{
    DeleteAllItems();
    int nStartPos = (m_nCurrentPage-1) * m_nPageSize;
    nStartPos = nStartPos > 0 ? nStartPos : 0;
    for (int i = 0; i < m_nPageSize && nStartPos + i < m_traceList.size(); ++i)
        InsertContent(m_traceList.at(nStartPos + i));
}

void CAsyncListCtrl::InsertContent(const Poco::SharedPtr<CTraceLog> traceLog)
{
    char buff[32];
    sprintf(buff, "%d", traceLog->m_nLine);
    int nIndex = this->GetItemCount();
    InsertItem(nIndex, buff);
    int nColumn = 1;
    for (int i = 0; i < traceLog->m_traceArray.size(); ++i)
    {
        if (i < m_columnSetting.size() && m_columnSetting.at(i)->m_bEnable)
            SetItemText(nIndex, nColumn++, traceLog->m_traceArray.at(i).c_str());
    }
}

void CAsyncListCtrl::SearchText(int nColumnIndex, const tstring& Text, BOOL bMatchCase)
{
    tstring strTarget;
    std::vector<Poco::SharedPtr<CTraceLog>> newTraceList;
    tstring strFind = Text;
    if (!bMatchCase)
        strFind = Poco::toUpper(Text);

    for (int i = 0; i < m_traceList.size(); ++i)
    {
        strTarget.clear();
        if (nColumnIndex == -1)
        {
            for (int j = 0; j < m_traceList.at(i)->m_traceArray.size(); ++j)
                strTarget += m_traceList.at(i)->m_traceArray.at(j);
        }
        else if (nColumnIndex < m_traceList.at(i)->m_traceArray.size())
            strTarget = m_traceList.at(i)->m_traceArray.at(nColumnIndex);//Why we need to do nColumnIndex + 1, because the first column is line number

        if (!bMatchCase && Poco::toUpper(strTarget).find(strFind) != tstring::npos)
            newTraceList.push_back(m_traceList.at(i));
        else if (strTarget.find(strFind) != tstring::npos)
            newTraceList.push_back(m_traceList.at(i));
    }

    m_traceList = newTraceList;
    m_nCurrentPage = 0;
    ShowItems(m_nCurrentPage);
}

int CAsyncListCtrl::GoToPage(int nPage)
{
    if (nPage > 0 && nPage < GetMaxPage())
    {
        m_nCurrentPage = nPage;
        ShowItems(m_nCurrentPage);
    }
    return m_nCurrentPage;
}

int CAsyncListCtrl::InsertNewColumn(int nCol, const LVCOLUMN* pColumn, int nRealColumnIndex)
{
    columnMap[nCol] = nRealColumnIndex;
    return InsertColumn(nCol, pColumn);
}