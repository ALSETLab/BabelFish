// RTDTParams.h : Declaration of the CRTDTParams

#pragma once
#include "DecodInf_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>
#include <map>
#include <vector>
#include "atlcomcli.h"

using namespace ATL;

using namespace std;

// CRTDTParams

class ATL_NO_VTABLE CRTDTParams :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public IObjectConstruct,
	public CComCoClass<CRTDTParams, &CLSID_RTDTParams>,
	public IDispatchImpl<IRTDTParams, &IID_IRTDTParams, &LIBID_DecodInfLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CRTDTParams()
		: m_u_32_year(0)
		, m_u_32_Month(0)
		, m_u_32_Day(0)
		, m_u_32_Hour(0)
		, m_u_32_Minute(0)
		, m_u_32_second(0)
		, m_curIndex(0)
		, m_tsVec(0)
		, m_dVar(0)
		, m_tsMillisecond(0)
		, m_flXVal(0)
		, m_flYVal(0)
		, m_flMagAng(0)
		, m_curItem(0)
		, m_flFREQ(0)
		, m_flDFREQ(0)
		, m_flANG(0)
		, m_flDIG(0)
		, m_flAnalog(0)
		, m_flDigital(0)
		, f(NULL)
		, m_nPhVals(0)
		, m_nFDQVals(0)
		, m_ncurPh(0)
		, m_pPhVals(NULL)
		, m_ncurFDQ(0)
		, m_pFDQVals(NULL)
		, m_pANVals(NULL)
		, m_nANVals(0)
		, m_ncurAN(0)
		, m_ncurDig(0)
		, m_nDigVals(0)
		, m_nDigHex(0)
		, m_ncurDigHex(0)
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		szMode = "a";
		m_szFName =  ".\\log1.txt";
		f = ::fopen(m_szFName.c_str(), szMode.c_str());
		//open the file with write permission
		if( !f )
			return -1;
		else
			AtlTrace("Success: log1 file is opened.\n");

		m_tsVec.resize(1,0);

		//phasor value vector
//		m_vecPhVals.resize(3, 0.0);
//		m_map_RtPhVals.resize(100);

		m_RTDTIt = m_map_RTDT_TS.begin();

//		m_RtPhValsIt = m_map_RtPhVals.begin();

		//create a aggregate object of the IDigField
		HRESULT hr = ::CoCreateInstance(
										CLSID_DigField, 
										NULL,
										CLSCTX_INPROC,
										IID_IDigField,
										(void**)&m_punkDig
										);
		if(FAILED(hr))
			return S_FALSE;

		//get the IDigField object
		if( FAILED( hr = m_punkDig->QueryInterface(IID_IDigField, (void**)&m_pDigFld)))
			return S_FALSE;

		CoInitialize(NULL);

		if(FAILED(hr = ::CoGetMalloc(1, (LPMALLOC*)&m_pMalloc)))
			return S_FALSE;
		else
			AtlTrace("Success: IMalloc is created in CFG2Params.h file.\n");

		//allocation of memory for phasors
		m_map_ppPhVals = (float**)m_pMalloc->Alloc( sizeof(float*) );
		if( !m_map_ppPhVals )
			return S_FALSE;
		else
		{
			for(int i=0; i < 100; ++i)
			{
				m_map_ppPhVals[i] = (float*)m_pMalloc->Alloc( sizeof(float) );
				if( !(m_map_ppPhVals[i]) )
					return S_FALSE;
			}
		}

		//allocation of memory for map of FREQ, DFREQ
		m_map_ppFDQVals = (float**)m_pMalloc->Alloc( sizeof(float*) );
		if( !m_map_ppFDQVals )
			return S_FALSE;
		else
		{
			for(int i=0; i < 100; ++i)
			{
				m_map_ppFDQVals[i] = (float*)m_pMalloc->Alloc( sizeof(float) );
				if( !(m_map_ppFDQVals[i]) )
					return S_FALSE;
			}
		}

		//allocation of memory for map of Analog signals
		m_map_ppANVals = (float**)m_pMalloc->Alloc( sizeof(float*) );
		if( !m_map_ppANVals )
			return S_FALSE;
		else
		{
			for(int i=0; i < 100; ++i)
			{
				m_map_ppANVals[i] = (float*)m_pMalloc->Alloc( sizeof(float) );
				if( !(m_map_ppANVals[i]) )
					return S_FALSE;
			}
		}


		//allocation of memory for map of Digital signals
		m_map_ppDigVals = (float**)m_pMalloc->Alloc( sizeof(float*) );
		if( !m_map_ppDigVals )
			return S_FALSE;
		else
		{
			for(int i=0; i < 100; ++i)
			{
				m_map_ppDigVals[i] = (float*)m_pMalloc->Alloc( sizeof(float) );
				if( !(m_map_ppDigVals[i]) )
					return S_FALSE;
			}
		}

		//allocation of memory for map of Digital signals
		m_map_ppDigHex = (char**)m_pMalloc->Alloc( 100 * sizeof(char*) );
		if( !m_map_ppDigHex )
			return S_FALSE;
		else
		{
/*			for(int i=0; i < 100; ++i)
			{
				m_map_ppDigHex[i] = (char*)m_pMalloc->Alloc( sizeof(char) );
				if( !(m_map_ppDigHex[i]) )
					return S_FALSE;
			}*/
		}

		return S_OK;
	}

	void FinalRelease()
	{
		//Release resources used by the critical section object.
		m_pMalloc->Free(m_pPhVals);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_RTDTPARAMS)

