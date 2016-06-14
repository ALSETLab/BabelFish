// TCPCMU1.h : Declaration of the CTCPCMU1

#pragma once
#include "Module1_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>
#include "PDCSocket.h"
#include <assert.h>
#include <map>
#include <vector>
#include <utility>
#include <comdef.h>
#include "comutil.h"
#include "atlcomcli.h"
#include <time.h>
#include <string>
#include "Decoder.h"
#include "DecodInf_i.h"


using namespace ATL;

using namespace std;

#define		MAX_NUM_SOCKETS		50

// CTCPCom1
struct cmp_str
{
	int operator() (string a, string b)
	{
//iITC		return (a.compare(b));
		return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}
};


// CTCPCMU1

class ATL_NO_VTABLE CTCPCMU1 :
	public CPDCSocket<CTCPCMU1>,
	public CDecoder<CTCPCMU1>,
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public IObjectConstruct,
	public CComCoClass<CTCPCMU1, &CLSID_TCPCMU1>,
	public IDispatchImpl<ITCPCMU1, &IID_ITCPCMU1, &LIBID_Module1Lib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CTCPCMU1()
		: m_bstrSockName(NULL)
		, m_bySYNC(0)
		, m_chSYNC1(0)
		, m_chSYNC2(0)
		, m_bCFG2(false)
		, m_pInnerUnk(NULL)
		, m_bSkip(false)
		, m_cnt(0)
		, m_strWarning("")
		, m_bConnected(FALSE)
		, m_u32_NoofAnalogs(0)
		, m_u32_NoofDigitals(0)
		, m_u32_TotAnalogs(0)
		, m_u32_TotDigitals(0)
		, m_u32_RefreshRate(0)
		, m_tsfirst(0)
		, m_tsSecond(0)
	{
		HRESULT hr;
//------------------------------------------------------------------------------
		if( FAILED( hr = ::CoGetMalloc(1, (LPMALLOC*)&m_pMalloc) ) )
		{
			AtlTrace("Error: failed to create IMalloc object.\n");
//			throw new CMemoryException();
			assert(NULL);
		}
		else
			AtlTrace("Success: IMalloc Created Successfully.\n");
//--------------------------------------------------------------------------------

		//set the iterator to the beginning of the map
		m_sockIt = m_SocketMap.begin();
		//set the iterator to the begging of the map
		m_CFG2It = m_map_CFG2Frame.begin();
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		HRESULT hr;
		//create the ICFG2Params object as aggregated
		if(FAILED( hr = ::CoCreateInstance( CLSID_CFG2Params,
											GetControllingUnknown(),
											CLSCTX_SERVER,
											IID_IUnknown,
											(void**)&m_pInnerUnk)))
											return S_FALSE;

		//create the IRTDTParams object as aggregated
		if(FAILED( hr = ::CoCreateInstance( CLSID_RTDTParams,
											GetControllingUnknown(),
											CLSCTX_SERVER,
											IID_IUnknown,
											(void**)&m_pInnerUnk1)))
											return S_FALSE;


		if(FAILED(hr = ::CoGetMalloc(1, (LPMALLOC*)&m_pMalloc)))
			return S_FALSE;
		else
			AtlTrace("Success: IMalloc is created in CFG2Params.h file.\n");

		//acquire IRTDTParams object to decode the Real time data of PDC
		if(FAILED(hr = m_pInnerUnk1->QueryInterface(IID_IRTDTParams, (void**)&m_pRTDTParams)))
			return 1;
		else
			AtlTrace("Success: IRTDTParams is acquired.\n");

		m_hEventRT = ::CreateEventA(NULL, TRUE, FALSE, "");
		if (m_hEventRT == NULL) 
		{ 
			printf("CreateEvent failed (%d)\n", GetLastError());
			return S_FALSE;
		}


		vecphdec.resize(3, -1);
		vecADdec.resize(3, -1);

		//initialize the time stamps
		::memset(&m_ts1, 0, sizeof(time_t));
		::memset(&m_ts2, 0, sizeof(time_t));

		return S_OK;
	}

	void FinalRelease()
	{
		//release the ICFG2Params object.
		m_pInnerUnk->Release();
		m_pInnerUnk1->Release();
		m_pMalloc->Release();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TCPCMU1)
DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(CTCPCMU1)
	COM_INTERFACE_ENTRY(ITCPCMU1)
	COM_INTERFACE_ENTRY(IObjectControl)
	COM_INTERFACE_ENTRY(IObjectConstruct)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_ICFG2Params, m_pInnerUnk)
	COM_INTERFACE_ENTRY_AGGREGATE(IID_IRTDTParams, m_pInnerUnk1)
END_COM_MAP()



// IObjectControl
public:
	STDMETHOD(Activate)();
	STDMETHOD_(BOOL, CanBePooled)();
	STDMETHOD_(void, Deactivate)();

	CComPtr<IObjectContext> m_spObjectContext;

	STDMETHOD(Construct)(IDispatch * pCtorObj)
	{
		CComPtr<IObjectConstructString> spObjectConstructString;
		HRESULT hr = pCtorObj->QueryInterface(&spObjectConstructString);
		if (SUCCEEDED(hr))
		{
			CComBSTR bstrConstruct;
			hr = spObjectConstructString->get_ConstructString(&bstrConstruct);
			if (SUCCEEDED(hr))
			{
				// TODO : Use the string to construct the object.
			}
		}
		return hr;
	}

//protected data members
protected:
	// stores list of sockets
	map<string, SOCKET, cmp_str> m_SocketMap;
	// iterator for the map<string, SOCKET>
	map<string, SOCKET, cmp_str>::iterator m_sockIt;
	// this stores a boolean value which is false because of failure to insert the item into the map
	pair<map<string, SOCKET, cmp_str>::iterator, bool> m_retIt;

//Real Time Data MAP
protected:
	//stores list of Real time data packets
	map<int, vector<char> >		m_map_RTData;	
	//iterator for the map<int, vector<char> >
	map<int, vector<char> >::iterator m_RTDIt;
	//this stores a boolean value which is false because of failure to insert the item into the map
	pair<map<int, vector<char> >::iterator, bool> m_retRTDIt;

//Configuration Frame2
protected:
	//stores list of Configuration frame2
	map<int, vector<char> >		m_map_CFG2Frame;
	//iterator for the map map<int, vector<char> >
	map<int, vector<char> >::iterator m_CFG2It;
	//this stores a boolean value which is false because of failure to insert the item into the map
	pair<map<int, vector<char> >::iterator, bool> m_retCFG2It;
//map list that maps PMU to number of phasors.
	map<int, int>	m_map_PMU2PHNMR;
	map<int, int>::iterator m_PMU2PHNMRIt;
	pair<map<int,int>::iterator, bool>	m_retPMU2PHNMR;
//map list that maps PMU to number of Analog.
	map<int, int>	m_map_PMU2ANNMR;
	map<int, int>::iterator m_PMU2ANNMRIt;
	pair<map<int,int>::iterator, bool>	m_retPMU2ANNMR;
//map list that maps PMU to number of Digital signals.
	map<int, int>	m_map_PMU2DGNMR;
	map<int, int>::iterator m_PMU2DGNMRIt;
	pair<map<int,int>::iterator, bool>	m_retPMU2DGNMR;
//map list that maps phasor to PMU number and PHUNIT
	map<int, vector<int> >	m_map_PH2PHUNIT;
	map<int, vector<int> >::iterator m_PH2PHUNITIt;
	pair<map<int, vector<int> >::iterator, bool>	m_retPH2PHUNIT;
//map list that maps analog signal to ANUNIT
	map<int, vector<int> >	m_map_AN2ANUNIT;
	map<int, vector<int> >::iterator m_AN2ANUNITIt;
	pair<map<int, vector<int> >::iterator, bool>	m_retAN2ANUNIT;
//list of places of the phasors in the real-time data
	//list for decod of phasor values
	map<ULONG, vector<ULONG> >	m_map_phdevals;
	map<ULONG, vector<ULONG> >::iterator mphdeIt;
	pair<map<ULONG, vector<ULONG> >::iterator, bool>	m_retphde;

//list for decod of Analog & Digital values
	map<ULONG, vector<ULONG> >	m_map_Anadevals;
	map<ULONG, vector<ULONG> >::iterator m_AnadeIt;
	pair<map<ULONG, vector<ULONG> >::iterator, bool>	m_retAnade;
//configuration frame2 vector
	vector<char>	vec;
	vector<char>	vec1;
	vector<ULONG>		vecphdec;
	vector<ULONG>		vecADdec;

