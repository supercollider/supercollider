#if !defined(_INPLACEEDITIMP_H)
#define _INPLACEEDITIMP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceEditImp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditImp

class CInPlaceEditImp : public CEdit
{
	CInPlaceEditImp(const CInPlaceEditImp& d);
	CInPlaceEditImp& operator=(const CInPlaceEditImp& d);

public:
	CInPlaceEditImp(const CString& srtInitText);
	virtual ~CInPlaceEditImp();

	// Attributes
	void SetText(const CString& strText);

	// Operations
	virtual void ProcessData();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEditImp)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEditImp)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Data
protected:
	CString m_strInitText;
};

inline CInPlaceEditImp::CInPlaceEditImp(const CString& strInitText)
	: m_strInitText(strInitText)
{
}

inline CInPlaceEditImp::~CInPlaceEditImp()
{
}

inline void CInPlaceEditImp::ProcessData()
{
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_INPLACEEDITIMP_H)
