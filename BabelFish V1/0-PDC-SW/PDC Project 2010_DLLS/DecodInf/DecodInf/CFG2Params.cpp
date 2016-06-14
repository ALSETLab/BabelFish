// CFG2Params.cpp : Implementation of CCFG2Params

#include "stdafx.h"
#include "CFG2Params.h"
#include <comdef.h>
#include <assert.h>


// CCFG2Params

HRESULT CCFG2Params::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;
	return hr;
} 

BOOL CCFG2Params::CanBePooled()
{
	return FALSE;
} 

void CCFG2Params::Deactivate()
{
	m_spObjectContext.Release();
} 

STDMETHODIMP CCFG2Params::FirstPhasor(BSTR* pbstrVal)
{
	// TODO: Add your implementation code here
	map<int, vector<char> >::iterator it;
	it = m_mapPhasors.begin();
	m_curIt = it;
	int n = ((*it).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));

	//copy the phasor name
	for(int i = 0; i < n; i++){
			vec[i] = ((*m_curIt).second).at(i);
	}

	_bstr_t _bstr(vec);
	//pass the phasor name to outside this function
	*pbstrVal = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::LastPhasor(BSTR* pbstrVal)
{
	// TODO: Add your implementation code here
	map<int, vector<char> >::iterator it;
	it = m_mapPhasors.end();
	int n = ((*it).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));

	//copy the phasor name
	for(int i = 0; i < n; i++){
			vec[i] = ((*it).second).at(i);
	}

	_bstr_t _bstr(vec);
	//pass the phasor name to outside this function
	*pbstrVal = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::NextPhasor(BSTR* pbstrVal)
{
	// TODO: Add your implementation code here

	//check if any more phasor name is left to return
	map<int, vector<char> >::iterator it;
	it = m_mapPhasors.end();
	if( m_curIt != it )	
		m_curIt++;	//still there are phasor names to return
	else	//no more phasor name to return
		return S_FALSE;

	int n = ((*m_curIt).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));


	//copy the phasor name
	for(int i = 0; i < n; i++){
		vec[i] = ((*m_curIt).second).at(i);
	}

	//pass the phasor name to outside this function
	_bstr_t _bstr(vec);
	*pbstrVal = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::InsertPhasor(BSTR bstrPhasor)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPhasor);
	
	//check the phasor name
	assert(_bstr.length());

	vector<char> vec;
	vec.resize(_bstr.length(), 0);
	//init the vector with phasor name
	if(!::memcpy(&vec[0], (LPCSTR)_bstr, _bstr.length()))
		return S_FALSE;
	//insert the phasor in the map
	m_ret = m_mapPhasors.insert(pair<int, vector<char> >(m_curItem, vec));
	if(m_ret.second == false)
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::ResizePhasorAndAppend(USHORT u16_newSize, BSTR bstr)
{
	// TODO: Add your implementation code here
	
	//check
	if(!u16_newSize){
		m_curItem++;
		return S_FALSE;
	}

	CComBSTR comBSTR(bstr);
	int n(0), i(0);
	vector<char> vec;
	map<int, vector<char> >::iterator it;
	it = m_mapPhasors.find(m_curItem);
	n = ((*it).second).size();
	vec.resize(n + u16_newSize, 0);
	do	
	{
		vec[i] = ((*it).second)[i];
	}while( ++i < n );

	for(int j = 0; j < u16_newSize; j++)
		vec[n + j] = comBSTR.m_str[j];
	AtlTrace("\n");
/*	for(int k = 0; k < (n + u16_newSize); k++)
		AtlTrace("%c",vec[k]);*/
	AtlTrace("\n");
	m_mapPhasors.erase(m_curItem);
	m_ret = m_mapPhasors.insert(pair<int, vector<char> >(m_curItem, vec));
	if(m_ret.second == false)
		return S_FALSE;
	it = m_mapPhasors.find(m_curItem);
	for(int k = 0; k < (n + u16_newSize); k++)
		AtlTrace("%c",((*it).second).at(k));
	AtlTrace("\n");
	
	return S_OK;
}



