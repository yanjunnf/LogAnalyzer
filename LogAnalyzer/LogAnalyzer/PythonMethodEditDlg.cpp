#include "stdafx.h"
#include "PythonMethodEditDlg.h"
#include "afxdialogex.h"
#include "CommonHeader.h"
#include "Poco\FileStream.h"
#include "PythonFunctionExecuter.h"

IMPLEMENT_DYNAMIC(CPythonMethodEditDlg, CDialogEx)

    CPythonMethodEditDlg::CPythonMethodEditDlg(const CString& strFunName, const std::vector<CString>& columnValues, 
    const std::vector<CString>& sourceCodeArray, const std::vector<int>& enableColumns, std::vector<CString>& pathList,
    CWnd* pParent /*=NULL*/)
    : CDialogEx(CPythonMethodEditDlg::IDD, pParent), m_strFunName(strFunName)
{
    m_columnValues = columnValues;
    m_enableColumns = enableColumns;
    m_sourceCodeArray = sourceCodeArray;
    m_pathArray = pathList;
    m_strResult = _T("");
}

CPythonMethodEditDlg::~CPythonMethodEditDlg()
{
}

void CPythonMethodEditDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT2_CODE, m_pythonEditor);
    DDX_Text(pDX, IDC_EDIT_FUN_NAME, m_strFunName);
    DDV_MaxChars(pDX, m_strFunName, 128);
    DDX_Text(pDX, IDC_EDIT_RESULT, m_strResult);
    DDX_Control(pDX, IDC_LIST_COLUMN_PARAMETER, m_listArugments);
    DDX_Control(pDX, IDC_COMBO_PATH_LIST, m_pathList);
}


BEGIN_MESSAGE_MAP(CPythonMethodEditDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_VALIDATE, &CPythonMethodEditDlg::OnBnClickedButtonValidate)
    ON_BN_CLICKED(IDOK, &CPythonMethodEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()



BOOL CPythonMethodEditDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_RETURN && this->GetFocus()->GetSafeHwnd() == m_pythonEditor.GetSafeHwnd())
        {
            char buff[2014];
            m_pythonEditor.GetLine(m_pythonEditor.LineFromChar(-1), buff);
            tstring strText(buff);
            int nIndex = strText.rfind(":");
            nIndex = strText.rfind("    ");
            nIndex = strText.length();
            if ((strText.rfind(":") == strText.length()-2) || (strText.find("    " ) == 0))
                m_pythonEditor.ReplaceSel(_T("\n    "));
            else
                m_pythonEditor.ReplaceSel(_T("\n"));
            m_pythonEditor.SetFocus();
            return true;
        }
        else if (pMsg->wParam == VK_TAB && this->GetFocus()->GetSafeHwnd() == m_pythonEditor.GetSafeHwnd())
        {
            m_pythonEditor.ReplaceSel("hello");
            TRACE("%d, %d, %d\n", pMsg->message, pMsg->wParam);
            for (int i = 0; i < 4; i++)
                ::SendMessage(m_pythonEditor, WM_CHAR, VK_SPACE, 0L);
            m_pythonEditor.SetFocus();
            return true;
        }
        return false;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CPythonMethodEditDlg::OnBnClickedButtonValidate()
{
    UpdateData(TRUE);
    if (m_strFunName.IsEmpty())
    {
        ERROR_MESSAGEBOX("The function name cannot be empty.");
        return;
    }
    //Create a py file
    char szPathTemp[MAX_PATH];
    GetModuleFileNameA(NULL, szPathTemp, MAX_PATH);
    tstring strEXEPath(szPathTemp);
    int nPos = strEXEPath.rfind("\\");
    if (nPos != tstring::npos)
    {
        strEXEPath = strEXEPath.substr(0, nPos);
        strEXEPath += "\\";
        std::string strFileName("TemplateFile.py");
        strFileName = strEXEPath + strFileName;

        Poco::FileOutputStream outputStream(strFileName);
        tstring strLineText;
        if (outputStream.good())
        {
            for (int i = 0; i < m_pythonEditor.GetLineCount(); ++i)
            {
                char buffer[MAX_LINE_LEN];
                m_pythonEditor.GetLine(i, buffer, MAX_LINE_LEN);
                outputStream << buffer << "\n";
            }
            outputStream.close();
        }
        HCURSOR holdcursor, hwaitcursor;
        hwaitcursor = LoadCursor(NULL,IDC_WAIT);
        holdcursor = ::SetCursor(hwaitcursor);

        try
        {
            CPythonFunctionExecuter executer("TemplateFile", m_strFunName.GetBuffer(m_strFunName.GetLength()));
            executer.addPath(strEXEPath);
            CString strPath;
            m_pathList.GetLBText(m_pathList.GetCurSel(), strPath);
            //The first parameter is source code path
            executer.PutArguments(strPath.GetBuffer(strPath.GetLength()));
            //The following path are selected column
            for (int i = 0; i < m_listArugments.GetItemCount(); ++i)
            {
                if (m_listArugments.GetCheck(i))
                {
                    CString strValue = m_listArugments.GetItemText(i, 2);
                    executer.PutArguments(strValue.GetBuffer(strValue.GetLength()));
                }
            }

            strEXEPath = executer.Execute();
            m_strResult = strEXEPath.c_str();
            UpdateData(FALSE);
        }
        catch (const std::exception& e)
        {
            tstring strMsg("Failed to validate. Exception:");
            strMsg += e.what();
            ERROR_MESSAGEBOX(strMsg.c_str());
        }
        catch (...)
        {
            ERROR_MESSAGEBOX("Failed to validate. Unknown Exception.");
        }
        ::SetCursor(holdcursor);
    }
}


