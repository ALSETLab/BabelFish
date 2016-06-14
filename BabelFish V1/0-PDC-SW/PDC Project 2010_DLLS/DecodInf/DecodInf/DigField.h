// DigField.h : Declaration of the CDigField

#pragma once
#include "DecodInf_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>
#include <mtxattr.h>
#include <vector>
#include <map>

using namespace ATL;

using namespace std;

// CDigField

class ATL_NO_VTABLE CDigField :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IObjectControl,
	public CComCoClass<CDigField, &CLSID_DigField>,
	public IDispatchImpl<IDigField, &IID_IDigField, &LIBID_DecodInfLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CDigField()
		: m_curDigItem(0)
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_vecDigFld.begin();

		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DIGFIELD)

BEGIN_COM_MAP(CDigField)
	COM_INTERFACE_ENTRY(IDigField)
	COM_INTERFACE_ENTRY(IObjectControl)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* const arr[] =
		{
			&IID_IDigField
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


// IDigField
public:
	STDMETHOD(get_DigFld)(CHAR* pVal);
	STDMETHOD(put_DigFld)(CHAR newVal);
private:
	// stores digital field of C37118 data packet frame
	vector<char> m_vecDigFld;
	// stores the current byte of the 2 byte digital signal of the c37118 data packet
	char m_curDigItem;
public:
	STDMETHOD(SaveDigHexVals)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(DigField), CDigField)
