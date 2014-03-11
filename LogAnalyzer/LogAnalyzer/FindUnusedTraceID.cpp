#include "stdafx.h"
#include "FindUnusedTraceID.h"
#include "afxdialogex.h"
#include "XMLParse.h"
#include "Poco\Process.h"

IMPLEMENT_DYNAMIC(CFindUnusedTraceID, CDialogEx)

CFindUnusedTraceID::CFindUnusedTraceID(Poco::SharedPtr<CReadLog> pReadLog, CWnd* pParent /*=NULL*/)
	: m_pReadLog(pReadLog), CDialogEx(CFindUnusedTraceID::IDD, pParent)
{
    m_strMessageFile = _T("");
}

CFindUnusedTraceID::~CFindUnusedTraceID()
{
}

void CFindUnusedTraceID::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_FIND_UNUSED_ID_RESULT, m_lstTrace);
    DDX_Text(pDX, IDC_EDIT_MESSAGE_FILE_PATH, m_strMessageFile);
    DDV_MaxChars(pDX, m_strMessageFile, 128);
    DDX_Control(pDX, IDC_COMBO_LEVEL, m_filterList);
    DDX_Control(pDX, IDC_COMBO_SOURCE_CODE_PATH, m_comboSourcePath);
}


BEGIN_MESSAGE_MAP(CFindUnusedTraceID, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_MESSAGE_FILE, &CFindUnusedTraceID::OnBnClickedButtonOpenMessageFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_SOURCE_CODE_FOLDER, &CFindUnusedTraceID::OnBnClickedButtonOpenSourceCodeFolder)
    ON_BN_CLICKED(ID_BTN_SEARCH, &CFindUnusedTraceID::OnBnClickedBtnSearch)
    ON_COMMAND(ID_OPEN_OPENSOURCEFILE, &CFindUnusedTraceID::OnQuickOpensourcefileForSpecTraceId)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_FIND_UNUSED_ID_RESULT, &CFindUnusedTraceID::OnRclickListFindUnusedIdResult)
END_MESSAGE_MAP()


BOOL CFindUnusedTraceID::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    //Init list control
    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Messaage(String) ID";
    lv.cx = 200;
    m_lstTrace.InsertColumn(0, &lv);

    lv.cx = 400;
    lv.pszText = "File Path";
    m_lstTrace.InsertColumn(1, &lv);
    m_lstTrace.SetExtendedStyle(m_lstTrace.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    m_popupMenu.LoadMenuA(IDR_MENU_OPEN_SOURCE_FILE);

    //Insert defined path
    if (!m_pReadLog.isNull())
    {
        /*
        const std::map<tstring, Poco::SharedPtr<CPathSetting>>& pathMap = m_pReadLog->getPathSetting();
        std::map<tstring, Poco::SharedPtr<CPathSetting>>::const_iterator ite = pathMap.cbegin();
        while (ite != pathMap.cend())
        {
            m_comboSourcePath.AddString(ite->second->m_strPath.c_str());
            ++ite;
        }
        */
    }

    m_filterList.SetCurSel(0);
    return TRUE;
}


void CFindUnusedTraceID::OnBnClickedButtonOpenMessageFile()
{
    CString strFile = _T("");
    CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.xml)|*.xml|"), NULL);

    if (dlgFile.DoModal())
    {
        m_strMessageFile = dlgFile.GetPathName();
        UpdateData(FALSE);
    }
}


void CFindUnusedTraceID::OnBnClickedButtonOpenSourceCodeFolder()
{
    UpdateData(TRUE);
    TCHAR szPath[MAX_PATH];
    CString str;
    ZeroMemory(szPath, sizeof(szPath));
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "Save Trace";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);

    if (lp && SHGetPathFromIDList(lp, szPath))
    {
        m_comboSourcePath.InsertString(0, szPath);
        m_comboSourcePath.SetCurSel(0);
    }
}

