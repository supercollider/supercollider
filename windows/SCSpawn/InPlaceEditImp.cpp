// InPlaceEditImp.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceEditImp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditImp

void CInPlaceEditImp::SetText(const CString& strText)
{
	m_strInitText = strText;

	SetWindowText(strText);
	SetSel(0, -1);
}

BOOL CInPlaceEditImp::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
			case VK_ESCAPE:
			case VK_RETURN:
			case VK_TAB:
				::PeekMessage(pMsg, NULL, NULL, NULL, PM_REMOVE);
				GetParent()->SetFocus();
				return TRUE;
			default:
				;
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CInPlaceEditImp, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEditImp)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEditImp message handlers

int CInPlaceEditImp::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CEdit::OnCreate(lpCreateStruct) == -1) 
		return -1;

	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	SetWindowText(m_strInitText);

	return 0;
}

void CInPlaceEditImp::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	ProcessData();
}

BOOL CInPlaceEditImp::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}
