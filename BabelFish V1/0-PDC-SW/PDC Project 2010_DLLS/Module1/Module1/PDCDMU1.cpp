// PDCDMU1.cpp : Implementation of CPDCDMU1

#include "stdafx.h"
#include "PDCDMU1.h"
#include <comdef.h>
#include <assert.h>


// CPDCDMU1

HRESULT CPDCDMU1::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;
	return hr;
} 

BOOL CPDCDMU1::CanBePooled()
{
	return FALSE;
} 

void CPDCDMU1::Deactivate()
{
	m_spObjectContext.Release();
} 


STDMETHODIMP CPDCDMU1::InsertPHMAN(USHORT U16_PMUIndex, BSTR bstrPHMAN)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPHMAN);
	map<int, vector<char> >::iterator it = m_map_Sel_Phasor_Names.end();

	//check the phasor name
	assert(_bstr.length());

	vector<char> vec;
	vec.resize(_bstr.length(), 0);

	//init the vector with phasor name
	if(!::memcpy(&vec[0], (LPCSTR)_bstr, _bstr.length()))
		return S_FALSE;

	//insert the phasor in the map
	m_retSelPhNmsIT = m_map_Sel_Phasor_Names.insert(pair<int, vector<char> >(m_map_Sel_Phasor_Names.size()+1, vec));
	if(m_retSelPhNmsIT.second == false)
		return S_FALSE;

	return S_OK;
}