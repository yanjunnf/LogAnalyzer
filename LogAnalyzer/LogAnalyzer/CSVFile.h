#ifndef __CSVFILE__
#define __CSVFILE__

#include "CommonHeader.h"
#include "Poco\SharedPtr.h"
#include <vector>
#include <map>

struct CColumn
{
    tstring m_strColumnName;
    std::vector<tstring> m_rowData;
};

class CCSVFile
{
public:
    CCSVFile(const tstring& strFileName) : m_strFileName(strFileName), m_strEncoding("UTF-8"), m_strSperator(","){}
    void AppendColumn(const tstring& strColumn);
    void AppendRow();

    void DeleteColumn(int nColumn);
    void DeleteRow(int nRow);

    void InsertData(int nColumn, int nRow, const tstring& strData);

    void SaveData();

#ifdef DEBUG
    void Test();
#endif
protected:
    tstring m_strFileName;
    //The default encoding is UTF-8
    tstring m_strEncoding;
    tstring m_strSperator;
    std::map<int, Poco::SharedPtr<CColumn>> m_dataMap;
};

#endif //__CSVFILE__