// PropertyListCtrl.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PropertyListCtrl.h"

#include "PropertyItem.h"

#include "UserMessageID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PROPERTY_LEFT_BORDER 16


/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl

CPropertyListCtrl::CPropertyListCtrl()
{
	m_nSelectedItem = -1;
	m_pWndInPlaceControl = NULL;
	m_pPropertyItemManager = NULL;
}

CPropertyItem* CPropertyListCtrl::GetPropertyItem(int nIndex)
{
	return nIndex >= 0 && nIndex < GetCount() ? reinterpret_cast<CPropertyItem*>(GetItemData(nIndex)) : NULL;
}

void CPropertyListCtrl::SetPropertyItemManager(CPropertyItemManager* pPropertyItemManager)
{
	m_pPropertyItemManager = pPropertyItemManager;
	m_pPropertyItemManager->InitControl(this);
}

void CPropertyListCtrl::GetItemValueRect(CRect& rect)
{
	rect.left += PROPERTY_LEFT_BORDER;
	rect.left += rect.Width() / 4;

	rect.DeflateRect(3, 0, 0, 1);
}

void CPropertyListCtrl::NoInPlaceControl()
{
	delete m_pWndInPlaceControl;
	m_pWndInPlaceControl = NULL;
}

void CPropertyListCtrl::ResetContent()
{
	NoInPlaceControl();

	m_nSelectedItem = -1;

	CListBox::ResetContent();
}

void CPropertyListCtrl::PreSubclassWindow() 
{	
	CListBox::PreSubclassWindow();

	ModifyStyle(0, LBS_OWNERDRAWFIXED|LBS_NOINTEGRALHEIGHT|LBS_NOTIFY);
}

BEGIN_MESSAGE_MAP(CPropertyListCtrl, CListBox)
	//{{AFX_MSG_MAP(CPropertyListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	ON_MESSAGE(WM_USER_SET_DATA, OnSetData)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl message handlers

void CPropertyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
#if 1
	if(lpDrawItemStruct->itemAction & ODA_FOCUS)
		return;
