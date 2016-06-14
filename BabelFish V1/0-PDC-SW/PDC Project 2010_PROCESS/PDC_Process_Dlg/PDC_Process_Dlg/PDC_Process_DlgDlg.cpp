
// PDC_Process_DlgDlg.cpp : implementation file
//

//*******************************************Activex settings*************************************************
// var_test_vi.cpp : Defines the entry point for the console application.
//this is a pilot version for PDC_Port.vi labview app in order to exchange data with 
//the vi using activex server of the labview and this application is a client to use labview
//activex server and connect to the PDC_Port.vi to exchange data with controls in PDC_Port.vi

#include "stdafx.h"
#include "PDC_Process_Dlg.h"
#include "PDC_Process_DlgDlg.h"
#include "afxdialogex.h"
//#include "CFG2Params.h"
#include <complex>
#include <math.h>

using namespace std;

#pragma warning(disable: 4996)

//static LabVIEW::_ApplicationPtr pLV(NULL);
//static LabVIEW::VirtualInstrumentPtr pVI;
static HANDLE hPHBOXEvent;
static HANDLE ghPHBOXThread;
static DWORD  dwPHB_Thread_Id;
static 	_variant_t vtRow;
static _variant_t vtStart;
static BOOL bNow(FALSE);

//*************************************************************************************************************
//static LabVIEW::_ApplicationPtr pLV1(NULL);
//static LabVIEW::VirtualInstrumentPtr pVI1;
//BOOL bOnce = FALSE;
//*************************************************************************************************************
#define PI 3.14159265
//*************************************************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CONNECTION_MSG		(WM_USER + 101)/*1125*/
#define HEADERFRAM_MSG		(WM_USER + 102)/*1126*/
#define CFG_1_MSG			(WM_USER + 103)/*1127*/
#define CFG_2_MSG			(WM_USER + 104)/*1128*/
#define UPDATE_PHASOR		(WM_USER + 105)/*1129*/
#define PMU_LIST_SEL		(WM_USER + 106)/*1130*/
#define Info_Type_SEL		(WM_USER + 107)/*1131*/
#define UPDATE_USER_CONFIG	(WM_USER + 108)/*1132*/
#define INFO_TYPE_LIST		(WM_USER + 109)/*1133*/
#define PAHSOR_PARAM_SEL	(WM_USER + 110)/*1134*/
#define CHOOSE_PHASOR_DATA	(WM_USER + 111)/*1135*/
#define UPDATE_RTDATA		(WM_APP +  112)/*1136*/
#define NO_UPDATE_ONLY_RUN	(WM_USER + 113)/*1137*/
#define BUTTON2_CLICKED		(WM_USER + 114)/*1138*/
#define UPDATE_CMPDATA		(WM_USER + 115)/*1139*/
#define ANALOG_LIST_SEL		(WM_USER + 116)/*1140*/
#define DIGITAL_LIST_SEL	(WM_USER + 117)/*1141*/
#define LOAD_CONFIG_DATA	(WM_USER + 118)/*1142*/
#define BUILD_DECODE_ARRAY	(WM_USER + 119)/*1143*/
#define REFRESH_RATE_SEL	(WM_USER + 120)/*1144*/
#define CHOSEN_DATA_UPDATE	(WM_USER + 121)/*1145*/


//control process structure
typedef struct _tagParam{
	void * p;
}PARAM;

// stores the ID of the process event thread
HANDLE h, hMain;

//initialize the static data members of the CPDC_Process_DlgDlg class
HANDLE CPDC_Process_DlgDlg::ghMutex = NULL;

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPDC_Process_DlgDlg dialog




