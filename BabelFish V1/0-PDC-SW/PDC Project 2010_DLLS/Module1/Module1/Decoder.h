#pragma once
#include <vector>

using namespace std;

template<class T> class CDecoder
{
public:


CDecoder::CDecoder(void)
{
}


CDecoder::~CDecoder(void)
{
}
protected:
	// converts binary data format to ascii data format
	DWORD BIN2ASCII(void);
	//converts Real Time Data
	DWORD BIN2ASCII_RT(void);
	// puts the binary data in this class
	DWORD PutBinData(vector<char> * pVecBinData);
};