BEGIN_COM_MAP(CRTDTParams)
	COM_INTERFACE_ENTRY(IRTDTParams)
	COM_INTERFACE_ENTRY(IObjectControl)
	COM_INTERFACE_ENTRY(IObjectConstruct)
	COM_INTERFACE_ENTRY(IDispatch)
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

// IRTDTParams
public:
	STDMETHOD(get_TS_Year)(ULONG* pVal);
	STDMETHOD(put_TS_Year)(ULONG newVal);
	STDMETHOD(get_TS_Month)(ULONG* pVal);
	STDMETHOD(put_TS_Month)(ULONG newVal);
	STDMETHOD(get_TS_Day)(ULONG* pVal);
	STDMETHOD(put_TS_Day)(ULONG newVal);
	STDMETHOD(get_TS_Hour)(ULONG* pVal);
	STDMETHOD(put_TS_Hour)(ULONG newVal);
	STDMETHOD(get_TS_Minute)(ULONG* pVal);
	STDMETHOD(put_TS_Minute)(ULONG newVal);
	STDMETHOD(get_TS_Second)(ULONG* pVal);
	STDMETHOD(put_TS_Second)(ULONG newVal);
private:
	// stores time stamp year
	int m_u_32_year;
	// stores timestamp month
	int m_u_32_Month;
	// stores timestamp for day of the year
	int m_u_32_Day;
	// stores the hour of the timestamp
	int m_u_32_Hour;
	// stores the minute of the timestamp
	int m_u_32_Minute;
	// stores the second of the timestamp
	int m_u_32_second;

//map to save the time stamps
	map<int, vector<int> >	m_map_RTDT_TS;
	map<int, vector<int> >::iterator	m_RTDTIt;
	pair<map<int, vector<int> >::iterator, bool>	m_retRTDT;
//map to save the phasor values X + iY, X & Y are saved in a vector of float or double
	//this vector will be inserted in a map<int, vector<int> >, thus we can save values 
	//of the phasors in the chosen data selected by user
	vector<float>	m_vecPhVals;
//	vector<vector<float> > m_map_RtPhVals;
//	vector<vector<float> >::iterator m_RtPhValsIt;
//	map<ULONG, vector<float> >	m_map_RtPhVals;
//	map<ULONG, vector<float> >::iterator	m_RtPhValsIt;
	pair<map<ULONG, vector<float> >::iterator, bool>  m_retRtPhVals;
	float * m_pPhVals;
	float ** m_map_ppPhVals;
	//This vector will be inserted in a map<int, vector<int> >, thus we can save values 
	//of the phasors in the chosen data selected by user
	float * m_pFDQVals;
	float ** m_map_ppFDQVals;
	map<int, vector<float> >	m_map_RtFDQVals;
//	map<int, vector<float> >::iterator	m_RtFDQValsIt;
	pair<map<int, vector<float> >::iterator, bool>  m_retRtFDQVals;
	//This vector will be inserted in a map<int, vector<int> >, thus we can save values 
	//of the analog signals in the chosen data selected by user
//	map<int, vector<float> >	m_map_RtANVals;
//	map<int, vector<float> >::iterator	m_RtANValsIt;
	// stores the analog value as a pointer to be inserted in the analog signals map
	float *m_pANVals;
	float ** m_map_ppANVals;
	pair<map<int, vector<float> >::iterator, bool>  m_retRtANVals;
	//This vector will be inserted in a map<int, vector<int> >, thus we can save values 
	//of the digital signals in the chosen data selected by user
//	map<int, vector<float> >	m_map_RtDigVals;
//	map<int, vector<float> >::iterator	m_RtDigValsIt;
	float *m_pDigVals;
	float ** m_map_ppDigVals;
	pair<map<int, vector<float> >::iterator, bool>  m_retRtDigVals;
	//This vector will be inserted in a map<int, vector<int> >, thus we can save values 
	//of the digital signals in the chosen data selected by user
	char * m_pDigHex;
	char ** m_map_ppDigHex;
//	map<int, string >	m_map_RtDigStr;
//	map<int, string >::iterator	m_RtDigStrIt;
	pair<map<int, string >::iterator, bool>  m_retRtDigStr;

	// stores an index to the current time stamp of real time data
	int m_curIndex;
	// stores the components of the COleDateTime
	vector<int> m_tsVec;
public:
	STDMETHOD(get_TS_OLE_VAR)(DOUBLE* pVal);
	STDMETHOD(put_TS_OLE_VAR)(DOUBLE newVal);
private:
	// stores VT_DATE var.date time
	double m_dVar;
public:
	STDMETHOD(get_Millisecond)(ULONG* pVal);
	STDMETHOD(put_Millisecond)(ULONG newVal);