BOOL CPythonMethodEditDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    //Init list control
    LV_COLUMN lv;
    lv.mask = LVCF_TEXT | LVCF_WIDTH;
    lv.pszText = "Enable";
    lv.cx = 50;
    m_listArugments.InsertColumn(0, &lv);
    lv.cx = 100;
    lv.pszText = "Column";
    m_listArugments.InsertColumn(1, &lv);
    lv.cx = 150;
    lv.pszText = "Value";
    m_listArugments.InsertColumn(2, &lv);
    m_listArugments.SetExtendedStyle(m_listArugments.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

    char buff[32];
    for (int i = 0; i < m_columnValues.size(); ++i)
    {
        sprintf(buff, "Column%d", i);
        m_listArugments.InsertItem(i, "");
        m_listArugments.SetItemText(i, 1, buff);
        m_listArugments.SetItemText(i, 2, m_columnValues.at(i));
    }

    //All columns
    for (int i = 0; i < m_enableColumns.size(); ++i)
        m_listArugments.SetCheck(m_enableColumns.at(i));

    if (m_sourceCodeArray.size() > 0)
    {
        m_pythonEditor.Clear();
        CString strSourceCode;
        for (int i = 0; i < m_sourceCodeArray.size(); ++i)
            strSourceCode += m_sourceCodeArray.at(i);
        m_pythonEditor.SetWindowText(strSourceCode);
    }

    for (int i = 0; i < m_pathArray.size(); ++i)
        m_pathList.AddString(m_pathArray.at(i));

    m_pathList.SetCurSel(0);

    UpdateData(FALSE);
    return TRUE;
}


void CPythonMethodEditDlg::OnBnClickedOk()
{
    UpdateData(TRUE);

    if (m_strFunName.IsEmpty())
    {
        ERROR_MESSAGEBOX("The Function name cannot be empty.");
        return;
    }

    m_enableColumns.clear();
    for (int i = 0; i < m_listArugments.GetItemCount(); ++i)
    {
        if (m_listArugments.GetCheck(i) == TRUE)
            m_enableColumns.push_back(i);
    }

    m_sourceCodeArray.clear();
    for (int i = 0; i < m_pythonEditor.GetLineCount(); ++i)
    {
        char buff[MAX_LINE_LEN];
        m_pythonEditor.GetLine(i, buff, MAX_LINE_LEN);
        m_sourceCodeArray.push_back(buff);
    }

    CDialogEx::OnOK();
}
