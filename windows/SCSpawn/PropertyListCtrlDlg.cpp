// PropertyListCtrlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PaUtils.h"
#include "PropertyListCtrlDlg.h"
#include ".\propertylistctrldlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrlDlg dialog

CPropertyListCtrlDlg::CPropertyListCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyListCtrlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyListCtrlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPropertyListCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPropertyListCtrlDlg)
	DDX_Control(pDX, IDC_LIST1, m_propertyControl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropertyListCtrlDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyListCtrlDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButtonExit)
	//}}AFX_MSG_MAP
  ON_WM_SIZE()
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrlDlg message handlers


BOOL CPropertyListCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  FillDataFromRegistry( );

  m_propertyManager.SetData(&m_data);
	m_propertyControl.SetPropertyItemManager(&m_propertyManager);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPropertyListCtrlDlg::FillDataFromRegistry( )
{
  // this retrieves the stored preferences, or default 
  // values if those aren't set
  
  // standard scsynth cmd line flags
  m_data.ConnectionType = AfxGetApp( )->GetProfileInt("Properties", "ConnectionType", 1 ); // default is 1 (UDP)
  m_data.PortNumber.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "PortNumber", 57110 )); // default is 57110
  m_data.TCPPassword = AfxGetApp( )->GetProfileString("Properties", "TCPPassword", ""); // default is no password
  m_data.MaxNumLogins.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "MaxNumLogins", 64 ));
  m_data.NumControlBusChannels.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumControlBusChannels", 4096 ));
  m_data.NumAudioBusChannels.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumAudioBusChannels", 128 ));
  m_data.NumInputBusChannels.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumInputBusChannels", 2));
  m_data.NumOutputBusChannels.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumOutputBusChannels", 2 ));
  m_data.BlockSize.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "BlockSize", 64 ));
  m_data.HardwareBufferSize.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "HardwareBufferSize", 0 ));
  m_data.HardwareSampleRate.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "HardwareSampleRate", 0 ));
  m_data.NumSampleBuffers.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumSampleBuffers", 1024 ));
  m_data.MaxNumNodes.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "MaxNumNodes", 1024 ));
  m_data.MaxNumSynthDefs.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "MaxNumSynthDefs", 1024 ));
  m_data.RTMemSize.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "RTMemSize", 2048 ));
  m_data.NumWireBuffers.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumWireBuffers", 64 ));
  m_data.NumRandomSeeds.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "NumRandomSeeds", 64 ));
  m_data.BoolLoadSynthdefs = AfxGetApp( )->GetProfileInt("Properties", "BoolLoadSynthdefs", 1 );
  
  // PortAudio stuff...
  m_data.AudioInputDevice = AfxGetApp( )->GetProfileString("Properties", "AudioInputDevice", ""); // default will be changed
  m_data.AudioInputDeviceIndex = PaUtils_GetIndexFromPaInDevName(m_data.AudioInputDevice);
  if(m_data.AudioInputDeviceIndex == -1) { // either because name is "" or device removed
    // either because name is "" or device removed
    if( m_data.AudioInputDevice.Compare("") != 0 ) {
      CString msg;
      msg.Format("The previously selected input device (%s)\nis no longer available. The default PortAudio input device has been selected",m_data.AudioInputDevice);
      ::MessageBox(NULL, msg, "Warning", MB_ICONWARNING | MB_OK);
    }
    m_data.AudioInputDeviceIndex = PaUtils_GetDefaultInputDeviceIndex( );
  }
  m_data.AudioInputDeviceNumChannels.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "AudioInputDeviceNumChannels", 2 ));

  m_data.AudioOutputDevice = AfxGetApp( )->GetProfileString("Properties", "AudioOutputDevice", ""); // default will be changed
  m_data.AudioOutputDeviceIndex = PaUtils_GetIndexFromPaOutDevName(m_data.AudioOutputDevice);
  if(m_data.AudioOutputDeviceIndex == -1) { 
    // either because name is "" or device removed
    if( m_data.AudioOutputDevice.Compare("") != 0 ) {
      CString msg;
      msg.Format("The previously selected output device (%s)\nis no longer available. The default PortAudio output device has been selected",m_data.AudioOutputDevice);
      ::MessageBox(NULL, msg, "Warning", MB_ICONWARNING | MB_OK);
    }
    m_data.AudioOutputDeviceIndex = PaUtils_GetDefaultOutputDeviceIndex( );
  }
  m_data.AudioOutputDeviceNumChannels.Format("%d",AfxGetApp( )->GetProfileInt("Properties", "AudioOutputDeviceNumChannels", 2 ));
}

