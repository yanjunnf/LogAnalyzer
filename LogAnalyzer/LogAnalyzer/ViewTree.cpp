
#include "stdafx.h"
#include "ViewTree.h"
#include "MessageParameter.h"
#include "Poco\SharedPtr.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
    ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CViewTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    CPoint ptCurSel(0,0);
    TVHITTESTINFO HitTestInfo;
    GetCursorPos(&ptCurSel);
    ScreenToClient(&ptCurSel);  
    HitTestInfo.pt = ptCurSel;  
    HTREEITEM hSelItem = HitTest(&HitTestInfo);
    HTREEITEM hRoot = GetRootItem();
    if (hSelItem == GetRootItem())
    {
    }
    else
    {
        HTREEITEM hParentItem = GetParentItem(hSelItem);
        Poco::SharedPtr<SMsgParamter> msgParamter = new SMsgParamter;
        CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
        if (pMainFrame)
        {
            if (hParentItem == GetRootItem())
            {
                //Click tree note of log file -- switch to the selected log file
                msgParamter->m_bIsFile = TRUE;
                msgParamter->m_strFileName = GetItemText(hSelItem);
                pMainFrame->SwitchDocument(msgParamter);
            }
            else
            {
                //Click tree note of component -- if the current active form contains the selected component, show details.
                msgParamter->m_bIsFile = FALSE;
                msgParamter->m_strComponentName = GetItemText(hSelItem);
                msgParamter->m_strFileName = GetItemText(hParentItem);
                pMainFrame->SwitchDocument(msgParamter);
            }
        }
    }
}

void CViewTree::RemoveLogFile(const CString& strFileName)
{
    HTREEITEM hRoot = GetRootItem();
    if (ItemHasChildren(hRoot))
    {
        HTREEITEM hChildItem = GetChildItem(hRoot);
        HTREEITEM hNextItem = NULL;
        while (hChildItem != NULL)
        {
            if (GetItemText(hChildItem) == strFileName)
            {
                DeleteItem(hChildItem);
                break;
            }
            hChildItem = GetNextSiblingItem(hChildItem);
        };
    }
}
