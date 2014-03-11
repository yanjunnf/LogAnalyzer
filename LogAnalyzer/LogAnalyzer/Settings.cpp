#include "stdafx.h"
#include "Settings.h"
#include "TraceLogger.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/String.h"
#include "Poco/File.h"
#include "Poco/RegularExpression.h"
#include "Poco/Environment.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/NumberParser.h"
#include "Tools.h"
#include <exception> 
#include <windows.h>
#include <set>
//////////////////////////////////////////////////////////////////////////
//CUISetting

tstring CUISetting::SETTING_SETTING_NAME = "[SETTING_NAME]";
tstring CUISetting::SETTING_TRACE_EXAMPLE = "[TRACE_EXAMPLE]";
tstring CUISetting::SETTING_REGULAR_EXPRESSION = "[TRACE_REGULAR_EXPRESSION]";
tstring CUISetting::SETTING_COLUMN = "[COLUMN]";
tstring CUISetting::SETTING_COMPONENT = "[COMPONENT]";
tstring CUISetting::SETTING_PATH = "[PATH]";
tstring CUISetting::SETTING_OTHER = "[OTHER]";
tstring CUISetting::SETTING_NOTEPADD_PATH = "[NOTEPAD_PATH]";
tstring CUISetting::SETTING_COLUMN_IDENTIFER = "[COLUMN_IDENTIFIER]";

tstring CUISetting::SETTING_NAME = "NAME";
tstring CUISetting::SETTING_VALUE = "VALUE";
tstring CUISetting::SETTING_COLOR_CONDITION = "COLOR_CONDITION";
tstring CUISetting::SETTING_COLOR = "COLOR";
tstring CUISetting::SETTING_ENABLE = "ENABLE";
tstring CUISetting::SETTING_COLUMN_INDEX = "COLUMN_INDEX";
tstring CUISetting::SETTING_CONTIAN = "CONTAIN";
tstring CUISetting::SETTING_SOURCE_PATH = "SOURCE_PATH";
//Python
tstring CUISetting::SETTING_PYTHON_MODULE = "[PYTHON_MODULE]";
tstring CUISetting::SETTING_PYTHON_SOURCE_CODE = "[PYTHON_SOURCE_CODE]";

CUISetting::CUISetting(const tstring& strSettingName, const tstring& strTraceExample, const tstring& strRegularExpression):
    m_strSettingName(strSettingName), m_strTraceExample(strTraceExample), m_strRegularExpression(strRegularExpression), 
    m_nColumnIdentifer(-1), m_bColumnIdentifier(FALSE), m_bPythonPlugin(FALSE), m_bModified(FALSE)
{
}

void CUISetting::DeleteColumn(const tstring& columnName)
{
    m_bModified = TRUE; 
    if (columnName.empty())
    {
        std::vector<Poco::SharedPtr<CColumnSetting>>::iterator ite = m_columns.begin();
        while (ite != m_columns.end())
        {
            if ((*ite)->m_strRealColumnName == columnName)
            {
                m_columns.erase(ite);
                break;
            }
            ++ite;
        }
    }
}

void CUISetting::DeleteSourcePath(const int nColumn)
{
    m_bModified = TRUE; 
    if (nColumn >= 0)
    {
        std::vector<Poco::SharedPtr<CSourceCoePath>>::iterator ite = m_sourcePath.begin();
        while (ite != m_sourcePath.end())
        {
            if ((*ite)->m_nColumnIndex == nColumn)
            {
                m_sourcePath.erase(ite);
                break;
            }
            ++ite;
        }
    }
}

void CUISetting::DeleteComponent(const tstring& component)
{
    m_bModified = TRUE; 
	if (m_components.empty())
	{
		std::vector<Poco::SharedPtr<CVirtualComponent>>::iterator ite = m_components.begin();
		while (ite != m_components.end())
		{
			if ((*ite)->GetComponentName() == component)
			{
				m_components.erase(ite);
				break;
			}
			++ite;
		}
	}
}

CUISetting::~CUISetting()
{
    SaveSetting();
}

void CUISetting::ReadLine(Poco::FileInputStream& stream, tstring& strLine, BOOL bTrim)
{
    while (!stream.eof())
    {
        std::getline(stream, strLine);
        if (bTrim)
            strLine = Poco::trim(strLine);

        if (strLine.empty() || strLine.at(0) == '#')
        {
            strLine.clear();
            continue;
        }
        break;
    }
}

