#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "LogAnalyzer.h"
#endif

#include "LogAnalyzerDoc.h"
#include "LogAnalyzerView.h"
#include "ReadLog.h"
#include "Settings.h"
#include "AnalyseProgressDlg.h"
#include "MainFrm.h"

#ifdef EXCEL_REPORT
    #include "CChart.h"
    #include "CApplication.h"
    #include "CWorkbook.h"
    #include "CWorkbooks.h"
    #include "CWorksheet.h"
    #include "CWorksheets.h"
    #include "CRange.h"
    #include "CChartObject.h"
    #include "CChartObjects.h"
#endif

#include "Poco\File.h"
#include "Poco\Process.h"
#include "Poco\NumberParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GET_CLASS_VIEW(classView) classView=((CMainFrame*)AfxGetMainWnd())->GetClassView();

IMPLEMENT_DYNCREATE(CLogAnalyzerView, CResizableFormView)

BEGIN_MESSAGE_MAP(CLogAnalyzerView, CResizableFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
    ON_BN_CLICKED(IDC_BUTTON_NEXT_PAGE, &CLogAnalyzerView::OnBnClickedButtonNextPage)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLogAnalyzerView::OnBnClickedButtonOpen)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_TRACE_IDS, &CLogAnalyzerView::OnCustomdrawListTrace)
    ON_BN_CLICKED(IDC_BUTTON_FIRST_PAGE, &CLogAnalyzerView::OnBnClickedButtonFirstPage)
    ON_BN_CLICKED(IDC_BUTTON_PREVIOUS_PAGE, &CLogAnalyzerView::OnBnClickedButtonPreviousPage)
    ON_BN_CLICKED(IDC_BUTTON_LAST_PAGE, &CLogAnalyzerView::OnBnClickedButtonLastPage)
    ON_NOTIFY(NM_CLICK, IDC_LIST_TRACE_IDS, &CLogAnalyzerView::OnClickListTraceIds)
    ON_COMMAND(ID_EDIT_OPENSOURCEFILE, &CLogAnalyzerView::OnEditOpensourcefile)
    ON_COMMAND(ID_EDIT_LOCATELOGTRACE, &CLogAnalyzerView::OnEditLocatelogtrace)
    ON_UPDATE_COMMAND_UI(ID_EDIT_OPENSOURCEFILE, &CLogAnalyzerView::OnUpdateEditOpensourcefile)
    ON_UPDATE_COMMAND_UI(ID_EDIT_LOCATELOGTRACE, &CLogAnalyzerView::OnUpdateEditLocatelogtrace)
    ON_BN_CLICKED(IDC_BUTTON_FIND, &CLogAnalyzerView::OnBnClickedButtonFind)
    ON_COMMAND(ID_GENERATEREPORT_CSV, &CLogAnalyzerView::OnGeneratereportCsv)
    ON_COMMAND(ID_GENERATEREPORT_PDF, &CLogAnalyzerView::OnGeneratereportPdf)
    ON_COMMAND(ID_GENERATEREPORT_EXCEL, &CLogAnalyzerView::OnGeneratereportExcel)
    ON_UPDATE_COMMAND_UI(ID_GENERATEREPORT_EXCEL, &CLogAnalyzerView::OnUpdateGeneratereportExcel)
END_MESSAGE_MAP()

CLogAnalyzerView::CLogAnalyzerView()
	: CResizableFormView(CLogAnalyzerView::IDD)
{
    m_strCurrentPage = _T("1");
    m_strSelectedMsg = _T("");
    m_strFindContent = _T("");
    m_strTraceCount = _T("Total : 0");
}

CLogAnalyzerView::~CLogAnalyzerView()
{
    RemoveItem();
}

void CLogAnalyzerView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TRACE_IDS, m_traceList);
    DDX_Control(pDX, IDC_COMBO_COLUMNS, m_comboColumns);
    DDX_Control(pDX, IDC_COMBO_SETTINGS, m_comboSettings);
    DDX_Text(pDX, IDC_EDIT_CURRENT_PAGE_SIZE, m_strCurrentPage);
    DDX_Text(pDX, IDC_EDIT_SELECTED_MSG, m_strSelectedMsg);
    DDX_Text(pDX, IDC_EDIT_FIND_CONTENT, m_strFindContent);
    DDX_Text(pDX, IDC_STATIC_COUNT, m_strTraceCount);
    DDX_Check(pDX, IDC_CHECK_MATCH_CASE, m_bMatchCase);
}