// ITCPCMU1
public:
	STDMETHOD(ConnectTo)(DWORD u32_ServIP, USHORT u16_Port, DWORD u32_MaxTimeout, DWORD u32_MaxIdeTime);
	STDMETHOD(SendTo)(DWORD hSocket, CHAR* s8_SendBuf, DWORD u32_Len);
	STDMETHOD(DisconnectClient)();
protected:
	DWORD Initialize1(void);
	DWORD Close(void);
	DWORD CreateSocket(void);
protected:
	DWORD ConnectTo1(DWORD u32_ServIP, USHORT u16_Port, DWORD u32_EventTimeout, DWORD u32_MaxIdleTime);
	DWORD SendTo1(SOCKET hSocket, char * s8_SendBuf, DWORD u32_Len);
	DWORD DisconnectClient1(SOCKET hSocket);
	DWORD ProcessEvents1(HWND hwnd, unsigned int msg, DWORD * pu32_Events, DWORD * pu32_IP, SOCKET * ph_Socket, char ** pps8_RecvBuf, DWORD * pu32_Read, DWORD * pu32_Sent);
	DWORD ProcessEvents2(HWND hwnd, unsigned int msg, HANDLE hEventRT);
public:
	STDMETHOD(ProcessEvents)(HWND hwnd, unsigned int msg);
	STDMETHOD(Initialize)(void);
	STDMETHOD(get_blobData)(BSTR* pVal);
	STDMETHOD(put_blobData)(BSTR newVal);
protected:
	CComBSTR m_bstrBlobData;
	char* m_s8_ReadBuffer;
	// IMalloc allocator
	LPMALLOC m_pMalloc;
public:
	STDMETHOD(get_PDCBinData)(BSTR* pVal);
	STDMETHOD(put_PDCBinData)(BSTR newVal);
private:
	CComBSTR m_pPDCBinData;
public:
	STDMETHOD(HeaderFrame)(void);
	STDMETHOD(Disable)(void);
	STDMETHOD(get_SocketName)(BSTR* pVal);
	STDMETHOD(put_SocketName)(BSTR newVal);
private:
	// stores name of the socket
	BSTR m_bstrSockName;
public:
	STDMETHOD(FindSocket)(BSTR SockName);
private:
	// stores a given found socket
	SOCKET m_foundSocket;
	// Creates different types of messages according to the key
	char* ConstructMsg(BSTR bstrKey);
	// constructs connection, enable real-time data transmission message
	char * constructCntMsg(void);
	// constructs the message for Header frame retrieval 
	char * constructHfMsg(void);
	// constructs the message sent to PDC Server to retrieve Configuration frame1
	char * constructCnfg1fMsg(void);
	// constructs message to retrieve  configuration frame2
	char * constructCnfg2fMsg(void);
	char * ConstructRTMsg(void);
	char * ConstructDisableMsg(void);
/*
public:
	STDMETHOD(get_SYNC)(USHORT* pVal);
	STDMETHOD(put_SYNC)(USHORT newVal);
*/
private:
	// stores the SYNC byte of a command
	USHORT m_bySYNC;
public:
	STDMETHOD(get_SYNC1)(unsigned char* pVal);
	STDMETHOD(put_SYNC1)(unsigned char newVal);
	STDMETHOD(get_SYNC2)(unsigned char* pVal);
	STDMETHOD(put_SYNC2)(unsigned char newVal);
private:
	// stores the first byte of the SYNC 
	unsigned char m_chSYNC1;
	// stores second byte of SYNC
	unsigned char m_chSYNC2;
	// if true means a configuration frame2 is read 
	bool m_bCFG2;
protected:
	// puts binary data vector into this class
	DWORD BIN2ASCII(void);
	//converts the Real Time binary data to ASCII
	DWORD BIN2ASCII_RT(void);
	// puts the binary data into this class
	DWORD PutBinData(vector<char> * pVecBinData);
private:
	// Reference to the aggregated object
	LPUNKNOWN m_pInnerUnk;
private:
	// stores IDCode of the PDC Server
	USHORT m_u16IDCode;
public:
	STDMETHOD(get_IDCode)(USHORT* pVal);
	STDMETHOD(put_IDCode)(USHORT newVal);
	STDMETHOD(ProcessEventsRT)(HWND hwnd, unsigned int msg, HANDLE hEventRT);
private:
	// stores Real Time Data decoder object, IRTDTParams
	CComPtr<IRTDTParams> m_pRTDTParams;
	// stores Configuration Frame2 decoder object, ICFG2Params
	CComPtr<ICFG2Params> pCFG2Params;