void CPropertyListCtrlDlg::LaunchSC( )
{
  // let's prepare a command line (DOES include the app name)

  CString networkParams;
  if (m_data.ConnectionType == 0)
    networkParams.Format("-u %s ", m_data.PortNumber);
  else if (m_data.ConnectionType == 1)
    networkParams.Format("-u %s ", m_data.PortNumber);
  else
    throw "Invalid m_data.ConnectionType";

  CString bussesParams;
  bussesParams.Format("-c %s -a %s -i %s -o %s ", m_data.NumControlBusChannels, m_data.NumAudioBusChannels, m_data.NumInputBusChannels, m_data.NumOutputBusChannels);

  CString sizesParams;
  sizesParams.Format("-z %s -Z %s -S %s ", m_data.BlockSize, m_data.HardwareBufferSize, m_data.HardwareSampleRate);

  CString serverMisc;
  serverMisc.Format("-b %s -n %s -d %s -m %s -w %s -r %s -D %d -l %s ",
    m_data.NumSampleBuffers, 
    m_data.MaxNumNodes, 
    m_data.MaxNumSynthDefs, 
    m_data.RTMemSize, 
    m_data.NumWireBuffers, 
    m_data.NumRandomSeeds,
    m_data.BoolLoadSynthdefs, // 1 or 0
    m_data.MaxNumLogins );

  CString pwdParam;
  if (m_data.TCPPassword.Compare("") == 0)
    pwdParam.Format("");
  else
    pwdParam.Format("-p %s ", m_data.TCPPassword );

  CString cmdLine;
#ifdef _DEBUG
  cmdLine.Format("scsynth_debug.exe %s %s %s %s %s", networkParams, bussesParams, sizesParams, serverMisc, pwdParam);
#else //_DEBUG
  cmdLine.Format("scsynth.exe %s %s %s %s %s", networkParams, bussesParams, sizesParams, serverMisc, pwdParam);
#endif //_DEBUG

  // next, we set the current process environment vars for SC3, because
  // the portaudio stuff uses environnment variables for its 
  // config and it's easier to set the CURRENT process env vars and 
  // tell CreateProcess to inherit them rather than painfully build
  // an env block including the original ones....
  BOOL berr;
  berr = ::SetEnvironmentVariable("SC_PORTAUDIO_INPUTS", m_data.AudioInputDeviceNumChannels);
  if (!berr)
    throw "Problem calling ::SetEnvironmentVariable(...)";
  berr = ::SetEnvironmentVariable("SC_PORTAUDIO_OUTPUTS", m_data.AudioOutputDeviceNumChannels);
  if (!berr)
    throw "Problem calling ::SetEnvironmentVariable(...)";
  CString strAudioInputDeviceIndex;
  strAudioInputDeviceIndex.Format("%d",m_data.AudioInputDeviceIndex);
  berr = ::SetEnvironmentVariable("SC_PORTAUDIO_INPUT_DEVICE", strAudioInputDeviceIndex);
  if (!berr)
    throw "Problem calling ::SetEnvironmentVariable(...)";
  CString strAudioOutputDeviceIndex;
  strAudioOutputDeviceIndex.Format("%d",m_data.AudioOutputDeviceIndex);
  berr = ::SetEnvironmentVariable("SC_PORTAUDIO_OUTPUT_DEVICE", strAudioOutputDeviceIndex);
  if (!berr)
    throw "Problem calling ::SetEnvironmentVariable(...)";

  STARTUPINFO startupInfo;
  PROCESS_INFORMATION result;

  startupInfo.cb = sizeof(STARTUPINFO);
  startupInfo.lpReserved = NULL;
  startupInfo.lpDesktop = NULL; //inherit
  startupInfo.lpTitle = NULL; // use exe name
  startupInfo.dwX = 0; // ignored because of dwFlags
  startupInfo.dwY = 0; // ignored because of dwFlags
  startupInfo.dwXSize = 0; // ignored because of dwFlags
  startupInfo.dwYSize = 0; // ignored because of dwFlags
  startupInfo.dwXCountChars = 0; // ignored because of dwFlags
  startupInfo.dwYCountChars = 0; // ignored because of dwFlags
  startupInfo.dwFillAttribute = 0; // ignored because of dwFlags
  startupInfo.dwFlags = 0; // nothing specified. use defaults everywhere  
  startupInfo.wShowWindow = 0; // ignored because of dwFlags;  
  startupInfo.cbReserved2 = 0;  
  startupInfo.lpReserved2 = NULL;  
  startupInfo.hStdInput = 0; // ignored because of dwFlags  
  startupInfo.hStdOutput = 0; // ignored because of dwFlags  
  startupInfo.hStdError = 0; // ignored because of dwFlags

  // CreateProcess Unicode wants a non-readonly lpCommandLine <sigh>
  // start Psycollider (try the exe version first)
  char* langCmdLine;
  langCmdLine = "Psycollider.exe";
  char* szCmdLine = new char[cmdLine.GetLength( )+ strlen(langCmdLine) + 4];
  strcpy(szCmdLine,langCmdLine);
  strcat(szCmdLine," \"");
  strcat(szCmdLine,cmdLine);
  strcat(szCmdLine,"\"");
  
  berr = CreateProcess(
    /* LPCTSTR lpApplicationName*/ NULL,
    /* LPTSTR lpCommandLine*/ szCmdLine,
    /* LPSECURITY_ATTRIBUTES lpProcessAttributes*/ NULL,
    /* LPSECURITY_ATTRIBUTES lpThreadAttributes*/ NULL,
    /* BOOL bInheritHandles*/ FALSE,
    /* DWORD dwCreationFlags*/ NULL,
    /* LPVOID lpEnvironment*/ NULL, // inherit parent
    /* LPCTSTR lpCurrentDirectory*/ NULL, // current folder
    /* LPSTARTUPINFO lpStartupInfo*/ &startupInfo,
    /* LPPROCESS_INFORMATION lpProcessInformation*/ &result
  );
  
  printf("%s\n", szCmdLine);
  if(!berr) {

	// try to spawn Psycollider.py calling python
    langCmdLine = "python.exe Psycollider.py";
	szCmdLine = new char[cmdLine.GetLength( )+ strlen(langCmdLine) + 4];
	strcpy(szCmdLine,langCmdLine);
	strcat(szCmdLine," \"");
	strcat(szCmdLine,cmdLine);
	strcat(szCmdLine,"\"");
	berr = CreateProcess(
		/* LPCTSTR lpApplicationName*/ NULL,
		/* LPTSTR lpCommandLine*/ szCmdLine,
		/* LPSECURITY_ATTRIBUTES lpProcessAttributes*/ NULL,
		/* LPSECURITY_ATTRIBUTES lpThreadAttributes*/ NULL,
		/* BOOL bInheritHandles*/ FALSE,
		/* DWORD dwCreationFlags*/ NULL,
		/* LPVOID lpEnvironment*/ NULL, // inherit parent
		/* LPCTSTR lpCurrentDirectory*/ NULL, // current folder
		/* LPSTARTUPINFO lpStartupInfo*/ &startupInfo,
		/* LPPROCESS_INFORMATION lpProcessInformation*/ &result
	);

	if(!berr) {
		char* msgBuf;
		DWORD errorCode = ::GetLastError( );
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &msgBuf,
			0,
			NULL
		);
		CString guiErrMsg;
		guiErrMsg.Format("Sorry. Couldn't launch scsynth.exe using ::CreateProcess(...).\nThe error is %s (%d).", msgBuf, errorCode);
		::MessageBox(NULL,guiErrMsg,"Error",MB_ICONERROR | MB_OK);
		::LocalFree( msgBuf);
		delete [] szCmdLine;
		return;
	}
  }
  delete [] szCmdLine;
  ::CloseHandle(result.hProcess);
  ::CloseHandle(result.hThread);
}