STDMETHODIMP CCFG2Params::InsertPMU(BSTR bstrPMU)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPMU);
	
	//check the PMU name
	assert(_bstr.length());

	vector<char> vec;
	vec.resize(_bstr.length(), 0);
	//init the vector with phasor name
	if(!::memcpy(&vec[0], (LPCSTR)_bstr, _bstr.length()))
		return S_FALSE;
	//insert the phasor in the map
	m_ret = m_mapPMUs.insert(pair<int, vector<char> >(m_curItem, vec));
	if(m_ret.second == false)
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::FirstPMU(BSTR* pbstrVal)
{
	// TODO: Add your implementation code here
	map<int, vector<char> >::iterator it;
	it = m_mapPMUs.begin();
	m_curpmuIt = it;
	int n = ((*it).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));

	//copy the phasor name
	for(int i = 0; i < n; i++){
			vec[i] = ((*m_curpmuIt).second).at(i);
	}

	_bstr_t _bstr(vec);
	//pass the phasor name to outside this function
	*pbstrVal = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::NextPMU(BSTR* pbstrVal)
{
	// TODO: Add your implementation code here

	//check if any more phasor name is left to return
	map<int, vector<char> >::iterator it;
	it = m_mapPMUs.end();
	if( m_curpmuIt != it )	
		m_curpmuIt++;	//still there are phasor names to return
	else	//no more phasor name to return
		return S_FALSE;

	int n = ((*m_curpmuIt).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));

	//copy the phasor name
	for(int i = 0; i < n; i++){
		vec[i] = ((*m_curpmuIt).second).at(i);
	}

	//pass the phasor name to outside this function
	_bstr_t _bstr(vec);
	*pbstrVal = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::get_NoofPMUs(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_mapPMUs.size();

	return S_OK;
}


STDMETHODIMP CCFG2Params::put_NoofPMUs(ULONG newVal)
{
	// TODO: Add your implementation code here

	return E_NOTIMPL;
}


STDMETHODIMP CCFG2Params::get_NoofPhasors(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_mapPhasors.size();

	return S_OK;
}


STDMETHODIMP CCFG2Params::put_NoofPhasors(ULONG newVal)
{
	// TODO: Add your implementation code here

	return E_NOTIMPL;
}


STDMETHODIMP CCFG2Params::InsertPMUPHN(BSTR PMUName, SHORT u16_PHN)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(PMUName);
	string str(_bstr);
	m_retIt = m_PMUPHNMap.insert(pair<string, int>(str, u16_PHN));

	if( m_retIt.second = false )
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::Find(BSTR bstrPMU, USHORT* u16_NoofPHN)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPMU);
	string str(_bstr);

	m_PMUPHNIt = m_PMUPHNMap.find(str);
	if( m_PMUPHNIt == m_PMUPHNMap.end() )
		return S_FALSE;
	else
		*u16_NoofPHN =(* m_PMUPHNIt).second;

	return S_OK;
}


STDMETHODIMP CCFG2Params::IndexPMU(ULONG u32_Index, BSTR* pbstrPMU)
{
	// TODO: Add your implementation code here
	map<int, vector<char> >::iterator it;
	it = m_mapPMUs.find(u32_Index);
	char * buf[128]; ::memset(buf, 0, sizeof(buf));
	return S_OK;
}


STDMETHODIMP CCFG2Params::FindPMUIndex(BSTR bstrPMU, ULONG* pu32_Index)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	BSTR bstr;
	_bstr_t _bPMU(bstrPMU);
	string str, strPMU;
	int pos(0);
	ULONG u32_Index(0);

	//check
	if( !_bPMU.length() )
		return S_FALSE;

	//extract PMU Name
	str = _bPMU;
	pos = str.find('/');
	strPMU = str.substr(0, pos);
	_bPMU = strPMU.c_str();
	//loop through the PMUs
	if(FAILED(hr=FirstPMU(&bstr)))
		return S_FALSE;
	
	if(!strPMU.compare(_bstr_t(bstr))){
//		*pu32_Index = (*m_curpmuIt).first;
			*pu32_Index = u32_Index;
		return S_OK;
	}
	else
	{
		AtlTrace("FindPMUIndex has not found the PMU Index.\n");
		u32_Index++;
	}

	while(hr == S_OK)
	{
		hr = NextPMU(&bstr);
		if(!strPMU.compare(_bstr_t(bstr))){
//			*pu32_Index = (*m_curpmuIt).first;
			*pu32_Index = u32_Index;
			return S_OK;
			break;
		}
		else
		{
			AtlTrace("FindPMUIndex has not found the PMU Index.\n");
			u32_Index++;
		}
	}

	return S_OK;
}


