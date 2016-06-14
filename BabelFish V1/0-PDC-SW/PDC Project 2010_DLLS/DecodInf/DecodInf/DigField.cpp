// DigField.cpp : Implementation of CDigField

#include "stdafx.h"
#include "DigField.h"


// CDigField

HRESULT CDigField::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;
	return hr;
} 

BOOL CDigField::CanBePooled()
{
	return FALSE;
} 

void CDigField::Deactivate()
{
	m_spObjectContext.Release();
} 


STDMETHODIMP CDigField::get_DigFld(CHAR* pVal)
{
	// TODO: Add your implementation code here

	m_curDigItem = m_vecDigFld.size();
	if( !m_curDigItem )
		return S_FALSE;

	vector<char>::iterator it = m_vecDigFld.begin();
	if( it != m_vecDigFld.end() )
	{
		*pVal = *it;
		m_vecDigFld.erase(m_vecDigFld.begin());
	}
	else
	{
		AtlTrace("ZERO BYTE DIGITAL SIGNAL REMAINED.\n");
		return S_FALSE;
	}

	return S_OK;
}


STDMETHODIMP CDigField::put_DigFld(CHAR newVal)
{
	// TODO: Add your implementation code here
	m_vecDigFld.push_back((unsigned char)newVal);

	return S_OK;
}


STDMETHODIMP CDigField::SaveDigHexVals(void)
{
	// TODO: Add your implementation code here

	return S_OK;
}
