// InPlaceEditPropItemString.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceEditPropItemString.h"

#include "UserMessageID.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditPropItemString

IMPLEMENT_DYNAMIC(CInPlaceEditPropItemString, CEdit)

void CInPlaceEditPropItemString::MoveControl(CRect& rect)
{
	rect.DeflateRect(0, 1);
	MoveWindow(rect, FALSE);
}

void CInPlaceEditPropItemString::ProcessData()
{
	GetOwner()->SendMessage(WM_USER_SET_DATA);
}

CWnd* CInPlaceEditPropItemString::CreateInPlaceEditPropItemString(const CString& strInitText, CWnd* pWndParent, CRect& rect, DWORD exDwStyle)
{
	CInPlaceEditPropItemString* pEdit = new CInPlaceEditPropItemString(strInitText);
	rect.DeflateRect(0, 1);
	pEdit->Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT|exDwStyle, rect, pWndParent, 1);
	//pEdit->CreateEx(WS_EX_TRANSPARENT, _T("EDIT"), NULL, WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_LEFT|exDwStyle, rect, pWndParent, 1);

	return pEdit;
}