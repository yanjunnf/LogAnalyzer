#ifndef __PYTHON_FUNCTION_EXECUTER__
#define __PYTHON_FUNCTION_EXECUTER__

#include "CommonHeader.h"
#include <vector>
#include <string>
#include <Python.h>

class CPythonFunctionExecuter
{
public:
    CPythonFunctionExecuter(const tstring& strModuleName, const tstring& strFunctionName) : 
      m_pModule(NULL), m_pFunction(NULL), m_strModuleName(strModuleName), m_strFunctionName(strFunctionName){Init();};
      tstring Execute();
      void addPath(const tstring& strPath);
      void PutArguments(const tstring& strArgument){m_argumentArray.push_back(strArgument);};
      ~CPythonFunctionExecuter();

protected:
    void Init();

protected:
    PyObject* m_pModule;
    PyObject* m_pFunction;
    tstring m_strModuleName;
    tstring m_strFunctionName;
    std::vector<tstring> m_argumentArray; 
};

#endif //__PYTHON_FUNCTION_EXECUTER__