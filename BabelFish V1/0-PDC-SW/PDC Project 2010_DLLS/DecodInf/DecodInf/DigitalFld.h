// DigitalFld.h : Declaration of the CDigitalFld

#pragma once
#include "DecodInf_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>

using namespace ATL;



// CDigitalFld

class ATL_NO_VTABLE CDigitalFld :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public CComCoClass<CDigitalFld, &CLSID_DigitalFld>,
	public IDispatchImpl<IDigitalFld, &IID_IDigitalFld, &LIBID_DecodInfLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CDigitalFld()
		: m_curDigItem(0)
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

DECLARE_REGISTRY_RESOURCEID(IDR_DIGITALFLD1)

BEGIN_COM_MAP(CDigitalFld)
	COM_INTERFACE_ENTRY(IDigitalFld)
	COM_INTERFACE_ENTRY(IObjectControl)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_IDigitalFld
		};
		for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}



// IObjectControl
public:
	STDMETHOD(Activate)();
	STDMETHOD_(BOOL, CanBePooled)();
	STDMETHOD_(void, Deactivate)();

	CComPtr<IObjectContext> m_spObjectContext;


// IDigitalFld
public:
private:
	// stores current byte of the digital signal
	char m_curDigItem;
};

OBJECT_ENTRY_AUTO(__uuidof(DigitalFld), CDigitalFld)
