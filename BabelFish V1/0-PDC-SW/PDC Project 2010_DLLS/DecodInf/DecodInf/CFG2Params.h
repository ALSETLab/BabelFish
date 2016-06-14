// CFG2Params.h : Declaration of the CCFG2Params

#pragma once
#include "DecodInf_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>
#include <vector>
#include <map>

using namespace std;

using namespace ATL;

// CTCPCom1
struct cmp_str
{
	int operator() (string a, string b)
	{
		return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
	}
};

// CCFG2Params

class ATL_NO_VTABLE CCFG2Params :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public IObjectConstruct,
	public CComCoClass<CCFG2Params, &CLSID_CFG2Params>,
	public IDispatchImpl<ICFG2Params, &IID_ICFG2Params, &LIBID_DecodInfLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CCFG2Params()
		: m_curItem(0)
		, m_u32NoofPMUs(0)
		, m_nAnalogs(0)
		, m_nDigitals(0)
		, m_curAngItem(0)
		, m_curDigItem(0)
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		HRESULT hr;

		CoInitialize(NULL);

		if(FAILED(hr = ::CoGetMalloc(1, (LPMALLOC*)&m_pMalloc)))
			return S_FALSE;
		else
			AtlTrace("Success: IMalloc is created in CFG2Params.h file.\n");

		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CFG2PARAMS)

BEGIN_COM_MAP(CCFG2Params)
	COM_INTERFACE_ENTRY(ICFG2Params)
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

// ICFG2Params
public:
	STDMETHOD(FirstPhasor)(BSTR* pbstrVal);
	STDMETHOD(LastPhasor)(BSTR* pbstrVal);
	STDMETHOD(NextPhasor)(BSTR* pbstrVal);
private:
	// stores PMU phasors in a map of char vectors
	map<int, vector<char> > m_mapPhasors;
	map<int, vector<char> >::iterator m_curIt;
	pair<map<int, vector<char> >::iterator, bool> m_ret;

	//stores PMU Names in a map of char vectors
	map<int, vector<char> > m_mapPMUs;
	map<int, vector<char> >::iterator m_curpmuIt;
	pair<map<int, vector<char> >::iterator, bool> m_PMUret;

	//stores PMU-to-NoPh map
	map<string, int, cmp_str> m_PMUPHNMap;
	// iterator for the map<string, int>
	map<string, int, cmp_str>::iterator m_PMUPHNIt;
	// this stores a boolean value which is false because of failure to insert the item into the map
	pair<map<string, int, cmp_str>::iterator, bool> m_retIt;

	//stores PMU-to-Noof-Analogs map
	map<string, int, cmp_str> m_map_PMU2ANNMR;
	// iterator for the map<string, int>
	map<string, int, cmp_str>::iterator m_PMU2ANNMRIt;

	//stores PMU-to-Noof-Digitals map
	map<string, int, cmp_str> m_map_PMU2DGNMR;
	// iterator for the map<string, int>
	map<string, int, cmp_str>::iterator m_PMU2DGNMRIt;

	// stores Analogs in a map of char vectors
	map<int, vector<char> > m_map_Analogs;
	map<int, vector<char> >::iterator m_curAngIt;
	pair<map<int, vector<char> >::iterator, bool> m_retAng;

	// stores Digital in a map of char vectors
	map<int, vector<char> > m_map_Digitals;
	map<int, vector<char> >::iterator m_curDigIt;
	pair<map<int, vector<char> >::iterator, bool> m_retDig;

public:
	STDMETHOD(InsertPhasor)(BSTR bstrPhasor);
	STDMETHOD(ResizePhasorAndAppend)(USHORT u16_newSize, BSTR bstr);
private:
	// stores the index of the current item beign processed in the map
	int m_curItem;
	// stores the IMalloc object
	LPMALLOC m_pMalloc;
public:
	STDMETHOD(InsertPMU)(BSTR bstrPMU);
	STDMETHOD(FirstPMU)(BSTR* pbstrVal);
	STDMETHOD(NextPMU)(BSTR* pbstrVal);
private:
	// stores the number of PMUs
	ULONG m_u32NoofPMUs;
public:
	STDMETHOD(get_NoofPMUs)(ULONG* pVal);
	STDMETHOD(put_NoofPMUs)(ULONG newVal);
	STDMETHOD(get_NoofPhasors)(ULONG* pVal);
	STDMETHOD(put_NoofPhasors)(ULONG newVal);
	STDMETHOD(InsertPMUPHN)(BSTR PMUName, SHORT u16_PHN);
	STDMETHOD(Find)(BSTR bstrPMU, USHORT* u16_NoofPHN);
	STDMETHOD(IndexPMU)(ULONG u32_Index, BSTR* pbstrPMU);
	STDMETHOD(FindPMUIndex)(BSTR bstrPMU, ULONG* pu32_Index);
	STDMETHOD(FindPhasorIndex)(BSTR bstrPhasor, ULONG* pu32_PhIndex);
private:
	// stores number of analogs in configuration frame2
	ULONG m_nAnalogs;
	// stores number of digital signals in configuration frame2
	ULONG m_nDigitals;
public:
	STDMETHOD(InsertAnalog)(BSTR bstrAnalog);
private:
	// stores current analog signal item
	unsigned long m_curAngItem;
public:
	STDMETHOD(ResizeAnalogAndAppend)(USHORT u16_newSize, BSTR bstrAnalog);
	STDMETHOD(FirstAnalog)(BSTR* pbstrAng);
	STDMETHOD(NextAnalog)(BSTR* pbstrAng);
	STDMETHOD(InsertPMUANNMR)(BSTR bstrPMU, USHORT u16_ANNMR);
	STDMETHOD(FindNoofAnalogs)(BSTR bstrPMU, USHORT* pu16_ANNMR);
	STDMETHOD(InsertPMUDGNMR)(BSTR bstrPMU, USHORT u16_DGNMR);
	STDMETHOD(FindNoofDigitals)(BSTR bstrPMU, USHORT* pu16_DGNMR);
	STDMETHOD(InsertDigital)(BSTR bstrDigital);
	STDMETHOD(FirstDigital)(BSTR* pbstrDigital);
	STDMETHOD(NextDigital)(BSTR* pbstrDigital);
	STDMETHOD(ResizeDigitalAndAppend)(USHORT u16_newSize, BSTR bstrDigital);
private:
	// stores the position of the digital signal in the digials map
	int m_curDigItem;
public:
	STDMETHOD(FindAnalogIndex)(BSTR bstrAnalog, ULONG* pu32_AnalogIndex);
	STDMETHOD(FindDigitalIndex)(BSTR bstrDigital, ULONG* pu32_DigitalIndex);
	STDMETHOD(get_TotNoofDigitals)(ULONG* pVal);
	STDMETHOD(put_TotNoofDigitals)(ULONG newVal);
	STDMETHOD(RestAnalogList)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(CFG2Params), CCFG2Params)
