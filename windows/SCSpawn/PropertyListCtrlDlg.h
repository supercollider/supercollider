// PropertyListCtrlDlg.h : header file
//

#if !defined(AFX_PROPERTYLISTCTRLDLG_H__466FB4E7_6072_11D3_A7E3_006008C8B630__INCLUDED_)
#define AFX_PROPERTYLISTCTRLDLG_H__466FB4E7_6072_11D3_A7E3_006008C8B630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"

#include "SCSpawnProperties.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrlDlg dialog

class CPropertyListCtrlDlg : public CDialog
{
	// Construction
public:
	CPropertyListCtrlDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//{{AFX_DATA(CPropertyListCtrlDlg)
	enum { IDD = IDD_PROPERTYLISTCTRL_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyListCtrlDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPropertyListCtrlDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPropertyListCtrl m_propertyControl;

	SCSpawn_PropertyItemManager m_propertyManager;
	CMyData m_data;
public:
  void FillDataFromRegistry( );
  void FillRegistryFromData( );
  void LaunchSC( );

  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLISTCTRLDLG_H__466FB4E7_6072_11D3_A7E3_006008C8B630__INCLUDED_)
