// SCSpawn.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PropertyListCtrlApp.h"
#include "PropertyListCtrlDlg.h"
#include "PaUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrlApp

BEGIN_MESSAGE_MAP(CPropertyListCtrlApp, CWinApp)
	//{{AFX_MSG_MAP(CPropertyListCtrlApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrlApp construction

CPropertyListCtrlApp::CPropertyListCtrlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPropertyListCtrlApp object

CPropertyListCtrlApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrlApp initialization

BOOL CPropertyListCtrlApp::InitInstance()
{
	AfxEnableControlContainer();

  PaUtils_CreatePaDevList( ); // this populates the array that the property manager will use

	CPropertyListCtrlDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
    dlg.FillRegistryFromData( );
    dlg.LaunchSC( );
	}
	else if (nResponse == IDCANCEL)
	{
    // don't do anything..... just quit.
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