BOOL CLogAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CResizableFormView::PreCreateWindow(cs);
}

void CLogAnalyzerView::OnInitialUpdate()
{
    // must add controls to the layout
    AddAnchor(IDC_COMBO_SETTINGS, TOP_LEFT, CSize(10, 0));
    AddAnchor(IDC_BUTTON_OPEN, CSize(10, 0), CSize(11, 0));
    AddAnchor(IDC_COMBO_COLUMNS, CSize(77, 0), CSize(87, 0));
    AddAnchor(IDC_EDIT_FIND_CONTENT, CSize(87, 0), CSize(97, 0));
    AddAnchor(IDC_BUTTON_FIND, CSize(97, 0), CSize(98, 0));
    AddAnchor(IDC_CHECK_MATCH_CASE, CSize(98, 0), CSize(100, 0));    
    AddAnchor(IDC_LIST_TRACE_IDS, TOP_LEFT, CSize(100, 99));
    AddAnchor(IDC_STATIC_COUNT, CSize(0, 100), CSize(10, 100));
    AddAnchor(IDC_EDIT_SELECTED_MSG, CSize(10, 100), CSize(94, 100));
    AddAnchor(IDC_BUTTON_FIRST_PAGE, CSize(96, 100), CSize(97, 100));
    AddAnchor(IDC_BUTTON_PREVIOUS_PAGE, CSize(97, 100), CSize(98, 100));
    AddAnchor(IDC_EDIT_CURRENT_PAGE_SIZE, CSize(98, 100), CSize(99, 100));
    AddAnchor(IDC_BUTTON_NEXT_PAGE, CSize(99, 100), CSize(100, 100));
    AddAnchor(IDC_BUTTON_LAST_PAGE, CSize(100, 100), BOTTOM_RIGHT);

    // complete initialization
    CResizableFormView::OnInitialUpdate();
	ResizeParentToFit();

    //Load all UI settings
    CLogAnalyzerApp* app = (CLogAnalyzerApp*)AfxGetApp();
    if (app != NULL)
    {
        std::vector<Poco::SharedPtr<CUISetting>> uiSettings = app->GetUISettings();
        for (int i = 0; i < uiSettings.size(); ++i)
        {
            m_comboSettings.AddString(uiSettings.at(i)->getName().c_str());
        }
        m_comboSettings.SetCurSel(0);
    }
    m_logger = GetDocument()->GetReadLog();

    m_comboColumns.AddString("All");
    m_comboColumns.SetCurSel(0);
    //
    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Line";
    lv.cx = 50;
    m_traceList.InsertColumn(0, &lv);
    m_traceList.SetExtendedStyle(m_traceList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void CLogAnalyzerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLogAnalyzerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

#ifdef _DEBUG
void CLogAnalyzerView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

void CLogAnalyzerView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

CLogAnalyzerDoc* CLogAnalyzerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogAnalyzerDoc)));
	return (CLogAnalyzerDoc*)m_pDocument;
}

//Next Page
void CLogAnalyzerView::OnBnClickedButtonNextPage()
{
    if (m_traceList.GetItemCount())
    {
        m_traceList.NextPage();
        m_strCurrentPage.Format("%d", m_traceList.GetCurrentPage());
        UpdateData(FALSE);
        EnableControls();
    }
}


