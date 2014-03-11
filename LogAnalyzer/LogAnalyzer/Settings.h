#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <map>
#include <vector>
#include "Poco/FileStream.h"
#include "CommonHeader.h"
#include "Poco\SharedPtr.h"

class CColumnColor
{
public:
    tstring m_strCondition;
    COLORREF m_nColor;
};

//The columns will be stored in order.
class CColumnSetting
{
public:
    CColumnSetting() : m_bEnable(TRUE), m_nColumnIndex(0){}
    BOOL m_bEnable;
    int m_nColumnIndex;
    //This field is unique
    tstring m_strRealColumnName;
    //Different condition shows different color
    std::vector<Poco::SharedPtr<CColumnColor>> m_columnColorArray;
};

class CSourceCoePath
{
public:
    CSourceCoePath() : m_bEnable(TRUE), m_nColumnIndex(0){}
    BOOL m_bEnable;
    int m_nColumnIndex;
    tstring m_strCondition;
	//Can store multi-paths, separate with ';'
    tstring m_strSourceCodePath;
};

class CTraceLog
{
public:
    CTraceLog() : m_nLine(0), m_nLineInSourceFile(-1){}
    int m_nLine;
    int m_nLineInSourceFile;
    tstring m_strSourceCodeFile;
    tstring m_strMessageFile;
    std::vector<tstring> m_traceArray;
};

//This class is used for UI Setting
class CVirtualComponent
{
public:
    CVirtualComponent(const tstring& strComponentName, int nColumnIndex, const tstring& strCondition, BOOL bEnable = TRUE) : 
      m_bEnable(bEnable), m_strComponentName(strComponentName), m_nColumnIndex(nColumnIndex), m_strCondition(strCondition){}
    virtual CVirtualComponent::~CVirtualComponent(){};
    const tstring& GetComponentName()const {return m_strComponentName;}
	int GetColumnIndex()const{return m_nColumnIndex;}
	tstring GetCondition()const{return m_strCondition;}
	BOOL GetEnable()const{return m_bEnable;}

protected:
    BOOL m_bEnable;
    tstring m_strComponentName;
    int m_nColumnIndex;
    tstring m_strCondition;
};

//This class is used for showing UI
class CComponent
{
public:
    CComponent(const tstring& strComponentName) : m_strComponentName(strComponentName){}
      virtual CComponent::~CComponent(){};
      const tstring& GetComponentName()const {return m_strComponentName;}
      void InsertLog(const Poco::SharedPtr<CTraceLog> log);
      void RemoveAllTraces();
      const std::vector<Poco::SharedPtr<CTraceLog>>& GetTraceLogs()const {return m_vectTraceLogs;}

protected:
    tstring m_strComponentName;
    std::vector<Poco::SharedPtr<CTraceLog>> m_vectTraceLogs;
};

class CPythonModule
{
public:
    CPythonModule(){}
    void SetFunctionName(const tstring& strFunctionName){m_strFunctionName = strFunctionName;};
    void SetSourceCode(const std::vector<tstring>& sourceCode){m_sourceCodeArray = sourceCode;}
    void InsertSourceCodeLine(const tstring& strLine){m_sourceCodeArray.push_back(strLine);}
    void SetArgColumns(const std::vector<int>& argColumns){m_argColumns = argColumns;};
    void ClearSourceCode(){m_sourceCodeArray.clear();}

    tstring GetFunctionName()const {return m_strFunctionName;}
    const std::vector<tstring>& GetSourceCode()const {return m_sourceCodeArray;}
    const std::vector<int>& GetArgColumns()const {return m_argColumns;}

protected:
    tstring m_strFunctionName;
    std::vector<tstring> m_sourceCodeArray;
    std::vector<int> m_argColumns;
};

class CUISetting
{
public:
    //Read setting from file
    CUISetting() : m_nColumnIdentifer(-1), m_bColumnIdentifier(FALSE), m_bPythonPlugin(FALSE), m_bModified(FALSE){};
    CUISetting(const tstring& strFileName) : m_strFileName(strFileName), m_nColumnIdentifer(-1), m_bColumnIdentifier(FALSE), m_bPythonPlugin(FALSE), m_bModified(FALSE){ParseSettingFile(m_strFileName);};
    //Create a new setting
    CUISetting(const tstring& strSettingName, const tstring& strTraceExample, const tstring& strRegularExpression);
	~CUISetting();

public:
    void SetName(const tstring& strName){m_bModified = TRUE; m_strSettingName = strName;}
    tstring getName()const{return m_strSettingName;}

    void SetTraceExample(const tstring& strTraceExample){m_bModified = TRUE; m_strTraceExample = strTraceExample;}
    tstring GetTraceExample()const{return m_strTraceExample;}