private:
	// stores the milliseconds
	unsigned long m_tsMillisecond;
	// critical section to protect the real time data values when BIN2ASCII_RT and UpdateRTData write and read from it within two threads
	CRITICAL_SECTION m_csCriticalSection;
public:
	STDMETHOD(SavePhVals)(FLOAT XVal, FLOAT YVal, FLOAT flDtType);
	STDMETHOD(get_PhasorXVal)(FLOAT* pVal);
	STDMETHOD(put_PhasorXVal)(FLOAT newVal);
private:
	// Stores X Value of a given phasor
	float m_flXVal;
public:
	STDMETHOD(get_PhasorYVal)(FLOAT* pVal);
	STDMETHOD(put_PhasorYVal)(FLOAT newVal);
private:
	// stores Y value of the phasor
	float m_flYVal;
	// stores type of phasor Magnitude/Angle
	float m_flMagAng;
public:
	STDMETHOD(get_MagAng)(FLOAT* pVal);
	STDMETHOD(put_MagAng)(FLOAT newVal);
	STDMETHOD(LoadPhVals)(FLOAT* pPhXVal, FLOAT* pPhYVal, FLOAT* pPhType);
private:
	// stores the current item in the vector
	int m_curItem;
public:
	STDMETHOD(CleanPhasors)(void);
	STDMETHOD(ChPhListBegin)(void);
	STDMETHOD(get_FREQ)(FLOAT* pVal);
	STDMETHOD(put_FREQ)(FLOAT newVal);
	STDMETHOD(get_DFREQ)(FLOAT* pVal);
	STDMETHOD(put_DFREQ)(FLOAT newVal);
private:
	// stores the FREQ value
	float m_flFREQ;
	// stores DFREQ value
	float m_flDFREQ;
	// stores analog PMU analog data
	float m_flANG;
	// stores digital data
	float m_flDIG;
public:
	STDMETHOD(SaveFDQANDGVals)(void);
	STDMETHOD(LoadFDQANDGVals)(FLOAT* pflFREQ, FLOAT* pflDFREQ, FLOAT* pflDIG, FLOAT* pflANG);
	STDMETHOD(get_Analog)(FLOAT* pVal);
	STDMETHOD(put_Analog)(FLOAT newVal);
private:
	// stores the analog signal
	float m_flAnalog;
	// stores digital value
	float m_flDigital;
public:
	STDMETHOD(get_Digital)(FLOAT* pVal);
	STDMETHOD(put_Digital)(FLOAT newVal);
	STDMETHOD(SaveADVals)(void);
	STDMETHOD(LoadANVals)(FLOAT* pflAnalogVal/*, FLOAT* pflDigitalVal*/);
	STDMETHOD(SaveDigVals)(void);
	STDMETHOD(LoadDigVals)(FLOAT* pflDigitalVal);
	STDMETHOD(DigHex2BinStr)(void);
private:
	// stores the real-time digital signal value as series of 0,1s in a string
	BSTR m_bstrRTDigVal;
	// stores the aggregate LPUNKNOWN object
	LPUNKNOWN m_punkDig;
	// stores the IDigField INTERFACE
	CComPtr<IDigField> m_pDigFld;
public:
	STDMETHOD(get_HexAnalog)(CHAR* pVal);
	STDMETHOD(put_HexAnalog)(CHAR newVal);
	STDMETHOD(get_DigBinString)(BSTR* pVal);
	STDMETHOD(put_DigBinString)(BSTR newVal);
	STDMETHOD(SaveDigHexVals)(void);
private:
	// stores the string value of the digital hex value of C37.118 message
	string m_strDigFld;
	// stores the IMalloc object
	LPMALLOC m_pMalloc;
public:
	STDMETHOD(LoadRtDigStrs)(BSTR* bstrDigBinVal);
private:
	// string to write to log file for testing & debugging purposes
	string m_strWarning;
	FILE * f;
	string szMode;
	string m_szFName;
public:
	STDMETHOD(CleanFDQ)(void);
	STDMETHOD(CleanANParams)(void);
	STDMETHOD(CleanDGParams)(void);
	STDMETHOD(CleanDigStr)(void);
private:
	// stores number of phasor values in the relavant map
	ULONG m_nPhVals;
	// stores number of FREQ, DFREQ, and so forth
	ULONG m_nFDQVals;
	// points to the current phasor
	ULONG m_ncurPh;
	// stores the current FREQ, DFREQ is being saved or loaded to present in the labview
	ULONG m_ncurFDQ;
	// number of analog signals selected and to be saved in the map of analog signals
	ULONG m_nANVals;
	// stores current analog signal loaded
	ULONG m_ncurAN;
	// stores the current digital signal which is being loaded to present in labview
	ULONG m_ncurDig;
	// stores number of digital signals in the chosen phasors listbox
	ULONG m_nDigVals;
	// stores the number of digital signals in the chosen phasors listbox entry of the digital signal string map entry
	ULONG m_nDigHex;
	// // stores the current digital signal string which is being loaded to present in labview
	ULONG m_ncurDigHex;
};

OBJECT_ENTRY_AUTO(__uuidof(RTDTParams), CRTDTParams)