public:
	// store the external pointer to the aggregated object
	LPUNKNOWN m_pInnerUnk1;
private:
	// skips certain steps of program to perform enable real-time data
	USHORT m_bSkip;
	HANDLE m_hEventRT;
public:
	STDMETHOD(get_SkipSteps4RT)(USHORT* pVal);
	STDMETHOD(put_SkipSteps4RT)(USHORT newVal);
	STDMETHOD(ContinuePacketReading)(void);
	STDMETHOD(get_PhMagAng)(ULONG* pVal);
	STDMETHOD(put_PhMagAng)(ULONG newVal);
	STDMETHOD(get_PhNumber)(ULONG* pVal);
	STDMETHOD(put_PhNumber)(ULONG newVal);
	STDMETHOD(InsertPhPosType)(void);
	STDMETHOD(get_PMUCount)(ULONG* pVal);
	STDMETHOD(put_PMUCount)(ULONG newVal);
private:
	// stores the number of decod vectors
	int m_cnt;
public:
	STDMETHOD(ClearDecodeVector)(void);
private:
	// check whether the packet is valid
	DWORD CRC_Check(long lPktSize);
	float IEEE_754_to_float(char * raw, int pos);
	int asc2bin ( char **index );// points to pointer to MSB ASCII digit
	int asc1bin(char **index);          /* points to pointer to ASCII digit */
public:
	STDMETHOD(get_AnalogNumber)(ULONG* pVal);
	STDMETHOD(put_AnalogNumber)(ULONG newVal);
	STDMETHOD(get_DigitalNumber)(ULONG* pVal);
	STDMETHOD(put_DigitalNumber)(ULONG newVal);
	STDMETHOD(InsertAnaPosType)(void);
	STDMETHOD(get_ADPMUCount)(ULONG* pVal);
	STDMETHOD(put_ADPMUCount)(ULONG newVal);
	STDMETHOD(FindNoofchAD)(ULONG* pu32_NoofchAD);
	STDMETHOD(get_LogFilNm)(BSTR* pVal);
	STDMETHOD(put_LogFilNm)(BSTR newVal);
private:
	// stores log file name
	BSTR m_bstrLogFil;
	FILE * f;
	string m_strWarning;
	// a flag stores the status of the connection
	BOOL m_bConnected;
public:
	STDMETHOD(get_PhDecSize)(ULONG* pVal);
	STDMETHOD(put_PhDecSize)(ULONG newVal);
	STDMETHOD(get_NoofChAnalog)(ULONG* pVal);
	STDMETHOD(put_NoofChAnalog)(ULONG newVal);
private:
	// stores number of analog signals
	ULONG m_u32_NoofAnalogs;
public:
	STDMETHOD(get_NoofChDigital)(ULONG* pVal);
	STDMETHOD(put_NoofChDigital)(ULONG newVal);
private:
	// stores number of digital signals
	ULONG m_u32_NoofDigitals;
public:
	STDMETHOD(get_NumAnalogs)(LONG* pVal);
	STDMETHOD(put_NumAnalogs)(LONG newVal);
private:
	// stores number of analog signals
	unsigned long m_u32_TotAnalogs;
public:
	STDMETHOD(get_NumDigitals)(ULONG* pVal);
	STDMETHOD(put_NumDigitals)(ULONG newVal);
private:
	// stores the number of digital signals
	unsigned long m_u32_TotDigitals;
public:
	STDMETHOD(FindTotalNoofPhasors)(ULONG* pu32_PHRCount);
	STDMETHOD(AddAnalogCh)(void);
	STDMETHOD(AddDigitalNumber)(void);
	STDMETHOD(get_RefreshRate)(ULONG* pVal);
	STDMETHOD(put_RefreshRate)(ULONG newVal);
private:
	// stores the refresh rate 
	ULONG m_u32_RefreshRate;
	// stores the first time stamp to be compared with second time stamp. the difference must as much as (1/Refresh Rate).
	time_t m_ts1;
	// stores the second timestamp. difference between the m_ts1 and m_ts2 is equal to (1/Refresh Rate).
	time_t m_ts2;
	// calculates difference between two time stamps
	DWORD TSDiff(time_t ts1, time_t ts2);
	// stores the first FRACSEC
	ULONG m_tsfirst;
	// stores the second FRACSEC
	ULONG m_tsSecond;
};

OBJECT_ENTRY_AUTO(__uuidof(TCPCMU1), CTCPCMU1)