void CLogAnalyzerView::OnBnClickedButtonOpen()
{
    //Open File
    if (!m_logger.isNull() && !(m_logger->GetActiveUISetting().isNull()))
    {
        if (!m_logger->GetActiveUISetting()->IsAvailable())
            if (AfxMessageBox("The UI setting is not available, Do you want to configurate it?", MB_OKCANCEL) == IDOK)
            {
                //OnAnalyseSetting();
                return;
            }
    }

    CString strFile = _T("");
    CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.log)|*.log|All Files (*.*)|*.*||"), NULL);

    if (dlgFile.DoModal() == IDOK)
    {
        strFile = dlgFile.GetPathName();

        CString strItem;
        m_comboSettings.GetLBText(m_comboSettings.GetCurSel(), strItem);
        CLogAnalyzerApp* app = (CLogAnalyzerApp*)AfxGetApp();
        if (app != NULL)
        {
            Poco::SharedPtr<CUISetting> currentSelSetting = app->GetUISetting(strItem.GetBuffer(strItem.GetLength()));
            if (!currentSelSetting.isNull())
            {
                if (!m_logger.isNull() && !m_logger->GetFileName().empty())
                {
                    m_logger->ClearAllComponents();
                    //Open another log file, remove the previous one
                    RemoveItem();
                }
                m_logger->SetActiveUISetting(currentSelSetting);
            }

            //Update list control
            while(m_comboColumns.DeleteString(0));
            while(m_traceList.DeleteColumn(0));
            m_comboColumns.AddString("All");
            //Init list control
            LV_COLUMN lv;
            lv.mask = LVCF_TEXT | LVCF_WIDTH;
            if (!currentSelSetting.isNull())
            {
                int nIndex = 0;
                //Insert the first column of Line number
                lv.pszText = "Line";
                lv.cx = 50;
                m_traceList.InsertColumn(nIndex++, &lv);

                const std::vector<Poco::SharedPtr<CColumnSetting>>& columns = currentSelSetting->GetAllColumns();
                for (int i = 0; i < columns.size(); ++i)
                {
                    if (columns.at(i)->m_bEnable)
                    {
                        lv.pszText = (LPSTR)columns.at(i)->m_strRealColumnName.c_str();
                        lv.cx = 100;
                        m_traceList.InsertNewColumn(nIndex++, &lv, i);
                        m_comboColumns.AddString(columns.at(i)->m_strRealColumnName.c_str());
                    }
                }
                m_traceList.SetColumnSetting(currentSelSetting->GetAllColumns());
            }
        }
        m_comboColumns.SetCurSel(0);

        m_traceList.DeleteAllTraceLog();

        m_logger->StartParse(tstring(strFile.GetBuffer(strFile.GetLength())));
        CAnalyseProgressDlg dlg(m_logger);
        dlg.DoModal();

        //Update tree view
        CClassView* pClassView = NULL;
        GET_CLASS_VIEW(pClassView);
        if (pClassView != NULL)
        {
            HTREEITEM hFile = pClassView->InsertLogFile(strFile);
            const std::map<tstring, Poco::SharedPtr<CComponent>>& components = m_logger->GetComponents();
            std::map<tstring, Poco::SharedPtr<CComponent>>::const_iterator ite = components.begin();
            while (ite != components.end())
            {
                pClassView->InsertComponent(hFile, ite->second->GetComponentName().c_str());
                ++ite;
            }
            pClassView->SetFocus();
        }

        //Update log file information/UI Setting in property panel
        CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
        if (pMainFrame != NULL)
        {
            Poco::SharedPtr<SLogFileProperty> logFilePro = new SLogFileProperty;
            logFilePro->m_strFileName = m_logger->GetFileName();
            logFilePro->m_nFileSize = m_logger->GetFileSize();

            pMainFrame->SetLogFileProperty(logFilePro);
            pMainFrame->SetCurrentUISettingProperty(m_logger->GetActiveUISetting());
        }

        //Update tab title
        CLogAnalyzerDoc* pDoc = (CLogAnalyzerDoc*)GetDocument();
        if (pDoc)
        {
            pDoc->SetTitle(dlgFile.GetFileName());
            pDoc->SetDocIdentifier(strFile);
        }
    }
}

void CLogAnalyzerView::OnCustomdrawListTrace(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
    int nItem = lplvcd->nmcd.dwItemSpec;
    int nColumn = lplvcd->iSubItem;
    COLORREF bkColor = RGB(255, 255, 255);
    CString strText;
    switch (lplvcd->nmcd.dwDrawStage) 
    { 
    case CDDS_PREPAINT : 
        { 
            *pResult = CDRF_NOTIFYITEMDRAW; 
            return; 
        } 
    case CDDS_ITEMPREPAINT: 
        { 
            *pResult = CDRF_NOTIFYSUBITEMDRAW; 
            return; 
        } 
    case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
        { 
            lplvcd->clrTextBk = GetColor(nItem, nColumn);
            *pResult = CDRF_DODEFAULT;  
            return; 
        }
    } 

    *pResult = 0;
}