#endif

	CPropertyItem* pPropertyItem = (CPropertyItem*)lpDrawItemStruct->itemData;
	ASSERT(pPropertyItem != NULL);

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	// Rámèek
	CRect rect;
	GetClientRect(rect);

	int nLeftBorder = rect.left + PROPERTY_LEFT_BORDER;

	// + / -
	bool bTabItem = ToBool(pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)));

	if(bTabItem)
	{
		CRect rcSign(lpDrawItemStruct->rcItem);
		rcSign.right = nLeftBorder;

		rcSign.DeflateRect(4, 4);

		rcSign.right += 1;
		rcSign.bottom += 1;

		dc.FillRect(rcSign, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
		dc.FrameRect(rcSign, CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));
		
		CPoint ptCenter(rcSign.CenterPoint());

		// minus		
		dc.MoveTo(ptCenter.x - 2, ptCenter.y);
		dc.LineTo(ptCenter.x + 3, ptCenter.y);

		if(!static_cast<CPropertyItemCategory*>(pPropertyItem)->GetChildrenVisible())
		{
			// plus
			dc.MoveTo(ptCenter.x, ptCenter.y - 2);
			dc.LineTo(ptCenter.x, ptCenter.y + 3);
		}
	}

	// Vertikála v ¾avo
	CPen pen(PS_SOLID, 1, RGB(198, 198, 198));
	CPen* pOldPen = dc.SelectObject(&pen);

	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, rect.bottom);

	// Spodná èiara
	rect = lpDrawItemStruct->rcItem;
	rect.left += PROPERTY_LEFT_BORDER;

	int nBottom = rect.bottom - 1;

	dc.MoveTo(nLeftBorder, nBottom);
	dc.LineTo(rect.right, nBottom);

	// Odde¾ovaè ståpcov
	nLeftBorder += rect.Width() / 4;

	dc.MoveTo(nLeftBorder, rect.top);
	dc.LineTo(nLeftBorder, nBottom);

	// Text položky
	rect.left += 1;
	rect.bottom -= 1;
	rect.right = nLeftBorder;

	int nCrBackground, nCrText;

	if((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		nCrBackground = COLOR_HIGHLIGHT;
		nCrText = COLOR_HIGHLIGHTTEXT;
	}
	else
	{
		nCrBackground = COLOR_WINDOW;
		nCrText = COLOR_WINDOWTEXT;
	}

	COLORREF crBackground = ::GetSysColor(nCrBackground);
	COLORREF crText = ::GetSysColor(nCrText);

	dc.FillSolidRect(rect, crBackground);
	COLORREF crOldBkColor = dc.SetBkColor(crBackground);
	COLORREF crOldTextColor = dc.SetTextColor(crText);

	rect.left += 3;
	rect.right -= 3;

	CFont* pOldFont = NULL;
	CFont fontLabel;

	if(bTabItem)
	{
		LOGFONT logFont;
		CFont* pFont = GetFont();
		pFont->GetLogFont(&logFont);

		logFont.lfWeight = FW_BOLD;
		fontLabel.CreateFontIndirect(&logFont);

		pOldFont = dc.SelectObject(&fontLabel);
	}

	// Popis položky
	dc.DrawText(pPropertyItem->GetName(), &rect, DT_SINGLELINE|DT_VCENTER);

	dc.SelectObject(pOldPen);
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	//dc.SetBkMode(TRANSPARENT);

	if(pOldFont != NULL)
		dc.SelectObject(pOldFont);

	// Hodnota položky
	if(!(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		rect = lpDrawItemStruct->rcItem;
		GetItemValueRect(rect);
		pPropertyItem->DrawValue(&dc, rect);

		TRACE("\nDraw value of %s", pPropertyItem->GetName());
	}

	dc.Detach();
}

void CPropertyListCtrl::DoCollapse(CPropertyItemCategory* pPropertyItemTab, int nItem)
{
	ASSERT(pPropertyItemTab->GetChildrenVisible());

	nItem++;

	for(int nNumber = pPropertyItemTab->GetNumberEnabledItems(); nNumber > 0; nNumber--)
		DeleteString(nItem);

	pPropertyItemTab->SetChildrenVisible(false);
}

void CPropertyListCtrl::DoExpand(CPropertyItemCategory* pPropertyItemTab, int& nItem)
{
	POSITION pos = pPropertyItemTab->GetHeadPosition();

	while(pos != NULL)
	{
		CPropertyItem* pPropertyItem = pPropertyItemTab->GetNext(pos);

		if(pPropertyItem->GetEnabled())
			InsertPropertyItem(pPropertyItem, ++nItem);
	}

	pPropertyItemTab->SetChildrenVisible();
}

void CPropertyListCtrl::DoCollapseExpand(int nItem, CPropertyItem* pPropertyItem)
{	
	if(pPropertyItem == NULL)
		pPropertyItem = GetPropertyItem(nItem);

	ASSERT(pPropertyItem != NULL);

	if(pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)))
	{
		if(SetCurrentData())
			NoInPlaceControl();

		CPropertyItemCategory* pPropertyItemTab = static_cast<CPropertyItemCategory*>(pPropertyItem);

		if(pPropertyItemTab->GetChildrenVisible())
			DoCollapse(pPropertyItemTab, nItem);
		else
			DoExpand(pPropertyItemTab, nItem);
	}
}

void CPropertyListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CListBox::OnLButtonDown(nFlags, point);

	if(point.x <= PROPERTY_LEFT_BORDER)
	{
		DoCollapseExpand(GetCurSel());
	}
}

void CPropertyListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListBox::OnLButtonDblClk(nFlags, point);

	DoCollapseExpand(GetCurSel());
}