void CPropertyListCtrlDlg::FillRegistryFromData( )
{
  // this retrieves the stored preferences, or default 
  // values if those aren't set
  
  // standard scsynth cmd line flags
  AfxGetApp( )->WriteProfileInt("Properties", "ConnectionType", m_data.ConnectionType ); 
  AfxGetApp( )->WriteProfileInt("Properties", "PortNumber", atoi(m_data.PortNumber ));
  AfxGetApp( )->WriteProfileString("Properties", "TCPPassword", m_data.TCPPassword );
  AfxGetApp( )->WriteProfileInt("Properties", "MaxNumLogins", atoi(m_data.MaxNumLogins));
  AfxGetApp( )->WriteProfileInt("Properties", "NumControlBusChannels", atoi(m_data.NumControlBusChannels));
  AfxGetApp( )->WriteProfileInt("Properties", "NumAudioBusChannels", atoi(m_data.NumAudioBusChannels));
  AfxGetApp( )->WriteProfileInt("Properties", "NumInputBusChannels", atoi(m_data.NumInputBusChannels));
  AfxGetApp( )->WriteProfileInt("Properties", "NumOutputBusChannels", atoi(m_data.NumOutputBusChannels));
  AfxGetApp( )->WriteProfileInt("Properties", "BlockSize", atoi(m_data.BlockSize));
  AfxGetApp( )->WriteProfileInt("Properties", "HardwareBufferSize", atoi(m_data.HardwareBufferSize));
  AfxGetApp( )->WriteProfileInt("Properties", "HardwareSampleRate", atoi(m_data.HardwareSampleRate));
  AfxGetApp( )->WriteProfileInt("Properties", "NumSampleBuffers", atoi(m_data.NumSampleBuffers));
  AfxGetApp( )->WriteProfileInt("Properties", "MaxNumNodes", atoi(m_data.MaxNumNodes));
  AfxGetApp( )->WriteProfileInt("Properties", "MaxNumSynthDefs", atoi(m_data.MaxNumSynthDefs));
  AfxGetApp( )->WriteProfileInt("Properties", "RTMemSize", atoi(m_data.RTMemSize));
  AfxGetApp( )->WriteProfileInt("Properties", "NumWireBuffers", atoi(m_data.NumWireBuffers));
  AfxGetApp( )->WriteProfileInt("Properties", "NumRandomSeeds", atoi(m_data.NumRandomSeeds));
  AfxGetApp( )->WriteProfileInt("Properties", "BoolLoadSynthdefs", m_data.BoolLoadSynthdefs);
  
  // PortAudio stuff...
  m_data.AudioInputDevice = PaUtils_GetPaInDevNameFromIndex(m_data.AudioInputDeviceIndex);
  AfxGetApp( )->WriteProfileString("Properties", "AudioInputDevice", m_data.AudioInputDevice);
  AfxGetApp( )->WriteProfileInt("Properties", "AudioInputDeviceNumChannels", atoi(m_data.AudioInputDeviceNumChannels ));
  m_data.AudioOutputDevice = PaUtils_GetPaOutDevNameFromIndex(m_data.AudioOutputDeviceIndex);
  AfxGetApp( )->WriteProfileString("Properties", "AudioOutputDevice", m_data.AudioOutputDevice);
  AfxGetApp( )->WriteProfileInt("Properties", "AudioOutputDeviceNumChannels", atoi(m_data.AudioOutputDeviceNumChannels));
}




void CPropertyListCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPropertyListCtrlDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting
    CString txt = "SuperCollider 3 Launch Panel";
    CFont font;
    VERIFY(font.CreateFont(
      36,                        // nHeight
      0,                         // nWidth
      0,                         // nEscapement
      0,                         // nOrientation
      FW_NORMAL,                 // nWeight
      FALSE,                     // bItalic
      FALSE,                     // bUnderline
      0,                         // cStrikeOut
      ANSI_CHARSET,              // nCharSet
      OUT_DEFAULT_PRECIS,        // nOutPrecision
      CLIP_DEFAULT_PRECIS,       // nClipPrecision
      DEFAULT_QUALITY,           // nQuality
      DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
      "Arial"));                 // lpszFacename
    CFont* prevFont = dc.SelectObject(&font);
    int prevBkMode = dc.SetBkMode(TRANSPARENT);
    dc.TextOut(10,10,txt);
    dc.SelectObject(prevFont);
    font.DeleteObject();
    dc.SetBkMode(prevBkMode);
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPropertyListCtrlDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPropertyListCtrlDlg::OnOK() 
{
	m_propertyManager.GetData(&m_data);
  CDialog::OnOK( );
}

void CPropertyListCtrlDlg::OnButtonExit() 
{
	m_propertyManager.GetData(&m_data);

	CDialog::OnOK();
}


void CPropertyListCtrlDlg::OnSize(UINT nType, int cx, int cy)
{
  ((CListBox*)(this->GetDlgItem(IDC_LIST1)))->SetColumnWidth(30);
}

void CPropertyListCtrlDlg::OnBnClickedOk()
{
  OnOK();
}
