#if !defined(AFX_PROPERTYLISTCTRL1_H__466FB4EF_6072_11D3_A7E3_006008C8B630__INCLUDED_)
#define AFX_PROPERTYLISTCTRL1_H__466FB4EF_6072_11D3_A7E3_006008C8B630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyListCtrl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "PropertyItem.h"

class CPropertyItemManager;

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl

class CPropertyListCtrl : public CListBox
{
	CPropertyListCtrl(const CPropertyListCtrl& d);
	CPropertyListCtrl& operator=(const CPropertyListCtrl& d);

public:
	CPropertyListCtrl();

	// Attributes
	void SetPropertyItemManager(CPropertyItemManager* pPropertyItemManager);
	CPropertyItem* GetPropertyItem(int nIndex);

	// Operations
	void InsertPropertyItem(CPropertyItem* pPropertyItem, int nIndex = 0);
	void ResetContent();
	void ShowInPlaceControl(bool bShow = true);
	void Refresh();

protected:
	void DoCollapse(CPropertyItemCategory* pPropertyItemTab, int nItem);
	void DoExpand(CPropertyItemCategory* pPropertyItemTab, int& nItem);

	void DoCollapseExpand(int nItem, CPropertyItem* pPropertyItem = NULL);

	void GetItemValueRect(CRect& rect);
	bool SetCurrentData();

	void NoInPlaceControl();

	bool LookupPropertyItem(char nStartChar);
	int FindPropertyItem(char nStartChar, int nFromIndex, int nCount);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyListCtrl)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CPropertyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyListCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSelChange();
	afx_msg LRESULT OnSetData(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Data
private:
	int m_nSelectedItem;
	CWnd* m_pWndInPlaceControl;

	CPropertyItemManager* m_pPropertyItemManager;

	friend class CPropertyItemManager;
};

inline CPropertyListCtrl::~CPropertyListCtrl()
{
	delete m_pWndInPlaceControl;
}

inline void CPropertyListCtrl::InsertPropertyItem(CPropertyItem* pPropertyItem, int nIndex)
{
	ASSERT(pPropertyItem != NULL);
	InsertString(nIndex, (LPCTSTR)pPropertyItem);
}

inline void CPropertyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/)
{
}

inline int CPropertyListCtrl::CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManager

class CPropertyItemManager
{
	CPropertyItemManager(const CPropertyItemManager& d);
	CPropertyItemManager& operator=(const CPropertyItemManager& d);

public:
	CPropertyItemManager();
	~CPropertyItemManager();

	// Attributes
	CPropertyItemCategory* GetCategoryTab(int nIndex) const;

	// Operations
	CPropertyItemCategory* AddNewTab(LPCTSTR pStrTabName);
	void InitControl(CPropertyListCtrl* pWndPropertyListCtrl);

	virtual void OnDataChanged(CPropertyItem* pPropertyItem, CPropertyListCtrl* pWndPropertyListCtrl, int nIndex);
	
	virtual bool SetData(const CObject* pData) = 0;
	virtual bool GetData(CObject* pData) const = 0;

	// Data
protected:
	CMyTypedPtrList<CObList, CPropertyItemCategory*> m_listPropertyItemTabs;
};

inline CPropertyItemManager::CPropertyItemManager()
{
}

inline CPropertyItemManager::~CPropertyItemManager()
{
}

inline void CPropertyItemManager::OnDataChanged(CPropertyItem* /*pPropertyItem*/, CPropertyListCtrl* /*pWndPropertyListCtrl*/, int /*nIndex*/)
{
}

/////////////////////////////////////////////////////////////////////////////
#define BEGIN_PROPERTY_TAB_IDNAME(idTabName, enabled) \
{ \
	CPropertyItemCategory* pNewTab = AddNewTab(LoadString(idTabName)); \
	pNewTab->SetEnabled(enabled); \
	CPropertyItem* pPropertyItem;
#define PROPERTY_ITEM_IDNAME(id, class_name, idItemName, enabled) \
	pPropertyItem = new class_name(LoadString(idItemName)); \
	pPropertyItem->SetEnabled(enabled); \
	pPropertyItem->SetPropertyID(id);\
	pNewTab->AddPropertyItem(pPropertyItem);

#define BEGIN_PROPERTY_TAB(tabName, enabled) \
{ \
	CPropertyItemCategory* pNewTab = AddNewTab(tabName); \
	pNewTab->SetEnabled(enabled); \
	CPropertyItem* pPropertyItem;
#define PROPERTY_ITEM(id, class_name, itemName, enabled) \
	pPropertyItem = new class_name(itemName); \
	pPropertyItem->SetEnabled(enabled); \
	pPropertyItem->SetPropertyID(id);\
	pNewTab->AddPropertyItem(pPropertyItem);
#define PROPERTY_ITEM_WITH_CTOR_PARAM(id, class_name, ctor_param, itemName, enabled) \
	pPropertyItem = new class_name(itemName,ctor_param); \
	pPropertyItem->SetEnabled(enabled); \
	pPropertyItem->SetPropertyID(id);\
	pNewTab->AddPropertyItem(pPropertyItem);
#define END_PROPERTY_TAB() \
}

#define BEGIN_ITERATE_PROPERTY_ITEMS() \
	POSITION pos = m_listPropertyItemTabs.GetHeadPosition(); \
	while(pos != NULL) \
	{ \
		CPropertyItemCategory* pPropertyItemTab = m_listPropertyItemTabs.GetNext(pos); \
		POSITION posItem = pPropertyItemTab->GetHeadPosition(); \
		while(posItem != NULL) \
		{ \
			CPropertyItem* pPropertyItem = pPropertyItemTab->GetNext(posItem); \
			switch(pPropertyItem->GetPropertyID()) \
			{
#define SET_ITEM_STRING(id, string) \
				case id: \
					static_cast<CPropertyItemString*>(pPropertyItem)->SetData(string); \
					break;
#define GET_ITEM_STRING(id, string) \
				case id: \
					string = static_cast<CPropertyItemString*>(pPropertyItem)->GetData(); \
					break;
#define SET_ITEM_LIST(id, item) \
				case id: \
				static_cast<CPropertyItemList*>(pPropertyItem)->SetData(item); \
					break;
#define GET_ITEM_LIST(id, item) \
				case id: \
					static_cast<CPropertyItemList*>(pPropertyItem)->GetData(item); \
					break;
#define END_ITERATE_PROPERTY_ITEMS() \
				default: ; \
			} \
		} \
	}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemManagerAdaptable

class CPropertyItemManagerAdaptable : public CPropertyItemManager
{
	CPropertyItemManagerAdaptable(const CPropertyItemManagerAdaptable& d);
	CPropertyItemManagerAdaptable& operator=(const CPropertyItemManagerAdaptable& d);

public:
	CPropertyItemManagerAdaptable();
	~CPropertyItemManagerAdaptable();

	// Operations
	void CheckState(CPropertyListCtrl* pWndPropertyListCtrl, int nFromControlIndex, BYTE nFromPropertyItemID);
};

inline CPropertyItemManagerAdaptable::CPropertyItemManagerAdaptable()
{
}

inline CPropertyItemManagerAdaptable::~CPropertyItemManagerAdaptable()
{
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLISTCTRL1_H__466FB4EF_6072_11D3_A7E3_006008C8B630__INCLUDED_)