BOOL CPropertyListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{		
		int nItem = GetCurSel();
		char nChar = char(pMsg->wParam);
		bool bDone = true;

		CPropertyItem* pPropertyItem = GetPropertyItem(nItem);

		if(pPropertyItem != NULL)
		{
			if(pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)))
			{
				bool bChildrenVisible = static_cast<CPropertyItemCategory*>(pPropertyItem)->GetChildrenVisible();

				switch(nChar)
				{
					case VK_RETURN:
						DoCollapseExpand(nItem, pPropertyItem);
						break;
					case VK_ADD:
						if(!bChildrenVisible)
							DoExpand((CPropertyItemCategory*)pPropertyItem, nItem);
						break;
					case VK_SUBTRACT:
						if(bChildrenVisible)
							DoCollapse((CPropertyItemCategory*)pPropertyItem, nItem);
						break;
					default:
						bDone = false;
				}
			}
			else
			{
				switch(nChar)
				{
					case VK_RETURN:
					case VK_TAB:
						if(m_pWndInPlaceControl != NULL)
							m_pWndInPlaceControl->SetFocus();
						break;
					default:
						bDone = false;
				}
			}
		}
		else
			bDone = false;

		if(bDone)
			return TRUE;
		else
		{
			if(GetFocus() == this && IsCharAlpha(nChar))
			{
				if(LookupPropertyItem(nChar))
					OnSelChange();

				return TRUE;
			}
		}
	}

	return CListBox::PreTranslateMessage(pMsg);
}

int CPropertyListCtrl::FindPropertyItem(char nStartChar, int nFromIndex, int nCount)
{
	const CString strStartChar(nStartChar);

	while(nCount--)
	{
		CPropertyItem* pPropertyItem = reinterpret_cast<CPropertyItem*>(GetItemData(nFromIndex));
		ASSERT(pPropertyItem != NULL);
			
		if(pPropertyItem->GetName().Left(1).CompareNoCase(strStartChar) == 0)
			return nFromIndex;

		nFromIndex++;
	}

	return -1;
}

bool CPropertyListCtrl::LookupPropertyItem(char nStartChar)
{
	int nCount = GetCount();

	if(nCount)
	{
		int nCurrItem = GetCurSel() + 1;

		if(nCurrItem == nCount)
			nCurrItem = 0;

		int nFindIndex = FindPropertyItem(nStartChar, nCurrItem, nCount - nCurrItem);

		if(nFindIndex == -1 && nCurrItem > 0)
			nFindIndex = FindPropertyItem(nStartChar, 0, nCurrItem);

		if(nFindIndex != -1)
		{
			SetCurSel(nFindIndex);
			return true;
		}
	}

	return false;
}

bool CPropertyListCtrl::SetCurrentData()
{
	if(m_pWndInPlaceControl != NULL && m_nSelectedItem != -1)
	{
		CPropertyItem* pPropertyItemEdited = GetPropertyItem(m_nSelectedItem);
		ASSERT(pPropertyItemEdited != NULL);

		pPropertyItemEdited->SetData(m_pWndInPlaceControl);
		m_pPropertyItemManager->OnDataChanged(pPropertyItemEdited, this, m_nSelectedItem);

		return true;
	}

	return false;
}

LRESULT CPropertyListCtrl::OnSetData(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	SetCurrentData();

	return TRUE;
}

void CPropertyListCtrl::OnSelChange()
{
	int nItem = GetCurSel();

	if(m_nSelectedItem != nItem)
	{
		//SetCurrentData();

		CPropertyItem* pPropertyItem = GetPropertyItem(nItem);
		ASSERT(pPropertyItem != NULL);

		CRect rect;

		GetItemRect(nItem, rect);
		GetItemValueRect(rect);

		pPropertyItem->CreateInPlaceControl(this, rect, m_pWndInPlaceControl);

		m_nSelectedItem = nItem;
	}
}

