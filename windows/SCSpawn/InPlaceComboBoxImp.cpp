// InPlaceComboBoxImp.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceComboBoxImp.h"

#include "UserMessageID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_IPLISTBOX_HEIGHT		16 * 8

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBEditImp

BOOL CInPlaceCBEditImp::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		CWnd* pOwner = GetOwner();
		WPARAM nChar = pMsg->wParam;

		switch(nChar)
		{
			case VK_ESCAPE:
			case VK_RETURN:
			case VK_TAB:
				::PeekMessage(pMsg, NULL, NULL, NULL, PM_REMOVE);
				pOwner->GetParent()->SetFocus();
				return TRUE;
			case VK_UP:
			case VK_DOWN:
				pOwner->SendMessage(WM_USER_ON_NEW_SELECTION, nChar);
				return TRUE;
			default:
				;
		}
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CInPlaceCBEditImp, CEdit)
	//{{AFX_MSG_MAP(CInPlaceCBEditImp)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBEditImp message handlers

BOOL CInPlaceCBEditImp::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListBoxImp

CInPlaceCBListBoxImp::CInPlaceCBListBoxImp()
{
}

CInPlaceCBListBoxImp::~CInPlaceCBListBoxImp()
{
}

BEGIN_MESSAGE_MAP(CInPlaceCBListBoxImp, CListBox)
	//{{AFX_MSG_MAP(CInPlaceCBListBoxImp)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListBoxImp message handlers

void CInPlaceCBListBoxImp::ProcessSelected(bool bProcess)
{
	ReleaseCapture();

	CWnd* pOwner = GetOwner();

	if(bProcess)
	{
		int nSelectedItem = GetCurSel();
		pOwner->SendMessage(WM_USER_ON_SELECTION_OK, nSelectedItem, GetItemData(nSelectedItem));
	}
	else
		pOwner->SendMessage(WM_USER_ON_SELECTION_CANCEL);
}

void CInPlaceCBListBoxImp::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonUp(nFlags, point);

	CRect rect;
	GetClientRect(rect);

	if(rect.PtInRect(point))
		ProcessSelected();
	//else
	//	ReleaseCapture();
}

BOOL CInPlaceCBListBoxImp::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
			case VK_RETURN:
				ProcessSelected();
				return TRUE;
			case VK_ESCAPE:
			case VK_TAB:
				ProcessSelected(false);
				return TRUE;
			default:
				;
		}
	}

	return CListBox::PreTranslateMessage(pMsg);
}

#if 0
/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListCtrlImp

CInPlaceCBListCtrlImp::CInPlaceCBListCtrlImp()
{
}

CInPlaceCBListCtrlImp::~CInPlaceCBListCtrlImp()
{
}

int CInPlaceCBListCtrlImp::GetCount() const
{
	return GetItemCount();
}

void CInPlaceCBListCtrlImp::GetText(int nIndex, CString& strText) const
{
	strText = GetItemText(nIndex, 0);
}

int CInPlaceCBListCtrlImp::SetCurSel(int nSelect)
{	 
	return SetItemState(nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED) ? 0 : LB_ERR;
}

int CInPlaceCBListCtrlImp::AddString(LPCTSTR pStrText)
{
	return InsertItem(GetItemCount(), pStrText);
}

void CInPlaceCBListCtrlImp::ResetContent()
{
	DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CInPlaceCBListCtrlImp, CListCtrl)
	//{{AFX_MSG_MAP(CInPlaceCBListCtrlImp)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListCtrlImp message handlers

void CInPlaceCBListCtrlImp::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);

	GetOwner()->SendMessage(WM_USER_ON_SELECTION_CANCEL);
}

void CInPlaceCBListCtrlImp::ProcessSelected()
{
	int nSelectedItem = GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	GetOwner()->SendMessage(WM_USER_ON_SELECTION_OK, nSelectedItem, GetItemData(nSelectedItem));
}

void CInPlaceCBListCtrlImp::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonUp(nFlags, point);

	ProcessSelected();
}

