#if !defined(_INPLACEEDITPROPITEMSTRING_H)
#define _INPLACEEDITPROPITEMSTRING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceEditPropItemString.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "InPlaceEditImp.h"

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditPropItemString

class CInPlaceEditPropItemString : public CInPlaceEditImp
{
	CInPlaceEditPropItemString(const CInPlaceEditPropItemString& d);
	CInPlaceEditPropItemString& operator=(const CInPlaceEditPropItemString& d);
protected:
	DECLARE_DYNAMIC(CInPlaceEditPropItemString)

public:
	CInPlaceEditPropItemString(const CString& srtInitText);
	~CInPlaceEditPropItemString();

	// Operations
	virtual void ProcessData();
	void MoveControl(CRect& rect);

	static CWnd* CreateInPlaceEditPropItemString(const CString& strInitText, CWnd* pWndParent, CRect& rect, DWORD exDwStyle = 0);
};

inline CInPlaceEditPropItemString::CInPlaceEditPropItemString(const CString& srtInitText) 
	: CInPlaceEditImp(srtInitText)
{
}

inline CInPlaceEditPropItemString::~CInPlaceEditPropItemString() 
{
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_INPLACEEDITPROPITEMSTRING_H)