COLORREF CLogAnalyzerView::GetColor(int nItem, int nColumn)
{
    COLORREF color = RGB(255, 255, 255);
    //The first column is line number, ignore it
    if (nColumn >= 1)
    {
        tstring strText = m_traceList.GetItemText(nItem, nColumn);
        //Why we need to decrease 1?
        const std::vector<Poco::SharedPtr<CColumnSetting>>& columns = m_logger->GetActiveUISetting()->GetAllColumns();
        int nIndex = m_traceList.GetRealColumnIndex(nColumn);

        if (columns.size() > nIndex)
        {   
            for (int i = 0; i < columns.at(nIndex)->m_columnColorArray.size(); ++i)
            {
                if (strText.find(columns.at(nIndex)->m_columnColorArray.at(i)->m_strCondition) != tstring::npos)
                {
                    color =  columns.at(nIndex)->m_columnColorArray.at(i)->m_nColor;
                    break;
                }
            }
        }
    }

    return color;
}

void CLogAnalyzerView::ShowComponent(const CString& strComponent)
{
    m_strSelComponent = strComponent;
    m_traceList.DeleteAllTraceLog();
    ShowTraces(strComponent, TRUE);

    //Update component properties in property panel
    const std::map<tstring, Poco::SharedPtr<CComponent>>& components = m_logger->GetComponents();
    std::map<tstring, Poco::SharedPtr<CComponent>>::const_iterator ite = components.find(m_strSelComponent.GetBuffer(m_strSelComponent.GetLength()));
    if (ite != components.end())
    {
        CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
        if (pMainFrame != NULL)
        {
            Poco::SharedPtr<SComponentProperty> compPro = new SComponentProperty;
            compPro->m_strComponentName = ite->second->GetComponentName();
            compPro->m_nLogTraceCount = ite->second->GetTraceLogs().size();
            pMainFrame->SetComponentProperty(compPro);
        }
    }
}

void CLogAnalyzerView::ShowTraces(const CString& strComponent, BOOL bShowAll)
{
    tstring strItemText = strComponent;
    const std::map<tstring, Poco::SharedPtr<CComponent>>& components = m_logger->GetComponents();
    std::map<tstring, Poco::SharedPtr<CComponent>>::const_iterator ite = components.find(strItemText);
    if (ite != components.end())
    {
        const std::vector<Poco::SharedPtr<CTraceLog>>& traceLogs = ite->second->GetTraceLogs();
        if (bShowAll)
        {
            //Only show the traces you want in list control
            m_traceList.DeleteAllTraceLog();
            for (int i = 0; i < traceLogs.size(); ++i)
                m_traceList.InsertTraceLog(traceLogs.at(i));
        }
        //Show all traces and go to the first trace log that fits to you want
        else
        {
            Poco::SharedPtr<CUISetting> uiSetting = m_logger->GetActiveUISetting();
            int nColumn = 0;
            int nCurSel = m_comboColumns.GetCurSel();
            if (nCurSel == 0)
                nColumn = -1; //All
            else
            {
                CString strItem;
                m_comboColumns.GetLBText(nCurSel, strItem);
                tstring strItemValue = strItem.GetBuffer(strItem.GetLength());
                if (!uiSetting.isNull())
                {
                    for (int i = 0; i < uiSetting->GetAllColumns().size(); ++i)
                    {
                        if (strItemValue == uiSetting->GetAllColumns().at(i)->m_strRealColumnName)
                        {
                            nColumn = i;
                            break;
                        }
                    }
                }
            }

            m_traceList.SearchText(nColumn, m_strFindContent.GetBuffer(m_strFindContent.GetLength()), m_bMatchCase);
        }
        m_strCurrentPage.Format("%d", m_traceList.GetCurrentPage());
        m_strTraceCount.Format("Total : %d", m_traceList.GetCount());
        UpdateData(FALSE);
    }
}

void CLogAnalyzerView::OnBnClickedButtonFirstPage()
{
    if (m_traceList.GetItemCount())
    {
        m_traceList.FirstPage();
        m_strCurrentPage.Format("%d", m_traceList.GetCurrentPage());
        UpdateData(FALSE);
        EnableControls();
    }
}