BOOL CInPlaceCBListCtrlImp::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			ProcessSelected();
			return TRUE;
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxImp

int CInPlaceComboBoxImp::m_nButtonDx = ::GetSystemMetrics(SM_CXHSCROLL);

IMPLEMENT_DYNAMIC(CInPlaceComboBoxImp, CWnd)

CInPlaceComboBoxImp::CInPlaceComboBoxImp()
{
	m_nCurrentSelection = -1;
}

CInPlaceComboBoxImp* CInPlaceComboBoxImp::CreateInPlaceComboBox(CWnd* pWndParent, CRect& rect)
{	
	CInPlaceComboBoxImp* pInPlaceComboBox = (CInPlaceComboBoxImp*)new CInPlaceComboBoxImp;
	
	pInPlaceComboBox->Create(NULL, "", WS_VISIBLE|WS_CHILD, rect, pWndParent, 1);

	return pInPlaceComboBox;
}

BEGIN_MESSAGE_MAP(CInPlaceComboBoxImp, CWnd)
	//{{AFX_MSG_MAP(CInPlaceComboBoxImp)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_USER_ON_SELECTION_OK, OnSelectionOk)
	ON_MESSAGE(WM_USER_ON_SELECTION_CANCEL, OnSelectionCancel)
	ON_MESSAGE(WM_USER_ON_NEW_SELECTION, OnNewSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxImp message handlers

int CInPlaceComboBoxImp::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rect;
	GetClientRect(rect);
	rect.DeflateRect(0, 1);
	rect.right -= m_nButtonDx;

	CWnd* pParent = GetParent();
	ASSERT(pParent != NULL);

	CFont* pFont = pParent->GetFont();

	// Vstupný riadok
	m_wndEdit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rect, this, 2);
	m_wndEdit.SetOwner(this);
	m_wndEdit.SetFont(pFont);

	// Zoznam položiek
	rect.right += m_nButtonDx - 1;
	rect.top = rect.bottom + 2;
	rect.bottom += 100;

	ClientToScreen(rect);
	pParent = pParent->GetParent();
	pParent->ScreenToClient(rect);
		
	m_wndList.Create(WS_BORDER|WS_CHILD|WS_VSCROLL, rect, pParent, 3);
	m_wndList.SetOwner(this);
	m_wndList.SetFont(pFont);

	return 0;
}

void CInPlaceComboBoxImp::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
		
	m_wndEdit.SetWindowPos(NULL, 0, 0, cx - m_nButtonDx, cy, SWP_NOZORDER|SWP_NOMOVE);
}

void CInPlaceComboBoxImp::MoveControl(CRect& rect)
{
	CRect prevRect;
	GetClientRect(prevRect);

	CWnd* pParent = GetParent();

	// Prekresli predošlú pozíciu
	ClientToScreen(prevRect);
	pParent->ScreenToClient(prevRect);
	pParent->InvalidateRect(prevRect);

	// Nová pozícia
	MoveWindow(rect, FALSE);

	pParent->ClientToScreen(rect);
	pParent = pParent->GetParent();	
	pParent->ScreenToClient(rect);

	m_wndList.SetWindowPos(NULL, rect.left, rect.bottom + 1, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

void CInPlaceComboBoxImp::ResetListBoxHeight()
{
	CRect rect;

	GetClientRect(rect);
	rect.right -= 1;

	int nItems = m_wndList.GetCount();
	int nListBoxHeight = nItems > 0 ? nItems * m_nButtonDx : DEFAULT_IPLISTBOX_HEIGHT;

	if(nListBoxHeight > DEFAULT_IPLISTBOX_HEIGHT)
		nListBoxHeight = DEFAULT_IPLISTBOX_HEIGHT;

	m_wndList.SetWindowPos(NULL, 0, 0, rect.Width(), nListBoxHeight, SWP_NOZORDER|SWP_NOMOVE);
}

void CInPlaceComboBoxImp::OnPaint() 
{
	CPaintDC dc(this);
	
	// Nakresli tlaèítko
	CRect rect;

	GetClientRect(rect);
	rect.left = rect.right - m_nButtonDx;

#if 1
	dc.DrawFrameControl(rect, DFC_SCROLL, m_wndList.IsWindowVisible() ? 
		DFCS_SCROLLDOWN|DFCS_PUSHED : DFCS_SCROLLDOWN);
#else
	dc.DrawFrameControl(rect, DFC_SCROLL, m_wndList.IsWindowVisible() ? 
		DFCS_SCROLLDOWN|DFCS_PUSHED|DFCS_FLAT : DFCS_SCROLLDOWN|DFCS_FLAT);
#endif
}

BOOL CInPlaceComboBoxImp::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;	
}

void CInPlaceComboBoxImp::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	CRect rect;
	GetClientRect(rect);

	CRect rectButton(rect);
	rectButton.left = rectButton.right - m_nButtonDx;

	if(rectButton.PtInRect(point))
	{
		int nDoAction = m_wndList.IsWindowVisible() ? SW_HIDE : SW_SHOW;

		m_wndList.ShowWindow(nDoAction);
		InvalidateRect(rectButton, FALSE);

		if(nDoAction == SW_SHOW)
		{
			m_wndList.SetFocus();
			m_wndList.SetCapture();
		}
	}
}