STDMETHODIMP CCFG2Params::FindPhasorIndex(BSTR bstrPhasor, ULONG* pu32_PhIndex)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	BSTR bstr;
	_bstr_t _bPhasor(bstrPhasor);
	string strPhasor, str;
	strPhasor = _bPhasor;
	//check
	if( !_bPhasor.length() )
		return S_FALSE;

	//loop through the PMUs
	if(FAILED(hr=FirstPhasor(&bstr)))
		return S_FALSE;
	str = _bstr_t(bstr);
	if(!strPhasor.compare(str)){
		*pu32_PhIndex = (*m_curIt).first;
		return S_OK;
	}
	else
		AtlTrace("FindPMUIndex has not found the PMU Index.\n");

	while(hr == S_OK)
	{
		hr = NextPhasor(&bstr);
		str = _bstr_t(bstr);

		if(!strPhasor.compare(str)){
			*pu32_PhIndex = (*m_curIt).first;
			return S_OK;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP CCFG2Params::InsertAnalog(BSTR bstrAnalog)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrAnalog);
	
	//check the phasor name
	assert(_bstr.length());

	vector<char> vec;
	vec.resize(_bstr.length(), 0);
	//init the vector with phasor name
	if(!::memcpy(&vec[0], (LPCSTR)_bstr, _bstr.length()))
		return S_FALSE;
	//insert the phasor in the map
	m_ret = m_map_Analogs.insert(pair<int, vector<char> >(m_curAngItem, vec));
	if(m_ret.second == false)
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::ResizeAnalogAndAppend(USHORT u16_newSize, BSTR bstrAnalog)
{
	// TODO: Add your implementation code here
	//check
	if(!u16_newSize){
		m_curAngItem++;
		return S_FALSE;
	}

	CComBSTR comBSTR(bstrAnalog);
	int n(0), i(0);
	vector<char> vec;
	map<int, vector<char> >::iterator it;
	it = m_map_Analogs.find(m_curAngItem);
	n = ((*it).second).size();
	vec.resize(n + u16_newSize, 0);
	do	
	{
		vec[i] = ((*it).second)[i];
	}while( ++i < n );

	for(int j = 0; j < u16_newSize; j++)
		vec[n + j] = comBSTR.m_str[j];
	AtlTrace("\n");
/*	for(int k = 0; k < (n + u16_newSize); k++)
		AtlTrace("%c",vec[k]);*/
	AtlTrace("\n");
	m_map_Analogs.erase(m_curAngItem);
	m_ret = m_map_Analogs.insert(pair<int, vector<char> >(m_curAngItem, vec));
	if(m_ret.second == false)
		return S_FALSE;
	it = m_map_Analogs.find(m_curAngItem);
	for(int k = 0; k < (n + u16_newSize); k++)
		AtlTrace("%c",((*it).second).at(k));
	AtlTrace("\n");

	return S_OK;
}


STDMETHODIMP CCFG2Params::FirstAnalog(BSTR* pbstrAng)
{
	// TODO: Add your implementation code here
	map<int, vector<char> >::iterator it;
	it = m_map_Analogs.begin();
	m_curAngIt = it;
	int n = ((*it).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));

	//copy the phasor name
	for(int i = 0; i < n; i++){
			vec[i] = ((*m_curAngIt).second).at(i);
	}

	_bstr_t _bstr(vec);
	//pass the phasor name to outside this function
	*pbstrAng = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::NextAnalog(BSTR* pbstrAng)
{
	// TODO: Add your implementation code here

	//check if any more phasor name is left to return
	map<int, vector<char> >::iterator it;
	it = m_map_Analogs.end();
//----------------------
	it--;
//----------------------
	if( m_curAngIt != it )	
		m_curAngIt++;	//still there are phasor names to return
	else	//no more phasor name to return
		return S_FALSE;

	int n = ((*m_curAngIt).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));


	//copy the phasor name
	for(int i = 0; i < n; i++){
		vec[i] = ((*m_curAngIt).second).at(i);
	}

	//pass the phasor name to outside this function
	_bstr_t _bstr(vec);
	*pbstrAng = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::InsertPMUANNMR(BSTR bstrPMU, USHORT u16_ANNMR)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPMU);
	string str(_bstr);
	m_retIt = m_map_PMU2ANNMR.insert(pair<string, int>(str, u16_ANNMR));

	if( m_retIt.second = false )
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::FindNoofAnalogs(BSTR bstrPMU, USHORT* pu16_ANNMR)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPMU);
	string str(_bstr);

	m_PMU2ANNMRIt = m_map_PMU2ANNMR.find(str);
	if( m_PMU2ANNMRIt == m_map_PMU2ANNMR.end() )
		return S_FALSE;
	else
		*pu16_ANNMR =(* m_PMU2ANNMRIt).second;

	return S_OK;
}