void CFindUnusedTraceID::OnBnClickedBtnSearch()
{
    UpdateData(TRUE);
    if (m_strMessageFile.IsEmpty())
    {
        ERROR_MESSAGEBOX("Please input message file full path.");
        return;
    }

    CString strSourcePath;
    if (m_comboSourcePath.GetCurSel() > 0)
        m_comboSourcePath.GetLBText(m_comboSourcePath.GetCurSel(), strSourcePath);
    else
        m_comboSourcePath.GetWindowText(strSourcePath);

    if (strSourcePath.IsEmpty())
    {
        ERROR_MESSAGEBOX("Please input source code path.");
        return;
    }
    
    GetDlgItem(ID_BTN_SEARCH)->EnableWindow(FALSE);
    HCURSOR holdcursor,hwaitcursor;
    hwaitcursor = LoadCursor(NULL,IDC_WAIT);
    holdcursor = ::SetCursor(hwaitcursor);

    XMLParser parser;
    parser.StartXML(tstring(m_strMessageFile.GetBuffer(m_strMessageFile.GetLength())));

    const std::vector<tstring>& msgIDs = parser.getMessageIDs();
    if (msgIDs.size() > 0)
    {
        //Remove the odd IDs
        m_messageIDMap.clear();

        std::vector<tstring> filters;
        std::vector<tstring> files;
        filters.push_back(CPPSOURCEFILE);
        filters.push_back(JAVAFILE);
        if (m_filterList.GetCurSel() == 1)
            filters.push_back(HEADERFILE);

        try
        {
            tstring strPath(strSourcePath.GetBuffer(strSourcePath.GetLength()));
            strPath = CTools::ReplaceEnvVar(strPath);
            CTools::IteratorDir(strPath, filters, files);
        }
        catch (const std::exception& e)
        {
            gTraceLogger.getLogger().information(tstring("CFindUnusedTraceID::OnBnClickedBtnSearch() - Exception:") + e.what());
            GetDlgItem(ID_BTN_SEARCH)->EnableWindow(TRUE);
            return;
        }

        for (int i = 0; i < files.size(); ++i)
        {
            CFindStringInFile findString(files.at(i));
            for (int j = 0; j < msgIDs.size(); ++j)
            {
                if (m_messageIDMap.find(msgIDs.at(j)) == m_messageIDMap.end())
                {
                    if (findString.FindString(msgIDs.at(j)))
                        m_messageIDMap[msgIDs.at(j)] = files.at(i);
                }
            }
        }

        //Show the result on UI
        if (m_messageIDMap.size() > 0)
        {
            m_lstTrace.DeleteAllItems();
            std::map<tstring, tstring>::iterator ite = m_messageIDMap.begin();
            int nIndex = 0;
            while (ite != m_messageIDMap.end())
            {
                m_lstTrace.InsertItem(nIndex, ite->first.c_str());
                m_lstTrace.SetItemText(nIndex++, 1, ite->second.c_str());
                ++ite;
            }
        }
    }
    ::SetCursor(holdcursor);
    GetDlgItem(ID_BTN_SEARCH)->EnableWindow(TRUE);
}


void CFindUnusedTraceID::OnQuickOpensourcefileForSpecTraceId()
{
    POSITION pt = m_lstTrace.GetFirstSelectedItemPosition();
    int nSelIndex = m_lstTrace.GetNextSelectedItem(pt);
    if (nSelIndex >= 0)
    {
        std::vector<tstring> args;
        if (!m_pReadLog->GetActiveUISetting().isNull())
        {
            tstring strCommandLine = m_pReadLog->GetActiveUISetting()->GetNodepadPath();
            CString strItemText(strCommandLine.c_str());
            strItemText += " \"";
            strItemText += m_lstTrace.GetItemText(nSelIndex, 1);
            strItemText += " \"";
            Poco::Process::launch(tstring(strItemText.GetBuffer(strItemText.GetLength())), args);
        }
    }
}

void CFindUnusedTraceID::OnRclickListFindUnusedIdResult(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    if (m_lstTrace.GetItemCount() > 0 && m_lstTrace.GetSelectedCount() > 0)
    {
        // Popup menu
        CMenu* pMenu = NULL;
        pMenu = m_popupMenu.GetSubMenu(0);
        if (pMenu != NULL)
        {
            CPoint point(pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y);
            m_lstTrace.ClientToScreen(&point);
            pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
        }
    }
    *pResult = 0;
}