CPDC_Process_DlgDlg::CPDC_Process_DlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPDC_Process_DlgDlg::IDD, pParent),
	m_pTCPCMU1(NULL),
	m_pTCPCMU2(NULL)
	, m_pPDCData(NULL)
	, m_iPhasors(0)
	, m_nPMU_Number(0)
	, m_lPHNMR(0)
	, z(0)
	, zchosendt(0)
	, prev(-1)
	, m_bSkip(FALSE)
	, m_bFillCluster(FALSE)
	, m_szLogFile("")
	, m_bConfig2Loaded(FALSE)
	, m_bSkipPlay(FALSE)
	, m_u32_Rfsh(0)
	, m_nIndex(0)
	, zchosendt1(0)
	, zchosendt2(0)
	, zchosendt3(0)
{
	HRESULT hr;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	if( FAILED( hr = ::CoGetMalloc(1, (LPMALLOC*)&m_pMalloc )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	else
	{
#ifdef	_DEBUG
		AtlTrace("Success: IMalloc is created.\n");
#endif
	}

	m_pPDCData = (char*)m_pMalloc->Alloc(64 * 1024);
	if( !m_pPDCData )
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	else
		::memset( m_pPDCData, 0, (64 * 1024) );

	::VariantClear(&PHAMTBL);
	::VariantClear(&PHANTBL);
	::VariantClear(&PHNTBL);
	::VariantClear(&PHCSTBL);
	::VariantClear(&SELPHDTLIST);
	::VariantClear(&PMUTBL);
	::VariantClear(&RTDTTSTBL);
	::VariantClear(&RTPHRPVAL);
	::VariantClear(&RTPHIPVAL);
	::VariantClear(&RTPHVALS);
	::VariantClear(&RTFQVALS);
	::VariantClear(&RTDFQVALS);
	::VariantClear(&RTAnalogVal);
	::VariantClear(&RTDigitalVal);
	::VariantClear(&RTDigStrs);
	::VariantClear(&LCHD);
	::VariantClear(&LCHD1);
	::VariantClear(&LCHDINX);
	::VariantClear(&ANDIGNAMETBL);
	::VariantClear(&PHNAMETBL);


	RTPHIPVAL.vt = VT_ARRAY | VT_R4;
	RTPHRPVAL.vt = VT_ARRAY | VT_R4;
	PHAMTBL.vt = VT_ARRAY | VT_BSTR;
	PHANTBL.vt = VT_ARRAY | VT_BSTR;
	PHNTBL.vt = VT_ARRAY | VT_BSTR;
	PHCSTBL.vt = VT_ARRAY | VT_BSTR;
	SELPHDTLIST.vt = VT_ARRAY | VT_BSTR;
	PMUTBL.vt = VT_ARRAY | VT_BSTR;
	RTDTTSTBL.vt = VT_ARRAY | VT_BSTR;
	RTPHVALS.vt = VT_ARRAY | VT_R8;
	RTFQVALS.vt = VT_ARRAY | VT_R4;
	RTDFQVALS.vt = VT_ARRAY | VT_R4;
	RTAnalogVal.vt = VT_ARRAY | VT_R4;
	RTDigitalVal.vt = VT_ARRAY | VT_R4;
	RTDigStrs.vt = VT_ARRAY | VT_BSTR;
	LCHD.vt = VT_ARRAY | VT_BSTR;
	LCHD1.vt = VT_ARRAY | VT_BSTR;
	LCHDINX.vt = VT_ARRAY | VT_I4;
	ANDIGNAMETBL.vt = VT_ARRAY | VT_BSTR;
	PHNAMETBL.vt = VT_ARRAY | VT_BSTR;

	//make safearray NULL
	RTPHIPVAL.parray = NULL;
	RTPHRPVAL.parray = NULL;
	PHAMTBL.parray = NULL;
	PHANTBL.parray = NULL;
	PHNTBL.parray = NULL;
	PHCSTBL.parray = NULL;
	SELPHDTLIST.parray = NULL;
	PMUTBL.parray = NULL;
	RTDTTSTBL.parray = NULL;
	RTPHVALS.parray = NULL;
	RTFQVALS.parray = NULL;
	RTDFQVALS.parray = NULL;
	RTAnalogVal.parray = NULL;
	RTDigitalVal.parray = NULL;
	RTDigStrs.parray = NULL;
	LCHD.parray = NULL;
	LCHD1.parray = NULL;
	LCHDINX.parray = NULL;
	ANDIGNAMETBL.parray = NULL;
	PHNAMETBL.parray = NULL;

	//initialize the log file.
	m_szLogFile	 = ".\\log.txt";
}

void CPDC_Process_DlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPDC_Process_DlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(CONNECTION_MSG, OnConnectionMsg)
	ON_MESSAGE(HEADERFRAM_MSG, OnHeaderFrameMsg)
	ON_MESSAGE(CFG_1_MSG, OnCFG_1_FrameMsg)
	ON_MESSAGE(CFG_2_MSG, OnCFG_2_FrameMsg)
	ON_MESSAGE(UPDATE_PHASOR, OnUpdatePhasor)
	ON_MESSAGE(PMU_LIST_SEL, OnPMUListSel)
	ON_MESSAGE(Info_Type_SEL, OnInfoTypeSel)
	ON_MESSAGE(INFO_TYPE_LIST, OnInfoTypeList)
	ON_MESSAGE(UPDATE_USER_CONFIG, OnUpdateUserConfig)
	ON_MESSAGE(PAHSOR_PARAM_SEL, OnPhasorParamSel)
	ON_MESSAGE(CHOOSE_PHASOR_DATA, OnChoosePhDt)
	ON_MESSAGE(UPDATE_RTDATA, OnUpdateRTData)
	ON_MESSAGE(NO_UPDATE_ONLY_RUN, OnRTOnlyRun)
	ON_MESSAGE(BUTTON2_CLICKED, OnButton2Clicked)
	ON_MESSAGE(UPDATE_CMPDATA, OnCMPData)
	ON_MESSAGE(ANALOG_LIST_SEL, OnAnaListSel)
	ON_MESSAGE(DIGITAL_LIST_SEL, OnDigListSel)
	ON_MESSAGE(LOAD_CONFIG_DATA, OnLoadConfigData)
	ON_MESSAGE(BUILD_DECODE_ARRAY, OnBldDecArr)
	ON_MESSAGE(REFRESH_RATE_SEL, OnRefshRateSel)
	ON_MESSAGE(CHOSEN_DATA_UPDATE, OnChDTUPDATE)
	ON_BN_CLICKED(IDC_BUTTON1, &CPDC_Process_DlgDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPDC_Process_DlgDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CPDC_Process_DlgDlg::OnBnClickedButton3)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CPDC_Process_DlgDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPDC_Process_DlgDlg message handlers

BOOL CPDC_Process_DlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: Add extra initialization here
	hMain = ::GetCurrentThread();
	StartCM();
//iITC	Connect();

									
	//call the thread that connects to the PDC Server and enables the real-time data 
	//transmission for the PDC server.
//iITC	::PostMessageA ( m_hWnd, CONNECTION_MSG, 0, 0 );
	
	//call the thread that retrieves the Header Frame from
	//the PDC server.
//iITC	::PostMessageA( m_hWnd, HEADERFRAM_MSG, 0, 0 );
	
	//call the thread that retrieves the CFG_1 frame from
	//the PDC Server.
//iITC	::PostMessageA ( m_hWnd, CFG_1_MSG, 0, 0 );

//Activex related initializations
	InitActivexServer();

	//Call the thread that retrieves the CFG_2 frame from
	//the PDC Server. this thread can be halted as the program
	//logic instructs it, only when the user requests for it 
	//or when the program needs to figure-out the real-time data
	//or wherever this frame is needed.
//	::PostMessageA ( m_hWnd, CFG_2_MSG, 0, 0 );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPDC_Process_DlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPDC_Process_DlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPDC_Process_DlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD CPDC_Process_DlgDlg::StartCM(void)
{
	//IP address of the PDC is: (130.237.53.177)  !!!!!!!!!!!!
	//Port Number of the KTH PDC is: (6714)	!!!!!!!!
	//remember that IP address in in text format and it must be converted to DWORD with
	//a call to inet_addr function
	HRESULT hr;
	
//	if(FAILED(::CoInitialize(NULL)))
	if(FAILED(::CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	if(FAILED( hr = ::CoCreateInstance( CLSID_TCPCMU1,
										NULL,
										CLSCTX_INPROC_SERVER,
										IID_ITCPCMU1,
										(void**)&m_pTCPCMU1
										)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//call Initialize()
	if( FAILED( hr = m_pTCPCMU1->Initialize() ) )
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	if(FAILED( hr = ::CoCreateInstance( CLSID_PDCDMU1,
										NULL,
										CLSCTX_INPROC_SERVER,
										IID_IPDCDMU1,
										(void**)&m_pDMU1
										)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	ghMutex = ::CreateMutexA(NULL, FALSE, NULL);
	if( ghMutex == NULL )
	{
		AtlTrace("Error: Failed to create mutex object.\n");
		return FALSE;
	}
	else
		AtlTrace("Success: Mutex object is created.\n");

	//create an event to control the phasor list box update with
	//double click of the user in PDC_Port.vi application
	hPHBOXEvent = ::CreateEventA(NULL, TRUE, FALSE, "");
	if( !hPHBOXEvent )
	{
		DWORD dwError = ::GetLastError();
		return FALSE;
	}
	else
	{
		AtlTrace("Success: Create event ");
	}
	//initialize the log file name in the ITCPCMU1 object
	_bstr_t bstrLgfilnm = m_szLogFile.c_str();
	if(FAILED( hr = m_pTCPCMU1->put_LogFilNm(bstrLgfilnm.copy())))
		return -1;

	return 0;
}


DWORD CPDC_Process_DlgDlg::Connect(void)
{
	HRESULT hr;
//	char szIP[32];
	DWORD u32_IP(0);
	USHORT u16_Port(0);
//	DWORD dwThreadID(0);
	DWORD dwResult(0);
	PARAM param; 
	BSTR bstrConName(NULL);
	_bstr_t _bstrConName;
	
	::memset(&param, 0, sizeof(PARAM));
//	::CoInitialize(NULL);
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	m_szFName = "C:\\KTH software to check\\VI TUTORIAL PROJECTS\\NetConfig.txt";
	
	if( m_bSkip )
	//load the networkk configuration parameters.
		LoadConfig();

	//call ConnectTo(...)
	//IP address is in form of DWORD, therefore it is inet_addr which converts "<IP address>" into DWORD type
	u32_IP = inet_addr(m_szIP.c_str());
	u16_Port = atoi(m_szPort.c_str());
/*
	//set the name of the socket
	_bstr_t bstrSockName("Connection Socket");
	if( FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
		assert(NULL);
*/
	//send Enable command to the server
	if(FAILED( hr = m_pTCPCMU1->ConnectTo( u32_IP, u16_Port, 0, 0)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//retreive the name of the connectio to the PDC Server
	if(FAILED( hr = m_pTCPCMU1->get_SocketName(&bstrConName)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	_bstrConName = bstrConName;
		
	//create a thread to wait on event FD_CONNECT and then process it
	HANDLE hThread = ::CreateThread( NULL,
										0,
										(LPTHREAD_START_ROUTINE)ProcessEventThread,
										(LPVOID)this,
										0,
										&dwThreadID
										);
	if (hThread == NULL)
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

//	DWORD dwUpdatRtDtThdID(0);
	//create a thread to wait on event FD_CONNECT and then process it
	hUpdatRtDtThd = ::CreateThread( NULL,
										0,
										(LPTHREAD_START_ROUTINE)UpdateRTDataThread,
										(LPVOID)this,
										0,
										&dwUpdatRtDtThdID
										);
	if (hUpdatRtDtThd == NULL)
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	::SuspendThread(hUpdatRtDtThd);

	ghMutex = ::CreateMutexA(NULL, FALSE, NULL);
	if( ghMutex == NULL )
	{
		AtlTrace("Error: Failed to create mutex object.\n");
		return FALSE;
	}
	else
		AtlTrace("Success: Mutex object is created.\n");
	/*
	//create an event to control the phasor list box update with
	//double click of the user in PDC_Port.vi application
	hPHBOXEvent = ::CreateEventA(NULL, TRUE, FALSE, "");
	if( !hPHBOXEvent )
	{
		DWORD dwError = ::GetLastError();
		return FALSE;
	}
	else
	{
		AtlTrace("Success: Create event ");
	}

	//create a thread to watch this event and 
	//handle the triggering of it from PDC_Port.vi
	ghPHBOXThread = ::CreateThread( NULL,
									0,
									(LPTHREAD_START_ROUTINE)UpdatePhasorLboxEvent,
									(LPVOID)this,
									0,
									&dwPHB_Thread_Id
									);
									*/
	return 0;
}

/*
//process control for the thread that updates the phasor list box
ULONG WINAPI CPDC_Process_DlgDlg::UpdatePhasorLboxEvent(void * pParam)
{
	DWORD dwRes(0);
	dwRes = ::WaitForSingleObject( hPHBOXEvent, INFINITE );
	switch( dwRes )
	{
		case	WAIT_OBJECT_0:
			__try
			{
					{
						HANDLE h = ::GetCurrentThread();

						AtlTrace("ProcessEventThead is called by thread: %d\n", ::GetThreadId(h));

						CPDC_Process_DlgDlg * p_This = (CPDC_Process_DlgDlg *)pParam;
						p_This->UpdatePhasorLbox();
					}
					break;
			}
			//any error triggers a process below
			__finally
			{//release the owner ship of the mutex by a given the thread
				if( !::ReleaseMutex(ghMutex) )
				{
					//handle error
				}
			}
		
		case WAIT_ABANDONED:
			assert(NULL);
	}

	return 0;
}
*/
//this is the thread control process that is called immediately after
//thread created. and thread starts to execute it and once it is finished
//the thread is killed

//this is a static function!
ULONG WINAPI CPDC_Process_DlgDlg::ProcessEventThread(void * pParam)
{
	DWORD dwRes(0);
	dwRes = ::WaitForSingleObject( ghMutex, 0 );
	switch( dwRes )
	{
		case	WAIT_OBJECT_0:
			__try
			{
					{
						h = ::GetCurrentThread();

						AtlTrace("ProcessEventThead is called by thread: %d\n", ::GetThreadId(h));

						CPDC_Process_DlgDlg * p_This = (CPDC_Process_DlgDlg *)pParam;
						p_This->ProcessEvents();
					}
					break;
			}
			//any error triggers a process below
			__finally
			{//release the owner ship of the mutex by a given the thread
				if( !::ReleaseMutex(ghMutex) )
				{
					//handle error
				}
			}
		
		case WAIT_ABANDONED:
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	}

	return 0;
}

//this is a static function!
ULONG WINAPI CPDC_Process_DlgDlg::UpdatePhasorThread(void * pParam)
{
	// _variant_t is a class wrapper for variant data types
	_variant_t one_str, two_str, three_str, four_str;
    _variant_t str_list;
	_variant_t list_phasor, list_phasor2, list_phasor3, list_phasor4;
/*
    int size = 0;
    char Path[1000],Password[60] = "";
 
    // a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
 
    using namespace LabVIEW;
	_ApplicationPtr pLV(NULL);
	VirtualInstrumentPtr pVI;

	char VIPath[100] =  VI_PATH;
//	CoInitialize(NULL);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Create Reference to LabVIEW ActiveX COM object
	pLV.CreateInstance("LabVIEW.Application");
	if (pLV == NULL)
	{	
		printf("LV must be running, exiting ...\n");
		return false;
		//break;
	}
    
	// Create Reference to VI
	pVI.CreateInstance("LabVIEW.VirtualInstrument");

	strcpy(Path, VIPath);
	//this is very importqnt
	_bstr_t bstrPath(Path);
	_bstr_t bstrPass("");

    // assign an object reference to the pVI.
//    pVI = pLV->GetVIReference(LPCTSTR(Path), LPCTSTR(Password), 0, 0x10);
	pVI = pLV->GetVIReference(bstrPath, bstrPass, 0, 0x10);*/
	SYSTEMTIME	st1, st2;
	::memset(&st1, 0, sizeof(SYSTEMTIME));
	::memset(&st2, 0, sizeof(SYSTEMTIME));

	//acquire the time before writing
	::GetSystemTime(&st1);
	VARIANT var;
	var.vt = VT_DATE;
	if( !::SystemTimeToVariantTime(&st1, &var.date) )
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
/*
    // configure the VI to show its front panel on Call.
    pVI->ShowFPOnCall = TRUE;
	pVI->OpenFrontPanel(VARIANT_TRUE, LabVIEW::eVisible); 
	*/
	//Transfer control to LabVIEW with the Run statement.  The call function passes the parameter names
	// and data to the LabVIEW VI.
	return 0;
}
/*
void CPDC_Process_DlgDlg::UpdatePhasorLbox(void)
{
//	::PostMessageA(m_hWnd, PMU_LIST_SEL, 0, 0);
	//update the phasor listbox
//	pVI->GetControlValue(_bstr_t("return type"));

	while(1)
	{
		if( pVI && bNow )
		{
			_variant_t phasors(vecPhasors[2]);
			phasors.vt = 8200;
			pVI->SetControlValue("PHASORX", phasors);
			//pVI->Run(0);
		}
	}
}
*/
void CPDC_Process_DlgDlg::ProcessEvents(void)
{
	HRESULT hr;
	BSTR bstrConName(NULL);
	_bstr_t _bstrConName;

	//retreive the name of the connectio to the PDC Server
	if(FAILED( hr = m_pTCPCMU1->get_SocketName(&bstrConName)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	_bstrConName = bstrConName;
	_bstr_t _b1 = "RealTimeData Socket";
	_bstr_t _b2 = "ConfigurationFrame2 Socket";

	_bstr_t _bstr = "";
	CWnd * pWnd = AfxGetApp()->GetMainWnd();
	if( (_bstrConName == _b1 ) && m_bSkip )
		m_pTCPCMU1->ProcessEventsRT(pWnd->m_hWnd, NO_UPDATE_ONLY_RUN, hPHBOXEvent);
	else if(( _bstrConName == _b2 ) && m_bSkip)
		m_pTCPCMU1->ProcessEvents(pWnd->m_hWnd, UPDATE_PHASOR);
	else if( !m_bSkip )
		m_pTCPCMU1->ProcessEventsRT(pWnd->m_hWnd, UPDATE_RTDATA, hPHBOXEvent);
	else
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
}

//sends commands to the server after connection is established
DWORD CPDC_Process_DlgDlg::Send(void)
{
	//create a thread that process events

	//send a command like 0, 1, 2, ... to the PDC server to communicate with it
	//we can send command 2 to get the header.
	return 0;
}

//this method starts the thread that connects to the PDC server and enables the real-data transmission
LRESULT CPDC_Process_DlgDlg::OnConnectionMsg(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;

	AtlTrace("OnConnectMsg is called.\n");

	//set the name of the socket
	_bstr_t bstrSockName("Connection Socket");
	if( FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	DWORD u32_Error = Connect();
	if( u32_Error ){
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
		return 1;
	}
		
	return 0;
}

//this method starts the thread which retrieves the header frame
LRESULT CPDC_Process_DlgDlg::OnHeaderFrameMsg(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;

	AtlTrace("OnHeaderFrameMsg is called.\n");

	//set the name of the socket
	_bstr_t bstrSockName("HeaderFrame Socket");
	if(FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//connect to the PDC Server
	DWORD u32_Error = Connect();
	if( u32_Error ){
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	}

	return 0;
}

//this method starts the thread which retrieves the CFG_1 Frame from the PDC server
LRESULT CPDC_Process_DlgDlg::OnCFG_1_FrameMsg(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;

	AtlTrace("OnCFG_1_FrameMsg is called.\n");


	//set the name of the socket
	_bstr_t bstrSockName("ConfigurationFrame1 Socket");
	if(FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//connect to the PDC Server
	DWORD u32_Error = Connect();
	if( u32_Error ){
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	}

	return 0;
}

//this method starts the thread which retrieves the CFG_2 Frame from the PDC Server
LRESULT CPDC_Process_DlgDlg::OnCFG_2_FrameMsg(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;

	AtlTrace("OnCFG_2_FrameMsg is called.\n");
	unsigned char ch;

	//check object exists
#ifdef _DEBUG
	assert(m_pTCPCMU1);
#endif
	unsigned char SYNC[2]; SYNC[0] = 0xaa;SYNC[1] = 0x31;
	//you want to disable the data you should send SYNC = 0xaa 01
	if( FAILED( hr = m_pTCPCMU1->put_SYNC1(SYNC[0])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC1(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("First byte of SYNC is acquired: 0x%x\n", ch);
	if( FAILED( hr = m_pTCPCMU1->put_SYNC2(SYNC[1])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC2(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("Second byte of SYNC is acquired: 0x0%x\n", ch);	

	//set the name of the socket
	_bstr_t bstrSockName("ConfigurationFrame2 Socket");
	if(FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//connect to the PDC Server
	DWORD u32_Error = Connect();
	if( u32_Error ){
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
	}

	return 0;
}

//disable real-time data button handler
void CPDC_Process_DlgDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	HRESULT hr;
	unsigned char ch;

	AtlTrace("Disable Real Time Data.\n");
	//check object exists
#ifdef _DEBUG
	assert(m_pTCPCMU1);
#endif
	unsigned char SYNC[2]; SYNC[0] = 0xaa;SYNC[1] = 0x01;
	//you want to disable the data you should send SYNC = 0xaa 01

	if( FAILED( hr = m_pTCPCMU1->put_SYNC1(SYNC[0])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC1(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("First byte of SYNC is acquired: 0x%x\n", ch);
	if( FAILED( hr = m_pTCPCMU1->put_SYNC2(SYNC[1])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC2(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("Second byte of SYNC is acquired: 0x0%x\n", ch);	
	//set the name of the socket
	_bstr_t bstrSockName("Disable RealTimeData Socket");
	if(FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//skip necessary step for enable real-time data
	m_bSkip = FALSE;

	//force the program to skip certain steps for enabling real-time data
	if( FAILED( hr = m_pTCPCMU1->put_SkipSteps4RT(0)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//connect to the PDC Server
	DWORD u32_Error = Connect();
	if( u32_Error ){
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
	}
}

//Enable real-time data button handler
void CPDC_Process_DlgDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	HRESULT hr;
	unsigned char ch;

	AtlTrace("Enable Real Time Data.\n");
	//check object exists
#ifdef _DEBUG
	assert(m_pTCPCMU1);
#endif

	//	unsigned char SYNC[2]; SYNC[0] = 0xaa;SYNC[1] = 0x02;
	unsigned char SYNC[2]; SYNC[0] = 0xaa;SYNC[1] = 0x01;
	//you want to disable the data you should send SYNC = 0xaa 01
	if( FAILED( hr = m_pTCPCMU1->put_SYNC1(SYNC[0])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	if( FAILED( hr = m_pTCPCMU1->get_SYNC1(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("First byte of SYNC is acquired: 0x%x\n", ch);
	if( FAILED( hr = m_pTCPCMU1->put_SYNC2(SYNC[1])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC2(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("Second byte of SYNC is acquired: 0x0%x\n", ch);	

	//set the name of the socket
	_bstr_t bstrSockName("RealTimeData Socket");
	if(FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//skip necessary step for enable real-time data
	m_bSkip = FALSE;

	//force the program to skip certain steps for enabling real-time data
	if( FAILED( hr = m_pTCPCMU1->put_SkipSteps4RT(0)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//connect to the PDC Server
	DWORD u32_Error = Connect();
	if( u32_Error ){
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
	}

	//find the Refresh Rate
	hr = m_pTCPCMU1->get_RefreshRate(&m_u32_Rfsh);

}

//Send Configuration frame2 button handler
void CPDC_Process_DlgDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	HRESULT hr;

	AtlTrace("OnCFG_2_FrameMsg is called.\n");
	unsigned char ch;

	//check object exists
#ifdef _DEBUG
	assert(m_pTCPCMU1);
#endif
	unsigned char SYNC[2]; SYNC[0] = 0xaa;SYNC[1] = 0x31;
	//you want to disable the data you should send SYNC = 0xaa 01
	if( FAILED( hr = m_pTCPCMU1->put_SYNC1(SYNC[0])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC1(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("First byte of SYNC is acquired: 0x%x\n", ch);
	if( FAILED( hr = m_pTCPCMU1->put_SYNC2(SYNC[1])))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	if( FAILED( hr = m_pTCPCMU1->get_SYNC2(&ch)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	AtlTrace("Second byte of SYNC is acquired: 0x0%x\n", ch);	

	//set the name of the socket
	_bstr_t bstrSockName("ConfigurationFrame2 Socket");
	if(FAILED( hr = m_pTCPCMU1->put_SocketName( bstrSockName.copy() )))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}
	
	m_bSkip = TRUE;		//raise Skip flag to instruct to program to skip certain
						//steps.
	if(FAILED( hr = m_pTCPCMU1->put_SkipSteps4RT(1)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//connect to the PDC Server
	DWORD u32_Error = Connect();
	if( u32_Error ){
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
	}

	//m_bConfig2Loaded is coninued.
	m_bConfig2Loaded = TRUE;
}

// Initiates Activex client for data exchange with activex server
DWORD CPDC_Process_DlgDlg::InitActivexServer(void)
{
	// _variant_t is a class wrapper for variant data types
	_variant_t one_str, two_str, three_str, four_str;
    _variant_t str_list;
	_variant_t list_phasor, list_phasor2, list_phasor3, list_phasor4;

    int size = 0;
 
    // a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;

	char VIPath[100] =  VI_PATH;
//	CoInitialize(NULL);

	// Create Reference to LabVIEW ActiveX COM object
	pLV.CreateInstance("LabVIEW.Application");
	if (pLV == NULL)
	{	
		printf("LV must be running, exiting ...\n");
		return false;
		//break;
	}
   
	// Create Reference to VI
	pVI.CreateInstance("LabVIEW.VirtualInstrument");

    char Path[1000],Password[60] = "";

	strcpy(Path, VIPath);
	//this is very importqnt
	_bstr_t bstrPath(Path);
	_bstr_t bstrPass("");

	//The following section is responsible for grabbing one 
	//vi to work with. The only vi we have now is PDC_Port.vi

    // assign an object reference to the pVI.
	pVI = pLV->GetVIReference(bstrPath, bstrPass, 0, 0x10);

	_variant_t empty = "";
	pVI->SetControlValue("output array 2", empty);
	pVI->SetControlValue("PHASORX", empty);
	pVI->SetControlValue("Phasor Name", empty);
	pVI->SetControlValue("Phasor    Amplitude", empty);
	pVI->SetControlValue("Phasor   Angle", empty);
	pVI->SetControlValue("Complex Format", empty);
	pVI->SetControlValue("output array", empty);
	pVI->SetControlValue("Phasor Time", empty);
	pVI->SetControlValue("Phasor Values", empty);
	pVI->SetControlValue("FREQ Values", empty);
	pVI->SetControlValue("DFREQ Values", empty);
	pVI->SetControlValue("Analog Params", empty);
	pVI->SetControlValue("Digital Params", empty);
	pVI->SetControlValue("Analog Signal", empty);
	pVI->SetControlValue("Digital Signal", empty);
	pVI->SetControlValue("Digital String", empty);
	pVI->SetControlValue("Array", empty);
	pVI->SetControlValue("Array 2", empty);
	pVI->SetControlValue("output array", empty);
	pVI->SetControlValue("output array 3", empty);
	pVI->SetControlValue("Phasor_Value_Table", empty);
	pVI->SetControlValue("output array 4", empty);
	pVI->SetControlValue("Phasor Index", empty);

	return 0;
}

ULONG WINAPI CPDC_Process_DlgDlg::UpdateRTDataThread(void * pParam)
{
	DWORD dwRes(0), dwID(0);
/*
//	::CoInitialize(NULL);
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;

	char VIPath[100] =  VI_PATH;

	// Create Reference to LabVIEW ActiveX COM object
	pLV1.CreateInstance("LabVIEW.Application");
	if (pLV1 == NULL)
	{	
		printf("LV must be running, exiting ...\n");
		return false;
		//break;
	}
    
	// Create Reference to VI
	pVI1.CreateInstance("LabVIEW.VirtualInstrument");

    char Path[1000],Password[60] = "";

	strcpy(Path, VIPath);
	//this is very importqnt
	_bstr_t bstrPath(Path);
	_bstr_t bstrPass("");

	//The following section is responsible for grabbing one 
	//vi to work with. The only vi we have now is PDC_Port.vi
	
    // assign an object reference to the pVI.
	pVI1 = pLV1->GetVIReference(bstrPath, bstrPass, 0, 0x10);
	*/
	AtlTrace("UpdateRTDataThread is called by thread: %d\n", ::GetThreadId(h));
	while(1)
	{
		dwRes = ::WaitForSingleObject( hPHBOXEvent, INFINITE );
		switch( dwRes )
		{
			case	WAIT_OBJECT_0:
//				__try
//				{
						{
							HANDLE h = ::GetCurrentThread();

							AtlTrace("ProcessEventThead is called by thread: %d\n", ::GetThreadId(h));
					
							AtlTrace("UpdateRTDataThread is called by thread: %d\n", ::GetThreadId(h));

							CPDC_Process_DlgDlg * p_This = (CPDC_Process_DlgDlg *)pParam;
							p_This->UpdateRTData();
//							::PostMessageA(NULL, UPDATE_RTDATA, 0, 0);
							p_This->m_pTCPCMU1->ContinuePacketReading();

							//stop the while loop in the CPDC_Process_DlgDlg::UpdateRTDataThread
							::ResetEvent(hPHBOXEvent);
						}
						break;
//				}
				//any error triggers a process below
//				__finally
//				{//release the owner ship of the mutex by a given the thread
//					AtlTrace("hPHBOXEvent: there is no release policy for this event yet.");
//				}
		
			case WAIT_ABANDONED:
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
		}
	}	
	::CoUninitialize();
/*
	CPDC_Process_DlgDlg * p_This = (CPDC_Process_DlgDlg *)pParam;
	p_This->UpdateRTData();
	if( hr = m_pTCPCMU1->ContinuePacketReading())
		assert(NULL);*/

	return 0;
}


DWORD CPDC_Process_DlgDlg::UpdateRTData(void)
{
	//convert time stamps
	DWORD dwRes;
	HRESULT hr;
	ULONG u32_Rfsh(0);

	dwRes = DecodeTimeStamp();
	if( dwRes < 0 )
		return -1;
		
	dwRes = DecodeAngDig();
	if( dwRes < 0 )
		return -1;
		
	dwRes = DecodeFQDQ();
	if( dwRes < 0 )
		return -1;
		
	dwRes = DecodePhaseRect();
	if( dwRes < 0 )
		return -1;
	
	//convert phasor values
	dwRes = DecodePhasorValues();
	if( dwRes < 0 )
		return -1;
	AtlTrace("updateRTData\n");

	//update data coming from PDC Server every 1/m_u32_Rfsh Hertz.
//iITC	if( m_u32_Rfsh > 0)
//iITC		::Sleep(m_u32_Rfsh);
	
	return 0;
}

// update the phasor parameters on PDC_Port.vi GUI
DWORD CPDC_Process_DlgDlg::UpdatePhasorParams(void)
{
	//Interface to the ICFG2Params object
	CComPtr<ICFG2Params>	pCFG2Params(NULL);
	HRESULT	hr;

	if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return 1;
	else
		AtlTrace("Success: ICFG2Params is acquired that contains the Configuration Frame2 decode data.\n");

	// _variant_t is a class wrapper for variant data types
	//PMU List
	_variant_t	list_pmu_names102, list_pmu_names202, list_pmu_names302, list_pmu_names402, list_pmu_names502;
	//phasor magnitude
	_variant_t list_names_Mag[16];
	//phasor angles
	_variant_t list_names_Ang[16];
 
    int size = 0;
    char Path[1000],Password[60] = "";
	char VIPath[100] =  VI_PATH;

	strcpy(Path, VIPath);
	//this is very importqnt
	_bstr_t bstrPath(Path);
	_bstr_t bstrPass("");

	//The following section is responsible for grabbing one 
	//vi to work with. The only vi we have now is PDC_Port.vi

    // assign an object reference to the pVI.
//iITC	pVI = pLV->GetVIReference(bstrPath, bstrPass, 0, 0x10);
	SYSTEMTIME	st1, st2;
	::memset(&st1, 0, sizeof(SYSTEMTIME));
	::memset(&st2, 0, sizeof(SYSTEMTIME));

	//acquire the time before writing
	::GetSystemTime(&st1);
	VARIANT var;
	var.vt = VT_DATE;
	if( !::SystemTimeToVariantTime(&st1, &var.date) )
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

    // configure the VI to show its front panel on Call.
    pVI->ShowFPOnCall = TRUE;

	vtRow = pVI->GetControlValue(_bstr_t("Row"));
//	list_phasor5 = pVI->GetControlValue(_bstr_t("MA"));
//-------------------------------------------------------------------------

	_bstr_t _bstrComp = "M";
	var.vt = VT_BSTR;
	var.bstrVal = _bstrComp;
	pVI->SetControlValue("MA", var);
//-------------------------------------------------------------------------
	//dynamically load the phasors into one list of phasor using safe array
	VARIANT PMUPHs;
	::VariantInit(&PMUPHs);
	BSTR bstrPHN, bstrANN, bstrDGN;
	long z(0);

	ULONG u32_NoofPhasors(0);
	ULONG u32_NoofPMUs(0);
	//find number of PMUs
	if(FAILED( hr = pCFG2Params->get_NoofPMUs(&u32_NoofPMUs)))
	{
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		return -1;
	}
	//check Number of PMUs against unacceptable value like <= 0
	if( u32_NoofPMUs < 0 )
	{
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		return -1;
	}
	//find out the phasor names

	if(FAILED(hr = pCFG2Params->FirstPhasor(&bstrPHN)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	if(FAILED(hr = pCFG2Params->NextPhasor(&bstrPHN)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	if(FAILED(hr = pCFG2Params->NextPhasor(&bstrPHN)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	if(FAILED(hr = pCFG2Params->NextPhasor(&bstrPHN)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

//******************************************************************************
	/* FOR TESTING PURPOSES
	//---------------
    // a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
	    str_list = pVI->GetControlValue(_bstr_t("Array"));
//	pVI->SetControlValue("output array", str_list);
            
	// We can use V_VT(&_variant_t) to test the type of our variant.  The variant
	// data structure stores several different type values and an attribute
	// for which type this variant should be converted to.
	if (V_VT(&one_str) == (VT_BSTR))
	{	printf("One String is a bstring\n");	}

	// assign our SAFEARRAY to the array type of the variant
    if (V_VT(&str_list) == ( VT_ARRAY | VT_BSTR ))
    {	printf("This is an array of bstrings\n");
        psa = V_ARRAY(&str_list);
    }
	
    LONG cElements, lLBound, lUBound;
    BSTR HUGEP *pbstr;
 
	// Get the number of dimensions of our array
    printf("Number of channels: %d\n",SafeArrayGetDim(psa));    
 
	// Get the lower bound
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr)) return false;
 
    //Get upper bound
    hr = SafeArrayGetUBound(psa, 1, &lUBound);
    if (FAILED(hr)) return false;
 
    //Get total number of elements
    cElements = lUBound-lLBound+1;
	AtlTrace("Number of PMUS inside the Array is: %d\n", cElements);

	// Use SafeArrayAccessData to place a lock on the data so that we
	// can access the data in our application
    hr = SafeArrayAccessData(psa, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))return false;

	//  Iterate through the array and print the values
    for (int i = 0; i < cElements; i++)
    {	_bstr_t mybstr(pbstr[i]);
        AtlTrace("Data:<%s>\n",(char*)mybstr);                                
    }

	// Unlock the data
	SafeArrayUnaccessData(psa);
	*/
	VARIANT PMUs;
	VariantInit( &PMUs );

	PMUs.vt = VT_ARRAY|VT_VARIANT;
	PMUs.parray = SafeArrayCreateVector( VT_BSTR, 0, u32_NoofPMUs );

	//create safe arrays equal to the number of PMUs
	//then in each safe array insert the number of phasors for each array.
	//1. Create a list of VARIANTs
//	vector<VARIANT>	vecPhasors(u32_NoofPMUs);
	VARIANT v;
	::VariantClear(&v);
	vecPhasors.resize(u32_NoofPMUs,v);
	vecAnalogs.resize(u32_NoofPMUs,v);
	vecDigitals.resize(u32_NoofPMUs,v);
	
	::VariantClear(&var);

	var.vt = VT_BSTR;
	var.bstrVal = bstrPHN;
	USHORT u16_PHN(0);
	USHORT u16_ANNMR(0);
	USHORT u16_DGNMR(0);
	
	if(FAILED( hr = pCFG2Params->FirstPMU(&bstrPHN)))
	{
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		return -1;
	}
	else
	{
		SafeArrayPutElement( PMUs.parray,&z, bstrPHN);
		if(FAILED(hr = pCFG2Params->Find(bstrPHN, &u16_PHN)))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		else
		{
			//create the safe array of the first pmu
			vecPhasors[0].vt = VT_ARRAY | VT_BSTR;
			vecPhasors[0].parray = SafeArrayCreateVector(VT_BSTR, 0, u16_PHN);

			//get the phasor name and inset it into the relevant safe array
			if(FAILED(hr = pCFG2Params->FirstPhasor(&bstrPHN)))
			{
				{
			#ifdef	_DEBUG
					assert(NULL);
			#endif
				}
				return -1;
			}
			else
				SafeArrayPutElement(vecPhasors[0].parray, &z, bstrPHN);

			//get the name of the rest of pahsors and pu it
			for(long kk = 1; kk < u16_PHN; kk++)
			{
				if(FAILED(hr=pCFG2Params->NextPhasor(&bstrPHN)))
				{
					{
				#ifdef	_DEBUG
						assert(NULL);
				#endif
					}
					break;
				}
				else
					SafeArrayPutElement(vecPhasors[0].parray, &kk, bstrPHN);
			}
		}

		if(FAILED( hr = pCFG2Params->FirstPMU(&bstrPHN)))
		{
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			return -1;
		}
		else
		{
			if(FAILED(hr = pCFG2Params->FindNoofAnalogs(bstrPHN, &u16_ANNMR)))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			else if(u16_ANNMR != 0)
			{
				//create analog signals array for this PMU
				vecAnalogs[0].vt = VT_ARRAY | VT_BSTR;
				vecAnalogs[0].parray = SafeArrayCreateVector(VT_BSTR, 0, u16_ANNMR);

				//get the phasor name and inset it into the relevant safe array
				if(FAILED(hr = pCFG2Params->FirstAnalog(&bstrANN)))
				{
					{
				#ifdef	_DEBUG
						assert(NULL);
				#endif
					}
					return -1;
				}
				else
					SafeArrayPutElement(vecAnalogs[0].parray, &z, bstrANN);

				//get the name of the rest of pahsors and pu it
				for(long kk = 1; kk < u16_ANNMR; kk++)
				{
					if(FAILED(hr=pCFG2Params->NextAnalog(&bstrANN)))
					{
						{
					#ifdef	_DEBUG
							assert(NULL);
					#endif
						}
						break;
					}
					else
						SafeArrayPutElement(vecAnalogs[0].parray, &kk, bstrANN);
				}
			}
			else
			{}
		}
		//Digitals
		if(FAILED( hr = pCFG2Params->FirstPMU(&bstrPHN)))
		{
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			return -1;
		}
		else
		{
			if(FAILED(hr = pCFG2Params->FindNoofDigitals(bstrPHN, &u16_DGNMR)) && (u16_DGNMR != 0))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			else if(u16_DGNMR != 0)
			{
				//create analog signals array for this PMU
				vecDigitals[0].vt = VT_ARRAY | VT_BSTR;
				vecDigitals[0].parray = SafeArrayCreateVector(VT_BSTR, 0, u16_DGNMR);

				//get the phasor name and inset it into the relevant safe array
				if(FAILED(hr = pCFG2Params->FirstDigital(&bstrDGN)))
				{
					{
				#ifdef	_DEBUG
						assert(NULL);
				#endif
					}
					return -1;
				}
				else
					SafeArrayPutElement(vecDigitals[0].parray, &z, bstrDGN);

				//get the name of the rest of pahsors and pu it
				for(long kk = 1; kk < u16_DGNMR; kk++)
				{
					if(FAILED(hr=pCFG2Params->NextDigital(&bstrDGN)))
					{
						{
					#ifdef	_DEBUG
							assert(NULL);
					#endif
						}
						break;
					}
					else
						SafeArrayPutElement(vecDigitals[0].parray, &kk, bstrDGN);
				}
			}
			else
			{}
		}
	}

	for(long zz=1; zz < (long)u32_NoofPMUs; zz++)
	{
		if( FAILED( hr = pCFG2Params->NextPMU(&bstrPHN)))
		{
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			break;
		}
		else
		{
			SafeArrayPutElement( PMUs.parray,&zz, bstrPHN);
			//get the number of phasor for this PMU
			if(FAILED(hr = pCFG2Params->Find(bstrPHN, &u16_PHN)))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			else
			{
				//create the safe array of the first pmu
				vecPhasors[zz].vt = VT_ARRAY | VT_BSTR;
				vecPhasors[zz].parray = SafeArrayCreateVector(VT_BSTR, 0, u16_PHN);

				//get the phasor name and inset it into the relevant safe array
				for(long kk = 0; kk < u16_PHN; kk++)
				{
					if(FAILED(hr=pCFG2Params->NextPhasor(&bstrPHN)))
					{
						{
					#ifdef	_DEBUG
							assert(NULL);
					#endif
						}
						break;
					}
					else
						SafeArrayPutElement(vecPhasors[zz].parray, &kk, bstrPHN);
				}
			}
			//rest of the Analog signals of the PMUs
			if(FAILED(hr = pCFG2Params->FindNoofAnalogs(bstrPHN, &u16_ANNMR)))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			else if( u16_ANNMR != 0 )
			{
				//create the safe array of the first pmu
				vecAnalogs[zz].vt = VT_ARRAY | VT_BSTR;
				vecAnalogs[zz].parray = SafeArrayCreateVector(VT_BSTR, 0, u16_ANNMR);

				//get the phasor name and inset it into the relevant safe array
				for(long kk = 0; kk < u16_ANNMR; kk++)
				{
					if(FAILED(hr=pCFG2Params->NextAnalog(&bstrANN)))
					{
						{
					#ifdef	_DEBUG
							assert(NULL);
					#endif
						}
						break;
					}
					else
						SafeArrayPutElement(vecAnalogs[zz].parray, &kk, bstrANN);
				}
			}
			else
			{
				AtlTrace("\nNo Analog signals for this PMU exists.\n");
			}
			//rest of the Digital signals of the PMUs
			if(FAILED(hr = pCFG2Params->FindNoofDigitals(bstrPHN, &u16_DGNMR)))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			else if( u16_DGNMR != 0 )
			{
				//get the number of phasor for this PMU
				if(FAILED(hr = pCFG2Params->FindNoofDigitals(bstrPHN, &u16_DGNMR)))
				{
			#ifdef	_DEBUG
					assert(NULL);
			#endif
				}
				else
				{
					//create the safe array of the first pmu
					vecDigitals[zz].vt = VT_ARRAY | VT_BSTR;
					vecDigitals[zz].parray = SafeArrayCreateVector(VT_BSTR, 0, u16_DGNMR);

					//get the phasor name and inset it into the relevant safe array
					for(long kk = 0; kk < u16_DGNMR; kk++)
					{
						if(FAILED(hr=pCFG2Params->NextDigital(&bstrDGN)))
						{
							{
						#ifdef	_DEBUG
								assert(NULL);
						#endif
							}
							break;
						}
						else
							SafeArrayPutElement(vecDigitals[zz].parray, &kk, bstrDGN);
					}
				}
			}
			else
			{
				AtlTrace("\nNo Digital signals for this PMU exists.\n");
			}
		}//if-then-NextPMU(...)
	}//end-of-loop PMUs

	//get the value of the bStart boolean
	vtStart = pVI->GetControlValue(_bstr_t("Row"));

	//copy the PMUs into the PMUTBL for use in
	//OnChoosePhDt method.
	PMUTBL.parray = SafeArrayCreateVector( VT_BSTR, 0, u32_NoofPMUs );
	if(FAILED( hr = ::VariantCopy(&PMUTBL, &PMUs)))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	//put the PMULIST into the PMU LIST BOX
	_variant_t pmuarr(PMUs);
	pmuarr.vt = 8200;
	pVI->SetControlValue("output array 2", pmuarr);
	bNow = TRUE;

	unsigned long u32_PHNMR(0);
	//Find the number of the Phasors
	DWORD dwErr = FindNoofPhasors(&u32_PHNMR);
	if( dwErr < 0 )
	AtlTrace("Error: %d\n", dwErr);

	PHAMTBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, u32_PHNMR);
	PHANTBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, u32_PHNMR);
	PHCSTBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, u32_PHNMR);
	PHNTBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, u32_PHNMR);
	SELPHDTLIST.parray = ::SafeArrayCreateVector(VT_BSTR, 0, (u32_PHNMR * 2) + 5);//found
	LCHDINX.parray = ::SafeArrayCreateVector(VT_I4, 0, (u32_PHNMR * 2) + 5);
	PHNAMETBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, (u32_PHNMR * 2) + 5);	//name of the phasors
	_variant_t empty = "";
	::SetEvent(hPHBOXEvent);
	
	//get number of analog and digital signals from the PDC_Port.vi
	_variant_t TotAnalog, TotDigital;
	TotAnalog = pVI->GetControlValue(_bstr_t("Analog Size"));
	TotDigital = pVI->GetControlValue(_bstr_t("Digital Size"));

	//store them in the CTCPCMU1 object for later reference
	if(FAILED(hr = m_pTCPCMU1->put_NumAnalogs(TotAnalog.intVal)))
	{
		AtlTrace("Failed to store the total number of the issues.\n");
		return -1;
	}
	else
		AtlTrace("Value of the total number of ananlogs is: %d\n", TotAnalog.intVal);

	//store them in the CTCPCMU1 object for later reference
	if(FAILED(hr = m_pTCPCMU1->put_NumDigitals(TotDigital.intVal)))
	{
		AtlTrace("Failed to store the total number of the issues.\n");
		return -1;
	}
	else
		AtlTrace("Value of the total number of Digitals is: %d\n", TotDigital.intVal);

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnButton2Clicked(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
//	if(pVI)
//		pVI->SetControlValue("output array 3", LCHD);
	if( !m_bSkipPlay )
	{
		m_bSkipPlay = TRUE;
		OnBnClickedButton2();
		m_bSkip = FALSE;
		::ResetEvent(hPHBOXEvent);
		::ResumeThread(hUpdatRtDtThd);
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnRTOnlyRun(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

//this method starts the thread which retrieves the CFG_2 Frame from the PDC Server
LRESULT CPDC_Process_DlgDlg::OnUpdateRTData(WPARAM wParam, LPARAM lParam)
{
	AtlTrace("CPDC_Process_DlgDlg::OnUpdateRTData method is called.\n");
	if(UpdateRTData() < 0)
		return -1;

	return 0;
}

//this method starts the thread which retrieves the CFG_2 Frame from the PDC Server
LRESULT CPDC_Process_DlgDlg::OnUpdatePhasor(WPARAM wParam, LPARAM lParam)
{
	AtlTrace("CPDC_Process_DlgDlg::OnUpdatePhasor method is called.\n");

	if(UpdatePhasorParams() < 0)
		return -1;

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnUpdateUserConfig(WPARAM wParam, LPARAM lParam)
{
	if( pVI )
	{
		m_szFName = "C:\\KTH software to check\\VI TUTORIAL PROJECTS\\NetConfig.txt";
		_variant_t varIP;
		AtlTrace("IP Address is entered\n");

		//get the IP Address
		varIP = pVI->GetControlValue(_bstr_t("IP Address"));
		if( varIP.vt == VT_BSTR )
		{
			_bstr_t _b(varIP.bstrVal);
			m_szIP = _b;
			AtlTrace(" IP Address is: %s\n", m_szIP.c_str());
		}
		//get the port number
		::VariantClear(&varIP);
		varIP = pVI->GetControlValue(_bstr_t("Port"));
		if( varIP.vt == VT_BSTR )
		{
			_bstr_t _b(varIP.bstrVal);
			m_szPort = _b;
			AtlTrace(" IP Address is: %s\n", m_szPort.c_str());
		}
		//get the IDCODE
		::VariantClear(&varIP);
		varIP = pVI->GetControlValue(_bstr_t("ID Code"));
		if( varIP.vt == VT_BSTR )
		{
			_bstr_t _b(varIP.bstrVal);
			m_szIDCode = _b;
			AtlTrace(" IP Address is: %s\n", m_szIDCode.c_str());
		}

		if( SaveConfig() < 0 )
			return -1;
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnInfoTypeSel(WPARAM wParam, LPARAM lParam)
{
	if( pVI )
	{
		m_iPhasors = lParam;
		AtlTrace("Information Type is: %d\n", lParam);
		pVI->SetControlValue("Information Type", lParam);
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnPMUListSel(WPARAM wParam, LPARAM lParam)
{
	_variant_t F;
	F.vt = VT_BOOL;
	F.boolVal = true;
	AtlTrace("PMU LIST BOX IS DOUBLE CLICKED.\n");
	m_nPMU_Number = lParam;
	long lLBound(0), lUBound(0), lDim(0);
	HRESULT hr;
	CComPtr<ICFG2Params>	pCFG2Params(NULL);
	ULONG iIndex(0), u32_Index(0);
	ULONG u32_NoofPMUs(0);
	USHORT U16_NoofAnalogs[] = {0,0,0,0};
	USHORT U16_NoofDigitals[] = {0,0,0,0};
	BSTR bstrPMU;

	if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return S_FALSE;
	else
		AtlTrace("Success: pCFG2Params acquired.\n");

	if( pVI && bNow )
	{
		_variant_t phasors(vecPhasors[lParam]);
		phasors.vt = 8200;
		pVI->SetControlValue("PHASORX", phasors);
		if(FAILED( hr = pCFG2Params->FirstPMU(&bstrPMU)))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		if(FAILED( hr = pCFG2Params->get_NoofPMUs(&u32_NoofPMUs)))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		if(FAILED( hr = pCFG2Params->FindNoofAnalogs(bstrPMU, &U16_NoofAnalogs[0])))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		if(FAILED( hr = pCFG2Params->FindNoofDigitals(bstrPMU, &U16_NoofDigitals[0])))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}

		for(int np=0; np < u32_NoofPMUs; np++)
		{
			if(FAILED( hr = pCFG2Params->FirstPMU(&bstrPMU)))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			if(FAILED( hr = pCFG2Params->FindNoofAnalogs(bstrPMU, &U16_NoofAnalogs[np])))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
			if(FAILED( hr = pCFG2Params->FindNoofDigitals(bstrPMU, &U16_NoofAnalogs[np])))
			{
		#ifdef	_DEBUG
				assert(NULL);
		#endif
			}
		}
		//calculate the size of the Analog Signals safe array
		if(U16_NoofAnalogs[lParam] != 0 )
		{
			_variant_t Analogs(vecAnalogs[lParam]);
			Analogs.vt = VT_ARRAY | VT_BSTR;
			pVI->SetControlValue("Analog Params", Analogs);
		}

		if(U16_NoofDigitals[lParam] != 0 )
		{
			_variant_t Digitals(vecDigitals[lParam]);
			Digitals.vt = VT_ARRAY | VT_BSTR;
			pVI->SetControlValue("Digital Params", Digitals);
		}
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnDigListSel(WPARAM wParam, LPARAM lParam)
{
	_variant_t F;
	F.vt = VT_BOOL;
	F.boolVal = true;
	char * sc;
	string str3;
	_bstr_t _b3;
	AtlTrace("Analog List Box is Double Clicked.\n");
	m_nPMU_Number = lParam;

	CComPtr<ICFG2Params>	pCFG2Params(NULL);
	HRESULT	hr;
	ULONG iIndex(0);

	if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return S_FALSE;
	else
		AtlTrace("Success: pCFG2Params acquired.\n");

	sc = (char*)m_pMalloc->Alloc(10);
	if( !sc )
	{
		AtlTrace("Error: Memory allocation for chosen data list box numbers failed.\n");
		return -1;
	}

// a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
	_variant_t PHN_LIST;
	PHN_LIST = pVI->GetControlValue("Digital Params");
	// assign our SAFEARRAY to the array type of the variant
    if (V_VT(&PHN_LIST) == ( VT_ARRAY | VT_BSTR ))
    {	printf("This is an array of bstrings\n");
        psa = V_ARRAY(&PHN_LIST);
    }
	
    LONG cElements, lLBound, lUBound;
    BSTR HUGEP *pbstr;
 
	// Get the number of dimensions of our array
    printf("Number of channels: %d\n",SafeArrayGetDim(psa));    
 
	// Get the lower bound
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr)) return false;
 
    //Get upper bound
    hr = SafeArrayGetUBound(psa, 1, &lUBound);
    if (FAILED(hr)) return false;
 
    //Get total number of elements
    cElements = lUBound-lLBound+1;
 
	// Use SafeArrayAccessData to place a lock on the data so that we
	// can access the data in our application
    hr = SafeArrayAccessData(psa, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))return false;
 
	//  Iterate through the array and print the values
    for (int i = 0; i < cElements; i++)
    {	_bstr_t mybstr(pbstr[i]);
        AtlTrace("Data:<%s>\n",(char*)mybstr);                                
    }
 
	// Unlock the data
	SafeArrayUnaccessData(psa);

//------------------------------------------------------------------

if( pVI )
	{
		_variant_t chosendt;
		chosendt = pVI->GetControlValue("element 3");

		string str, str2;
		_bstr_t	_b;

		if( chosendt.vt == VT_BSTR )
			_b = chosendt.bstrVal;

		//if chose data list is not empty search for a given phasor it otherwise just enter
		//phasor into it
		if( !m_map_choose_phdt.empty())
		{
			//check if we already have the phasor in the table so not to enter it again.
			for( m_chphdtIt = m_map_choose_phdt.begin(); m_chphdtIt != m_map_choose_phdt.end(); m_chphdtIt++)
			{
				str = (*m_chphdtIt).second;
				if( !str.compare(_b) )
				{
					//pVI->Run(0);
					return 0;
				}
			}
		}
		//it is a new Analog Signal enter it
		str = _b;
		str2 = _b;

//-----------------real-time decoding necessary info to extract ends here. starts here.
		if(lParam < 0 )
			return 0;

//		if(lParam > m_map_choose_phdt.size() )
//			return 0;

		AtlTrace("selected PMU name is: %s\n", (const char*)_bstr_t(pbstr[lParam]));

		//find the index of the PMU in the list of the PMUs
		if(hr = FAILED(pCFG2Params->FindPMUIndex(pbstr[lParam], &iIndex)))
			return S_FALSE;
		else
		{
			//save the number of the phasor which in the real-time packet should be
			//decoded based on the user chosen data
			if(FAILED(hr = m_pTCPCMU1->put_ADPMUCount(iIndex)))
				return -1;
		}

		//find the index of the phasor in the list of the phasors
		if(hr = FAILED( pCFG2Params->FindDigitalIndex(pbstr[lParam], &iIndex)))
			return S_FALSE;
		else
		{
			//save the number of the phasor which in the real-time packet should be
			//decoded based on the user chosen data
			if(FAILED(hr = m_pTCPCMU1->put_DigitalNumber(iIndex)))
				return -1;

			//put 100000 for analog number to difrerentiate digital value from analog value
			if(FAILED( hr = m_pTCPCMU1->put_AnalogNumber(100000)))
				return -1;

			//count up the number of digital sinals
			if(FAILED(hr = m_pTCPCMU1->AddDigitalNumber()))
				return -1;
		}
		//insert the phasor position and type of chosen data list
		//this is called from within Modul1::BIN2ASCCI_RT in order
		//to know which phasor, Amplitude/Angle to be decoded and 
		if(FAILED(hr = m_pTCPCMU1->InsertAnaPosType()))
			return -1;
//-----------------real-time decoding necessary info to extract ends here.

		m_retchphdt = m_map_choose_phdt.insert(pair<int, string >(zchosendt, str));
		if( m_retchphdt.second == false)
			return 0;

		if( chosendt.vt == VT_BSTR )
		{
			_bstr_t _b(chosendt.bstrVal);
			if( _b.length() == 0 )
				return -1;
			else
			{
				_bstr_t mybstr(pbstr[lParam]);
				//Add a preamble number here
				::itoa(++m_nIndex, sc, 10);
				_b3 = "(";
				_b3 += sc;
				_b3 += ")";
				_b3 += " ";
				str3 += _b3;
				str3 += str;
				str.swap(str3);
				str3.empty();
				_b = str.c_str();

				if( !ANDIGNAMETBL.parray )
				{
					ULONG u32_NoofAnalogs(0);
					ULONG u32_NoofDigitals(0);
/*					if(FAILED( hr = m_pTCPCMU1->get_NoofChAnalog(&u32_NoofAnalogs)))
						return -1;
					if(FAILED( hr = m_pTCPCMU1->get_NoofChDigital(&u32_NoofDigitals)))
						return -1;*/
					if(FAILED( hr = pCFG2Params->get_TotNoofDigitals(&u32_NoofAnalogs)))
						return -1;
					ANDIGNAMETBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, /*(u32_NoofAnalogs + u32_NoofDigitals)*/10);
				}
				//ADD THE CHOSEN PHASOR PARAMETER TO THE CHOSEN DATA LIST BOX
				if( FAILED( hr = ::SafeArrayPutElement(/*SELPHDTLIST.parray*/ANDIGNAMETBL.parray, &zchosendt1, /*chosendt.bstrVal*/_b.copy())))
					return -1;
				else
				{
					//---------------------------enter the chosen phasor name into the relevant list
					m_retchph = m_map_chosen_phasor.insert(pair<int, string>(zchosendt, str));
					if( m_retchph.second == false )
					{
				#ifdef	_DEBUG
						assert(NULL);
				#endif
					}
					//--------------------------------------------
					if( FAILED( hr = ::SafeArrayPutElement(SELPHDTLIST.parray, &zchosendt, /*chosendt.bstrVal*/_b.copy())))
						return -1;
					if(FAILED( hr = ::SafeArrayPutElement(LCHDINX.parray, &zchosendt, &m_nIndex)))
						return -1;
					//--------------------------------------------
					pVI->SetControlValue("output array 4", /*SELPHDTLIST*/ANDIGNAMETBL);
//					pVI->SetControlValue("Phasor_Value_Table", SELPHDTLIST);
					pVI->SetControlValue("Array 2", SELPHDTLIST);
					pVI->SetControlValue("Phasor Index", LCHDINX);
					zchosendt++;
					zchosendt1++;
				}
			}
		}
		else
		{
		}
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnAnaListSel(WPARAM wParam, LPARAM lParam)
{
	_variant_t F;
	F.vt = VT_BOOL;
	F.boolVal = true;
	char * sc;
	string str3;
	_bstr_t _b3;
	AtlTrace("Analog List Box is Double Clicked.\n");
	m_nPMU_Number = lParam;

	CComPtr<ICFG2Params>	pCFG2Params(NULL);
	HRESULT	hr;
	ULONG iIndex(0);

	if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return S_FALSE;
	else
		AtlTrace("Success: pCFG2Params acquired.\n");

	sc = (char*)m_pMalloc->Alloc(10);
	if( !sc )
	{
		AtlTrace("Error: Memory allocation for chosen data list box numbers failed.\n");
		return -1;
	}

	// a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
	_variant_t PHN_LIST;
	PHN_LIST = pVI->GetControlValue("Analog Params");
	// assign our SAFEARRAY to the array type of the variant
    if (V_VT(&PHN_LIST) == ( VT_ARRAY | VT_BSTR ))
    {	printf("This is an array of bstrings\n");
        psa = V_ARRAY(&PHN_LIST);
    }
	
    LONG cElements, lLBound, lUBound;
    BSTR HUGEP *pbstr;
 
	// Get the number of dimensions of our array
    printf("Number of channels: %d\n",SafeArrayGetDim(psa));    
 
	// Get the lower bound
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr)) return false;
 
    //Get upper bound
    hr = SafeArrayGetUBound(psa, 1, &lUBound);
    if (FAILED(hr)) return false;
 
    //Get total number of elements
    cElements = lUBound-lLBound+1;
 
	// Use SafeArrayAccessData to place a lock on the data so that we
	// can access the data in our application
    hr = SafeArrayAccessData(psa, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))return false;
 
	//  Iterate through the array and print the values
    for (int i = 0; i < cElements; i++)
    {	_bstr_t mybstr(pbstr[i]);
        AtlTrace("Data:<%s>\n",(char*)mybstr);                                
    }
 
	// Unlock the data
	SafeArrayUnaccessData(psa);

//------------------------------------------------------------------

if( pVI )
	{
		_variant_t chosendt;
		chosendt = pVI->GetControlValue("element 2");

		string str, str2;
		_bstr_t	_b;

		if( chosendt.vt == VT_BSTR )
			_b = chosendt.bstrVal;

		//if chose data list is not empty search for a given phasor it otherwise just enter
		//phasor into it
		if( !m_map_choose_phdt.empty())
		{
			//check if we already have the phasor in the table so not to enter it again.
			for( m_chphdtIt = m_map_choose_phdt.begin(); m_chphdtIt != m_map_choose_phdt.end(); m_chphdtIt++)
			{
				str = (*m_chphdtIt).second;
				if( !str.compare(_b) )
				{
					//pVI->Run(0);
					return 0;
				}
			}
		}
		//it is a new Analog Signal enter it
		str = _b;
		str2 = _b;

//-----------------real-time decoding necessary info to extract ends here. starts here.
		if(lParam < 0 )
			return 0;
//		if(lParam > m_map_choose_phdt.size() )
//			return 0;
		AtlTrace("selected PMU name is: %s\n", (const char*)_bstr_t(pbstr[lParam]));
		//find the index of the PMU in the list of the PMUs
		if(hr = FAILED(pCFG2Params->FindPMUIndex(pbstr[lParam], &iIndex)))
			return S_FALSE;
		else
		{
			//save the number of the phasor which in the real-time packet should be
			//decoded based on the user chosen data
			if(FAILED(hr = m_pTCPCMU1->put_ADPMUCount(iIndex)))
				return -1;
		}

		//find the index of the phasor in the list of the phasors
		if(hr = FAILED( pCFG2Params->FindAnalogIndex(pbstr[lParam], &iIndex)))
			return S_FALSE;
		else
		{
			//save the number of the phasor which in the real-time packet should be
			//decoded based on the user chosen data
			if(FAILED(hr = m_pTCPCMU1->put_AnalogNumber(iIndex)))
				return -1;
			//put 100000 as digital number as a dummy digital number to mark this one as an analog number 
			//and not a digital number
			if(FAILED(hr = m_pTCPCMU1->put_DigitalNumber(100000)))
				return -1;
			//count up the number of digital signals selected in chosen phasor list box
			if(FAILED( hr = m_pTCPCMU1->AddAnalogCh()))
				return -1;
		}
		//insert the phasor position and type of chosen data list
		//this is called from within Modul1::BIN2ASCCI_RT in order
		//to know which phasor, Amplitude/Angle to be decoded and 
		if(FAILED(hr = m_pTCPCMU1->InsertAnaPosType()))
			return -1;
//-----------------real-time decoding necessary info to extract ends here.

		m_retchphdt = m_map_choose_phdt.insert(pair<int, string >(zchosendt, str));
		if( m_retchphdt.second == false)
			return 0;

		if( chosendt.vt == VT_BSTR )
		{
			_bstr_t _b(chosendt.bstrVal);
			if( _b.length() == 0 )
				return -1;
			else
			{
				_bstr_t mybstr(pbstr[lParam]);
//------------------------------
				//Add a preamble number here
				::itoa(++m_nIndex, sc, 10);
				_b3 = "(";
				_b3 += sc;
				_b3 += ")";
				_b3 += " ";
				str3 += _b3;
				str3 += str;
				str.swap(str3);
				str3.empty();
//---------------------------------------------
//				str += " of ";
//				str += mybstr;
				_b = str.c_str();
				//ADD THE CHOSEN PHASOR PARAMETER TO THE CHOSEN DATA LIST BOX
				if( !ANDIGNAMETBL.parray )
				{
					ULONG u32_NoofAnalogs(0);
					ULONG u32_NoofDigitals(0);
					if(FAILED( hr = m_pTCPCMU1->get_NoofChAnalog(&u32_NoofAnalogs)))
						return -1;
					if(FAILED( hr = m_pTCPCMU1->get_NoofChDigital(&u32_NoofDigitals)))
						return -1;
					ANDIGNAMETBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, /*(u32_NoofAnalogs + u32_NoofDigitals)*/10);
				}
				if( FAILED( hr = ::SafeArrayPutElement(/*SELPHDTLIST.parray*/ANDIGNAMETBL.parray, &zchosendt1, /*chosendt.bstrVal*/_b.copy())))
					return -1;
				else
				{
					//---------------------------enter the chosen phasor name into the relevant list
					m_retchph = m_map_chosen_phasor.insert(pair<int, string>(zchosendt, str));
					if( m_retchph.second == false )
					{
				#ifdef	_DEBUG
						assert(NULL);
				#endif
					}
					//--------------------------------------------
					if( FAILED( hr = ::SafeArrayPutElement(SELPHDTLIST.parray, &zchosendt, /*chosendt.bstrVal*/_b.copy())))
						return -1;
					if(FAILED( hr = ::SafeArrayPutElement(LCHDINX.parray, &zchosendt, &m_nIndex)))
						return -1;
					//--------------------------------------------
					pVI->SetControlValue("output array 4", /*SELPHDTLIST*/ANDIGNAMETBL);
//					pVI->SetControlValue("Phasor_Value_Table", SELPHDTLIST);
					pVI->SetControlValue("Array 2", SELPHDTLIST);
					pVI->SetControlValue("Phasor Index", LCHDINX);
					zchosendt++;
					zchosendt1++;
				}
			}
		}
		else
		{
		}
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnInfoTypeList(WPARAM wParam, LPARAM lParam)
{
	if( pVI )
	{
		_variant_t varIndex;
		varIndex = pVI->GetControlValue(_bstr_t("Row2"));
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnPhasorParamSel(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;

	if( pVI )
	{
		AtlTrace("The Selected Row in Phasor Parameter List is: %d\n", lParam);
		_variant_t selPh = pVI->GetControlValue(_bstr_t("Selected Phasor"));
		_bstr_t _b(selPh.bstrVal);
		string str;
		//check if we already have the phasor in the table so not to enter it again.
		for( m_selphIt = m_map_sel_phasor.begin(); m_selphIt != m_map_sel_phasor.end(); m_selphIt++)
		{
			str = (*m_selphIt).second;
			if( !str.compare(_b) )
				return 0;
		}
		//it is a new phasor enter it
		str = _b;
		m_retselph = m_map_sel_phasor.insert(pair<int, string >(z, str));
		if( m_retselph.second == false)
		{
			return 0;
		}
		//it is a new phasor enter it into the phasor name table.
		::SafeArrayPutElement(PHNTBL.parray, &z, selPh.bstrVal);
		//update the phasor names table
		pVI->SetControlValue("Phasor Name", PHNTBL);

		//phasor angle and phasor amplitude tables update

		//1. construct M1, M2, .... Mx names
		//2. write them into the phasor amplitude names.
		char PHAMN[4];::memset(PHAMN, 0, sizeof(PHAMN));
		char preamb[2] = {'M', '\0'};
		if( !sprintf(PHAMN, "%s%d",preamb, (z+1)) )
			return -1;
		_bstr_t _b1 = PHAMN;
		//add this phasor amplitude to the phasor amplitude table
		if( FAILED( hr = ::SafeArrayPutElement(PHAMTBL.parray, &z, _b1.copy())))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		//write the array into the phasor amplitude table
		pVI->SetControlValue("Phasor    Amplitude", PHAMTBL);

		//1. construct A1, AX, ... Ax names
		//2. write them into the phasor Angle names
//		char PHANN[4]; ::memset(PHANN, 0, sizeof(PHANN));
//		char preamba[2] = {'A', '\0'};
		preamb[0] = 'A';
		preamb[1] = '\0';
//		if( !sprintf(PHANN, "%s%d\n", preamba, (z+1)))
		if( !sprintf(PHAMN, "%s%d",preamb, (z+1)) )
			return -1;

//		_b1 = PHANN;
		_b1 = PHAMN;
		//add this pahsor Angle name to the phasor amplitude table
		if(FAILED( hr = ::SafeArrayPutElement(PHANTBL.parray, &z, _b1.copy())))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		//write the array into the phasor angle table
		pVI->SetControlValue("Phasor   Angle", PHANTBL);
		//next element of table
		z++; prev = lParam;
		//keep Labview running
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnChoosePhDt(WPARAM wParam, LPARAM lParam)
{
//-------------------------------------------------------------------		
	CComPtr<ICFG2Params>	pCFG2Params(NULL);
	HRESULT	hr;
	ULONG iIndex(0), nIndex(0);
	char * sc;
	string str3;
	_bstr_t _b3;

	if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return S_FALSE;
	else
		AtlTrace("Success: pCFG2Params acquired.\n");

	sc = (char*)m_pMalloc->Alloc(10);
	if( !sc )
	{
		AtlTrace("Error: Memory allocation for chosen data list box numbers failed.\n");
		return -1;
	}

	// a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
	_variant_t PHN_LIST;
	PHN_LIST = pVI->GetControlValue("Phasor Name");
	// assign our SAFEARRAY to the array type of the variant
    if (V_VT(&PHN_LIST) == ( VT_ARRAY | VT_BSTR ))
    {	printf("This is an array of bstrings\n");
        psa = V_ARRAY(&PHN_LIST);
    }
	
    LONG cElements, lLBound, lUBound;
    BSTR HUGEP *pbstr;
 
	// Get the number of dimensions of our array
    printf("Number of channels: %d\n",SafeArrayGetDim(psa));    
 
	// Get the lower bound
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr)) return false;
 
    //Get upper bound
    hr = SafeArrayGetUBound(psa, 1, &lUBound);
    if (FAILED(hr)) return false;
 
    //Get total number of elements
    cElements = lUBound-lLBound+1;
 
	// Use SafeArrayAccessData to place a lock on the data so that we
	// can access the data in our application
    hr = SafeArrayAccessData(psa, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))return false;
 
	//  Iterate through the array and print the values
    for (int i = 0; i < cElements; i++)
    {	_bstr_t mybstr(pbstr[i]);
        printf("Data:<%s>\n",(char*)mybstr);                                
    }
 
	// Unlock the data
	SafeArrayUnaccessData(psa);

//------------------------------------------------------------------
	//get the name of the phasor parameter
	if( pVI )
	{
		_variant_t chosendt;
		chosendt = pVI->GetControlValue("element");

		string str, str2;
		_bstr_t	_b;

		if( chosendt.vt == VT_BSTR )
			_b = chosendt.bstrVal;

		//if chose data list is not empty search for a given phasor it otherwise just enter
		//phasor into it
		if( !m_map_choose_phdt.empty())
		{
			//check if we already have the phasor in the table so not to enter it again.
			for( m_chphdtIt = m_map_choose_phdt.begin(); m_chphdtIt != m_map_choose_phdt.end(); m_chphdtIt++)
			{
				str = (*m_chphdtIt).second;
				if( !str.compare(_b) )
				{
					//pVI->Run(0);
					return 0;
				}
			}
		}
		//it is a new phasor enter it
		str = _b;
		str2 = _b;

//-----------------real-time decoding necessary info to extract ends here. starts here.
		if(lParam < 0 )
			return 0;
//		if(lParam > m_map_choose_phdt.size() )
//			return 0;
			AtlTrace("selected PMU name is: %s\n", (const char*)_bstr_t(pbstr[lParam]));
			//find the index of the PMU in the list of the PMUs
			if(hr = FAILED(pCFG2Params->FindPMUIndex(pbstr[lParam], &iIndex)))
				return S_FALSE;
			else
			{
				//save the number of the phasor which in the real-time packet should be
				//decoded based on the user chosen data
				if(FAILED(hr = m_pTCPCMU1->put_PMUCount(iIndex)))
					return -1;
			}

			//find the index of the phasor in the list of the phasors
			if(hr = FAILED( pCFG2Params->FindPhasorIndex(pbstr[lParam], &iIndex)))
				return S_FALSE;
			else
			{
				//save the number of the phasor which in the real-time packet should be
				//decoded based on the user chosen data
				if(FAILED(hr = m_pTCPCMU1->put_PhNumber(iIndex)))
					return -1;
			}
			if( str2.empty())
				return -1;
			//find the magnitude/angle code write it to phasor decode vector
			if( str2[0] == 'M'){
				//save that the user chose to read the magnitude
				//of the given phasor in the real-time packet
				if(FAILED(hr = m_pTCPCMU1->put_PhMagAng(0)))
					return -1;
			}
			else if( str2[0] == 'A'){
				//save that the user chose to read the magnitude
				//of the given phasor in the real-time packet
				if(FAILED(hr = m_pTCPCMU1->put_PhMagAng(1)))
					return -1;
			}
			else
				{
			#ifdef	_DEBUG
					assert(NULL);
			#endif
				}

			//insert the phasor position and type of chosen data list
			//this is called from within Modul1::BIN2ASCCI_RT in order
			//to know which phasor, Amplitude/Angle to be decoded and 
			if(FAILED(hr = m_pTCPCMU1->InsertPhPosType()))
				return -1;
//-----------------real-time decoding necessary info to extract ends here.

		m_retchphdt = m_map_choose_phdt.insert(pair<int, string >(zchosendt, str));
		if( m_retchphdt.second == false)
			return 0;

		if( chosendt.vt == VT_BSTR )
		{
			_bstr_t _b(chosendt.bstrVal);
			if( _b.length() == 0 )
				return -1;
			else
			{
				_bstr_t mybstr(pbstr[lParam]);
				str += " of ";
				str += mybstr;
//------------------------------
				//Add a preamble number here
				::itoa(++m_nIndex, sc, 10);
				_b3 = "(";
				_b3 += sc;
				_b3 += ")";
				_b3 += " ";
				str3 += _b3;
				str3 += str;
				str.swap(str3);
				str3.empty();
//---------------------------------------------
				_b = str.c_str();
				//ADD THE CHOSEN PHASOR PARAMETER TO THE CHOSEN DATA LIST BOX
				if( FAILED( hr = ::SafeArrayPutElement(SELPHDTLIST.parray, &zchosendt, /*chosendt.bstrVal*/_b.copy())))
					return -1;
				else
				{
					//---------------------------enter the chosen phasor name into the relevant list
					m_retchph = m_map_chosen_phasor.insert(pair<int, string>(zchosendt, str));
					if( m_retchph.second == false )
					{
				#ifdef	_DEBUG
						assert(NULL);
				#endif
					}
					if( FAILED( hr = ::SafeArrayPutElement(PHNAMETBL.parray, &zchosendt2, /*chosendt.bstrVal*/_b.copy())))
						return -1;
					if(FAILED( hr = ::SafeArrayPutElement(LCHDINX.parray, &zchosendt, &m_nIndex)))
						return -1;
					//--------------------------------------------
//					pVI->SetControlValue("output array", SELPHDTLIST);
					pVI->SetControlValue("Phasor_Value_Table", PHNAMETBL);
					pVI->SetControlValue("Array 2", SELPHDTLIST);
					pVI->SetControlValue("Phasor Index", LCHDINX);
					zchosendt++;
					zchosendt2++;
				}
			}
		}
		else
		{
		}
	}

	return 0;
}

void CPDC_Process_DlgDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	DWORD dwExitCode(0);
	OnBnClickedOk();
	CDialogEx::OnClose();
}


void CPDC_Process_DlgDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	DWORD dwExitCode(0);
	::GetExitCodeThread(h, &dwExitCode);
	::TerminateThread(h, dwExitCode);
	::TerminateThread(hMain, dwExitCode);
	::MessageBoxA(NULL, "OnClose", "", IDOK);
	OnBnClickedOk();
}


// Load configuration information
//	::ShellExecuteA(NULL, "", "", NULL, NULL, SW_SHOWNORMAL);
int CPDC_Process_DlgDlg::LoadConfig(void)
{
//Open the configuration file
	FILE * f = NULL;
	string szMode = "r";
	char szIP[32];	
	char szPort[32];
	char szIDCode[32];
	_variant_t var;

	var.vt = VT_BSTR;

	::memset(szIP, 0, sizeof(m_szIP));
	::memset(szPort, 0, sizeof(m_szPort));
	::memset(szIDCode, 0, sizeof(m_szIDCode));


	f = ::fopen(m_szFName.c_str(), szMode.c_str());
	//open the file with write permission
	if( !f )
		return -1;
	else
		AtlTrace("Success: Network configuration file is opened.\n");

	char VIPath[100] =  VI_PATH;

	//this is very importqnt
	_bstr_t bstrPath(VIPath);
	_bstr_t bstrPass("");

	strcpy(VIPath, VIPath);
    // assign an object reference to the pVI.
/*iITC
	pVI = pLV->GetVIReference(bstrPath, bstrPass, 0, 0x10);
	pVI->ShowFPOnCall = TRUE;
*/
	//get one line of the configuration.
	::fgets(szIP, sizeof(szIP), f);
	m_szIP = szIP;
	_bstr_t _b(szIP);
	var.bstrVal = _b;
	pVI->SetControlValue(_bstr_t("IP Address"), var);
	AtlTrace("IP Address is: %s\n", szIP);
	//get one line of the configuration.
	::fgets(szPort, sizeof(szPort), f);
	m_szPort = szPort;
	::VariantClear(&var);
	_b = szPort;
	var.vt = VT_BSTR;
	var.bstrVal = _b;
	pVI->SetControlValue(_bstr_t("Port"), var);
	AtlTrace("IP Address is: %s\n", szPort);
	//get one line of the configuration.
	::fgets(szIDCode, sizeof(szIDCode), f);
	m_szIDCode = szIDCode;
	::VariantClear(&var);
	var.vt = VT_BSTR;
	_b = szIDCode;
	var.bstrVal = _b;
	pVI->SetControlValue(_bstr_t("ID Code"), var);
	AtlTrace("IP Address is: %s\n", szIDCode);
	USHORT u16_IDCode = atoi(szIDCode);
	HRESULT hr;
	if(FAILED( hr = m_pTCPCMU1->put_IDCode(u16_IDCode)))
		return -1;
//write the IP address into the user configuration file
	if( !::fread(szIP, 1, sizeof(szIP), f) )
		return -1;
	else
		AtlTrace("success: we write the IP address into the file.\n");

	return 0;
}

// saves the user network configuration
DWORD CPDC_Process_DlgDlg::SaveConfig(void)
{
	//save the IP Address of the PDC Server, Port number , and IDCODE
	//in a text file
	FILE * f;
	string szMode = "w+";
	char buf[500];
//	fpos_t position;
	
	//open the file with write permission
	f = ::fopen(m_szFName.c_str(), szMode.c_str());
	if( !f )
		return -1;
	else
		AtlTrace("Success: Network configuration file is opened.\n");

	//write the IP address into the network configuration file
	if( !::fwrite(m_szIP.c_str(), 1, m_szIP.size(), f) )
		return -1;
//	::fwrite("\n", 1, 1, f);
	//write the port number into the network configuration file
	if( !::fwrite(m_szPort.c_str(), 1, m_szPort.size(), f) )
		return -1;

//	::fwrite("\n", 1, 1, f);

	//write the IDCODE into the network configuration file
	if( !::fwrite(m_szIDCode.c_str(), 1, m_szIDCode.size(), f) )
		return -1;

//	::fwrite("\n", 1, 1, f);

	//rewind the pinter to the beginig of the file
	::rewind(f);

	//read the conents of the file
	if( !::fread(buf, 1, 500, f) )
		return -1;
	AtlTrace("string is: %s\n", buf);

	//Close the file
	::fclose(f);

	return 0;
}


// Finds total number of phasors of PMUs of PDC Server
DWORD CPDC_Process_DlgDlg::FindNoofPhasors(unsigned long * m_u32_PHNMR)
{
	TRY
	{
		//number of pmus
		HRESULT hr;
		ULONG u32_PMUNMR =vecPhasors.size();
		vector<LONG> vecPHNMR;
		vector<VARIANT>::iterator	it;
		//allocate memory
		vecPHNMR.resize(u32_PMUNMR);
		LONG u32_Ubound(0), u32_Lbound(0);
		*m_u32_PHNMR = 0;

		for(it = vecPhasors.begin(); it != vecPhasors.end(); it++)
		{
			hr = ::SafeArrayGetUBound(((*it).parray), 1, &u32_Ubound);
			if(FAILED(hr)) ::AfxThrowInvalidArgException();

			hr = ::SafeArrayGetLBound(((*it).parray), 1, &u32_Lbound);
			if(FAILED(hr)) ::AfxThrowInvalidArgException();

			vecPHNMR.push_back(u32_Ubound - u32_Lbound + 1);
			*m_u32_PHNMR += u32_Ubound - u32_Lbound + 1;
		}
	}
	CATCH(CException, e)
	{
		char szErrMsg[256];
		e->GetErrorMessage((LPTSTR)szErrMsg, 256);
		AtlTrace("Error: %s\n", szErrMsg);
	}
	END_CATCH

	return 0;
}

void CPDC_Process_DlgDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	HRESULT hr;
	//disconnect the connection with PDC Server
	if(FAILED( hr = m_pTCPCMU1->DisconnectClient()))
	{
#ifdef	_DEBUG
		assert(NULL);
#endif
	}

	exit(0);
	CDialogEx::OnOK();
}

// converts binary timestamp data into text format
DWORD CPDC_Process_DlgDlg::DecodeTimeStamp(void)
{
	CComPtr<IRTDTParams>	pRTDTParams(NULL);
	HRESULT	hr;
	long z(0);
	ULONG u_32_year(0);
	ULONG u_32_month(0);
	ULONG u_32_day(0);
	ULONG u_32_hour(0);
	ULONG u_32_Minute(0);
	ULONG u_32_second(0);
	VARIANT var;
	VARIANT var1;

	::VariantClear(&var);
	::VariantClear(&var1);

	AtlTrace("OnUpdateRTData is called.\n");

	if( pVI )
	{
		if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_IRTDTParams, (void**)&pRTDTParams)))
			return 1;
		else
			AtlTrace("Success: ICFG2Params is acquired that contains the Configuration Frame2 decode data.\n");

		//get the year
		if(FAILED( hr = pRTDTParams->get_TS_Year( &u_32_year )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		//bad data
		if( u_32_year != 2013 )
			return 0;

		//get the month
		if(FAILED( hr = pRTDTParams->get_TS_Month( &u_32_month )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		//get the day
		if(FAILED( hr = pRTDTParams->get_TS_Day( &u_32_day )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		//get the hour
		if(FAILED( hr = pRTDTParams->get_TS_Hour( &u_32_hour )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		//get the minute
		if(FAILED( hr = pRTDTParams->get_TS_Minute( &u_32_Minute )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		//get the second
		if(FAILED( hr = pRTDTParams->get_TS_Second( &u_32_second )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		//construct a COleDateTime object from the acquired timestamp
		COleDateTime oleTS( (int)u_32_year, (int)u_32_month, (int)u_32_day
							, (int)u_32_hour, (int)u_32_Minute, (int)u_32_second);

		//get the milliseconds
		ULONG u_32_Milliseconds(0);
		if( FAILED( hr = pRTDTParams->get_Millisecond( &u_32_Milliseconds )))
		{
			pVI->SetControlValue("Phasor Time", RTDTTSTBL);
			return -1;
		}

		char tsbuf[128]; ::memset(tsbuf, 0, sizeof(tsbuf));
		::sprintf(tsbuf, "%02u:%02u:%02u,%04u    %u-%u-%u", u_32_hour, u_32_Minute, u_32_second, u_32_Milliseconds, u_32_year, u_32_month, u_32_day);
		AtlTrace("%s\n", tsbuf);
		VARIANT signaltime; ::VariantClear(&signaltime);
		signaltime.vt = VT_BSTR;
		_bstr_t _bstime = tsbuf;
		signaltime.bstrVal = _bstime.copy();
		SYSTEMTIME st; ::memset(&st, 0, sizeof(st));

		oleTS.GetAsSystemTime(st);

		_variant_t TimeStamp;
		var.vt = VT_DATE;
		if( !::SystemTimeToVariantTime(&st, &var.date) )
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
		
		//find the number of selected phasors
		int nSize(0);
		nSize = m_map_choose_phdt.size();

		if( !RTDTTSTBL.parray )
			RTDTTSTBL.parray = ::SafeArrayCreateVector( VT_BSTR, 0, nSize );

		for( long k = 0; k < nSize; k++ )
		{
			if( FAILED( hr = ::SafeArrayPutElement(RTDTTSTBL.parray, &k, _bstime.copy())))
			{
				pVI->SetControlValue("Phasor Time", RTDTTSTBL);
				return -1;
			}
		}
		
		pVI->SetControlValue("Phasor Time", RTDTTSTBL);
		pVI->SetControlValue("output array", LCHD);
		if( LCHD1.parray )
			pVI->SetControlValue("output array 3", LCHD1);
	}

	return 0;
}


// Decodes the real-time phasor values
DWORD CPDC_Process_DlgDlg::DecodePhasorValues(void)
{
	ULONG	u32_NoofPhasors(0);
	long lIndex(0);
	CComPtr<IRTDTParams>	pRTDTParams(NULL);
	HRESULT hr;		
	string str;
	float  flPhVal(0.0), flPhXVal(0.0), flPhYVal(0.0), flPhType(0), flAngleVal(0.0);
	double dPhVal(0.0), dAngleVal;

	VARIANT var;
	::VariantClear(&var);
	var.vt = VT_BSTR;
	_bstr_t _b;
	_b = "UPDATE";
	var.bstrVal = _b;

	if( pVI )
	{
		if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_IRTDTParams, (void**)&pRTDTParams)))
			return -1;
		else
			AtlTrace("Success: ICFG2Params is acquired that contains the Configuration Frame2 decode data.\n");

		//find the number of phasors in chosen data list box
//iITC		u32_NoofPhasors = m_map_choose_phdt.size();
		if(FAILED(hr = m_pTCPCMU1->get_PhDecSize(&u32_NoofPhasors)))
			return -1;
		if( !u32_NoofPhasors )
			return -1;

		if( !RTPHVALS.parray )
			RTPHVALS.parray = SafeArrayCreateVector( VT_R8, 0, u32_NoofPhasors );

		if(FAILED(hr = pRTDTParams->ChPhListBegin()))
			return -1;

		//insert X phasor and Y phasor values
		if(FAILED( hr = pRTDTParams->LoadPhVals( &flPhXVal, &flPhYVal, &flPhType )))
			return -1;

		AtlTrace("phasor x value is: %e-------phasor y value is: %e\n\n", flPhXVal, flPhYVal);

		if( !flPhType )
		{//Mangnitude of phasor
			flPhVal = ::sqrt(::pow(flPhXVal, 2) + ::pow(flPhYVal, 2));
			flPhVal /= 1000;
			dPhVal = (double)flPhVal;
			if( dPhVal != 0)
			{
				if(FAILED(hr = ::SafeArrayPutElement(RTPHVALS.parray, &lIndex, &dPhVal/*Amplitude in Kilo volts*/)))
				{
/*iITC					if(FAILED( hr = pRTDTParams->CleanPhasors() ))
						return -1;*/
					goto exit;
//					return -1;
				}
				else
					lIndex++;
			}
		}
		else
		{//phase of the phasor
			flAngleVal = atan2f(flPhYVal, flPhXVal);
			flAngleVal *= (180/PI);
			dAngleVal = (double)flAngleVal;

			if(FAILED(hr = ::SafeArrayPutElement(RTPHVALS.parray, &lIndex, &dAngleVal/*Angle in degrees*/)))
			{
/*iITC				if(FAILED( hr = pRTDTParams->CleanPhasors() ))
					return -1;*/
					goto exit;
//				return -1;
			}
			else
				lIndex++;
		}
		
		while( hr == S_OK )
		{
			//insert X phasor and Y phasor values
			hr = pRTDTParams->LoadPhVals( &flPhXVal, &flPhYVal, &flPhType );
			if( hr == S_FALSE )
				break;

			AtlTrace("phasor x value is: %e-------phasor y value is: %e\n\n", flPhXVal, flPhYVal);

			if( !flPhType )
			{//Mangnitude of phasor
				flPhVal = ::sqrt(::pow(flPhXVal, 2) + ::pow(flPhYVal, 2));
				flPhVal /= 1000;
				dPhVal = (double)flPhVal;
			
				if( dPhVal != 0 ){
					if(FAILED(hr = ::SafeArrayPutElement(RTPHVALS.parray, &lIndex, &dPhVal/*Phasor Amplitude in Kilo Volts*/)))
					{
/*iITC						if(FAILED( hr = pRTDTParams->CleanPhasors() ))
							return -1;*/
					goto exit;
//						return -1;
					}
					else
						lIndex++;
				}
			}
			else
			{//phase of the phasor
				flAngleVal = atan2f(flPhYVal, flPhXVal);
				flAngleVal *= (180/PI);
				dAngleVal = (double)flAngleVal;

				if(FAILED(hr = ::SafeArrayPutElement(RTPHVALS.parray, &lIndex, &dAngleVal/*Phasor Angle in degrees*/)))
					{
/*iITC						if(FAILED( hr = pRTDTParams->CleanPhasors() ))
							return -1;*/
					goto exit;
//						return -1;
					}
				else
					lIndex++;
			}
		}
exit:
		pVI->ShowFPOnCall = TRUE;
		_variant_t empty = "";
		//update the PDC_Port.vi Phasor Value Table table
		pVI->SetControlValue("Phasor Values", empty);
		pVI->SetControlValue("Phasor Values", RTPHVALS);
		pVI->SetControlValue("output array", LCHD);
		if(FAILED( hr = pRTDTParams->CleanPhasors() ))
		{
			pVI->ShowFPOnCall = TRUE;
			//update the PDC_Port.vi Phasor Value Table table
			pVI->SetControlValue("Phasor Values", RTPHVALS);
			return -1;
		}
		
		::Sleep(7);
		if(FAILED( hr = m_pTCPCMU1->ContinuePacketReading()))
		{
	#ifdef	_DEBUG
			assert(NULL);
	#endif
		}
	}
	
	return 0;
}


// decodes the analog and digital values
DWORD CPDC_Process_DlgDlg::DecodeAngDig(void)
{
	CComPtr<IRTDTParams>	pRTDTParams(NULL);
	ULONG	u32_NoofPhasors(0);
	VARIANT RTPHVALS;
	long lIndex(0);
	ULONG u32_NoofchAnalog(0);
	ULONG u32_NoofchDigital(0);
	HRESULT hr;
	float  flangVal(0), fldigVal(0);
	BSTR	bstr(NULL);
	_bstr_t	_b("");

	if( pVI )
	{
		if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_IRTDTParams, (void**)&pRTDTParams)))
			return 1;
		else
			AtlTrace("Success: ICFG2Params is acquired that contains the Configuration Frame2 decode data.\n");

		if(FAILED( hr = m_pTCPCMU1->get_NoofChAnalog(&u32_NoofchAnalog)))
			return -1;
/*
		if( !u32_NoofchAnalog )
			return -1;
*/
//-----------------digital signals
		if(FAILED( hr = m_pTCPCMU1->get_NoofChDigital(&u32_NoofchDigital)))
			return -1;
/*
		if( !u32_NoofchDigital )
			return -1;
*/
		if( !u32_NoofchDigital && !u32_NoofchAnalog )
			return -1;

		if( !RTDigitalVal.parray && u32_NoofchDigital )
			RTDigitalVal.parray = ::SafeArrayCreateVector(VT_R4, 0, u32_NoofchDigital);

		//create array of strings for binary digital strings
		if( !RTDigStrs.parray && u32_NoofchDigital )
			RTDigStrs.parray = ::SafeArrayCreateVector(VT_BSTR, 0, u32_NoofchDigital);

		if( !RTAnalogVal.parray && u32_NoofchAnalog )
			RTAnalogVal.parray = ::SafeArrayCreateVector(VT_R4, 0, ( u32_NoofchAnalog ));

		//this part is acquired by the issues of
		//point to the begginig of the FREQ/DFREQ list
		if(FAILED(hr = pRTDTParams->ChPhListBegin()))
			return -1;

		if( u32_NoofchAnalog )
		{
			//fetch analog values
			if(FAILED( hr = pRTDTParams->LoadANVals( &flangVal)))
				goto exit1;
	//-------------------------------------------------------
	//		FOR TESTING PURPOSES ONLY
	//-------------------------------------------------------
			flangVal = 21.000346;
	//-----------------------------------------------------------

			if(FAILED(hr = ::SafeArrayPutElement(RTAnalogVal.parray, &lIndex, &flangVal)))
				goto exit1;
			else
				lIndex++;

			while( (hr == S_OK ) && ( lIndex < u32_NoofchAnalog ) )
			{
				//insert FREQ and DFREQ values
				if(FAILED( hr = pRTDTParams->LoadANVals( &flangVal)))
					goto exit1;

				AtlTrace("Analog signal value is: %16.4f-------\n\n", flangVal);

				if(FAILED(hr = ::SafeArrayPutElement(RTAnalogVal.parray, &lIndex, &flangVal)))
					goto exit1;
				else
					lIndex++;
			}
		}
//-----------digital signals
		lIndex = 0;

		if( u32_NoofchDigital )
		{
			//insert digital values
			if(FAILED( hr = pRTDTParams->LoadDigVals( &fldigVal)))
				goto exit1;

			if(FAILED(hr = ::SafeArrayPutElement(RTDigitalVal.parray, &lIndex, &fldigVal)))
				goto exit1;
			else
				lIndex++;

			while( (hr == S_OK ) && (lIndex < u32_NoofchDigital ) )
			{
				//insert digital values
				if(FAILED( hr = pRTDTParams->LoadDigVals( &fldigVal)))
					goto exit1;

				AtlTrace("Digital signal value is: %16.4f-------\n\n", fldigVal);

				if(FAILED(hr = ::SafeArrayPutElement(RTDigitalVal.parray, &lIndex, &fldigVal)))
					goto exit1;
				else
					lIndex++;
			}

	//----------digital binary strings
			lIndex = 0;

			//fetch digital binary strings
			if(FAILED(hr = pRTDTParams->LoadRtDigStrs(&bstr)))
				return -1;

			if(FAILED(hr = ::SafeArrayPutElement(RTDigStrs.parray, &lIndex, bstr)))
				goto exit1;
			else
				lIndex++;

			while( (hr == S_OK ) && (lIndex < u32_NoofchDigital ) )
			{
				//insert digital values
				if(FAILED( hr = pRTDTParams->LoadRtDigStrs( &bstr)))
					return -1;
				_b = bstr;

				AtlTrace("Digital signal value is: %s-------\n\n", (const char*)_b);

				if(FAILED(hr = ::SafeArrayPutElement(RTDigitalVal.parray, &lIndex, bstr)))
					goto exit1;
				else
					lIndex++;
			}
		}
exit1:
		pRTDTParams->CleanANParams();
		pRTDTParams->CleanDGParams();
		pRTDTParams->CleanDigStr();
		//		pVI1->ShowFPOnCall = TRUE;
		//update the PDC_Port.vi Analog signal Value Table table
		pVI->SetControlValue("Analog Signal", RTAnalogVal);
		//update the PDC_Port.vi Analog signal Value Table table
		pVI->SetControlValue("Digital Signal", RTDigitalVal);
		//update the PDC_Port.vi Analog signal Value Table table
		pVI->SetControlValue("Digital String", RTDigStrs);
	}

	return 0;
}

// decodes FREQ and DFREQ values
DWORD CPDC_Process_DlgDlg::DecodeFQDQ(void)
{
	CComPtr<IRTDTParams>	pRTDTParams(NULL);
	ULONG	u32_NoofPhasors(0);
	VARIANT RTPHVALS;
	long lIndex(0);
	HRESULT hr;
	float  flfqVal(0), fldfqVal(0), flangVal(0), fldigVal(0);

	if( pVI )
	{
		if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_IRTDTParams, (void**)&pRTDTParams)))
			return 1;
		else
			AtlTrace("Success: ICFG2Params is acquired that contains the Configuration Frame2 decode data.\n");

		//find the number of phasors in chosen data list box
//iITC		u32_NoofPhasors = m_map_choose_phdt.size();
		if(FAILED(hr = m_pTCPCMU1->get_PhDecSize(&u32_NoofPhasors)))
			return -1;
		if( !u32_NoofPhasors )
			return -1;
		
		if( !RTFQVALS.parray )
			RTFQVALS.parray = ::SafeArrayCreateVector(VT_R4, 0, (u32_NoofPhasors));
		if(! RTDFQVALS.parray )
			RTDFQVALS.parray = ::SafeArrayCreateVector(VT_R4, 0, (u32_NoofPhasors));

		//point to the begginig of the FREQ/DFREQ list
		if(FAILED(hr = pRTDTParams->ChPhListBegin()))
			return -1;

		//insert FREQ and DFREQ values
		if(FAILED( hr = pRTDTParams->LoadFDQANDGVals( &flfqVal, &fldfqVal, &flangVal, &fldigVal )))
			return -1;

		AtlTrace("FREQ value is: %e-------DFREQ value is: %e\n\n", flfqVal, fldfqVal);

		if(FAILED(hr = ::SafeArrayPutElement(RTFQVALS.parray, &lIndex, &flfqVal)))
		{
			pVI->ShowFPOnCall = TRUE;
			//update the PDC_Port.vi Phasor Value Table table
			pVI->SetControlValue("FREQ Values", RTFQVALS);
			//update the PDC_Port.vi Phasor Value Table table
			pVI->SetControlValue("DFREQ Values", RTDFQVALS);
/*iITC			if(FAILED( hr = pRTDTParams->CleanPhasors() ))
				return -1;*/
			return 0;
		}
		else
		{
			if(FAILED(hr = ::SafeArrayPutElement(RTDFQVALS.parray, &lIndex, &fldfqVal)))
			{
				pVI->ShowFPOnCall = TRUE;
				//update the PDC_Port.vi Phasor Value Table table
				pVI->SetControlValue("FREQ Values", RTFQVALS);
				//update the PDC_Port.vi Phasor Value Table table
				pVI->SetControlValue("DFREQ Values", RTDFQVALS);
/*iITC				if(FAILED( hr = pRTDTParams->CleanPhasors() ))
					return -1;*/
				return 0;
			}
			else
				lIndex++;
		}

		while( (hr == S_OK ) && (lIndex < u32_NoofPhasors ) )
		{
			//insert FREQ and DFREQ values
			if(FAILED( hr = pRTDTParams->LoadFDQANDGVals( &flfqVal, &fldfqVal, &flangVal, &fldigVal )))
				return -1;

			AtlTrace("FREQ value is: %e-------DFREQ value is: %e\n\n", flfqVal, fldfqVal);

			if(FAILED(hr = ::SafeArrayPutElement(RTFQVALS.parray, &lIndex, &flfqVal)))
			{
				pVI->ShowFPOnCall = TRUE;
				//update the PDC_Port.vi Phasor Value Table table
				pVI->SetControlValue("FREQ Values", RTFQVALS);
				//update the PDC_Port.vi Phasor Value Table table
				pVI->SetControlValue("DFREQ Values", RTDFQVALS);
/*iITC				if(FAILED( hr = pRTDTParams->CleanPhasors() ))
					return -1;*/
				return 0;
			}
			else
			{
				if(FAILED(hr = ::SafeArrayPutElement(RTDFQVALS.parray, &lIndex, &fldfqVal)))
				{
					pVI->ShowFPOnCall = TRUE;
					//update the PDC_Port.vi Phasor Value Table table
					pVI->SetControlValue("FREQ Values", RTFQVALS);
					//update the PDC_Port.vi Phasor Value Table table
					pVI->SetControlValue("DFREQ Values", RTDFQVALS);
/*					if(FAILED( hr = pRTDTParams->CleanPhasors() ))
						return -1;*/
					return 0;
				}
				else
					lIndex++;
			}
		}
		pRTDTParams->CleanFDQ();


	//update the PDC_Port.vi Phasor Value Table table
		pVI->SetControlValue("FREQ Values", RTFQVALS);
		//update the PDC_Port.vi Phasor Value Table table
		pVI->SetControlValue("DFREQ Values", RTDFQVALS);
		pVI->SetControlValue("output array", LCHD);
	}

	return 0;
}

// Decode the phasor values as complex numbers
DWORD CPDC_Process_DlgDlg::DecodePhaseRect(void)
{
	CComPtr<IRTDTParams>	pRTDTParams(NULL);
	ULONG	u32_NoofPhasors(0);
	VARIANT RTPHVALS;
	long lIndex(0);
	HRESULT hr;
	float  flPhVal(0), flPhXVal(0), flPhYVal(0), flPhType(0);

	::VariantClear(&RTPHVALS);

	string str;
	VARIANT var1, var2;
	::VariantClear(&var1);
	var1.vt = VT_R4;
	::VariantClear(&var2);
	var2.vt = VT_R4;

	if( /*pVI1*/pVI )
	{
		if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_IRTDTParams, (void**)&pRTDTParams)))
			return 1;
		else
			AtlTrace("Success: ICFG2Params is acquired that contains the Configuration Frame2 decode data.\n");

		//find the number of phasors in chosen data list box
		u32_NoofPhasors = m_map_choose_phdt.size();
		if( !u32_NoofPhasors )
		{
			return -1;
		}
		
		if( !RTPHIPVAL.parray )
			RTPHIPVAL.parray = ::SafeArrayCreateVector(VT_R4, 0, (u32_NoofPhasors));
		if(! RTPHRPVAL.parray )
			RTPHRPVAL.parray = ::SafeArrayCreateVector(VT_R4, 0, (u32_NoofPhasors));

		if(FAILED(hr = pRTDTParams->ChPhListBegin()))
			return -1;

		//insert X phasor and Y phasor values
		if(FAILED( hr = pRTDTParams->LoadPhVals( &flPhXVal, &flPhYVal, &flPhType )))
			return -1;

		AtlTrace("phasor x value is: %e-------phasor y value is: %e\n\n", flPhXVal, flPhYVal);

		if(FAILED(hr = ::SafeArrayPutElement(RTPHRPVAL.parray, &lIndex, &flPhXVal)))
			return 0;
		else
		{
			if(FAILED(hr = ::SafeArrayPutElement(RTPHIPVAL.parray, &lIndex, &flPhXVal)))
			{
				return 0;
			}
			else
				lIndex++;
		}
		
		while( hr == S_OK )
		{
			//insert X phasor and Y phasor values
			hr = pRTDTParams->LoadPhVals( &flPhXVal, &flPhYVal, &flPhType );
			if( hr == S_FALSE )
				break;

			AtlTrace("phasor x value is: %e-------phasor y value is: %e\n\n", flPhXVal, flPhYVal);

			if(FAILED(hr = ::SafeArrayPutElement(RTPHRPVAL.parray, &lIndex, &flPhXVal)))
			{
				return 0;
			}
			else
			{
				if(FAILED(hr = ::SafeArrayPutElement(RTPHIPVAL.parray, &lIndex, &flPhYVal)))
				{
					return 0;
				}
				else
					lIndex++;
			}
		}

		//update the PDC_Port.vi Phasor Value Table table
		pVI->SetControlValue("Real", RTPHRPVAL);
		pVI->SetControlValue("Img", RTPHIPVAL);
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnCMPData(WPARAM wParam, LPARAM lParam)
{
/*	VARIANT var;
	m_bFillCluster = TRUE;
	*/
	return 0;
}


LRESULT CPDC_Process_DlgDlg::OnLoadConfigData(WPARAM wParam, LPARAM lParam)
{
	//check for config frame 2 info
	if( !m_bConfig2Loaded )
	{
		//send a config2 command to the server in order to collect that info
		OnBnClickedButton3();
		//set a flag m_bConfig2Loaded
		m_bConfig2Loaded = TRUE;
		AtlTrace("Success: configuration frame2 infor is read now.\n");
	}
	else
		AtlTrace("Success: configuration frame2 infor is already read.\n");

	::PostMessageA(this->m_hWnd, BUILD_DECODE_ARRAY, 0, 0);
	
	return 0;
}


//Build a decode array based using the Load/Save Configuration XML file.
LRESULT CPDC_Process_DlgDlg::OnBldDecArr(WPARAM wParam, LPARAM lParam)
{
	CComPtr<ICFG2Params>	pCFG2Params(NULL);
	HRESULT	hr;
	ULONG nPHNMR(0);
	ULONG iIndex(0);
	string str, str2, str3, str4;
	_bstr_t	_b, mybstr1;
	zchosendt2 = 0;
	int index(0);
	//empty the phasor chosen data map
	m_map_choose_phdt.clear();

	if(FAILED(hr = m_pTCPCMU1->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return S_FALSE;
	else
		AtlTrace("Success: pCFG2Params acquired.\n");

	//clear the decode phasors list, threrefore no redundancy of phasor parameters.
	m_pTCPCMU1->ClearDecodeVector();

// a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
	_variant_t PHN_LIST;
//	PHN_LIST = pVI->GetControlValue("output array 3");
	PHN_LIST = pVI->GetControlValue("Array");
	// assign our SAFEARRAY to the array type of the variant
    if (V_VT(&PHN_LIST) == ( VT_ARRAY | VT_BSTR ))
    {	printf("This is an array of bstrings\n");
        psa = V_ARRAY(&PHN_LIST);
    }
	
    LONG cElements, lLBound, lUBound;
    BSTR HUGEP *pbstr;
	int pos(0); 
	// Get the number of dimensions of our array
    printf("Number of channels: %d\n",SafeArrayGetDim(psa));    
 
	// Get the lower bound
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr)) return false;
 
    //Get upper bound
    hr = SafeArrayGetUBound(psa, 1, &lUBound);
    if (FAILED(hr)) return false;
 
    //Get total number of elements
    cElements = lUBound-lLBound+1;
	::VariantClear(&SELPHDTLIST);

	LCHD.parray = ::SafeArrayCreateVector(VT_BSTR, 0, cElements);

	/*	
	if( !cElements )
	{
		::MessageBoxA(NULL, "There is no Configuration XML File for chosen data is empty", "", IDOK);
		return 0;
	}
*/
	// Use SafeArrayAccessData to place a lock on the data so that we
	// can access the data in our application
    hr = SafeArrayAccessData(psa, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))return false;
 
	//build chosen phasors prefix text for me.

	//find total number of the chosen dat
	if( FAILED( hr = m_pTCPCMU1->FindTotalNoofPhasors(&nPHNMR)))
		return -1;
	
	vector<vector<string> > strprfx;
	vector<vector<string> >::iterator strprfxIT;
	vector<string>::iterator vecIT;

	vector<vector<string> > strprfxa;



	strprfx.resize((nPHNMR*2));
	strprfxa.resize((nPHNMR*2));
	for(strprfxIT = strprfx.begin(); strprfxIT != strprfx.end(); strprfxIT++)
	{
		(*strprfxIT).resize(nPHNMR*2);
	}

	for(strprfxIT = strprfxa.begin(); strprfxIT != strprfxa.end(); strprfxIT++)
	{
		(*strprfxIT).resize(nPHNMR*2);
	}

	int nsufix(0), nsufix1(0), inx(1), inx1(2);
	char * p;
	p = (char*)m_pMalloc->Alloc(nPHNMR*2);
	if( !p )
	{
		AtlTrace("allocation for ---Mx of ---- failed.\n");
	}

	for( nsufix = 0; nsufix < (nPHNMR*2); ++nsufix)
	{
		for(nsufix1 =0, strprfxIT = strprfx.begin(); /*strprfxIT != strprfx.end()*/nsufix1 < (nPHNMR*2); strprfxIT++)
		{
			strprfx[nsufix][nsufix1] += '(';
			::itoa((nsufix1+1), p, 10);
			strprfx[nsufix][nsufix1] += p;
			strprfx[nsufix][nsufix1] += ')';
			strprfx[nsufix][nsufix1] += ' ';
			strprfx[nsufix][nsufix1] += 'M';
			::itoa((nsufix+1), p, 10);
			strprfx[nsufix][nsufix1] += p;
			strprfx[nsufix][nsufix1] += " of ";
			nsufix1 += 1;
/*			strprfx[nsufix][nsufix1] += '(';
			::itoa((nsufix1+1), p, 10);
			strprfx[nsufix][nsufix1] += p;
			strprfx[nsufix][nsufix1] += ')';
			strprfx[nsufix][nsufix1] += ' ';
			strprfx[nsufix][nsufix1] += 'A';
			::itoa((nsufix+1), p, 10);
			strprfx[nsufix][nsufix1] += p;
			strprfx[nsufix][nsufix1] += " of ";
			nsufix1 += 1;
*/
		}
	}


	for( nsufix = 0; nsufix < (nPHNMR*2); ++nsufix)
	{
		for(nsufix1 =0, strprfxIT = strprfx.begin(); /*strprfxIT != strprfx.end()*/nsufix1 < (nPHNMR*2); strprfxIT++)
		{
			strprfxa[nsufix][nsufix1] += '(';
			::itoa((nsufix1+1), p, 10);
			strprfxa[nsufix][nsufix1] += p;
			strprfxa[nsufix][nsufix1] += ')';
			strprfxa[nsufix][nsufix1] += ' ';
			strprfxa[nsufix][nsufix1] += 'A';
			::itoa((nsufix+1), p, 10);
			strprfxa[nsufix][nsufix1] += p;
			strprfxa[nsufix][nsufix1] += " of ";
			nsufix1 += 1;
/*			strprfx[nsufix][nsufix1] += '(';
			::itoa((nsufix1+1), p, 10);
			strprfx[nsufix][nsufix1] += p;
			strprfx[nsufix][nsufix1] += ')';
			strprfx[nsufix][nsufix1] += ' ';
			strprfx[nsufix][nsufix1] += 'A';
			::itoa((nsufix+1), p, 10);
			strprfx[nsufix][nsufix1] += p;
			strprfx[nsufix][nsufix1] += " of ";
			nsufix1 += 1;
*/
		}
	}
	long lIndex = 0;
	VARIANT var;
	var.vt = VT_BSTR;
	
	//  Iterate through the array and print the values
    for (int i = 0; i < cElements; i++, zchosendt++, lIndex++)
    {	
		_bstr_t mybstr(pbstr[i]);
		if( !mybstr.length() )
			return 0;

		AtlTrace("Data:<%s>\n",(char*)mybstr);
		str += mybstr;
//----------------------------------------------
//		unsigned pos = str.find(' ');
//		str = str.substr(pos+1);
//----------------------------------------------
		//the list of chosen phasors comming from NI contains empty
		//strings that must be skipped
		if( str.empty() )
			continue;

		//insert the phasor name into the safearray of the chosen data list
		var.bstrVal = mybstr.copy();
		if(FAILED( hr = ::SafeArrayPutElement(LCHD.parray, &lIndex, var.bstrVal)))
			assert(NULL);

		pVI->SetControlValue("Array 2", LCHD);

		BOOL bFound(FALSE);	//this flag is used for the 

		//check whether it is a phasor or a digital or analog signal
//		str2 = str.substr(0, /*6*/10);		//this must be modified to support general case such as "Mxxxx of " which is more than 6 characters!!!
		unsigned zpos = str.find_first_of(' ');
		unsigned zpos1 = str.find_first_of(' ', zpos+1);
		unsigned zpos2 = str.find_first_of(' ', zpos1+1);
		str2 = str.substr(0, zpos2+1);


		for( int j = 0; j < (nPHNMR * 2); j++)
		{
			for( int k = 0; k < (nPHNMR * 2); k++)
			{
				if( str2.empty() )
					continue;

				if( (!str2.compare(strprfx[j][k])) ||  (!str2.compare(strprfxa[j][k])))
				{
					pos = str.find('/');
//					str2 = str.substr(/*6*/10, pos-/*6*/10);
//					str2 = str.substr(zpos+6, pos-(zpos+6));
					str2 = str.substr(zpos2+1, pos-(zpos2+1));
					mybstr ="";
					mybstr = str2.c_str();
					AtlTrace("%s\n", (char*)mybstr);
					mybstr1 = str.c_str();
					if( FAILED( hr = ::SafeArrayPutElement(PHNAMETBL.parray, &zchosendt2, mybstr1.copy())))
						return -1;
					pVI->SetControlValue("Phasor_Value_Table", PHNAMETBL);
//---------------------------index code--------------
/*					str4 = str[1];
	//				mybstr1 = str4.c_str();
					index = ::atoi(str4.c_str());
					index = (zchosendt2+1);
					if( FAILED( hr = ::SafeArrayPutElement(LCHDINX.parray, &zchosendt3, &index)))
						return -1;
					pVI->SetControlValue("Phasor Index", LCHDINX);*/
//--------------------------------------------------
					zchosendt2++;
					bFound = TRUE;
					break;	//iITC
				}
				else	
				{
				}
			}
		}
		
		if( !bFound )	//it is a digital or a analog signal
		{
			//extract the PMU name and Signal name.
			unsigned pos1 = str.find(' ');
			unsigned pos2 = str.find('/'); 
			str2 = str.substr(pos1+1, pos2-(pos1+1));
//			unsigned pos = str.find_first_of('/');
//			AtlTrace("END OF THE PMU NAME is at position: %d\n", pos);
//			str2 = str.substr(0, pos);
			AtlTrace("PMU Name is: %s\n", str2.c_str());
			//save the pmu name
			mybstr = str2.c_str();
			mybstr1 = str.c_str();
			if( !ANDIGNAMETBL.parray )
			{
				ULONG u32_NoofAnalogs(0);
				ULONG u32_NoofDigitals(0);
				if(FAILED( hr = m_pTCPCMU1->get_NoofChAnalog(&u32_NoofAnalogs)))
					return -1;
				if(FAILED( hr = m_pTCPCMU1->get_NoofChDigital(&u32_NoofDigitals)))
					return -1;
				ANDIGNAMETBL.parray = ::SafeArrayCreateVector(VT_BSTR, 0, /*(u32_NoofAnalogs + u32_NoofDigitals)*/10);
			}
			if( FAILED( hr = ::SafeArrayPutElement(ANDIGNAMETBL.parray, &zchosendt1/*&zchosendt2*/, mybstr1.copy())))
				return -1;
			pVI->SetControlValue("output array 4", ANDIGNAMETBL);
//---------------------------index code--------------
/*			str4 = str[1];
//				mybstr1 = str4.c_str();
			index = ::atoi(str4.c_str());
			if( FAILED( hr = ::SafeArrayPutElement(LCHDINX.parray, &zchosendt3, &index)))
				return -1;
			pVI->SetControlValue("Phasor Index", LCHDINX);*/
//--------------------------------------------------
//			zchosendt2++;
			zchosendt1++;
		}

//---------------------------enter the chosen phasor name into the relevant list
//---------------------------index code--------------
					str4 = str[1];
					index = ::atoi(str4.c_str());
					index = (zchosendt3+1);
					if( FAILED( hr = ::SafeArrayPutElement(LCHDINX.parray, &zchosendt3, &index)))
						return -1;
					pVI->SetControlValue("Phasor Index", LCHDINX);
		zchosendt3++;
//--------------------------------------------------

		if(bFound)
		{
			//find the index of the PMU in the list of the PMUs
			if(hr = FAILED(pCFG2Params->FindPMUIndex(mybstr.copy(), &iIndex)))
				return S_FALSE;
			else
			{
				//save the number of the phasor which in the real-time packet should be
				//decoded based on the user chosen data
				if(FAILED(hr = m_pTCPCMU1->put_PMUCount(iIndex)))
					return -1;
			}

			//clear the str2 and store the phasor name
			str2.clear();
//------------------------------------------------------------
//				size_t pos2 = str.find('/'); 
//				str2 = str.substr(zpos+6, pos2);
			str2 = str.substr(zpos2+1, str.length() -(zpos2+1));
//				str2 = str.substr(/*6*/10, str.length() - /*6*/10);
//------------------------------------------------------------
			mybstr ="";
			mybstr = str2.c_str();
			AtlTrace("%s\n", (char*)mybstr);

			//find the index of the phasor in the list of the phasors
			if(hr = FAILED( pCFG2Params->FindPhasorIndex(mybstr.copy(), &iIndex)))
				return S_FALSE;
			else
			{
				//save the number of the phasor which in the real-time packet should be
				//decoded based on the user chosen data
				if(FAILED(hr = m_pTCPCMU1->put_PhNumber(iIndex)))
					return -1;
			}

			if( str.empty())
				return -1;
			//if this is a phasor
			//find the magnitude/angle code write it to phasor decode vector
			if( str[/*0*//*4*/zpos+1] == 'M'){
				//save that the user chose to read the magnitude
				//of the given phasor in the real-time packet
				if(FAILED(hr = m_pTCPCMU1->put_PhMagAng(0)))
					return -1;
			}
			else if( str[/*0*//*4*/zpos+1] == 'A'){
				//save that the user chose to read the magnitude
				//of the given phasor in the real-time packet
				if(FAILED(hr = m_pTCPCMU1->put_PhMagAng(1)))
					return -1;
			}
			else
				{
			#ifdef	_DEBUG
					assert(NULL);
			#endif
				}

			if( str2.empty())
				return -1;

			//insert the phasor position and type of chosen data list
			//this is called from within Modul1::BIN2ASCCI_RT in order
			//to know which phasor, Amplitude/Angle to be decoded and 
			if(FAILED(hr = m_pTCPCMU1->InsertPhPosType()))
				return -1;

	//-----------------real-time decoding necessary info to extract ends here.

			m_retchphdt = m_map_choose_phdt.insert(pair<int, string >(zchosendt, str));
			if( m_retchphdt.second == false)
				return 0;
		}
		else	//this is a digital or analog signal
		{
			size_t pos1 = str.find(' ');
			size_t pos2 = str.find('/');
			str2 = str.substr(pos1+1, str.length()-(pos2-1));
			mybstr ="";
			mybstr = str2.c_str();
			AtlTrace("%s\n", (char*)mybstr);
			BSTR bstrPMU;
			if(hr = FAILED(pCFG2Params->FirstPMU(&bstrPMU)))
				return S_FALSE;

			//find the index of the PMU in the list of the PMUs
			if(hr = FAILED(pCFG2Params->FindPMUIndex(mybstr.copy(), &iIndex)))
				return S_FALSE;
			else
			{
				//save the number of the phasor which in the real-time packet should be
				//decoded based on the user chosen data
				if(FAILED(hr = m_pTCPCMU1->put_ADPMUCount(iIndex)))
					return -1;
			}

			//search in analog and digital list and call the corrct process
			//related to analog and digital signals.
			//find the index of the Analog signal in the list of analog signals
			unsigned xpos = str.find(' ');
			mybstr = (str.substr(xpos+1)).c_str();
			hr = pCFG2Params->FindAnalogIndex(mybstr.copy(), &iIndex);
			if( hr == S_FALSE)
			{
				//add the analog/or digital signal name	
//				mybstr = str.c_str();
				//find the index of the phasor in the list of the phasors
				if(hr = FAILED( pCFG2Params->FindDigitalIndex(mybstr.copy(), &iIndex)))
					return S_FALSE;
				else
				{
					//save the number of the phasor which in the real-time packet should be
					//decoded based on the user chosen data
					if(FAILED(hr = m_pTCPCMU1->put_DigitalNumber(iIndex)))
						return -1;

					//put 100000 for analog number to difrerentiate digital value from analog value
					if(FAILED( hr = m_pTCPCMU1->put_AnalogNumber(100000)))
						return -1;

					//count up the number of digital sinals
					if(FAILED(hr = m_pTCPCMU1->AddDigitalNumber()))
						return -1;

					//insert the phasor position and type of chosen data list
					//this is called from within Modul1::BIN2ASCCI_RT in order
					//to know which phasor, Amplitude/Angle to be decoded and 
					if(FAILED(hr = m_pTCPCMU1->InsertAnaPosType()))
					return -1;

	//-----------------real-time decoding necessary info to extract ends here.

				m_retchphdt = m_map_choose_phdt.insert(pair<int, string >(zchosendt, str));
				if( m_retchphdt.second == false)
					return 0;
				}
			}
			else
			{
				//save the number of the phasor which in the real-time packet should be
				//decoded based on the user chosen data
				if(FAILED(hr = m_pTCPCMU1->put_AnalogNumber(iIndex)))
					return -1;
				//put 100000 as digital number as a dummy digital number to mark this one as an analog number 
				//and not a digital number
				if(FAILED(hr = m_pTCPCMU1->put_DigitalNumber(100000)))
					return -1;
				//count up the number of digital signals selected in chosen phasor list box
				if(FAILED( hr = m_pTCPCMU1->AddAnalogCh()))
					return -1;

				//insert the phasor position and type of chosen data list
				//this is called from within Modul1::BIN2ASCCI_RT in order
				//to know which phasor, Amplitude/Angle to be decoded and 
				if(FAILED(hr = m_pTCPCMU1->InsertAnaPosType()))
					return -1;
		//-----------------real-time decoding necessary info to extract ends here.

				m_retchphdt = m_map_choose_phdt.insert(pair<int, string >(zchosendt, str));
				if( m_retchphdt.second == false)
					return 0;
			}
		}

		str.clear();
    }//end-of-loop 
	
	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnRefshRateSel(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
	//lParam is the refresh rate in milliseconds.
	//following table is Refresh Rate in Hertz to time delay in milliseconds
	//-----------------------------------------------------------
	//Frequency				time delay[milliseconds]
	//-----------------------------------------------------------
	//   1Hz							1000
	//   2Hz							500
	//   5Hz							200
	//this time delay is placed in the communication module to force the data 
	//retrieval from the PDC Server is delayed for a 1000, 500, and 200 milliseconds
	//delayed so that we delay data retrieval from PDC Server by these delay times in order to 
	//simulate refresh rate concept in PDC Application.
	if(FAILED(hr = m_pTCPCMU1->put_RefreshRate(lParam)))
		return S_FALSE;
	else
	{
		AtlTrace("Success: New Refresh Rate selected and it is stored in ITCPCMU1 Object.\n");
		AtlTrace("Refresh Rate value is: %d[milliseconds]\n", lParam);
	}

	return 0;
}

LRESULT CPDC_Process_DlgDlg::OnChDTUPDATE(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
// a SAFEARRAY is used to access the data from an array
    SAFEARRAY *psa = NULL;
	PHN_LIST = pVI->GetControlValue("Array 2");
	// assign our SAFEARRAY to the array type of the variant
    if (V_VT(&PHN_LIST) == ( VT_ARRAY | VT_BSTR ))
    {	printf("This is an array of bstrings\n");
        psa = V_ARRAY(&PHN_LIST);
    }
	
    LONG cElements, lLBound, lUBound;
    BSTR HUGEP *pbstr;
	int pos(0); 
	// Get the number of dimensions of our array
    printf("Number of channels: %d\n",SafeArrayGetDim(psa));    
 
	// Get the lower bound
    hr = SafeArrayGetLBound(psa, 1, &lLBound);
    if (FAILED(hr)) return false;
 
    //Get upper bound
    hr = SafeArrayGetUBound(psa, 1, &lUBound);
    if (FAILED(hr)) return false;
 
	// Use SafeArrayAccessData to place a lock on the data so that we
	// can access the data in our application
    hr = SafeArrayAccessData(psa, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))return false;

	//Get total number of elements
    cElements = lUBound-lLBound+1;
	::VariantClear(&SELPHDTLIST);

	LCHD1.parray = ::SafeArrayCreateVector(VT_BSTR, 0, cElements);


	long lIndex = 0;
	VARIANT var;
	var.vt = VT_BSTR;
	string str;
	//  Iterate through the array and print the values
    for (int i = 0; i < cElements; i++, lIndex++)
    {	
		_bstr_t mybstr(pbstr[i]);
		if( !mybstr.length() )
			break;

		AtlTrace("Data:<%s>\n",(char*)mybstr);
		str += mybstr;
		//the list of chosen phasors comming from NI contains empty
		//strings that must be skipped
		if( str.empty() )
			continue;

		//insert the phasor name into the safearray of the chosen data list
		var.bstrVal = mybstr.copy();
		if(FAILED( hr = ::SafeArrayPutElement(LCHD1.parray, &lIndex, var.bstrVal)))
			assert(NULL);
	}

	pVI->SetControlValue("output array", LCHD1);
	return 0;
}