STDMETHODIMP CCFG2Params::InsertPMUDGNMR(BSTR bstrPMU, USHORT u16_DGNMR)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPMU);
	string str(_bstr);
	m_retIt = m_map_PMU2DGNMR.insert(pair<string, int>(str, u16_DGNMR));

	if( m_retIt.second = false )
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::FindNoofDigitals(BSTR bstrPMU, USHORT* pu16_DGNMR)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrPMU);
	string str(_bstr);

	m_PMU2DGNMRIt = m_map_PMU2DGNMR.find(str);
	if( m_PMU2DGNMRIt == m_map_PMU2DGNMR.end() )
		return S_FALSE;
	else
		*pu16_DGNMR =(* m_PMU2DGNMRIt).second;

	return S_OK;
}


STDMETHODIMP CCFG2Params::InsertDigital(BSTR bstrDigital)
{
	// TODO: Add your implementation code here
	_bstr_t _bstr(bstrDigital);
	
	//check the phasor name
	assert(_bstr.length());

	vector<char> vec;
	vec.resize(_bstr.length(), 0);
	//init the vector with phasor name
	if(!::memcpy(&vec[0], (LPCSTR)_bstr, _bstr.length()))
		return S_FALSE;
	//insert the phasor in the map
	m_ret = m_map_Digitals.insert(pair<int, vector<char> >(m_curDigItem, vec));
	if(m_ret.second == false)
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::FirstDigital(BSTR* pbstrDigital)
{
	// TODO: Add your implementation code here
	map<int, vector<char> >::iterator it;
	it = m_map_Digitals.begin();
	m_curDigIt = it;
	int n = ((*it).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));

	//copy the phasor name
	for(int i = 0; i < n; i++){
			vec[i] = ((*m_curDigIt).second).at(i);
	}

	_bstr_t _bstr(vec);
	//pass the phasor name to outside this function
	*pbstrDigital = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::NextDigital(BSTR* pbstrDigital)
{
	// TODO: Add your implementation code here

	//check if any more phasor name is left to return
	map<int, vector<char> >::iterator it;
	it = m_map_Digitals.end();
	if( m_curDigIt != it )	
		m_curDigIt++;	//still there are phasor names to return
	else	//no more phasor name to return
		return S_FALSE;

	int n = ((*m_curDigIt).second).size();

	char vec[64];
	::memset(vec, 0, sizeof(vec));


	//copy the phasor name
	for(int i = 0; i < n; i++){
		vec[i] = ((*m_curDigIt).second).at(i);
	}

	//pass the phasor name to outside this function
	_bstr_t _bstr(vec);
	*pbstrDigital = _bstr.copy();

	return S_OK;
}


STDMETHODIMP CCFG2Params::ResizeDigitalAndAppend(USHORT u16_newSize, BSTR bstrDigital)
{
	// TODO: Add your implementation code here
	//check
	if(!u16_newSize){
		m_curAngItem++;
		return S_FALSE;
	}

	CComBSTR comBSTR(bstrDigital);
	int n(0), i(0);
	vector<char> vec;
	map<int, vector<char> >::iterator it;
	it = m_map_Digitals.find(m_curDigItem);
	n = ((*it).second).size();
	vec.resize(n + u16_newSize, 0);
	do	
	{
		vec[i] = ((*it).second)[i];
	}while( ++i < n );

	for(int j = 0; j < u16_newSize; j++)
		vec[n + j] = comBSTR.m_str[j];
	AtlTrace("\n");
/*	for(int k = 0; k < (n + u16_newSize); k++)
		AtlTrace("%c",vec[k]);*/
	AtlTrace("\n");
	m_map_Digitals.erase(m_curDigItem);
	m_ret = m_map_Digitals.insert(pair<int, vector<char> >(m_curDigItem, vec));
	if(m_ret.second == false)
		return S_FALSE;
	it = m_map_Digitals.find(m_curDigItem);
	for(int k = 0; k < (n + u16_newSize); k++)
		AtlTrace("%c",((*it).second).at(k));
	AtlTrace("\n");

	return S_OK;
}


STDMETHODIMP CCFG2Params::FindAnalogIndex(BSTR bstrAnalog, ULONG* pu32_AnalogIndex)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	BSTR bstr;
	_bstr_t _bAnalog(bstrAnalog);
	string strAnalog, str;
	strAnalog = _bAnalog;
	//check
	if( !_bAnalog.length() )
		return S_FALSE;

	//loop through the PMUs
	if(FAILED(hr=FirstAnalog(&bstr)))
		return S_FALSE;
	str = _bstr_t(bstr);
	if(!strAnalog.compare(str)){
		*pu32_AnalogIndex = (*m_curAngIt).first;
		return S_OK;
	}
	else
		AtlTrace("FindAnalogIndex has not found the Analog signal Index.\n");

	while(hr == S_OK)
	{
		hr = NextAnalog(&bstr);
		str = _bstr_t(bstr);

		if(!strAnalog.compare(str)){
			*pu32_AnalogIndex = (*m_curAngIt).first;
			return S_OK;
			break;
		}
	}
//--------------------
	return hr;
//--------------------
}


