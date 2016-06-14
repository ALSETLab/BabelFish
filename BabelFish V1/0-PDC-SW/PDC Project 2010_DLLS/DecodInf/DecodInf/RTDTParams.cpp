// RTDTParams.cpp : Implementation of CRTDTParams

#include "stdafx.h"
#include "RTDTParams.h"
#include <assert.h>
#include <comdef.h>



// CRTDTParams

HRESULT CRTDTParams::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;

	return hr;
} 

BOOL CRTDTParams::CanBePooled()
{
	return FALSE;
} 

void CRTDTParams::Deactivate()
{
	m_spObjectContext.Release();
} 

STDMETHODIMP CRTDTParams::get_TS_Year(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	
	
	*pVal = m_u_32_year;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_Year(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	
	
	m_u_32_year = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_TS_Month(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	
	
	*pVal = m_u_32_Month;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_Month(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	
	
	m_u_32_Month = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_TS_Day(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	*pVal = m_u_32_Day;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_Day(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	m_u_32_Day = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_TS_Hour(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	*pVal = m_u_32_Hour;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_Hour(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	m_u_32_Hour = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_TS_Minute(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	*pVal = m_u_32_Minute;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_Minute(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	m_u_32_Minute = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_TS_Second(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	*pVal = m_u_32_second;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_Second(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	m_u_32_second = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_TS_OLE_VAR(DOUBLE* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	*pVal = m_dVar;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_TS_OLE_VAR(DOUBLE newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	m_dVar = newVal;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_Millisecond(ULONG* pVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.

	*pVal = m_tsMillisecond;

	//release ownership of the critical section.
	

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_Millisecond(ULONG newVal)
{
	// TODO: Add your implementation code here
	//request ownership of the critical section.
	

	DWORD dwID = ::GetThreadId(::GetCurrentThread());
	AtlTrace("Current thread id is: %d\n", dwID);

	m_tsMillisecond = newVal;

	//release ownership of the critical section.

	return S_OK;
}


STDMETHODIMP CRTDTParams::SavePhVals(FLOAT XVal, FLOAT YVal, FLOAT flDtType)
{
	// TODO: Add your implementation code here
			//allocate memory for a phasor magnitude and angle
	m_pPhVals = (float*)m_pMalloc->Alloc(3 * sizeof(float));
	if( !m_pPhVals )
		return S_FALSE;

	::memset( m_pPhVals, 0, sizeof(m_pPhVals) );

	//assign the phasor values to the pointer
	m_pPhVals[0] = m_flXVal;
	m_pPhVals[1] = m_flYVal;
	m_pPhVals[2] = m_flMagAng;

	//insert the phasor into the phasors map
	m_map_ppPhVals[m_nPhVals] = m_pPhVals;
	m_nPhVals++;	//next entry in the map
	m_pPhVals = NULL; //point the temporary pointer to NULL
/*
vector<float>	vecfl;

	vecfl.push_back(m_flXVal);
	vecfl.push_back(m_flYVal);
	vecfl.push_back(m_flMagAng);

	//add the phasor value
	m_retRtPhVals = m_map_RtPhVals.insert(pair<ULONG, vector<float> >(m_nPhVals, vecfl));
	if( m_retRtPhVals.second == false )
	{
		m_retRtPhVals = m_map_RtPhVals.insert(pair<int, vector<float> >(m_nPhVals, vecfl));
		m_nPhVals++;
	}
	else
		m_nPhVals++;
		*/
/*
vector<float>	vecfl;

	vecfl.push_back(m_flXVal);
	vecfl.push_back(m_flYVal);
	vecfl.push_back(m_flMagAng);
	
	m_map_RtPhVals[m_nPhVals].push_back(m_flXVal);
	m_map_RtPhVals[m_nPhVals].push_back(m_flYVal);
	m_map_RtPhVals[m_nPhVals].push_back(m_flMagAng);
	m_nPhVals++;
	*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::get_PhasorXVal(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flXVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_PhasorXVal(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flXVal = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_PhasorYVal(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flYVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_PhasorYVal(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flYVal = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_MagAng(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flMagAng;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_MagAng(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flMagAng = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::LoadPhVals(FLOAT* pPhXVal, FLOAT* pPhYVal, FLOAT* pPhType)
{
	// TODO: Add your implementation code here
	float * ptmp(NULL);

	if( m_ncurPh < m_nPhVals )
	{
		ptmp = m_map_ppPhVals[m_ncurPh];

		*pPhXVal = ptmp[0];
		*pPhYVal = ptmp[1];
		*pPhType = ptmp[2];
		m_ncurPh++;
	}
	else
		return S_FALSE;

	/*
	if( m_nPhVals > 0 )
	{
		if( m_RtPhValsIt == m_map_RtPhVals.end() )
			return S_FALSE;

		*pPhXVal   = ((*m_RtPhValsIt).second)[0];
		*pPhYVal   = ((*m_RtPhValsIt).second)[1];
		*pPhType = ((*m_RtPhValsIt).second)[2];
		m_RtPhValsIt++;
	}
	else
		return S_FALSE;
	*/	
/*
	if( m_nPhVals > 0 )
	{
		if( m_RtPhValsIt == m_map_RtPhVals.end() )
			return S_FALSE;

//		vector<float>	tempVec = (*m_RtPhValsIt);
		vector<float>	tempVec = m_map_RtPhVals[m_ncurPh];
		*pPhXVal   = tempVec[0];
		*pPhYVal   = tempVec[1];
		*pPhType = tempVec[2];
		m_RtPhValsIt++;
		m_nPhVals--;
	}
	else
		return S_FALSE;
	*/	
	return S_OK;
}


STDMETHODIMP CRTDTParams::CleanPhasors(void)
{
	// TODO: Add your implementation code here
/*	m_strWarning = "CleanPhasors is called.\n";
	//write the IP address into the network configuration file
	if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
		return -1;
	*/	
	float * ptmp(NULL),*ptmp1(NULL);
	int n(0);
	while( m_nPhVals > 0)
	{
		ptmp = m_map_ppPhVals[n++];
		if( ptmp != NULL)
		{
			m_pMalloc->Free(ptmp);
			m_nPhVals--;
		}
	}
/*	if(!m_map_RtPhVals.empty())
	{
		m_RtPhValsIt = m_map_RtPhVals.begin();
		while( m_RtPhValsIt != m_map_RtPhVals.end() )
		{
//			(*m_RtPhValsIt).second.erase((*m_RtPhValsIt).second.begin(), (*m_RtPhValsIt).second.end());
			m_RtPhValsIt->clear();
			m_RtPhValsIt++;
		}
		m_map_RtPhVals.clear();
		m_nPhVals = 0;
	}
	*/
/*
	if(!m_map_RtFDQVals.empty() )
	{
		m_RtFDQValsIt = m_map_RtFDQVals.begin();
		while( m_RtFDQValsIt != m_map_RtFDQVals.end() )
		{
			(*m_RtFDQValsIt).second.erase((*m_RtFDQValsIt).second.begin(), (*m_RtFDQValsIt).second.end());
			m_RtFDQValsIt++;
		}
		m_map_RtFDQVals.clear();
	}
	*/
/*	if(!m_map_RtANVals.empty())
	{
		m_RtANValsIt = m_map_RtANVals.begin();
		while( m_RtANValsIt != m_map_RtFDQVals.end() )
		{
			(*m_RtANValsIt).second.erase((*m_RtANValsIt).second.begin(), (*m_RtANValsIt).second.end());
			m_RtANValsIt++;
		}
		m_map_RtANVals.clear();
	}
	*/
/*
	if(!m_map_RtDigVals.empty())
	{
		m_RtDigValsIt = m_map_RtDigVals.begin();
		while( m_RtDigValsIt != m_map_RtDigVals.end())
		{
			(*m_RtDigValsIt).second.erase((*m_RtDigValsIt).second.begin(), (*m_RtDigValsIt).second.end());
			m_RtDigValsIt++;
		}
		m_map_RtDigVals.clear();
	}
	*/
/*
	if(!m_map_RtDigStr.empty())
	{
		m_RtDigStrIt = m_map_RtDigStr.begin();
		while( m_RtDigStrIt != m_map_RtDigStr.end() )
		{
			(*m_RtDigStrIt).second.erase((*m_RtDigStrIt).second.begin(), (*m_RtDigStrIt).second.end());
			m_RtDigStrIt++;
		}
		m_map_RtDigStr.clear();
	}
*/	
	return S_OK;
}


STDMETHODIMP CRTDTParams::ChPhListBegin(void)
{
	// TODO: Add your implementation code here
//	m_RtPhValsIt = m_map_RtPhVals.begin();
//	m_RtFDQValsIt = m_map_RtFDQVals.begin();
//	m_RtANValsIt = m_map_RtANVals.begin();
//	m_RtDigValsIt = m_map_RtDigVals.begin();
//	m_RtDigStrIt = m_map_RtDigStr.begin();
	m_ncurPh = 0;
	m_ncurFDQ = 0;
	m_ncurAN = 0;
	m_ncurDig = 0;
	m_ncurDigHex = 0;

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_FREQ(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flFREQ;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_FREQ(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flFREQ = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_DFREQ(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flDFREQ;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_DFREQ(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flDFREQ = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::SaveFDQANDGVals(void)
{
	// TODO: Add your implementation code here
	m_pFDQVals = (float*)m_pMalloc->Alloc(4 * sizeof(float));
	if( !m_pFDQVals )
		return S_FALSE;

	::memset( m_pFDQVals, 0, sizeof(m_pFDQVals) );

	//assign the phasor values to the pointer
	m_pFDQVals[0] = m_flFREQ;
	m_pFDQVals[1] = m_flDFREQ;
	m_pFDQVals[2] = m_flAnalog;
	m_pFDQVals[3] = m_flDigital;

	//insert the phasor into the phasors map
	m_map_ppFDQVals[m_nFDQVals] = m_pFDQVals;
	m_nFDQVals++;	//next entry in the map
	m_pFDQVals = NULL; //point the temporary pointer to NULL
//	vector<float>	vecfl;
/*
	vecfl[0] = m_flFREQ;
	vecfl[1] = m_flDFREQ;
	vecfl[2] = m_flAnalog;
	vecfl[3] = m_flDigital;
	*/
/*
	vecfl.push_back(m_flFREQ);
	vecfl.push_back(m_flDFREQ);
	vecfl.push_back(m_flAnalog);
	vecfl.push_back(m_flDigital);

	if( m_nFDQVals < 0 )
		return S_OK;

	m_retRtFDQVals = m_map_RtFDQVals.insert(pair<int, vector<float> >(m_nFDQVals, vecfl));
	if( m_retRtFDQVals.second == false )
	{
		m_retRtFDQVals = m_map_RtFDQVals.insert(pair<int, vector<float> >(m_nFDQVals, vecfl));
		m_nFDQVals++;
	}
	else
		m_nFDQVals++;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::LoadFDQANDGVals(FLOAT* pflFREQ, FLOAT* pflDFREQ, FLOAT* pflDIG, FLOAT* pflANG)
{
	// TODO: Add your implementation code here
	float * ptmp(NULL);

	if( m_ncurFDQ < m_nFDQVals )
	{
		ptmp = m_map_ppFDQVals[m_ncurFDQ];

		*pflFREQ = ptmp[0];
		*pflDFREQ = ptmp[1];
		*pflDIG = ptmp[2];
		m_ncurFDQ++;
	}
	else
		return S_FALSE;

/*
	if( m_nFDQVals > 0 )
	{
		if( m_RtFDQValsIt == m_map_RtFDQVals.end() )
			return S_FALSE;
		*pflFREQ   = ((*m_RtFDQValsIt).second)[0];
		*pflDFREQ   = ((*m_RtFDQValsIt).second)[1];
		*pflDIG = ((*m_RtFDQValsIt).second)[2];
//iITC		*pflANG = ((*m_RtFDQValsIt).second)[3];
		m_RtFDQValsIt++;
	}
	else
		return S_FALSE;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::get_Analog(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flAnalog;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_Analog(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flAnalog = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_Digital(FLOAT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_flDigital;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_Digital(FLOAT newVal)
{
	// TODO: Add your implementation code here
	m_flDigital = newVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::SaveADVals(void)
{
	// TODO: Add your implementation code here
	m_pANVals = (float*)m_pMalloc->Alloc(sizeof(float));
	if( !m_pANVals )
		return S_FALSE;

	::memset( m_pANVals, 0, sizeof(m_pANVals) );

	//assign the phasor values to the pointer
	*m_pANVals = m_flAnalog;

	//insert the phasor into the phasors map
	m_map_ppANVals[m_nANVals] = m_pANVals;
	m_nANVals++;	//next entry in the map
	m_pANVals = NULL; //point the temporary pointer to NULL

	/*
	vector<float>	vecfl(1,0);

	vecfl[0] = m_flAnalog;

	m_retRtANVals = m_map_RtANVals.insert(pair<int, vector<float> >(m_map_RtANVals.size(), vecfl));
	if( m_retRtANVals.second == false )
		return S_FALSE;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::LoadANVals(FLOAT* pflAnalogVal/*, FLOAT* pflDigitalVal*/)
{
	// TODO: Add your implementation code here
	float * ptmp(NULL);

	if( m_ncurAN < m_nANVals )
	{
		ptmp = m_map_ppANVals[m_ncurAN];

		*pflAnalogVal = *ptmp;
		m_ncurAN++;
	}
	else
		return S_FALSE;

/*
	if( m_map_RtANVals.size() > 0 )
	{
		if( m_RtANValsIt == m_map_RtANVals.end() )
			return S_FALSE;
		*pflAnalogVal   = ((*m_RtANValsIt).second)[0];
		m_RtANValsIt++;
	}
	else
		return S_FALSE;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::SaveDigVals(void)
{
	// TODO: Add your implementation code here
	m_pDigVals = (float*)m_pMalloc->Alloc(sizeof(float));
	if( !m_pDigVals )
		return S_FALSE;

	::memset( m_pDigVals, 0, sizeof(m_pDigVals) );

	//assign the phasor values to the pointer
	*m_pDigVals = m_flDigital;

	//insert the phasor into the phasors map
	m_map_ppDigVals[m_nDigVals] = m_pDigVals;
	m_nDigVals++;	//next entry in the map
	m_pDigVals = NULL; //point the temporary pointer to NULL

/*
	vector<float>	vecfl(1,0);

	vecfl[0] = m_flDigital;
	m_retRtDigVals = m_map_RtDigVals.insert(pair<int, vector<float> >(m_map_RtDigVals.size(), vecfl));
	if( m_retRtDigVals.second == false )
		return S_FALSE;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::LoadDigVals(FLOAT* pflDigitalVal)
{
	// TODO: Add your implementation code here
		float * ptmp(NULL);

	if( m_ncurDig < m_nDigVals )
	{
		ptmp = m_map_ppDigVals[m_ncurDig];

		*pflDigitalVal = *ptmp;
		m_ncurDig++;
	}
	else
		return S_FALSE;


	/*
	if( m_map_RtDigVals.size() > 0 )
	{
		if( m_RtDigValsIt == m_map_RtDigVals.end() )
			return S_FALSE;
		*pflDigitalVal = ((*m_RtDigValsIt).second)[0];
		m_RtDigValsIt++;
	}
	else
		return S_FALSE;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::DigHex2BinStr(void)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	char * pdby(NULL);
	char  c(0);
	
	pdby = (char*)m_pMalloc->Alloc(2);
	if( !pdby )
	{
		AtlTrace("Error: failed to allocate memory for two bytes of hex digital signal---CRTDTParams::DigHex2BinStr\n");
		return S_FALSE;
	}
	//1. fetch digital signal
	if(FAILED(hr = m_pDigFld->get_DigFld(&c)))
		return S_FALSE;
	else
		pdby[0] = c;
	c = 0;
	if(FAILED(hr = m_pDigFld->get_DigFld(&c)))
		return S_FALSE;
	else
		pdby[1] = c;
	//2. detect the bits of the digital signal and set the 
	//equvalent "0" or "1" in the output string to present
	// in the labview
	AtlTrace("Hex value of the digital signal is: %02x%02x\n", (unsigned char)pdby[0], (unsigned char)pdby[1]);
	//analyze the digital signal hex string and convert it to the a strig of '0', '1'

	m_strDigFld.clear();

	c = (unsigned char)pdby[0];
	AtlTrace("%02x\n", c);
	if(c & DET8)
		m_strDigFld.insert(m_strDigFld.begin(), '1');
	else
		m_strDigFld.insert(m_strDigFld.begin(), '0');

	if(c & DET7)
		m_strDigFld.insert(m_strDigFld.begin()+1, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+1, '0');

	if(c & DET6)
		m_strDigFld.insert(m_strDigFld.begin()+2, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+2, '0');

	if(c & DET5)
		m_strDigFld.insert(m_strDigFld.begin()+3, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+3, '0');

	if(c & DET4)
		m_strDigFld.insert(m_strDigFld.begin()+4, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+4, '0');

	if(c & DET3)
		m_strDigFld.insert(m_strDigFld.begin()+5, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+5, '0');

	if(c & DET2)
		m_strDigFld.insert(m_strDigFld.begin()+6, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+6, '0');

	if(c & DET1)
		m_strDigFld.insert(m_strDigFld.begin()+7, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+7, '0');

	c = (unsigned char)pdby[1];
	if(c & DET12)
		m_strDigFld.insert(m_strDigFld.begin() + 8, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin() + 8, '0');

	if(c & DET11)
		m_strDigFld.insert(m_strDigFld.begin()+9, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+9, '0');

	if(c & DET10)
		m_strDigFld.insert(m_strDigFld.begin()+10, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+10, '0');

	if(c & DET9)
		m_strDigFld.insert(m_strDigFld.begin()+11, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+11, '0');

	if(c & DET16)
		m_strDigFld.insert(m_strDigFld.begin()+12, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+12, '0');

	if(c & DET15)
		m_strDigFld.insert(m_strDigFld.begin()+13, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+13, '0');

	if(c & DET14)
		m_strDigFld.insert(m_strDigFld.begin()+14, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+14, '0');

	if(c & DET13)
		m_strDigFld.insert(m_strDigFld.begin()+15, '1');
	else
		m_strDigFld.insert(m_strDigFld.begin()+15, '0');

	//print the output binary string
	AtlTrace("Digital binary string is: %s\n", m_strDigFld.c_str());

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_HexAnalog(CHAR* pVal)
{
	// TODO: Add your implementation code here
	
	return S_OK;
}


STDMETHODIMP CRTDTParams::put_HexAnalog(CHAR newVal)
{
	// TODO: Add your implementation code here
	HRESULT hr;
	//insert one byte of analog signal as hex
	if(FAILED(hr = m_pDigFld->put_DigFld(newVal)))
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CRTDTParams::get_DigBinString(BSTR* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_bstrRTDigVal;

	return S_OK;
}


STDMETHODIMP CRTDTParams::put_DigBinString(BSTR newVal)
{
	// TODO: Add your implementation code here

	return E_NOTIMPL;
}


STDMETHODIMP CRTDTParams::SaveDigHexVals(void)
{
	// TODO: Add your implementation code here
	m_pDigHex = (char*)m_pMalloc->Alloc(m_strDigFld.size());
	if( !m_pDigHex )
		return S_FALSE;

	::memset( m_pDigHex, 0, sizeof(m_pANVals) );

	//assign the phasor values to the pointer
	::strcpy(m_pDigHex, m_strDigFld.c_str());
	AtlTrace("Digital string is: %s\n", m_strDigFld.c_str());
	//insert the phasor into the phasors map
	m_map_ppDigHex[m_nDigHex] = m_pDigHex;
	m_nDigHex++;	//next entry in the map
	m_pDigHex = NULL; //point the temporary pointer to NULL
	

/*
	m_retRtDigStr = m_map_RtDigStr.insert(pair<int, string >(m_map_RtDigStr.size(), m_strDigFld));
	if( m_retRtDigStr.second = false )
		return S_FALSE;
	else
	{
		AtlTrace("Binary string of Digital signal is saved.\n");
		//empty the string for the next digital signal
		m_strDigFld.clear();
	}
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::LoadRtDigStrs(BSTR* bstrDigBinVal)
{
	// TODO: Add your implementation code here
	char * ptmp(NULL);
	_bstr_t _b = "";

	if( m_ncurDigHex < m_nDigHex )
	{
		ptmp = m_map_ppDigHex[m_ncurDigHex];

		_b = ptmp;
		*bstrDigBinVal = _b.copy();
		m_ncurDigHex++;
	}
	else
		return S_FALSE;
	
/*
	string str;
	_bstr_t _b = "";

	if( m_map_RtDigStr.size() > 0 )
	{
		if( m_RtDigStrIt == m_map_RtDigStr.end() )
			return S_FALSE;
		str = ((*m_RtDigStrIt).second);
		m_RtDigStrIt++;
		_b = str.c_str();
		*bstrDigBinVal = _b.copy();
	}
	else
		return S_FALSE;
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::CleanFDQ(void)
{
	// TODO: Add your implementation code here
	float * ptmp(NULL);
	int n(0);
	while( m_nFDQVals > 0)
	{
		ptmp = m_map_ppFDQVals[n++];
		if( ptmp != NULL)
		{
			m_pMalloc->Free(ptmp);
			m_nFDQVals--;
		}
	}
/*
	if( m_nFDQVals > 0 )
	{
		m_RtFDQValsIt = m_map_RtFDQVals.begin();
		while( m_RtFDQValsIt != m_map_RtFDQVals.end() )
		{
			(*m_RtFDQValsIt).second.erase((*m_RtFDQValsIt).second.begin(), (*m_RtFDQValsIt).second.end());
			m_RtFDQValsIt++;
		}
		m_map_RtFDQVals.clear();
		m_nFDQVals = 0;
	}
	*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::CleanANParams(void)
{
	// TODO: Add your implementation code here
	float * ptmp(NULL);
	int n(0);
	while( m_nANVals > 0)
	{
		ptmp = m_map_ppANVals[n++];
		if( ptmp != NULL)
		{
			m_pMalloc->Free(ptmp);
			m_nANVals--;
		}
	}
/*
	if(!m_map_RtANVals.empty())
	{
		m_RtANValsIt = m_map_RtANVals.begin();
		while( m_RtANValsIt != m_map_RtFDQVals.end() )
		{
			(*m_RtANValsIt).second.erase((*m_RtANValsIt).second.begin(), (*m_RtANValsIt).second.end());
			m_RtANValsIt++;
		}
		m_map_RtANVals.clear();
	}
*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::CleanDGParams(void)
{
	// TODO: Add your implementation code here
	float * ptmp(NULL);
	int n(0);
	while( m_nDigVals > 0)
	{
		ptmp = m_map_ppDigVals[n++];
		if( ptmp != NULL)
		{
			m_pMalloc->Free(ptmp);
			m_nDigVals--;
		}
	}

/*
	if(!m_map_RtDigVals.empty())
	{
		m_RtDigValsIt = m_map_RtDigVals.begin();
		while( m_RtDigValsIt != m_map_RtDigVals.end())
		{
			(*m_RtDigValsIt).second.erase((*m_RtDigValsIt).second.begin(), (*m_RtDigValsIt).second.end());
			m_RtDigValsIt++;
		}
		m_map_RtDigVals.clear();
	}
	*/
	return S_OK;
}


STDMETHODIMP CRTDTParams::CleanDigStr(void)
{
	// TODO: Add your implementation code here
	char * ptmp(NULL);
	int n(0);
	while( m_nDigHex > 0)
	{
		ptmp = m_map_ppDigHex[n++];
		if( ptmp != NULL)
		{
			m_pMalloc->Free(ptmp);
			m_nDigHex--;
		}
	}
	
/*
	if(!m_map_RtDigStr.empty())
	{
		m_RtDigStrIt = m_map_RtDigStr.begin();
		while( m_RtDigStrIt != m_map_RtDigStr.end() )
		{
			(*m_RtDigStrIt).second.erase((*m_RtDigStrIt).second.begin(), (*m_RtDigStrIt).second.end());
			m_RtDigStrIt++;
		}
		m_map_RtDigStr.clear();
	}
*/
	return S_OK;
}
