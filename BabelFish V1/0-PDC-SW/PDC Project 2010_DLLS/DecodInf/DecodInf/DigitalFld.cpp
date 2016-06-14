// DigitalFld.cpp : Implementation of CDigitalFld

#include "stdafx.h"
#include "DigitalFld.h"


// CDigitalFld

HRESULT CDigitalFld::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;
	return hr;
} 

BOOL CDigitalFld::CanBePooled()
{
	return FALSE;
} 

void CDigitalFld::Deactivate()
{
	m_spObjectContext.Release();
} 