void CLogAnalyzerView::EnableControls()
{
    if (m_traceList.GetCurrentPage() == 1)
    {
        GetDlgItem(IDC_BUTTON_FIRST_PAGE)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_PREVIOUS_PAGE)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_NEXT_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_LAST_PAGE)->EnableWindow(TRUE);
    }
    else if (m_traceList.GetCurrentPage() == m_traceList.GetMaxPage())
    {
        GetDlgItem(IDC_BUTTON_FIRST_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_PREVIOUS_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_NEXT_PAGE)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_LAST_PAGE)->EnableWindow(FALSE);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_FIRST_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_PREVIOUS_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_NEXT_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_LAST_PAGE)->EnableWindow(TRUE);
    }
}


void CLogAnalyzerView::OnBnClickedButtonPreviousPage()
{
    if (m_traceList.GetItemCount())
    {
        m_traceList.PreviousPage();
        m_strCurrentPage.Format("%d", m_traceList.GetCurrentPage());
        UpdateData(FALSE);
        EnableControls();
    }
}


void CLogAnalyzerView::OnBnClickedButtonLastPage()
{
    if (m_traceList.GetItemCount())
    {
        m_traceList.LastPage();
        m_strCurrentPage.Format("%d", m_traceList.GetCurrentPage());
        UpdateData(FALSE);
        EnableControls();
    }
}


void CLogAnalyzerView::OnClickListTraceIds(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    m_strSelectedMsg.Empty();
    if (!m_logger->GetActiveUISetting().isNull())
    {
        const std::vector<Poco::SharedPtr<CColumnSetting>>& columns = m_logger->GetActiveUISetting()->GetAllColumns();
        //The first column is line number, ignore the column 0, start from column 1
        int nIndex = 1;
        for (int i = 0; i < columns.size(); ++i)
        {
            if (columns.at(i)->m_bEnable)
            {
                m_strSelectedMsg += m_traceList.GetItemText(pNMItemActivate->iItem, nIndex);
                m_strSelectedMsg += " ";
                ++nIndex;
            }
        }
        m_strSelectedMsg.Trim();
        UpdateData(false);
    }

    *pResult = 0;
}

void CLogAnalyzerView::RemoveItem()
{
    //Shut down child window, notify its parent to remove related item from tree view
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    if (pMainFrame)
    {
        pMainFrame->RemoveLogFileFromTreeView(m_logger->GetFileName().c_str());
    }
}

