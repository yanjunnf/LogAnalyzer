#include "stdafx.h"
#include "TraceLogger.h"
#include "PythonFunctionExecuter.h"

#pragma comment(lib, "python33.lib")

void CPythonFunctionExecuter::Init()
{
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("import os.path");
    PyRun_SimpleString("sys.path.append('./')");
    
    m_pModule = PyImport_ImportModule(m_strModuleName.c_str());
    if (m_pModule != NULL)
        m_pFunction = PyObject_GetAttrString(m_pModule, m_strFunctionName.c_str());
    else
        gTraceLogger.getLogger().error("ERROR - CPythonFunctionExecuter::Init Cannot create pyhone module. Module Name=" + m_strModuleName);
}

void CPythonFunctionExecuter::addPath(const tstring& strPath)
{
    if (!strPath.empty())
        PyRun_SimpleString(strPath.c_str());
}

CPythonFunctionExecuter::~CPythonFunctionExecuter()
{
    if (m_pFunction)
        Py_DecRef(m_pFunction);
    if (m_pModule)
        Py_DecRef(m_pModule);

    Py_Finalize();
}

tstring CPythonFunctionExecuter::Execute()
{
    tstring strResult;
    try
    {
        if (m_argumentArray.size() > 0 && !m_strModuleName.empty() && !m_strFunctionName.empty())
        {
            const char* pCharSresult = NULL;
            PyObject* pModule = NULL;
            PyObject* pFunc = NULL;
            PyObject* pArgs = NULL;

            pArgs = PyTuple_New(m_argumentArray.size());

            for (int i = 0; i < (int)m_argumentArray.size(); ++i)
            {
                PyObject* pValue = Py_BuildValue("s", m_argumentArray.at(i).c_str());
                PyTuple_SetItem(pArgs, i, pValue);
            }

            if (m_pFunction)
            {
                PyObject* pResult = PyObject_CallObject(m_pFunction, pArgs);
                if (pResult != NULL)
                {
                    PyArg_Parse(pResult, "s", &pCharSresult);
                    if (pCharSresult != NULL)
                        strResult = pCharSresult;
                    Py_DecRef(pResult);
                }
                else
                    strResult = "No Return Value";
            }
            else
            {
                if (m_pModule == NULL)
                    strResult = "Failed to import module, please check the python source code.";
                else
                    strResult = "Failed to get this function : " + m_strFunctionName;
            }
            if (pArgs != NULL)
                Py_DecRef(pArgs);
        }
    }
    catch (const std::exception& e)
    {
        gTraceLogger.getLogger().critical(tstring("Failed to execute python function. Exception : ") + e.what());
        throw e;
    }
    catch (...)
    {
        gTraceLogger.getLogger().critical(tstring("Failed to execute python function. Unknown exception : "));
        throw;
    }
    
    return strResult;
}