void CPropertyListCtrl::ShowInPlaceControl(bool bShow)
{
	if(m_pWndInPlaceControl)
	{
		if(bShow)
		{
			CPropertyItem* pPropertyItem = GetPropertyItem(m_nSelectedItem);
			ASSERT(pPropertyItem != NULL);

			CRect rect;

			GetItemRect(m_nSelectedItem, rect);
			GetItemValueRect(rect);

			pPropertyItem->CreateInPlaceControl(this, rect, m_pWndInPlaceControl);

		}

		m_pWndInPlaceControl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
}

void CPropertyListCtrl::Refresh()
{
	ShowInPlaceControl(false);

	CRect rect;

	CDC dc;
	dc.Attach(*GetDC());
	dc.SelectObject(GetFont());

	int nItems = GetCount();
	COLORREF crBkColor = dc.GetBkColor();

	for(int i = 0; i < nItems; i++)
	{
		CPropertyItem* pPropertyItem = reinterpret_cast<CPropertyItem*>(GetItemData(i));

		if(!pPropertyItem->IsKindOf(RUNTIME_CLASS(CPropertyItemCategory)))
		{
			GetItemRect(i, rect);
			GetItemValueRect(rect);

			rect.DeflateRect(1, 1);
			dc.FillSolidRect(&rect, crBkColor);
			rect.InflateRect(1, 1);

			pPropertyItem->DrawValue(&dc, rect);
		}
	}

	dc.Detach();

	ShowInPlaceControl();
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManager

CPropertyItemCategory* CPropertyItemManager::GetCategoryTab(int nIndex) const
{
	POSITION pos = m_listPropertyItemTabs.FindIndex(nIndex);
	ASSERT(pos != NULL);

	return m_listPropertyItemTabs.GetAt(pos);
}

CPropertyItemCategory* CPropertyItemManager::AddNewTab(LPCTSTR pStrTabName)
{
	CPropertyItemCategory* pNewTab = new CPropertyItemCategory(pStrTabName);

	m_listPropertyItemTabs.AddTail(pNewTab);

	return pNewTab;
}

void CPropertyItemManager::InitControl(CPropertyListCtrl* pWndPropertyListCtrl)
{
	int nItem = 0;
	POSITION pos = m_listPropertyItemTabs.GetHeadPosition();
	
	while(pos != NULL)
	{
		CPropertyItemCategory* pPropertyItemTab = m_listPropertyItemTabs.GetNext(pos);

		if(pPropertyItemTab->GetEnabled())
		{
			pWndPropertyListCtrl->InsertPropertyItem(pPropertyItemTab, nItem);
			pWndPropertyListCtrl->DoExpand(pPropertyItemTab, nItem);

			nItem++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManagerAdaptable

void CPropertyItemManagerAdaptable::CheckState(CPropertyListCtrl* pWndPropertyListCtrl, int nFromControlIndex, BYTE nFromPropertyItemID)
{
	bool bDoInsert = false;

	POSITION pos = m_listPropertyItemTabs.GetHeadPosition();
	
	while(pos != NULL)
	{
		CPropertyItemCategory* pPropertyItemTab = m_listPropertyItemTabs.GetNext(pos);
		POSITION posTab = pPropertyItemTab->GetHeadPosition();

		if(bDoInsert)
		{
			if(pPropertyItemTab->GetEnabled())
			{
				pWndPropertyListCtrl->InsertPropertyItem(pPropertyItemTab, ++nFromControlIndex);
				pPropertyItemTab->SetChildrenVisible();
			}
			else
				// Táto záložka nieje prístupná
				continue;
		}
		
		while(posTab != NULL)
		{
			CPropertyItem* pPropertyItem = pPropertyItemTab->GetNext(posTab);

			if(bDoInsert)
			{
				if(pPropertyItem->GetEnabled())
					pWndPropertyListCtrl->InsertPropertyItem(pPropertyItem, ++nFromControlIndex);
			}
			else if(nFromPropertyItemID == pPropertyItem->GetPropertyID())
			{
				// Našiel si položku od ktorej by mali nasledova zmeny
				int nItems = pWndPropertyListCtrl->GetCount();

				if(nItems != nFromControlIndex + 1)
				{
					// Zmaž všetky položky 
					int i = nItems - 1;
					nItems -= (nFromControlIndex + 1);
					
					while(nItems-- > 0)
					{
						pWndPropertyListCtrl->DeleteString(i--);
					}
				}

				bDoInsert = true;
			}
		}
	}
}