STDMETHODIMP CCFG2Params::FindDigitalIndex(BSTR bstrDigital, ULONG* pu32_DigitalIndex)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	BSTR bstr;
	_bstr_t _bDigital(bstrDigital);
	string strDigital, str;
	strDigital = _bDigital;
	//check
	if( !_bDigital.length() )
		return S_FALSE;

	//loop through the PMUs
	if(FAILED(hr=FirstDigital(&bstr)))
		return S_FALSE;
	str = _bstr_t(bstr);
	if(!strDigital.compare(str)){
		*pu32_DigitalIndex = (*m_curDigIt).first;
		return S_OK;
	}
	else
		AtlTrace("FindAnalogIndex has not found the Analog signal Index.\n");

	while(hr == S_OK)
	{
		hr = NextDigital(&bstr);
		str = _bstr_t(bstr);

		if(!strDigital.compare(str)){
			*pu32_DigitalIndex = (*m_curDigIt).first;
			return S_OK;
			break;
		}
	}

	return S_OK;
}


STDMETHODIMP CCFG2Params::get_TotNoofDigitals(ULONG* pVal)
{
	// TODO: Add your implementation code here
	HRESULT hr;

	if(FAILED( hr = m_map_Analogs.size() ))
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CCFG2Params::put_TotNoofDigitals(ULONG newVal)
{
	// TODO: Add your implementation code here
	
	return E_NOTIMPL;
}


STDMETHODIMP CCFG2Params::RestAnalogList(void)
{
	// TODO: Add your implementation code here
	m_curAngIt = m_map_Analogs.begin();

	return S_OK;
}
