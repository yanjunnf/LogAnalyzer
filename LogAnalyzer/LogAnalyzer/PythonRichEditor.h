#pragma once


// CPythonRichEditor

class CPythonRichEditor : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CPythonRichEditor)

public:
	CPythonRichEditor();
	virtual ~CPythonRichEditor();

protected:
    //afx_msg UINT OnGetDlgCode();
	DECLARE_MESSAGE_MAP()
};