void CInPlaceComboBoxImp::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	m_wndEdit.SetFocus();
}

void CInPlaceComboBoxImp::HideListBox()
{
	m_wndList.ShowWindow(SW_HIDE);

	CRect rectButton;

	GetClientRect(rectButton);
	rectButton.left = rectButton.right - m_nButtonDx;

	InvalidateRect(rectButton, FALSE);

	m_wndEdit.SetFocus();
}

LRESULT CInPlaceComboBoxImp::OnSelectionOk(WPARAM wParam, LPARAM /*lParam*/)
{
	HideListBox();

	SetCurSelToEdit(m_nCurrentSelection = int(wParam));

	GetOwner()->SendMessage(WM_USER_SET_DATA);

	return TRUE;
}

LRESULT CInPlaceComboBoxImp::OnSelectionCancel(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	HideListBox();

	return TRUE;
}

LRESULT CInPlaceComboBoxImp::OnNewSelection(WPARAM wParam, LPARAM /*lParam*/)
{
	int nItems = m_wndList.GetCount();

	if(nItems > 0)
	{
		if(wParam == VK_UP)
		{
			if(m_nCurrentSelection > 0)
				SetCurSel(m_nCurrentSelection - 1);
		}
		else
		{
			if(m_nCurrentSelection < nItems - 1)
				SetCurSel(m_nCurrentSelection + 1);
		}
	}

	return TRUE;
}

void CInPlaceComboBoxImp::SetCurSelToEdit(int nSelect)
{
	CString strText;

	if(nSelect != -1)
		m_wndList.GetText(nSelect, strText);
		
	m_wndEdit.SetWindowText(strText);
	m_wndEdit.SetSel(0, -1); 
}

int CInPlaceComboBoxImp::SetCurSel(int nSelect, bool bSendSetData)
{
	if(nSelect >= m_wndList.GetCount())
		return CB_ERR;


	int nRet = m_wndList.SetCurSel(nSelect);

	if(nRet != -1)
	{
		SetCurSelToEdit(nSelect);
		m_nCurrentSelection = nSelect;

		if(bSendSetData)
			GetOwner()->SendMessage(WM_USER_SET_DATA);
	}

	return nRet;
}

CString CInPlaceComboBoxImp::GetTextData() const
{
	CString strText;

	if(m_nCurrentSelection != -1)
		m_wndList.GetText(m_nCurrentSelection, strText);

	return strText;
}

int CInPlaceComboBoxImp::AddString(LPCTSTR pStrText, DWORD nData)
{
	int nIndex = m_wndList.AddString(pStrText);

	return m_wndList.SetItemData(nIndex, nData);
}

void CInPlaceComboBoxImp::ResetContent()
{
	m_wndList.ResetContent();

	m_nCurrentSelection = -1;
}