void CLogAnalyzerView::OnEditOpensourcefile()
{
    if (m_logger->GetActiveUISetting().isNull())
    {
        ERROR_MESSAGEBOX("UI Setting has incorrect info.");
        return;
    }

    BOOL bPython = m_logger->GetActiveUISetting()->GetPythonModuleFlag();
    int nIdentifier = m_logger->GetActiveUISetting()->GetColumnIdentifier();
    if (nIdentifier == -1 && !bPython)
    {
        ERROR_MESSAGEBOX("Cannot open related source file since this UI Setting doesn't have column identifier or define python function.");
        return;
    }

    //Get the selected component name
    tstring strText(m_strSelComponent.GetBuffer(m_strSelComponent.GetLength())); 
    //Get the selected trace log

    const std::map<tstring, Poco::SharedPtr<CComponent>>& components = m_logger->GetComponents();
    std::map<tstring, Poco::SharedPtr<CComponent>>::const_iterator ite = components.find(strText);

    HCURSOR holdcursor, hwaitcursor;
    hwaitcursor = LoadCursor(NULL,IDC_WAIT);
    holdcursor = ::SetCursor(hwaitcursor);

    try
    {
        if (ite != components.end())
        {
            //Search trace log
            const std::vector<Poco::SharedPtr<CTraceLog>>& traceLogs = ite->second->GetTraceLogs();
            for (int i = 0; i < traceLogs.size(); ++i)
            {
                TCHAR itemText[512] = {0};
                POSITION pt = m_traceList.GetFirstSelectedItemPosition();
                int nSelIndex = m_traceList.GetNextSelectedItem(pt);
                CString strLine = m_traceList.GetItemText(nSelIndex, 0);
                int nLine = Poco::NumberParser::parse(strLine.GetBuffer(strLine.GetLength()));
                if (nLine == traceLogs.at(i)->m_nLine)
                {
                    m_logger->GetAssocatedSourceFiles(traceLogs.at(i));
                    if (!traceLogs.at(i)->m_strSourceCodeFile.empty())
                    {
                        //Open file and go to the specify line
                        std::vector<tstring> args;
                        itoa(traceLogs.at(i)->m_nLineInSourceFile, itemText, 10);
                        tstring strCommandLine = m_logger->GetActiveUISetting()->GetNodepadPath();
                        strCommandLine += " -n";
                        strCommandLine += itemText;
                        strCommandLine += " \"";
                        strCommandLine += traceLogs.at(i)->m_strSourceCodeFile;
                        strCommandLine += "\"";
                        Poco::Process::launch(strCommandLine, args);
                        break;
                    }
                    else
                        ERROR_MESSAGEBOX("Cannot find the trace Id, Please modify the path for the related component.");
                    break;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        CString strMessage = "Exception :";
        strMessage += e.what();
        ERROR_MESSAGEBOX(strMessage);
    }
    catch (...)
    {
        ERROR_MESSAGEBOX("Unknown exception");
    }
    ::SetCursor(holdcursor);
}


void CLogAnalyzerView::UpdateUISetting()
{
    if (!m_logger->GetActiveUISetting().isNull())
    {
        tstring strSettingName = m_logger->GetActiveUISetting()->getName();
        CLogAnalyzerApp* app = (CLogAnalyzerApp*)AfxGetApp();
        if (app != NULL)
        {
            Poco::SharedPtr<CUISetting> currentSelSetting = app->GetUISetting(strSettingName);
            if (!currentSelSetting.isNull())
                m_logger->SetActiveUISetting(currentSelSetting);
        }
    }
    
}

void CLogAnalyzerView::OnEditLocatelogtrace()
{
    if (m_logger->GetActiveUISetting().isNull())
    {
        ERROR_MESSAGEBOX("CLogAnalyseDlg::OnQuickLocatethislineinnotepadd : UI Setting has incorrect info.");
        return;
    }
    //Get the selected component name
    tstring strText(m_strSelComponent.GetBuffer(m_strSelComponent.GetLength())); 
    //Get the selected trace log

    const std::map<tstring, Poco::SharedPtr<CComponent>>& components = m_logger->GetComponents();
    std::map<tstring, Poco::SharedPtr<CComponent>>::const_iterator ite = components.find(strText);
    if (ite != components.end())
    {
        //Search trace log
        const std::vector<Poco::SharedPtr<CTraceLog>>& traceLogs = ite->second->GetTraceLogs();
        TCHAR itemText[512] = {0};
        POSITION pt = m_traceList.GetFirstSelectedItemPosition();
        int nSelIndex = m_traceList.GetNextSelectedItem(pt);
        CString strLine = m_traceList.GetItemText(nSelIndex, 0);
        int nLine = Poco::NumberParser::parse(strLine.GetBuffer(strLine.GetLength()));

        for (int i = 0; i < traceLogs.size(); ++i)
        {
            if (nLine == traceLogs.at(i)->m_nLine)
            {
                std::vector<tstring> args;
                itoa(traceLogs.at(i)->m_nLine, itemText, 10);
                tstring strCommandLine = m_logger->GetActiveUISetting()->GetNodepadPath();
                strCommandLine += " -n";
                strCommandLine += itemText;
                strCommandLine += " \"";
                strCommandLine += m_logger->GetFileName();
                strCommandLine += "\"";
                Poco::Process::launch(strCommandLine, args);
                break;
            }
        }
    }
}

void CLogAnalyzerView::OnUpdateEditOpensourcefile(CCmdUI *pCmdUI)
{
    if (m_traceList.GetSelectedCount() == 1)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CLogAnalyzerView::OnUpdateEditLocatelogtrace(CCmdUI *pCmdUI)
{
    if (m_traceList.GetSelectedCount() == 1)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CLogAnalyzerView::OnBnClickedButtonFind()
{
    UpdateData(TRUE);

    if (m_strFindContent.IsEmpty())
        ShowTraces(m_strSelComponent, TRUE);
    else
        ShowTraces(m_strSelComponent, FALSE);
}


void CLogAnalyzerView::OnGeneratereportCsv()
{
}


void CLogAnalyzerView::OnGeneratereportPdf()
{
}


void CLogAnalyzerView::OnGeneratereportExcel()
{
#ifdef EXCEL_REPORT
    try
    {
        if (m_logger->GetFileName().empty())
            return;

        CApplication app;
        CWorkbook exlBook;  
        CWorkbooks exlBooks;  
        CWorksheet exlSheet;  
        CWorksheets exlSheets;
        CChart chart;
        CRange range;
        CChartObjects chartobjects;
        LPDISPATCH lpDisp;

        COleVariant covTrue((short)TRUE), covFalse((short)FALSE), covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

        // Start Excel and get the Application object.
        if(!app.CreateDispatch("Excel.Application"))
        {
            ERROR_MESSAGEBOX("Couldn't start Excel and get an application 0bject");
            return;
        }

        app.put_Visible(TRUE);

        // Get Workbooks collection.
        lpDisp = app.get_Workbooks();
        ASSERT(lpDisp);
        // Attach the IDispatch pointer
        exlBooks.AttachDispatch(lpDisp);
        exlBook = exlBooks.Add(covOptional);

        // Get sheets.
        lpDisp = exlBook.get_Sheets();
        ASSERT(lpDisp);
        exlSheets.AttachDispatch(lpDisp);

        lpDisp = exlSheets.get_Item( COleVariant((short)(1)) );
        ASSERT(lpDisp);
        // Attach the lpDisp pointer to a Worksheet object.
        exlSheet.AttachDispatch(lpDisp);

        const std::map<tstring, Poco::SharedPtr<CComponent>>& componentMap = m_logger->GetComponents();
        char charPos[32];
        int nSize = 0;
        if (componentMap.size() > 0)
        {   
            std::map<tstring, Poco::SharedPtr<CComponent>>::const_iterator ite = componentMap.begin();
            int nIndex = 3;

            while (ite != componentMap.end())
            {
                if (ite->second->GetTraceLogs().size() > 0)
                {
                    sprintf(charPos, "A%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    ASSERT(lpDisp);
                    range.AttachDispatch(lpDisp);  // Attach the IDispatch pointer
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(ite->second->GetComponentName().c_str()));
                    range.put_Value2(COleVariant(ite->second->GetComponentName().c_str()));

                    int nTrace = 0, nNotification = 0, nError = 0, nInformation = 0, nDebug = 0, nCritical = 0, nWarning = 0, nFatal = 0;
                    if (nSize == 0)
                    {
                        nTrace = 10;
                        nNotification = 10;
                        nError = 5, nInformation = 50, nDebug = 2, nCritical = 2, nWarning = 9, nFatal = 1;
                    }
                    else if (nSize == 1)
                    {
                        nTrace = 10;
                        nNotification = 25;
                        nError = 4, nInformation = 36, nDebug = 5, nCritical = 0, nWarning = 20, nFatal = 1;
                    }
                    else if (nSize == 2)
                    {
                        nTrace = 0;
                        nNotification = 5;
                        nError = 4, nInformation = 100, nDebug = 5, nCritical = 5, nWarning = 15, nFatal = 0;
                    }
                    else if (nSize == 3)
                    {
                        nTrace = 0;
                        nNotification = 5;
                        nError = 4, nInformation = 82, nDebug = 36, nCritical = 1, nWarning = 50, nFatal = 0;
                    }
                    //ite->second->getLogTypeCount(nTrace, nNotification, nError, nInformation, nDebug, nCritical, nWarning, nFatal);
                    //Trace
                    sprintf(charPos, "B%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nTrace);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Debug
                    sprintf(charPos, "C%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nDebug);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Information
                    sprintf(charPos, "D%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nInformation);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Warning
                    sprintf(charPos, "E%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nWarning);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Error
                    sprintf(charPos, "F%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nError);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Critical
                    sprintf(charPos, "G%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nCritical);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Notification
                    sprintf(charPos, "H%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nNotification);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    //Fatal
                    sprintf(charPos, "I%d", nIndex);
                    lpDisp = exlSheet.get_Range(COleVariant(charPos), COleVariant(charPos));
                    range.AttachDispatch(lpDisp);
                    sprintf(charPos, "%d", nFatal);
                    range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant(charPos));
                    range.put_Value2(COleVariant(charPos));
                    ++nIndex;
                    ++nSize;
                }
                ++ite;
            }
        }

        lpDisp = exlSheet.get_Range(COleVariant("B2"), COleVariant("B2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("TRACE"));
        range.put_Value2(COleVariant("TRACE"));

        lpDisp = exlSheet.get_Range(COleVariant("C2"), COleVariant("C2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("DEBUG"));
        range.put_Value2(COleVariant("DEBUG"));

        lpDisp = exlSheet.get_Range(COleVariant("D2"), COleVariant("D2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("INFORMATION"));
        range.put_Value2(COleVariant("INFORMATION"));

        lpDisp = exlSheet.get_Range(COleVariant("E2"), COleVariant("E2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("WARNING"));
        range.put_Value2(COleVariant("WARNING"));

        lpDisp = exlSheet.get_Range(COleVariant("F2"), COleVariant("F2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("ERROR"));
        range.put_Value2(COleVariant("ERROR"));

        lpDisp = exlSheet.get_Range(COleVariant("G2"), COleVariant("G2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("CRITICAL"));
        range.put_Value2(COleVariant("CRITICAL"));

        lpDisp = exlSheet.get_Range(COleVariant("H2"), COleVariant("H2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("NOTIFICATION"));
        range.put_Value2(COleVariant("NOTIFICATION"));

        lpDisp = exlSheet.get_Range(COleVariant("I2"), COleVariant("I2"));
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);
        range.put_Value(COleVariant((long)DISP_E_PARAMNOTFOUND, VT_ERROR), COleVariant("FATAL"));
        range.put_Value2(COleVariant("FATAL"));

        // The cells are populated. To start the chart,
        // declare some long variables and site the chart.
        long left, top, width, height;
        left = 200;
        top = 200;
        width = 450;
        height = 350;

        lpDisp = exlSheet.ChartObjects(covOptional);

        ASSERT(lpDisp);
        chartobjects.AttachDispatch(lpDisp); // Attach the lpDisp pointer
        // for ChartObjects to the chartobjects
        // object.
        CChartObject chartobject = chartobjects.Add(left, top, width, height); 
        //defines the rectangle, 
        // adds a new chart at that rectangle and 
        // assigns its object reference to a 
        // ChartObject variable named chartobject
        chart.AttachDispatch(chartobject.get_Chart());

        sprintf(charPos, "I%d", nSize + 2);
        lpDisp = exlSheet.get_Range(COleVariant("A2"), COleVariant(charPos));
        // The range containing the data to be charted.
        ASSERT(lpDisp);
        range.AttachDispatch(lpDisp);

        VARIANT var; // ChartWizard needs a Variant for the Source range.
        var.vt = VT_DISPATCH; // .vt is the usable member of the tagVARIANT
        var.pdispVal = lpDisp; // Assign IDispatch pointer

        chart.ChartWizard(var,                    // Source.
            COleVariant((short)52),  // Gallery: stacked column.
            covOptional,             // Format, use default.
            COleVariant((short)1),   // PlotBy: xlRows.
            COleVariant((short)1),   // CategoryLabels.
            COleVariant((short)1),   // SeriesLabels.
            COleVariant((short)TRUE),  // HasLegend.
            COleVariant("Trace Report"),  // Title.
            COleVariant("Components"),    // CategoryTitle.
            COleVariant("Trace Count"),  // ValueTitles.
            covOptional              // ExtraTitle.
            );

        range.ReleaseDispatch();   
        exlSheet.ReleaseDispatch();   
        exlSheets.ReleaseDispatch();   
        exlBook.ReleaseDispatch();   
        exlBooks.ReleaseDispatch();
        chart.ReleaseDispatch();
        chartobject.ReleaseDispatch();
        app.ReleaseDispatch();
    }
    catch(COleException *e)
    {
        char buf[1024];
        sprintf(buf, "COleException. SCODE: %08lx.", (long)e->m_sc);
        ::MessageBox(NULL, buf, "COleException", MB_SETFOREGROUND | MB_OK);
    }

    catch(COleDispatchException *e)
    {
        char buf[1024];
        sprintf(buf,
            "COleDispatchException. SCODE: %08lx, Description: \"%s\".", (long)e->m_wCode,
            (LPSTR)e->m_strDescription.GetBuffer(1024));
        ::MessageBox(NULL, buf, "COleDispatchException", MB_SETFOREGROUND | MB_OK);
    }

    catch(...)
    {
        ::MessageBox(NULL, "General Exception caught.", "Catch-All", MB_SETFOREGROUND | MB_OK);
    }
#endif
}


void CLogAnalyzerView::OnUpdateGeneratereportExcel(CCmdUI *pCmdUI)
{
#ifdef EXCEL_REPORT
    pCmdUI->Enable(true);
#else
    pCmdUI->Enable(false);
#endif
}
