// PDCDMU1.h : Declaration of the CPDCDMU1

#pragma once
#include "Module1_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>
#include <map>
#include <vector>

using namespace ATL;

using namespace std;

// CPDCDMU1

class ATL_NO_VTABLE CPDCDMU1 :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public IObjectConstruct,
	public CComCoClass<CPDCDMU1, &CLSID_PDCDMU1>,
	public IDispatchImpl<IPDCDMU1, &IID_IPDCDMU1, &LIBID_Module1Lib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPDCDMU1()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PDCDMU1)

BEGIN_COM_MAP(CPDCDMU1)
	COM_INTERFACE_ENTRY(IPDCDMU1)
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

protected:
//selected phasors map
	map<int, vector<char> > m_map_Sel_Phasor_Names;
	//iterator for the m_map_Sel_Phasor_Names
	map<int, vector<char> >::iterator  m_SelPhNmsIT;
	//this stores a boolean value which is false because of failure to insert the item into the map
	pair<map<int, vector<char> >::iterator, bool> m_retSelPhNmsIT;
// IPDCDMU1
public:
	STDMETHOD(InsertPHMAN)(USHORT U16_PMUIndex, BSTR bstrPHMAN);
};

OBJECT_ENTRY_AUTO(__uuidof(PDCDMU1), CPDCDMU1)
