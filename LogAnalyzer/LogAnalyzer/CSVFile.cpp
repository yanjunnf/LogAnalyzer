#include "stdafx.h"
#include "CSVFile.h"
#include "Poco\FileStream.h"

void CCSVFile::AppendColumn(const tstring& strColumn)
{
    int nSize = m_dataMap.size();
    Poco::SharedPtr<CColumn> pColumn = new CColumn;
    if (pColumn)
    {
        pColumn->m_strColumnName = strColumn;
        m_dataMap[nSize] = pColumn;
    }
}

void CCSVFile::AppendRow()
{
    std::map<int, Poco::SharedPtr<CColumn>>::iterator ite = m_dataMap.begin();
    while (ite != m_dataMap.end())
    {
        ite->second->m_rowData.push_back("");
        ++ite;
    }
}

void CCSVFile::DeleteColumn(int nColumn)
{
    if (nColumn < m_dataMap.size())
        m_dataMap.erase(nColumn);
}

void CCSVFile::DeleteRow(int nRow)
{
    if (m_dataMap.size() > 0)
    {
        int nRowCount = m_dataMap[0]->m_rowData.size();
        if (nRow < nRowCount)
        {
            std::map<int, Poco::SharedPtr<CColumn>>::iterator ite = m_dataMap.begin();
            while (ite != m_dataMap.end())
            {
                ite->second->m_rowData.erase(ite->second->m_rowData.begin() + nRow);
                ++ite;
            }
        }
    }
}

void CCSVFile::InsertData(int nColumn, int nRow, const tstring& strData)
{
    if (nColumn < m_dataMap.size())
    {
        int nRowCount = m_dataMap[0]->m_rowData.size();
        if (nRow < nRowCount)
            m_dataMap[nColumn]->m_rowData[nRow] = strData;
    }
}

void CCSVFile::SaveData()
{
    Poco::FileStream fileStream(m_strFileName);
    
    if (m_strEncoding == "UTF-8")
    {
        //Write BOM Header
        fileStream << '\xEF';
        fileStream << '\xBB';
        fileStream << '\xBF';
    }

    if (m_dataMap.size() > 0)
    {
        //Writer column
        std::map<int, Poco::SharedPtr<CColumn>>::iterator ite = m_dataMap.begin();
        tstring strLine;
        while (ite != m_dataMap.end())
        {
            strLine += ite->second->m_strColumnName;
            strLine += m_strSperator;
            ++ite;
        }
        //Remove the last separator character
        strLine = strLine.substr(0, strLine.length() - m_strSperator.length());
        fileStream << strLine << std::endl;

        //Write Row
        int nRowCount = m_dataMap[0]->m_rowData.size();
        for (int i = 0; i < nRowCount; ++i)
        {
            strLine.clear();
            ite = m_dataMap.begin();
            while (ite != m_dataMap.end())
            {
                strLine += ite->second->m_rowData[i];
                strLine += m_strSperator;
                ++ite;
            }
            //Remove the last separator character
            strLine = strLine.substr(0, strLine.length() - m_strSperator.length());
            fileStream << strLine << std::endl;
        }
        
    }
    
    fileStream.close();
}

#ifdef DEBUG
void CCSVFile::Test()
{
    CCSVFile file("C:\\haode.csv");
    file.AppendColumn("Hello");
    file.AppendColumn("World");
    file.AppendColumn("你好");
    file.AppendRow();
    file.AppendRow();

    file.InsertData(0, 0, "1");
    file.InsertData(0, 1, "2");        
    file.InsertData(1, 0, "3");
    file.InsertData(1, 1, "4");
    file.InsertData(2, 0, "12");
    file.InsertData(2, 1, "22");
    file.SaveData();
}
#endif