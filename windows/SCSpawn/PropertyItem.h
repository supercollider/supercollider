#if !defined(_PROPERTYITEM_H)
#define _PROPERTYITEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyItem.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "MyList.h"
#include "SmartString.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyItem

class CPropertyItem : public CObject
{
	CPropertyItem(const CPropertyItem& d);
	CPropertyItem& operator=(const CPropertyItem& d);

protected:
	DECLARE_DYNAMIC(CPropertyItem)

public:
	CPropertyItem(LPCTSTR pStrName = NULL);
	virtual ~CPropertyItem();

	// Attributes
	const CString& GetName() const;
	void SetName(LPCTSTR pStrName);

	bool GetEnabled() const;
	bool SetEnabled(bool bEnable = true);

	BYTE GetPropertyID() const;
	void SetPropertyID(BYTE nPropertyID);

	// Operations
	virtual void DrawValue(CDC* pDC, CRect& rect);
	virtual void CreateInPlaceControl(CWnd* pWndParent, CRect& rect, CWnd*& pWndInPlaceControl);
	virtual void SetData(CWnd* pWndInPlaceControl);

protected:
	void DestroyInPlaceControl(CWnd*& pWndInPlaceControl);

	// Data
private:
	CString m_strName;
	bool m_bEnabled;
	BYTE m_nPropertyID;
};

inline CPropertyItem::CPropertyItem(LPCTSTR pStrName)
{
	m_strName = pStrName;
	m_bEnabled = true;
	m_nPropertyID = 0;
}

inline CPropertyItem::~CPropertyItem()
{
}

inline const CString& CPropertyItem::GetName() const
{
	return m_strName;
}

inline void CPropertyItem::SetName(LPCTSTR pStrName)
{
	m_strName = pStrName;
}

inline bool CPropertyItem::GetEnabled() const
{
	return m_bEnabled;
}

inline BYTE CPropertyItem::GetPropertyID() const
{
	return m_nPropertyID;
}

inline void CPropertyItem::SetPropertyID(BYTE nPropertyID)
{
	m_nPropertyID = nPropertyID;
}

inline void CPropertyItem::DrawValue(CDC* /*pDC*/, CRect& /*rect*/)
{
}