void CUISetting::ParseSettingFile(const tstring& strFile)
{
    try
    {
        if (!m_strFileName.empty())
        {
            Poco::File settingFile(m_strFileName);
            if (settingFile.exists())
            {
                int nIndex = 0;
                Poco::FileInputStream fileStream(m_strFileName);
                tstring strLineText;
                if (fileStream.good())
                {
                    std::vector<tstring> results;
                    while (!fileStream.eof())
                    {
                        ReadLine(fileStream, strLineText);
                        if (!strLineText.empty() && strLineText.at(0) !='#')
                        {
                            if (strLineText == SETTING_SETTING_NAME)
                            {
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_NAME)
                                    m_strSettingName = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get property:" + SETTING_TRACE_EXAMPLE).c_str());
                            }
                            else if (strLineText == SETTING_TRACE_EXAMPLE)
                            {  
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_VALUE)
                                    m_strTraceExample = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get property:" + SETTING_TRACE_EXAMPLE).c_str());
                            }
                            else if (strLineText == SETTING_REGULAR_EXPRESSION)
                            {
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_VALUE)
                                    m_strRegularExpression = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get property:" + SETTING_REGULAR_EXPRESSION).c_str());
                            }
                            else if (strLineText == SETTING_COLUMN)
                            {
                                Poco::SharedPtr<CColumnSetting> column = new CColumnSetting;
                                
                                //Name
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_NAME)
                                    column->m_strRealColumnName = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get name property:" + SETTING_REGULAR_EXPRESSION).c_str());

                                //Enable
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_ENABLE)
                                    column->m_bEnable = Poco::toUpper(results.at(1)) == "TRUE" ? TRUE : FALSE;
                                else
                                    throw std::exception(tstring("An error when get enable property:" + SETTING_REGULAR_EXPRESSION).c_str());
                                column->m_nColumnIndex = nIndex++;

                                m_columns.push_back(column);
                            }
                            else if (strLineText == SETTING_COLOR_CONDITION)
                            {
                                Poco::SharedPtr<CColumnColor> color = new CColumnColor;
                                //Condition
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_CONTIAN)
                                    color->m_strCondition = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get contain property:" + SETTING_COLOR_CONDITION).c_str());
                                
                                //Color value
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_COLOR)
                                    color->m_nColor = Poco::NumberParser::parse(results.at(1));
                                else
                                    throw std::exception(tstring("An error when get color property:" + SETTING_COLOR_CONDITION).c_str());
                                
                                m_columns.at(m_columns.size()-1)->m_columnColorArray.push_back(color);
                            }
                            else if (strLineText == SETTING_COMPONENT)
                            {
                                int nColumnIndex = 0;
                                tstring strComponentName;
                                tstring strContian;
                                BOOL bEnable = TRUE;
                                //Column index
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_COLUMN_INDEX)
                                    nColumnIndex = Poco::NumberParser::parse(results.at(1));
                                else
                                    throw std::exception(tstring("An error when get column index :" + SETTING_COMPONENT).c_str());

                                //Name
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_NAME)
                                    strComponentName = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get component name:" + SETTING_COMPONENT).c_str());

                                //Contain
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_CONTIAN)
                                    strContian = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get contain:" + SETTING_COMPONENT).c_str());
                                
                                //Enable
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_ENABLE)
                                    bEnable = Poco::toUpper(results.at(1)) == "TRUE" ? TRUE : FALSE;
                                else
                                    throw std::exception(tstring("An error when get enable:" + SETTING_COMPONENT).c_str());
                                
                                Poco::SharedPtr<CVirtualComponent> component = new CVirtualComponent(strComponentName, nColumnIndex, strContian, bEnable);
                                m_components.push_back(component);
                            }
                            else if (strLineText == SETTING_PATH)
                            {
                                Poco::SharedPtr<CSourceCoePath> sourcePath = new CSourceCoePath;
                                //Column index
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_COLUMN_INDEX)
                                    sourcePath->m_nColumnIndex = Poco::NumberParser::parse(results.at(1));
                                else
                                    throw std::exception(tstring("An error when get column index :" + SETTING_PATH).c_str());

                                //Contain
                                std::getline(fileStream, strLineText);
                                strLineText = Poco::trim(strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_CONTIAN)
                                    sourcePath->m_strCondition = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get contain:" + SETTING_PATH).c_str());

                                //Source Path
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_SOURCE_PATH)
                                    sourcePath->m_strSourceCodePath = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get source path:" + SETTING_PATH).c_str());

                                //Enable
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_ENABLE)
                                    sourcePath->m_bEnable = Poco::toUpper(results.at(1)) == "TRUE" ? TRUE : FALSE;
                                else
                                    throw std::exception(tstring("An error when get enable:" + SETTING_PATH).c_str());

                                
                                m_sourcePath.push_back(sourcePath);
                            }
                            else if (strLineText == SETTING_NOTEPADD_PATH)
                            {
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_VALUE)
                                    m_strNotepadPath = results.at(1);
                                else
                                    throw std::exception(tstring("An error when get notepad++ path:" + SETTING_NOTEPADD_PATH).c_str());
                            }
                            else if (strLineText == SETTING_COLUMN_IDENTIFER)
                            {
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_VALUE)
                                {
                                    m_bColumnIdentifier = TRUE;
                                    m_nColumnIdentifer = Poco::NumberParser::parse(results.at(1));
                                }
                                else
                                    throw std::exception(tstring("An error when get column identifier:" + SETTING_NOTEPADD_PATH).c_str());
                            }
                            else if (strLineText == SETTING_PYTHON_MODULE)
                            {
                                m_pythonModule = new CPythonModule;
                                m_bPythonPlugin = TRUE;
                                //Function Name
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_NAME)
                                    m_pythonModule->SetFunctionName(results.at(1));
                                else
                                    m_bPythonPlugin = FALSE;

                                //Columns
                                ReadLine(fileStream, strLineText);
                                CTools::SplitString(strLineText, "=", results);
                                if (results.size() == 2 && results.at(0) == SETTING_VALUE)
                                {
                                    std::vector<int> argColumns_int;
                                    std::vector<tstring> argColumns_Str;
                                    CTools::SplitString(results.at(1), ",", argColumns_Str);
                                    for (int i = 0; i < argColumns_Str.size(); ++i)
                                        argColumns_int.push_back(Poco::NumberParser::parse(argColumns_Str.at(i)));
                                    m_pythonModule->SetArgColumns(argColumns_int);
                                }
                                else
                                    m_bPythonPlugin = FALSE;
                                //else
                                //    throw std::exception(tstring("An error when get column arguments for python function:" + SETTING_NOTEPADD_PATH).c_str());
                            }
                            else if (strLineText == SETTING_PYTHON_SOURCE_CODE)
                            {
                                //Value
                                if (!m_pythonModule.isNull())
                                {
                                    tstring strKey = SETTING_VALUE + "=";
                                    ReadLine(fileStream, strLineText);
                                    int nIndex = strLineText.find(strKey);
                                    if (nIndex != tstring::npos)
                                    {
                                        tstring strValue = strLineText.substr(strKey.length(), strLineText.length() - strKey.length());
                                        m_pythonModule->InsertSourceCodeLine(strValue);
                                    }
                                    else
                                        m_pythonModule->InsertSourceCodeLine("");
                                    //else
                                    //    throw std::exception(tstring("An error when get Python source code:" + SETTING_NOTEPADD_PATH).c_str());
                                }
                            }
                        }
                    }
                    fileStream.close();
                }
                else
                    gTraceLogger.getLogger().information(tstring("CUISetting::ParseSettingFile() - File is invalid : ") + m_strFileName);
            }
        }
    }
    catch (const std::exception& e)
    {
        gTraceLogger.getLogger().information(tstring("CUISetting::ParseSettingFile() - Exception : ") + e.what());
        throw e;
    }
}
void CUISetting::SaveSetting()
{
    if (m_bModified)
    {
        //Read settings from file
        TCHAR szPathTemp[512];
        GetModuleFileNameA(NULL, szPathTemp, MAX_PATH);
        tstring strSettingFile(szPathTemp);
        int nPos = strSettingFile.rfind("\\");
        if (nPos != tstring::npos)
        {
            strSettingFile = strSettingFile.substr(0, nPos);
            strSettingFile += "\\Setting\\";
            strSettingFile += m_strSettingName;
            strSettingFile += ".conf";
            //Poco::File settingFile(strSettingFile);
            //if (settingFile.exists())
            {
                Poco::FileOutputStream fileStream(strSettingFile);
                tstring strLineText;
                if (fileStream.good())
                {
                    //Setting Name
                    fileStream << SETTING_SETTING_NAME << "\n";
                    fileStream << "  " << SETTING_NAME << "=" << m_strSettingName << "\n";
                    //Trace  example
                    fileStream << SETTING_TRACE_EXAMPLE << "\n";
                    fileStream << "  " << SETTING_VALUE << "=" << m_strTraceExample << "\n";

                    //Regular expression
                    fileStream << SETTING_REGULAR_EXPRESSION << "\n";
                    fileStream << "  " << SETTING_VALUE << "=" << m_strRegularExpression << "\n";

                    //Column                    
                    for (int i = 0; i < m_columns.size(); ++i)
                    {
                        fileStream << SETTING_COLUMN << "\n";
                        fileStream << "  " << SETTING_NAME << "=" << m_columns.at(i)->m_strRealColumnName << "\n";
                        //Enable
                        if (m_columns.at(i)->m_bEnable)
                            fileStream << "  " << SETTING_ENABLE << "=TRUE" << "\n";
                        else
                            fileStream << "  " << SETTING_ENABLE << "=FALSE" << "\n";
                        //Color condition
                        for (int j = 0; j < m_columns.at(i)->m_columnColorArray.size(); ++j)
                        {
                            fileStream << "  " << SETTING_COLOR_CONDITION << "\n";
                            fileStream << "    " << SETTING_CONTIAN  << "=" <<  m_columns.at(i)->m_columnColorArray.at(j)->m_strCondition << "\n";
                            fileStream << "    " << SETTING_COLOR << "=" <<  m_columns.at(i)->m_columnColorArray.at(j)->m_nColor << "\n";
                        }
                    }

                    //Component
                    for (int i = 0; i < m_components.size(); ++i)
                    {
                        fileStream << SETTING_COMPONENT << "\n";
                        fileStream << "  " << SETTING_COLUMN_INDEX << "=" << m_components.at(i)->GetColumnIndex() << "\n";
                        fileStream << "  " << SETTING_NAME << "=" << m_components.at(i)->GetComponentName() << "\n";
                        fileStream << "  " << SETTING_CONTIAN << "=" <<  m_components.at(i)->GetCondition() << "\n";
                        fileStream << "  " << SETTING_ENABLE << "=" << (m_components.at(i)->GetEnable() ? "TRUE" : "FALSE") << "\n";
                    }
                    //Path
                    for (int i = 0; i < m_sourcePath.size(); ++i)
                    {
                        fileStream << SETTING_PATH<< "\n";
                        fileStream << "  " << SETTING_COLUMN_INDEX << "=" << m_sourcePath.at(i)->m_nColumnIndex << "\n";
                        fileStream << "  " << SETTING_CONTIAN << "=" << m_sourcePath.at(i)->m_strCondition << "\n";
                        fileStream << "  " << SETTING_SOURCE_PATH << "=" <<  m_sourcePath.at(i)->m_strSourceCodePath << "\n";
                        fileStream << "  " << SETTING_ENABLE << "=" << (m_sourcePath.at(i)->m_bEnable ? "TRUE" : "FALSE") << "\n";
                    }

                    //Column Identifier
                    if (m_bColumnIdentifier)
                    {
                        fileStream << SETTING_COLUMN_IDENTIFER << "\n";
                        fileStream << "  " << SETTING_VALUE << "=" << m_nColumnIdentifer << "\n";
                    }
                    
                    //Python function
                    if (!m_pythonModule.isNull() && m_bPythonPlugin)
                    {
                        fileStream << SETTING_PYTHON_MODULE << "\n";
                        fileStream << "  " << SETTING_NAME << "=" << m_pythonModule->GetFunctionName() << "\n";
                        tstring strArgColumns;
                        char buff[32];
                        for (int i = 0; i < m_pythonModule->GetArgColumns().size(); ++i)
                        {
                            sprintf(buff, "%d,", m_pythonModule->GetArgColumns().at(i));
                            strArgColumns += buff;
                        }
                        if (strArgColumns.length() > 0)
                            strArgColumns = strArgColumns.substr(0, strArgColumns.length()-1);
                        fileStream << "  " << SETTING_VALUE << "=" << strArgColumns << "\n";

                        for (int i = 0; i < m_pythonModule->GetSourceCode().size(); ++i)
                        {
                            fileStream << "  "<< SETTING_PYTHON_SOURCE_CODE << "\n";
                            fileStream << "    " << SETTING_VALUE << "=" << m_pythonModule->GetSourceCode().at(i) << "\n";
                        }
                    }

                    //Notepad++ path
                    fileStream << SETTING_NOTEPADD_PATH << "\n";
                    fileStream << "  " << SETTING_VALUE << "=" << m_strNotepadPath << "\n";

                    fileStream.close();
                    m_bModified = FALSE;
                }
            }
        }
    }
}

BOOL CUISetting::IsAvailable()const
{
    BOOL bRet = TRUE;
    if (m_strTraceExample.empty())
        bRet = FALSE;
    else if (m_strRegularExpression.empty())
        bRet = FALSE;
    else if (m_strSettingName.empty())
        bRet = FALSE;

    return bRet;
}
//////////////////////////////////////////////////////////////////////////
//CVirtualComponent
void CComponent::RemoveAllTraces()
{
    m_vectTraceLogs.clear();
}

void CComponent::InsertLog(const Poco::SharedPtr<CTraceLog> log)
{
    m_vectTraceLogs.push_back(log);
}