    void SetRegularExpression(const tstring& strRegularExpression){m_bModified = TRUE; m_strRegularExpression = strRegularExpression;}
    tstring GetRegularExpression()const{return m_strRegularExpression;}

    void SetNodepadPath(const tstring& strNodepadPath){m_bModified = TRUE; m_strNotepadPath = strNodepadPath;}
    tstring GetNodepadPath()const{return m_strNotepadPath;}

    void InsertColumn(Poco::SharedPtr<CColumnSetting> column){m_bModified = TRUE; m_columns.push_back(column);}
    void DeleteColumn(const tstring& columnName);
    const std::vector<Poco::SharedPtr<CColumnSetting>>& GetAllColumns()const{return m_columns;}
    void ClearAllColumns(){m_bModified = TRUE; m_columns.clear();}

    void InsertSourcePath(Poco::SharedPtr<CSourceCoePath> sourcePath){m_bModified = TRUE; m_sourcePath.push_back(sourcePath);}
    void DeleteSourcePath(const int nColumn);
    const std::vector<Poco::SharedPtr<CSourceCoePath>>& GetAllSourceCodePath()const{return m_sourcePath;}
    void ClearAllSourcePath(){m_bModified = TRUE; m_sourcePath.clear();}

	void InsertComponent(Poco::SharedPtr<CVirtualComponent> component){m_bModified = TRUE; m_components.push_back(component);}
	void DeleteComponent(const tstring& component);
	const std::vector<Poco::SharedPtr<CVirtualComponent>>& GetAllComponent()const{return m_components;}
	void ClearAllComponent(){m_bModified = TRUE; m_components.clear();}

    void SetModifyFlag(BOOL bModify){m_bModified = bModify;}
    BOOL IsModify()const{return m_bModified;}
    
    void SetColumnIdentifierEnable(BOOL bIdentifier){m_bModified = TRUE; m_bColumnIdentifier = bIdentifier;}
    BOOL GetColumnIdentifierEnable()const{return m_bColumnIdentifier;}
    void SetColumnIdentifier(int columnIndex){m_bModified = TRUE; m_nColumnIdentifer = columnIndex;}
    int GetColumnIdentifier()const{return m_nColumnIdentifer;}

    Poco::SharedPtr<CPythonModule> GetPythonModule()
    {
        if (m_pythonModule.isNull())
            m_pythonModule = new CPythonModule;
        return m_pythonModule;
    }
    void SetPythonModuleFlag(BOOL bPython){m_bPythonPlugin = bPython;}
    BOOL GetPythonModuleFlag()const{return m_bPythonPlugin;}

    void ParseSettingFile(const tstring& strFile);
    void SaveSetting();

    BOOL IsAvailable()const;

private:
    void ReadLine(Poco::FileInputStream& stream, tstring& strLine, BOOL bTrim = TRUE);

protected:
    static tstring SETTING_SETTING_NAME;
    static tstring SETTING_TRACE_EXAMPLE;
    static tstring SETTING_REGULAR_EXPRESSION;
    static tstring SETTING_COLUMN;
    static tstring SETTING_COMPONENT;
    static tstring SETTING_PATH;
    static tstring SETTING_OTHER;
    static tstring SETTING_NOTEPADD_PATH;
    static tstring SETTING_COLUMN_IDENTIFER;

    static tstring SETTING_NAME;
    static tstring SETTING_VALUE;
    static tstring SETTING_COLOR_CONDITION;
    static tstring SETTING_COLOR;
    static tstring SETTING_ENABLE;
    static tstring SETTING_COLUMN_INDEX;
    static tstring SETTING_CONTIAN;
    static tstring SETTING_SOURCE_PATH;
    static tstring SETTING_PYTHON_MODULE;
    static tstring SETTING_PYTHON_SOURCE_CODE;

protected:
    tstring m_strFileName;
    tstring m_strSettingName;
    tstring m_strTraceExample;
    tstring m_strRegularExpression;
    tstring m_strNotepadPath;
    //This property indicates which column can be used to locate the trace in source file
    BOOL m_bColumnIdentifier;
    BOOL m_bPythonPlugin;
    int m_nColumnIdentifer;

    //Columns & Source Code Path
    std::vector<Poco::SharedPtr<CColumnSetting>> m_columns;
    std::vector<Poco::SharedPtr<CSourceCoePath>> m_sourcePath;
	std::vector<Poco::SharedPtr<CVirtualComponent>> m_components;
    Poco::SharedPtr<CPythonModule> m_pythonModule;
    
    BOOL m_bModified;
};

#endif //__SETTINGS_H__