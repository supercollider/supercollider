#if !defined(_INPLACECOMBOBOXIMP_H)
#define _INPLACECOMBOBOXIMP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceComboBoxImp.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright © 1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBEditImp

class CInPlaceCBEditImp : public CEdit
{
	CInPlaceCBEditImp(const CInPlaceCBEditImp& d);
	CInPlaceCBEditImp& operator=(const CInPlaceCBEditImp& d);

public:
	CInPlaceCBEditImp();
	virtual ~CInPlaceCBEditImp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceCBEditImp)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceCBEditImp)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

inline CInPlaceCBEditImp::CInPlaceCBEditImp()
{
}

inline CInPlaceCBEditImp::~CInPlaceCBEditImp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListBoxImp

class CInPlaceCBListBoxImp : public CListBox
{
	CInPlaceCBListBoxImp(const CInPlaceCBListBoxImp& d);
	CInPlaceCBListBoxImp& operator=(const CInPlaceCBListBoxImp& d);

public:
	CInPlaceCBListBoxImp();
	virtual ~CInPlaceCBListBoxImp();

	// Operations
protected:
	void ProcessSelected(bool bProcess = true);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceCBListBoxImp)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceCBListBoxImp)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#if 0
/////////////////////////////////////////////////////////////////////////////
// CInPlaceCBListCtrlImp

class CInPlaceCBListCtrlImp : public CListCtrl
{
	CInPlaceCBListCtrlImp(const CInPlaceCBListCtrlImp& d);
	CInPlaceCBListCtrlImp& operator=(const CInPlaceCBListCtrlImp& d);

public:
	CInPlaceCBListCtrlImp();
	virtual ~CInPlaceCBListCtrlImp();

	// Attributes
	int GetCount() const;
	void GetText(int nIndex, CString& strText) const;

	// Operations
	int SetCurSel(int nSelect);	
	int AddString(LPCTSTR pStrText);

	void ResetContent();

protected:
	void ProcessSelected();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceCBListCtrlImp)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceCBListCtrlImp)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceComboBoxImp

class CInPlaceComboBoxImp : public CWnd
{
	CInPlaceComboBoxImp(const CInPlaceComboBoxImp& d);
	CInPlaceComboBoxImp operator=(const CInPlaceComboBoxImp& d);

protected:
	DECLARE_DYNAMIC(CInPlaceComboBoxImp)

public:
	CInPlaceComboBoxImp();
	virtual ~CInPlaceComboBoxImp();

	// Attributes
public:
	int GetCurrentSelection() const;
	DWORD GetDWordData() const;
	CString GetTextData() const;

	// Operations
public:
	static CInPlaceComboBoxImp* CreateInPlaceComboBox(CWnd* pWndParent, CRect& rect);

	int SetCurSel(int nSelect, bool bSendSetData = true);
	int AddString(LPCTSTR pStrText, DWORD nData = 0);

	void ResetContent();
	void ResetListBoxHeight();

	void MoveControl(CRect& rect);

private:
	void SetCurSelToEdit(int nSelect);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceComboBoxImp)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceComboBoxImp)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSelectionOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectionCancel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewSelection(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void HideListBox();
	
	// Data
private:
	static int m_nButtonDx;

	int m_nCurrentSelection;

	CInPlaceCBEditImp m_wndEdit;
	CInPlaceCBListBoxImp m_wndList;
};

inline CInPlaceComboBoxImp::~CInPlaceComboBoxImp()
{
}

inline int CInPlaceComboBoxImp::GetCurrentSelection() const
{
	return m_nCurrentSelection;
}

inline DWORD CInPlaceComboBoxImp::GetDWordData() const
{
	return m_nCurrentSelection != -1 ? m_wndList.GetItemData(m_nCurrentSelection) : 0;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_INPLACECOMBOBOXIMP_H)
