
// PDC_Process_DlgDlg.h : header file
//

#pragma once
           
 

#include "Module1_i.h"
#include "DecodInf_i.h"
#include "conio.h"
#include "comutil.h"
#include <vector>
#include <map>
#include "atlcomcli.h"


using namespace std;

//--------------data structure to complete the outputing of the data from PDC_Port.vi to french/spanish teams applicatons
typedef struct _tagPDC_OUTPUT_DATA{
	string strPHNAME;
	float	fPHMag;
	float	fPHAng;
	float	fPHRecXVal;
	float	fPHRecYVal;
}PDC_OUTPUT_DATA;
//----------------------------------------------

// CPDC_Process_DlgDlg dialog
class CPDC_Process_DlgDlg : public CDialogEx
{
DECLARE_MESSAGE_MAP()
// Construction
public:
	CPDC_Process_DlgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PDC_PROCESS_DLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnConnectionMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHeaderFrameMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCFG_1_FrameMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCFG_2_FrameMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdatePhasor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPMUListSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAnaListSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDigListSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInfoTypeSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateUserConfig(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInfoTypeList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPhasorParamSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChoosePhDt(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateRTData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRTOnlyRun(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnButton2Clicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCMPData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadConfigData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBldDecArr(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefshRateSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChDTUPDATE(WPARAM wParam, LPARAM lParam);
public:
	DWORD StartCM(void);

private:
	CComPtr<ITCPCMU1>	m_pTCPCMU1;
	ITCPCMU1 * m_pTCPCMU2;
public:
	DWORD Connect(void);
	static ULONG WINAPI  ProcessEventThread(void * pParam);
	static ULONG WINAPI  UpdatePhasorThread(void * pParam);
	static ULONG WINAPI  UpdateRTDataThread(void * pParam);

	void ProcessEvents(void);
	DWORD Send(void);
private:
	char* m_pPDCData;
	LPMALLOC m_pMalloc;
protected:
	static HANDLE ghMutex;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
private:
	// Initiates Activex client for data exchange with activex server
	DWORD CPDC_Process_DlgDlg::InitActivexServer(void);
	// update the phasor parameters on PDC_Port.vi GUI
	DWORD UpdatePhasorParams(void);
	//update the Real-Time Data on PDC_Port.vi GUI
	DWORD UpdateRTData(void);

private:
	vector<VARIANT>	vecPhasors;
	vector<VARIANT> vecAnalogs;
	vector<VARIANT> vecDigitals;
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
private:
	// Information type combo is selected and current value is
	int m_iPhasors;
	// Load configuration information
	int LoadConfig(void);
	// stores the IP address
	string m_szIP;
	// saves the user network configuration
	DWORD SaveConfig(void);
	// stores the user network configuration
	string m_szFName;
	// stores Port number of the PDC Server
	string m_szPort;
	// stores IDCODE of the PDC Server
	string m_szIDCode;
	// stores index of the PMU in the PMU List
	int m_nPMU_Number;
	// stores the data management unit object
	CComPtr<IPDCDMU1> m_pDMU1;
	// Finds total number of phasors of PMUs of PDC Server
	DWORD FindNoofPhasors(unsigned long * m_u32_PHNMR);
	// stores total number of phasor numbers
	long m_lPHNMR;
	VARIANT PMUTBL;
	VARIANT	PHAMTBL;
	VARIANT PHANTBL;
	VARIANT PHNTBL;
	VARIANT PHCSTBL;
	VARIANT SELPHDTLIST;
	VARIANT RTDTTSTBL;
	VARIANT RTPHRPVAL;
	VARIANT RTPHIPVAL;
	VARIANT RTPHVALS;
	VARIANT RTFQVALS;
	VARIANT RTDFQVALS;
	VARIANT RTAnalogVal;
	VARIANT RTDigitalVal;
	VARIANT RTDigStrs;
	VARIANT LCHD;
	VARIANT LCHD1;
	VARIANT LCHDINX;
	VARIANT ANDIGNAMETBL;
	VARIANT PHNAMETBL;
	_variant_t PHN_LIST;

	//labview related objects
	_ApplicationPtr	pLV;
	VirtualInstrumentPtr pVI;
	//OUTPUTING PDC DATA RELATED STRUCTURES
	PDC_OUTPUT_DATA	*	pPDCOUTDT;
	long z;
	long zchosendt;
	long prev;
	//map to save selected phasor and check if there is a
	//repetition phasor
	map<int, string > m_map_sel_phasor;
	map<int, string >::iterator	m_selphIt;
	pair<map<int, string >::iterator, bool> m_retselph;
	//map to save chosen data and check whether there 
	//is a repetition phasor
	map<int, string> m_map_choose_phdt;
	map<int, string>::iterator m_chphdtIt;
	pair<map<int, string>::iterator, bool> m_retchphdt;
	//list of name of chosen phasors
	map<int, string> m_map_chosen_phasor;
	map<int, string>::iterator m_chphIt;
	pair<map<int, string>::iterator, bool> m_retchph;

public:
	afx_msg void OnBnClickedOk();
private:
	// IRTDTParams object to decode real time data
	CComPtr<IRTDTParams> m_pRTDTParams;
	// boolean to control the flow of the connection when enable real time
	BOOL m_bSkip;
protected:
	// converts binary timestamp data into text format
	DWORD DecodeTimeStamp(void);
public:
	// Decodes the real-time phasor values
	DWORD DecodePhasorValues(void);
	// Decode the phasor values as complex numbers
	DWORD DecodePhaseRect(void);
protected:
	// Fills the output data cluster with data values coming from PDC Server
	HANDLE hUpdatRtDtThd;
private:
	// start the output cluster
	BOOL m_bFillCluster;
	// decodes FREQ and DFREQ values
	DWORD DecodeFQDQ(void);
	// decodes the analog and digital values
	DWORD DecodeAngDig(void);
	// stores log file
	string m_szLogFile;
	DWORD	dwThreadID;
	DWORD dwUpdatRtDtThdID;
	// shows that the configuration frame2 is loaded
	BOOL m_bConfig2Loaded;
	// stores "play" status of the PDC application, if this flag is true it means the PDC application already playing some phasors, it user clicks play button again it will not work
	BOOL m_bSkipPlay;
	// stores the refresh rate
	ULONG m_u32_Rfsh;
	// stores the preamble number in the chosen data list box
	int m_nIndex;
	// stores the index for analog and digital signals
	long zchosendt1;
	// stores the index of phasor names to insert in safe array
	long zchosendt2;
	long zchosendt3;
};