inline void CPropertyItem::SetData(CWnd* /*pWndInPlaceControl*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CListPropertyItems

typedef CMyTypedPtrList<CObList, CPropertyItem*> CListPropertyItems;

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemCategory

class CPropertyItemCategory : public CPropertyItem
{
	CPropertyItemCategory(const CPropertyItemCategory& d);
	CPropertyItemCategory& operator=(const CPropertyItemCategory& d);

protected:
	DECLARE_DYNAMIC(CPropertyItemCategory)

public:
	CPropertyItemCategory(LPCTSTR pStrName = NULL);
	virtual ~CPropertyItemCategory();

	// Attributes
	void AddPropertyItem(CPropertyItem* pPropertyItem);

	bool GetChildrenVisible() const;
	void SetChildrenVisible(bool bVisible = true);

	CListPropertyItems* GetListPropertyItems();

	int GetNumberItems() const;
	int GetNumberEnabledItems() const;

	POSITION GetHeadPosition() const;

	// Operations
	CPropertyItem* GetNext(POSITION& rPosition) const;
	CPropertyItem* GetPropertyItem(int nPropertyID);

	// Data
private:
	CListPropertyItems m_listPropertyItems;
	bool m_bChildrenVisible;
};

inline CPropertyItemCategory::CPropertyItemCategory(LPCTSTR pStrName)
	: CPropertyItem(pStrName)
{
	m_bChildrenVisible = false;
}

inline CPropertyItemCategory::~CPropertyItemCategory()
{
}

inline CListPropertyItems* CPropertyItemCategory::GetListPropertyItems()
{
	return &m_listPropertyItems;
}

inline void CPropertyItemCategory::AddPropertyItem(CPropertyItem* pPropertyItem)
{
	m_listPropertyItems.AddTail(pPropertyItem);
}

inline bool CPropertyItemCategory::GetChildrenVisible() const
{
	return m_bChildrenVisible;
}

inline void CPropertyItemCategory::SetChildrenVisible(bool bVisible)
{
	m_bChildrenVisible = bVisible;
}

inline int CPropertyItemCategory::GetNumberItems() const
{
	return m_listPropertyItems.GetCount();
}

inline POSITION CPropertyItemCategory::GetHeadPosition() const
{
	return m_listPropertyItems.GetHeadPosition();
}

inline CPropertyItem* CPropertyItemCategory::GetNext(POSITION& rPosition) const
{	
	return m_listPropertyItems.GetNext(rPosition);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemString

class CPropertyItemString : public CPropertyItem
{
	CPropertyItemString(const CPropertyItemString& d);
	CPropertyItemString& operator=(const CPropertyItemString& d);

protected:
	DECLARE_DYNAMIC(CPropertyItemString)

public:
	CPropertyItemString(LPCTSTR pStrName = NULL);
	virtual ~CPropertyItemString();

	// Attributes
	const CString& GetData() const;
	void SetData(const CString& strText);

	// Operations
	virtual void DrawValue(CDC* pDC, CRect& rect);
	virtual void CreateInPlaceControl(CWnd* pWndParent, CRect& rect, CWnd*& pWndInPlaceControl);
	virtual void SetData(CWnd* pWndInPlaceControl);

	// Data
private:
	CString m_strText;
};

inline CPropertyItemString::CPropertyItemString(LPCTSTR pStrName)
	: CPropertyItem(pStrName)
{
}

inline CPropertyItemString::~CPropertyItemString()
{
}

inline const CString& CPropertyItemString::GetData() const
{
	return m_strText;
}

inline void CPropertyItemString::SetData(const CString& strText)
{
	m_strText = strText;
}

inline void CPropertyItemString::DrawValue(CDC* pDC, CRect& rect)
{
	pDC->DrawText(m_strText, &rect, DT_SINGLELINE|DT_VCENTER);
}

class CInPlaceComboBoxImp;

/////////////////////////////////////////////////////////////////////////////
// CPropertyItemList

class CPropertyItemList : public CPropertyItem
{
	CPropertyItemList(const CPropertyItemList& d);
	CPropertyItemList& operator=(const CPropertyItemList& d);

protected:
	DECLARE_DYNAMIC(CPropertyItemList)

public:
	CPropertyItemList(LPCTSTR pStrName);
	virtual ~CPropertyItemList();

	// Attributes
	template <class ctype>
	void GetData(ctype& nItem) const
	{
		nItem = ctype(m_nItem);
	}

	void GetData(bool& nItem) const;

	template <class ctype>
	void SetData(ctype nItem)
	{
		m_nItem = int(nItem);
	}

	virtual LPCTSTR GetItemData(int nItem) const;

	// Operations
	virtual void DrawValue(CDC* pDC, CRect& rect);
	virtual void CreateInPlaceControl(CWnd* pWndParent, CRect& rect, CWnd*& pWndInPlaceControl);
	virtual void SetData(CWnd* pWndInPlaceControl);

	void SetItemListData(CInPlaceComboBoxImp* pWndInPlaceControl);

	// Data
private:
	int m_nItem;
};

inline CPropertyItemList::CPropertyItemList(LPCTSTR pStrName)
	: CPropertyItem(pStrName)
{
	m_nItem = 0;
}

inline CPropertyItemList::~CPropertyItemList()
{
}

inline void CPropertyItemList::GetData(bool& nItem) const
{
	nItem = ToBool(m_nItem);
}

inline LPCTSTR CPropertyItemList::GetItemData(int /*nItem*/) const
{
	return NULL;
}

inline void CPropertyItemList::DrawValue(CDC* pDC, CRect& rect)
{
	pDC->DrawText(GetItemData(m_nItem), -1, &rect, DT_SINGLELINE|DT_VCENTER);
}

/////////////////////////////////////////////////////////////////////////////

#define BEGIN_LIST_ITEM_DATA_TYPE(new_class) \
class new_class : public CPropertyItemList \
{ \
	new_class(const new_class& d); \
	new_class& operator=(const new_class& d); \
public: \
	new_class(LPCTSTR pStrName = NULL) : CPropertyItemList(pStrName) {}; \
	virtual ~new_class() {}; \
	virtual LPCTSTR GetItemData(int nItem) const; \
private: \
	static CSmartString m_data[]; \
}; \
CSmartString new_class::m_data[] = { 
#define ID_STRING_ITEM_DATA(idString) \
	CSmartString(UINT(idString))
#define LPCTSTR_STRING_ITEM_DATA(string) \
	CSmartString(LPCTSTR(string))
#define END_LIST_ITEM_DATA_TYPE(new_class) \
}; \
LPCTSTR new_class::GetItemData(int nItem) const \
{ \
	return nItem < sizeof(new_class::m_data)/sizeof(CSmartString) ? LPCTSTR(m_data[nItem]) : NULL; \
}

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_PROPERTYITEM_H)