// PropertyItem.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PropertyItem.h"

#include "InPlaceEditPropItemString.h"
#include "InPlaceComboBoxImp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyItem

IMPLEMENT_DYNAMIC(CPropertyItem, CObject)

bool CPropertyItem::SetEnabled(bool bEnable)
{
	bool bChanged = m_bEnabled != bEnable;
	m_bEnabled = bEnable;

	return bChanged;
}

void CPropertyItem::CreateInPlaceControl(CWnd* /*pWndParent*/, CRect& /*rect*/, CWnd*& pWndInPlaceControl)
{
	DestroyInPlaceControl(pWndInPlaceControl);
}

void CPropertyItem::DestroyInPlaceControl(CWnd*& pWndInPlaceControl)
{
	if(pWndInPlaceControl != NULL)
	{
		delete pWndInPlaceControl;
		pWndInPlaceControl = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemCategory

IMPLEMENT_DYNAMIC(CPropertyItemCategory, CPropertyItem)

int CPropertyItemCategory::GetNumberEnabledItems() const
{
	int nNumberEnabledItems = 0;
	POSITION pos = m_listPropertyItems.GetHeadPosition();

	while(pos != NULL)
	{
		if(m_listPropertyItems.GetNext(pos)->GetEnabled())
			nNumberEnabledItems++;
	}

	return nNumberEnabledItems;
}

CPropertyItem* CPropertyItemCategory::GetPropertyItem(int nPropertyID)
{
	POSITION pos = m_listPropertyItems.GetHeadPosition();

	while(pos != NULL)
	{
		CPropertyItem* pPropertyItem = m_listPropertyItems.GetNext(pos);

		if(pPropertyItem->GetPropertyID() == nPropertyID)
			return pPropertyItem;
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyItemString

IMPLEMENT_DYNAMIC(CPropertyItemString, CPropertyItem)

void CPropertyItemString::CreateInPlaceControl(CWnd* pWndParent, CRect& rect, CWnd*& pWndInPlaceControl)
{
	if(pWndInPlaceControl != NULL && pWndInPlaceControl->IsKindOf(RUNTIME_CLASS(CInPlaceEditPropItemString)))
		((CInPlaceEditPropItemString*)pWndInPlaceControl)->MoveControl(rect);
	else	
	{
		DestroyInPlaceControl(pWndInPlaceControl);

		pWndInPlaceControl = CInPlaceEditPropItemString::CreateInPlaceEditPropItemString(m_strText, pWndParent, rect);
	}

	((CInPlaceEditPropItemString*)pWndInPlaceControl)->SetText(m_strText);
}

void CPropertyItemString::SetData(CWnd* pWndInPlaceControl)
{
	pWndInPlaceControl->GetWindowText(m_strText);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemList

IMPLEMENT_DYNAMIC(CPropertyItemList, CPropertyItem)

void CPropertyItemList::CreateInPlaceControl(CWnd* pWndParent, CRect& rect, CWnd*& pWndInPlaceControl)
{
	if(pWndInPlaceControl != NULL && pWndInPlaceControl->IsKindOf(RUNTIME_CLASS(CInPlaceComboBoxImp)))
	{
		((CInPlaceComboBoxImp*)pWndInPlaceControl)->MoveControl(rect);
		((CInPlaceComboBoxImp*)pWndInPlaceControl)->ResetContent();
	}
	else
	{
		DestroyInPlaceControl(pWndInPlaceControl);
		
		pWndInPlaceControl = (CWnd*)CInPlaceComboBoxImp::CreateInPlaceComboBox(pWndParent, rect);
	}

	SetItemListData((CInPlaceComboBoxImp*)pWndInPlaceControl);
	((CInPlaceComboBoxImp*)pWndInPlaceControl)->SetCurSel(m_nItem, false);
	((CInPlaceComboBoxImp*)pWndInPlaceControl)->ResetListBoxHeight();
}

void CPropertyItemList::SetData(CWnd* pWndInPlaceControl)
{
	m_nItem = ((CInPlaceComboBoxImp*)pWndInPlaceControl)->GetCurrentSelection();
}

void CPropertyItemList::SetItemListData(CInPlaceComboBoxImp* pWndInPlaceControl)
{
	LPCTSTR pStrText;

	for(int i = 0; (pStrText = GetItemData(i)) != NULL; i++)
		pWndInPlaceControl->AddString(pStrText);
}