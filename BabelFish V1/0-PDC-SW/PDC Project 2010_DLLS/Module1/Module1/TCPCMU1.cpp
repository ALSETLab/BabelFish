// TCPCMU1.cpp : Implementation of CTCPCMU1

#include "stdafx.h"
#include "TCPCMU1.h"
#include <assert.h>
#include "decodInf_i.c"
#include <ATLComTime.h>

/*
static LabVIEW::_ApplicationPtr pLV(NULL);
static LabVIEW::VirtualInstrumentPtr pVI;
*/
#pragma warning(disable: 4996)

// CTCPCMU1

HRESULT CTCPCMU1::Activate()
{
	HRESULT hr = GetObjectContext(&m_spObjectContext);
	if (SUCCEEDED(hr))
		return S_OK;
	return hr;
} 

BOOL CTCPCMU1::CanBePooled()
{
	return FALSE;
} 

void CTCPCMU1::Deactivate()
{
	m_spObjectContext.Release();
} 

STDMETHODIMP CTCPCMU1::ConnectTo(DWORD u32_ServIP, USHORT u16_Port, DWORD u32_EventTimeout, DWORD u32_MaxIdeTime)
{
	// TODO: Add your implementation code here
	if( ConnectTo1(u32_ServIP, u16_Port, u32_EventTimeout, u32_MaxIdeTime) < 0 )
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::SendTo(DWORD hSocket, CHAR* s8_SendBuf, DWORD u32_Len)
{
	// TODO: Add your implementation code here
	if( SendTo1(hSocket, s8_SendBuf, u32_Len) )
		return S_FALSE;

	AtlTrace("Success: We managed to send the a packet to the server.\n");

	return S_OK;
}


STDMETHODIMP CTCPCMU1::DisconnectClient()
{
	// TODO: Add your implementation code here
	if(m_bConnected)
	{
		DisconnectClient1( ((*m_sockIt).second) );
		m_bConnected = FALSE;
	}

	return S_OK;
}

DWORD CTCPCMU1::Initialize1(void)
{
	WSADATA	wsaData;
	USHORT u16_err(0);

	u16_err = ::WSAStartup(MAKEWORD(2,0), &wsaData);
	if( !u16_err )
	{
//---------------------------------------------------------------------------
	char * m_s8_ReadBuffer1(NULL);
	//allocate memory for reading from the PDC contact
	m_s8_ReadBuffer1 = (char*) m_pMalloc->Alloc(READ_BUFFER_SIZE);
	if( !m_s8_ReadBuffer ){
//			throw new CMemoryException();
		assert(NULL);
		AtlTrace("Error: failed to allocate memory for the Read Buffer for socket.\n");
	}
	else
		AtlTrace("Success: Read Buffer for the socket is allocated.\n");

//-----------------------------------------------------------------------------
		//set the initialization flag.
		m_bInitialized = TRUE;
		AtlTrace("Success: win32 socket dll is loaded.\n");
	}
	else
		AtlTrace("Error: The library initialization failed.\n");
	return 0;
}


DWORD CTCPCMU1::Close(void)
{
	return 0;
}


DWORD CTCPCMU1::CreateSocket(void)
{
	DWORD u_32err;
	SOCKET hSocket(NULL);

	if( !m_bInitialized ){
		//initialize socket libraries
		u_32err = Initialize1();
		if( u_32err )
			return u_32err;
	}

	//create the socket
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if( hSocket == INVALID_SOCKET )
		return WSAGetLastError();
	else
		AtlTrace("Success: Socket created.\n");
	
	//create an event to be associated with this socket, 
	//when event triggers we get a notification from 
	//windows socket system and there we can take some 
	//action such as read data from the buffer of 
	//the socket, or send data to server, and so forth
	_bstr_t	_bstr;
	HRESULT hr;
	BSTR bstr(NULL);

	if(FAILED( hr = get_SocketName(&bstr)))
		assert(NULL);

	_bstr = bstr;
	string str = "";
	str = (char*)_bstr;

	m_hEvent = WSACreateEvent();
	if( m_hEvent == WSA_INVALID_EVENT ){
		u_32err = WSAGetLastError();
		closesocket(hSocket);
		return u_32err;
	}
	else{
		AtlTrace("Success: Event of this socket is created");
		if( WSAEventSelect(hSocket, m_hEvent, FD_ALL_EVENTS) == SOCKET_ERROR){
			u_32err = WSAGetLastError();
			WSACloseEvent(m_hEvent);
			closesocket(hSocket);
			return u_32err;
		}
		else
		{
			AtlTrace("Success: Succeeded to associate the event with the socket.\n");

			//Not more than 50 sockets allowed
			if( m_SocketMap.size() <= MAX_NUM_SOCKETS ){
				//insert the socket in list of the socket
				m_retIt = m_SocketMap.insert( pair<string, SOCKET>(str, hSocket));
				if( m_retIt.second == false ){
					AtlTrace("Warning: CreateSocket funtion tried to insert a socket into the list.\n");
					AtlTrace("which already exited.\n{");
				}
				else{
					AtlTrace("Success: CreateSocket function succeeded to insert a socket into the map.\n");
				}
			}
			else
			{
				AtlTrace("Warning: you cannot create more than 50 sockets, therefore this action is stopped.\n");
				return 1;
			}
		}
	}

	m_sockIt = m_SocketMap.begin();
	//print the sockets
	while(m_sockIt != m_SocketMap.end()){
		AtlTrace("first socket name is: %s\n", ((*m_sockIt).first).c_str());
		m_sockIt++;
	}

	return 0;
}


DWORD CTCPCMU1::ConnectTo1(DWORD u32_ServIP, USHORT u16_Port, DWORD u32_EventTimeout, DWORD u32_MaxIdleTime)
{
	HRESULT hr;
	BSTR bstr(NULL);
	_bstr_t _bstr;
	USHORT u_16_skip(0);
	string szMode = "a";
	BSTR bstrFilNm;

	if(FAILED( hr = get_SkipSteps4RT(&u_16_skip)))
		return -1;

	//get log file name
	if(FAILED( hr = get_LogFilNm(&bstrFilNm)))
		return -1;
	else
	{
	//	fpos_t position;
		_bstr_t _bFilNm = bstrFilNm;
		//open the file with write permission
		f = ::fopen(_bFilNm, szMode.c_str());
		if( !f )
			return -1;
		else
			AtlTrace("Success: Log file is opened.\n");
	}

	if( u_16_skip )
	{
		//create a client socket
		DWORD u32_Error =  CreateSocket();
		if( u32_Error )
			return u32_Error;

		//find the socket
		if(FAILED( hr = get_SocketName(&bstr)))
//iITC			assert(NULL);
		return -1;

		//search for the socket
		if(FAILED( hr = FindSocket(bstr)))
//iITC			assert(NULL);
		return -1;

		SOCKADDR_IN sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = u32_ServIP;
		sockaddr.sin_port = htons(u16_Port);

		AtlTrace("socket name is: %s\n", ((*m_sockIt).first).c_str());

		//connect to the server
		if( connect( (*m_sockIt).second, (LPSOCKADDR)&sockaddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR )
		{
			u32_Error = WSAGetLastError();
			//iITC take other necessary actions here
			if( u32_Error != WSAEWOULDBLOCK )
				return u32_Error;

			DWORD u32_Res = ::WaitForSingleObject(m_hEvent, INFINITE);
			if( u32_Res == WAIT_OBJECT_0 )
			{
				DWORD u32_Events(0);
				WSANETWORKEVENTS	PDC_Events;

				AtlTrace("An evnet happened.\n");
				if(::WSAEnumNetworkEvents( (*m_sockIt).second, m_hEvent, &PDC_Events ) == SOCKET_ERROR )
				{
					u32_Error = WSAGetLastError();
					//remove the socket with problem
					//socketlist.Remove(m_hSocket);
					//goto EXIT;
					return u32_Error;
				}
				while( !(PDC_Events.lNetworkEvents & FD_CONNECT))
					AtlTrace("Still waiting for connection\n");

				if ( PDC_Events.lNetworkEvents & FD_CONNECT )
				{
					if( PDC_Events.iErrorCode[FD_CONNECT_BIT] != 0)
					{
						::MessageBoxA(NULL, "The network is down", "Network Alet", IDOK);
						exit(1);
					}
				}
			}

			m_strWarning = "Connection with the PDC Server is acomplished\n";
			//write the IP address into the network configuration file
//iITC			if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
//iITC				return -1;
			//send Enable command to the server
			BYTE msg[18];
			HRESULT hr;
			::memset(msg, 0, sizeof(msg));

			if( FAILED( hr = SendTo1((*m_sockIt).second, /*(char*)msg*/ConstructMsg( bstr ), 18)))
			{
				return S_FALSE;
			}
		}
		//client is not yet connected wait for FD_CONNECT
		//iITC development needs to be done here.
	}
	else
	{
		//send Enable command to the server
		BYTE msg[18];
		HRESULT hr;
		::memset(msg, 0, sizeof(msg));
			
		//find the socket
		if(FAILED( hr = get_SocketName(&bstr)))
//				assert(NULL);
			return -1;

		_bstr_t _b = "ConfigurationFrame2 Socket";
		//search for the socket
		if(FAILED( hr = FindSocket(_b.copy())))
//				assert(NULL);
			return -1;

		_bstr_t _bDisable = "Disable RealTimeData Socket";
		if( FAILED( hr = SendTo1((*m_sockIt).second, ConstructMsg( _bDisable.copy() ), 18)))
		{
//				assert(NULL);
			return S_FALSE;
		}

		if( FAILED( hr = SendTo1((*m_sockIt).second, ConstructMsg( bstr ), 18)))
		{
//				assert(NULL);
			return S_FALSE;
		}
	}

	m_bConnected = TRUE;

	return 0;
}

// Processes events associted with socket exchanged  between client and server
//waits for incoming events on the port and processes them
//returns the events that occured and the socket and it's IP_adress which caused the event
//If the event is FD_RERAD the data will be returned in ps8_RecvBuf which must be cleared with DeleteLeft() by the caller.
//If the event FD_WRITE the remaining data in the send buffer will be sent and pu32_Sent receves the bytes sent.
//If there is more data to be read or sent, the next call to ProcessEvents() will process the next block of data.
//returns ERROR_TIMEOUT if during the given timeout no eventoccurres
//pu32_IP = 0x6401a8c0 -> 192.168.1.100

//SYNC = 0X01 : represents the real time data
//SYNC = 0x11 : represents the Header Frame
//SYNC = 0X21 : represents the configuraton frame1
//SYNC = 0X31 : represents the configuration frame2
DWORD CTCPCMU1::ProcessEvents1(
								HWND hwnd,
								unsigned int msg,
								DWORD * pu32_Events, 
								DWORD * pu32_IP, 
								SOCKET * ph_Socket, 
								char ** pps8_RecvBuf, 
								DWORD * pu32_Read, 
								DWORD * pu32_Sent
							)
{
	DWORD u32_Error;
	WSANETWORKEVENTS PDC_Events;

	DWORD u32_Flags(0);
	WSABUF	Buffer;
	Buffer.buf = m_s8_ReadBuffer;
	Buffer.len = READ_BUFFER_SIZE;
	DWORD u32_Read(0);
	char  data[READ_BUFFER_SIZE];
	int iResult(0);
	char c(0), c9(0), c8(0);
	int nCount(0);
	int test_size(0);
	char buf[1500];
	int k = 0;
	HRESULT hr;
	char st[8], *stop;::memset(st, 0, sizeof(st)); 
	long nPktSize(-1);	
//------------------------------------
	char c1(0);
	char sync(0);
//------------------------------------
//---------------------------------------------
	char * lt;
	//allocate memory for time stamp decode
	lt = (char*)m_pMalloc->Alloc(6);
	if( !lt )
		assert(NULL);
	::memset(lt, 0, sizeof(lt));
//----------------------------------------------
	m_map_CFG2Frame.insert(pair<int, vector<char> >(1, vec));

	AtlTrace("0x%02x\n", c1 & 0xff);
	::sprintf(lt, "%02x ",  c1 & 0xff);
	m_strWarning = lt;
	//write the IP address into the network configuration file
	if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
		return -1;

	//get the SYNC word from the ITCPCMU1 object'S Properties
	if(FAILED( hr = get_SYNC1( (unsigned char*)&c1 )))
		return 1;
	else
		AtlTrace("First byte of SYNC is: 0x%02x\n", (unsigned char)c1);

	if(FAILED( hr = get_SYNC2((unsigned char*) &sync )))
		return 1;
	else
		AtlTrace("Second byte of SYNC is: 0x%02x\n", (unsigned char)sync);

	//iITC	::memset(*pps8_RecvBuf, 0, READ_BUFFER_SIZE);
	::memset(data, 0, READ_BUFFER_SIZE);
	::memset(buf, 0, 1500);
	//wait for events with calling WSAWaiForMultipleEventsEx
	//WSAWaitForMultipleEventsEx must to design to check other problems with server refer to documentation design!
	//I must think about this one.

	AtlTrace("socket name is: %s\n", ((*m_sockIt).first).c_str());

	while(1)
	{
		DWORD u32_Res = ::WaitForSingleObject( m_hEvent, INFINITE);
		if( u32_Res == WAIT_OBJECT_0 )
		{
			//get the events sent to this object 
			if(::WSAEnumNetworkEvents( (*m_sockIt).second, m_hEvent, &PDC_Events ) == SOCKET_ERROR )
			{
				u32_Error = WSAGetLastError();
				//remove the socket with problem
				//socketlist.Remove(m_hSocket);
				//goto EXIT;
				AtlTrace("Error: WSAEnumNetworkEvents failed. error no is: %d\n", u32_Error);
				return u32_Error;
			}
			//check what event has been sent.
			//Read event has occured so read the data sent from server
			//into the buffer
			if ( PDC_Events.lNetworkEvents & FD_READ )
			{
				//( SYNC = AA1 ) then data frame message is started.
				while((iResult = recv((*m_sockIt).second, &c, 1, 0)) > 0)
				{
					c = (unsigned char)c;
					c8= (unsigned char)c;
					if( (c) == c1  )
					{
						AtlTrace("%02x ", c & 0xff);
						if(iResult = recv((*m_sockIt).second, &c, 1, 0))/*bytes*/
						{
							c = (unsigned char)c;
							c9 = (unsigned char)c;
							if( c == sync )/*0x01*//*0x11*//*0x21*//*0X31*/
								AtlTrace("%02x ", c & 0xff);
								m_bCFG2 = TRUE;
								break;
						}
					}
				}//end-of-while loop

				//it is configuration frame2
				if( m_bCFG2 )
				{
					vec.push_back(c8);
					vec.push_back(c9);
				}

				for(int i=0; (iResult > 0); i++){
					iResult = ::recv((*m_sockIt).second, &c, 1, 0);
					if( (iResult > 0 ))
					{
						AtlTrace("%02x ", (c & 0xff));
						::sprintf(lt, "%02x ",  c & 0xff);
						m_strWarning = lt;
						//write the IP address into the network configuration file
//iITC						if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
//iITC							return -1;
						c = (unsigned char)c;
						if( (c) == c1  )
						{
							m_pPDCBinData.Append(c);
							//it is configuration frame2
							if( m_bCFG2 )
								vec.push_back(c);
							if(iResult = recv((*m_sockIt).second, &c, 1, 0))/*bytes*/
							{
								c = (unsigned char)c;
								m_pPDCBinData.Append(c);
								//it is configuration frame2
								if(vec.size() == nPktSize ){
									AtlTrace("%02x ", c & 0xff);
									::sprintf(lt, "%02x ",  c & 0xff);
									m_strWarning = lt;
									//write the IP address into the network configuration file
//iITC									if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
//iITC										return -1;
									break;
									}
							}
						}
						c = (unsigned char)c;
						m_pPDCBinData.Append(c);
						//it is configuration frame2
						if( m_bCFG2 )
							vec.push_back(c);
					}
					nCount++;	//total number of the characters read
					if( (nCount >= 4) && (vec.size() >= 4) )
					{
						::sprintf(st, "%02x%02x", vec.at(2) & 0xff, vec.at(3) & 0xff);
						nPktSize = (USHORT)strtol(st,&stop, 16);
					}
				}//end-of-for loop
				AtlTrace("\nTotal Number of data is: %d", nCount);
				AtlTrace("\n");
				for(int k=0; k < (int)vec.size(); k++)
				{
					AtlTrace("%02x ", vec[k] & 0xff);
					::sprintf(lt, "%02x ",  vec[k] & 0xff);
					m_strWarning = lt;
					//write the IP address into the network configuration file
					if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
						return -1;
				}
				AtlTrace("\nsize of CFG2 is: %d\n", vec.size());
				m_retCFG2It = m_map_CFG2Frame.insert(pair<int, vector<char> >(0, vec));
				if( m_retCFG2It.second == false)
					assert(NULL);

				if( vec.size() == nPktSize )
					BIN2ASCII();
				
				::PostMessageA(hwnd, msg, 0, 0);
				break;
			}
		}//end-of-waitforsingleobject
	}//end-of-while() loop
	return 0;
}


DWORD CTCPCMU1::SendTo1(SOCKET hSocket, char * s8_SendBuf, DWORD u32_Len)
{
	WSABUF	k_Buf;
	k_Buf.buf = s8_SendBuf;
	k_Buf.len = u32_Len;
	DWORD u32_Sent;

	//send a packet to the server on a connected socket
	if( ::WSASend(hSocket, &k_Buf, 1, &u32_Sent, 0, 0, 0) == SOCKET_ERROR)
		return ::WSAGetLastError();

	return 0;
}


DWORD CTCPCMU1::DisconnectClient1(SOCKET hSocket)
{
	HRESULT hr;

	if( hr = FAILED( Disable()))
		return -1;
	//1. close connection
	::closesocket((*m_sockIt).second);
	//2. close the event and destroy it
	::WSACloseEvent(m_hEvent);
	::WSACleanup();

	return 0;
}

//waits for incoming events on the port and processes them
//returns the events that occured and the socket and it's IP_adress which caused the event
//If the event is FD_RERAD the data will be returned in ps8_RecvBuf which must be cleared with DeleteLeft() by the caller.
//If the event FD_WRITE the remaining data in the send buffer will be sent and pu32_Sent receves the bytes sent.
//If there is more data to be read or sent, the next call to ProcessEvents() will process the next block of data.
//returns ERROR_TIMEOUT if during the given timeout no eventoccurres
//pu32_IP = 0x6401a8c0 -> 192.168.1.100
STDMETHODIMP CTCPCMU1::ProcessEvents(HWND hwnd, unsigned int msg)
{
	// TODO: Add your implementation code here
	DWORD * pu32_Events(0);
	DWORD * pu32_IP(0);
	SOCKET * p_hSocket(0);
	char * ps8_RecvBuf(0);
	DWORD * PU32_Read(0);
	DWORD * pu32_Sent(0);

	
	DWORD u32_Error = ProcessEvents1( hwnd, msg, pu32_Events,
										pu32_IP,		//output
										p_hSocket,		//output
										&m_s8_ReadBuffer,	//output
										PU32_Read,		//output
										pu32_Sent		//output
										);
	if( u32_Error )
		return S_FALSE;
	
	return S_OK;
}


STDMETHODIMP CTCPCMU1::Initialize(void)
{
	// TODO: Add your implementation code here
	if( Initialize1() )
		return S_FALSE;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_blobData(BSTR* pVal)
{
	// TODO: Add your implementation code here
	m_bstrBlobData.AppendBytes(reinterpret_cast<char*>(m_s8_ReadBuffer), READ_BUFFER_SIZE);
	::MessageBox(NULL, CW2CT(m_bstrBlobData), NULL, MB_OK);
	*pVal = m_bstrBlobData.Copy();

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_blobData(BSTR newVal)
{
	// TODO: Add your implementation code here


	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_PDCBinData(BSTR* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_pPDCBinData.Copy();
	if(*pVal == NULL)
		assert(NULL);

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_PDCBinData(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_pPDCBinData.AppendBSTR(newVal);

	return S_OK;
}


STDMETHODIMP CTCPCMU1::HeaderFrame(void)
{
	// TODO: Add your implementation code here
		//send Enable command to the server
		BYTE msg[18];
		HRESULT hr;
		::memset(msg, 0, sizeof(msg));
		unsigned int short	crc = 0xFFFF;
		unsigned int short  temp;
		unsigned int short quick;
		unsigned int long  SOC = 0X00000000;

		//fill the SYNC code
		msg[0] = 0xAA;
		msg[1] = 0x41;
		//fill the FRAMESIZE= 18
		msg[2] = 0x00;
		msg[3] = 0x12;
		//fill the IDCODE of the PDC
		msg[4] = 0x03;
		msg[5] = 0x09;
		//fill the SOC
		SOC = (unsigned long)::time(NULL);
		BYTE * ptr(NULL);
		ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
		msg[6] = 0X00; msg[6]  = *(ptr--);
		msg[7] = 0x00; msg[7]  = *(ptr--);
		msg[8] = 0x00; msg[8]  = *(ptr--);
		msg[9] = 0x00; msg[9]  = *(ptr--);
		//fill the FRACSEC
		msg[10] = 0x00;
		msg[11] = 0x00;
		msg[12] = 0x00;
		msg[13] = 0x00;
		//fill CMD which is here 2 send enable the real-time data transmission
		msg[14] = 0x00;
		msg[15] = 0x3;

		//compute CRC
		for( int i =0; i < (18 - 2); i++ )
		{
			temp = (crc >> 8) ^ msg[i];
			crc <<= 8;
			quick = temp ^ (temp >> 4);
			crc ^= quick;
			quick <<= 5;
			crc ^= quick;
			quick <<= 7;
			crc ^= quick;
		}
		//fill the CRC-CCITT
		msg[16] = (crc & 0x0000ff00) >> 8;
		msg[17] = crc & 0x000000ff;

		if( FAILED( hr = SendTo1(m_hSocket, (char*)msg, 18))){
			assert(NULL);
			return S_FALSE;
		}

	return S_OK;
}


STDMETHODIMP CTCPCMU1::Disable(void)
{
	// TODO: Add your implementation code here
		//send Disable command to the server
		BYTE msg[18];
		HRESULT hr;

		if( FAILED( hr = SendTo1(/*m_hSocket*/(*m_sockIt).second, ConstructDisableMsg(), 18))){
			assert(NULL);
			return S_FALSE;
		}

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_SocketName(BSTR* pVal)
{
	// TODO: Add your implementation code here
	assert(m_bstrSockName);

	*pVal = m_bstrSockName;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_SocketName(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_bstrSockName = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::FindSocket(BSTR SockName)
{
	// TODO: Add your implementation code here
	_bstr_t bstrSockName = SockName;
	
	//search for the socket
	m_sockIt = m_SocketMap.find(string((char*)bstrSockName));
	if( m_sockIt == m_SocketMap.end() )
		return S_FALSE;		//socket not found

	AtlTrace( "Found Socket Name is: %s\n", ((*m_sockIt).first).c_str() );
		
	return S_OK;	//socket found
}



// Creates different types of messages according to the key
char* CTCPCMU1::ConstructMsg(BSTR bstrKey)
{
	_bstr_t _bstrKey, _bstrCntKey, _bstrHfKey, _bstrCnfg1Key, _bstrCnfg2Key, _bstrDisableKey;
	_bstr_t _bstrRTKey;
	_bstrKey += bstrKey;
	_bstrCntKey += "Connection Socket";
	_bstrHfKey += "HeaderFrame Socket";
	_bstrCnfg1Key += "ConfigurationFrame1 Socket";
	_bstrCnfg2Key += "ConfigurationFrame2 Socket";
	_bstrRTKey += "RealTimeData Socket";
	_bstrDisableKey += "Disable RealTimeData Socket";
	char * pMsg(NULL);

	//compare with different keys and decide which message to construct
	if( _bstrKey == _bstrCntKey )
	{
		//construct message for connection socket.
		return constructCntMsg();
	}
	else if( _bstrKey == _bstrHfKey )
	{
		//construct message for HeaderFrame socket.
		return constructHfMsg();
	}
	else if( _bstrKey == _bstrCnfg1Key )
	{
		return constructCnfg1fMsg();
	}
	else if( _bstrKey == _bstrCnfg2Key )
	{
		return constructCnfg2fMsg();
	}
	else if( _bstrKey == _bstrRTKey )
	{
		return ConstructRTMsg();
	}
	else if( _bstrKey == _bstrDisableKey )
	{
		return ConstructDisableMsg();
	}
	else
	{
		AtlTrace("Warning: Unknown message format is requested.\n");
		assert(NULL);
	}

	return NULL;
}

// constructs connection, enable real-time data transmission message
char * CTCPCMU1::ConstructDisableMsg()
{
//this mesage constructed for the 
//iITC BYTE msg[18];
BYTE * msg(NULL);
unsigned int short	crc = 0xFFFF;
unsigned int short  temp;
unsigned int short quick;
unsigned int long  SOC = 0X00000000;
HRESULT hr;
unsigned short u16_IDCode(0);

	msg = (BYTE*)m_pMalloc->Alloc(18);
	if( !msg )
		assert(NULL);
	::memset(msg, 0, sizeof(msg));

	//get the IDCode
	if(FAILED(hr = get_IDCode(&u16_IDCode)))
		return NULL;
	//fill the SYNC code
	msg[0] = 0xAA;
	msg[1] = 0x41;
	//fill the FRAMESIZE= 18
	msg[2] = 0x00;
	msg[3] = 0x12;
//fill the IDCODE of the PDC
	msg[4] = ((u16_IDCode >> 8) & 0xff);
	msg[5] = u16_IDCode  & 0xff;
//fill the SOC
	SOC = (unsigned long)::time(NULL);
	BYTE * ptr(NULL);
	ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
	msg[6] = 0X00; msg[6]  = *(ptr--);
	msg[7] = 0x00; msg[7]  = *(ptr--);
	msg[8] = 0x00; msg[8]  = *(ptr--);
	msg[9] = 0x00; msg[9]  = *(ptr--);
	//fill the FRACSEC
	msg[10] = 0x00;
	msg[11] = 0x00;
	msg[12] = 0x00;
	msg[13] = 0x00;
	//fill CMD which is here 2 send enable the real-time data transmission
	//send a header frame command = 2 = 0x0002	
	msg[14] = 0x00;
	msg[15] = 0x01;

	//compute CRC
	for( int i =0; i < (18 - 2); i++ )
	{
		temp = (crc >> 8) ^ msg[i];
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	//fill the CRC-CCITT
	msg[16] = (crc & 0x0000ff00) >> 8;
	msg[17] = crc & 0x000000ff;

	return (char*)msg;
}

// constructs connection, enable real-time data transmission message
char * CTCPCMU1::ConstructRTMsg(void)
{
//this mesage constructed for the 
//iITC BYTE msg[18];
BYTE * msg(NULL);
unsigned int short	crc = 0xFFFF;
unsigned int short  temp;
unsigned int short quick;
unsigned int long  SOC = 0X00000000;
HRESULT hr;
unsigned short u16_IDCode(0);

	msg = (BYTE*)m_pMalloc->Alloc(18);
	if( !msg )
		assert(NULL);
	::memset(msg, 0, sizeof(msg));

	//get the IDCode
	if(FAILED(hr = get_IDCode(&u16_IDCode)))
		return NULL;
	//fill the SYNC code
	msg[0] = 0xAA;
	msg[1] = 0x41;
	//fill the FRAMESIZE= 18
	msg[2] = 0x00;
	msg[3] = 0x12;
//fill the IDCODE of the PDC
	msg[4] = ((u16_IDCode >> 8) & 0xff);
	msg[5] = u16_IDCode  & 0xff;
//fill the SOC
	SOC = (unsigned long)::time(NULL);
	BYTE * ptr(NULL);
	ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
	msg[6] = 0X00; msg[6]  = *(ptr--);
	msg[7] = 0x00; msg[7]  = *(ptr--);
	msg[8] = 0x00; msg[8]  = *(ptr--);
	msg[9] = 0x00; msg[9]  = *(ptr--);
	//fill the FRACSEC
	msg[10] = 0x00;
	msg[11] = 0x00;
	msg[12] = 0x00;
	msg[13] = 0x00;
	//fill CMD which is here 2 send enable the real-time data transmission
	//send a header frame command = 2 = 0x0002	
	msg[14] = 0x00;
	msg[15] = 0x02;

	//compute CRC
	for( int i =0; i < (18 - 2); i++ )
	{
		temp = (crc >> 8) ^ msg[i];
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	//fill the CRC-CCITT
	msg[16] = (crc & 0x0000ff00) >> 8;
	msg[17] = crc & 0x000000ff;

	return (char*)msg;
}

// constructs connection, enable real-time data transmission message
char * CTCPCMU1::constructCntMsg(void)
{
BYTE *msg(NULL);

unsigned int short	crc = 0xFFFF;
unsigned int short  temp;
unsigned int short quick;
unsigned int long  SOC = 0X00000000;

	msg = (BYTE*)m_pMalloc->Alloc(18);
	::memset(msg, 0, sizeof(msg));
	//fill the SYNC code
	msg[0] = 0xAA;
	msg[1] = 0x41;
	//fill the FRAMESIZE= 18
	msg[2] = 0x00;
	msg[3] = 0x12;
	//fill the IDCODE of the PDC
	msg[4] = 0x03;
	msg[5] = 0x09;
	//fill the SOC
	SOC = (unsigned long)::time(NULL);
	BYTE * ptr(NULL);
	ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
	msg[6] = 0X00; msg[6]  = *(ptr--);
	msg[7] = 0x00; msg[7]  = *(ptr--);
	msg[8] = 0x00; msg[8]  = *(ptr--);
	msg[9] = 0x00; msg[9]  = *(ptr--);
	//fill the FRACSEC
	msg[10] = 0x00;
	msg[11] = 0x00;
	msg[12] = 0x00;
	msg[13] = 0x00;
	//fill CMD which is here 2 send enable the real-time data transmission
	msg[14] = 0x00;
	msg[15] = 0x02;

	//compute CRC
	for( int i =0; i < (18 - 2); i++ )
	{
		temp = (crc >> 8) ^ msg[i];
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}
	//fill the CRC-CCITT
	msg[16] = (crc & 0x0000ff00) >> 8;
	msg[17] = crc & 0x000000ff;


	return (char*)msg;
}


// constructs the message for Header frame retrieval 
char * CTCPCMU1::constructHfMsg(void)
{
//this mesage constructed for the 
//iITC BYTE msg[18];
BYTE * msg(NULL);
unsigned int short	crc = 0xFFFF;
unsigned int short  temp;
unsigned int short quick;
unsigned int long  SOC = 0X00000000;

	msg = (BYTE*)m_pMalloc->Alloc(18);
	if( !msg )
		assert(NULL);
	::memset(msg, 0, sizeof(msg));
	//fill the SYNC code
	msg[0] = 0xAA;
	msg[1] = 0x41;
	//fill the FRAMESIZE= 18
	msg[2] = 0x00;
	msg[3] = 0x12;
	//fill the IDCODE of the PDC
	msg[4] = 0x03;
	msg[5] = 0x09;

	//fill the SOC
	SOC = (unsigned long)::time(NULL);
	BYTE * ptr(NULL);
	ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
	msg[6] = 0X00; msg[6]  = *(ptr--);
	msg[7] = 0x00; msg[7]  = *(ptr--);
	msg[8] = 0x00; msg[8]  = *(ptr--);
	msg[9] = 0x00; msg[9]  = *(ptr--);
	//fill the FRACSEC
	msg[10] = 0x00;
	msg[11] = 0x00;
	msg[12] = 0x00;
	msg[13] = 0x00;
	//fill CMD which is here 2 send enable the real-time data transmission
	//send a header frame command = 3 = 0x0003	
	msg[14] = 0x00;
	msg[15] = 0x03;

	//compute CRC
	for( int i =0; i < (18 - 2); i++ )
	{
		temp = (crc >> 8) ^ msg[i];
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	//fill the CRC-CCITT
	msg[16] = (crc & 0x0000ff00) >> 8;
	msg[17] = crc & 0x000000ff;

	return (char*)msg;
}


// constructs the message sent to PDC Server to retrieve Configuration frame1
char * CTCPCMU1::constructCnfg1fMsg(void)
{
//this mesage constructed for the 
//iITC BYTE msg[18];
BYTE * msg(NULL);
unsigned int short	crc = 0xFFFF;
unsigned int short  temp;
unsigned int short quick;
unsigned int long  SOC = 0X00000000;

	msg = (BYTE*)m_pMalloc->Alloc(18);
	if( !msg )
		assert(NULL);
	::memset(msg, 0, sizeof(msg));
	//fill the SYNC code
	msg[0] = 0xAA;
	msg[1] = 0x41;
	//fill the FRAMESIZE= 18
	msg[2] = 0x00;
	msg[3] = 0x12;
	//fill the IDCODE of the PDC
	msg[4] = 0x03;
	msg[5] = 0x09;
	//fill the SOC
	SOC = (unsigned long)::time(NULL);
	BYTE * ptr(NULL);
	ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
	msg[6] = 0X00; msg[6]  = *(ptr--);
	msg[7] = 0x00; msg[7]  = *(ptr--);
	msg[8] = 0x00; msg[8]  = *(ptr--);
	msg[9] = 0x00; msg[9]  = *(ptr--);
	//fill the FRACSEC
	msg[10] = 0x00;
	msg[11] = 0x00;
	msg[12] = 0x00;
	msg[13] = 0x00;
	//fill CMD which is here 2 send enable the real-time data transmission
	//send a header frame command = 4 = 0x0004	
	msg[14] = 0x00;
	msg[15] = 0x04;

	//compute CRC
	for( int i =0; i < (18 - 2); i++ )
	{
		temp = (crc >> 8) ^ msg[i];
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	//fill the CRC-CCITT
	msg[16] = (crc & 0x0000ff00) >> 8;
	msg[17] = crc & 0x000000ff;

	return (char*)msg;
}

// constructs message to retrieve  configuration frame2
char * CTCPCMU1::constructCnfg2fMsg(void)
{
//this mesage constructed for the 
//iITC BYTE msg[18];
BYTE * msg(NULL);
unsigned int short	crc = 0xFFFF;
unsigned int short  temp;
unsigned int short quick;
unsigned int long  SOC = 0X00000000;
HRESULT hr;
unsigned short u16_IDCode(0);

	msg = (BYTE*)m_pMalloc->Alloc(18);
	if( !msg )
		assert(NULL);
	::memset(msg, 0, sizeof(msg));

	//get the IDCode
	if(FAILED(hr = get_IDCode(&u16_IDCode)))
		return NULL;
	//fill the SYNC code
	msg[0] = 0xAA;
	msg[1] = 0x41;
	//fill the FRAMESIZE= 18
	msg[2] = 0x00;
	msg[3] = 0x12;

	msg[4] = ((u16_IDCode >> 8) & 0xff);
	msg[5] = u16_IDCode  & 0xff;

	//fill the SOC
	SOC = (unsigned long)::time(NULL);
	BYTE * ptr(NULL);
	ptr = (BYTE*) &SOC + sizeof(unsigned long int) - 1;
	msg[6] = 0X00; msg[6]  = *(ptr--);
	msg[7] = 0x00; msg[7]  = *(ptr--);
	msg[8] = 0x00; msg[8]  = *(ptr--);
	msg[9] = 0x00; msg[9]  = *(ptr--);
	//fill the FRACSEC
	msg[10] = 0x00;
	msg[11] = 0x00;
	msg[12] = 0x00;
	msg[13] = 0x00;
	//fill CMD which is here 2 send enable the real-time data transmission
	//send a header frame command = 5 = 0x0005	
	msg[14] = 0x00;
	msg[15] = 0x05;

	//compute CRC
	for( int i =0; i < (18 - 2); i++ )
	{
		temp = (crc >> 8) ^ msg[i];
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	//fill the CRC-CCITT
	msg[16] = (crc & 0x0000ff00) >> 8;
	msg[17] = crc & 0x000000ff;

	return (char*)msg;
}

/*
STDMETHODIMP CTCPCMU1::get_SYNC(USHORT* pVal)
{
	// TODO: Add your implementation code here
	unsigned char SYNC[2];
	::memset(SYNC, 0, sizeof(SYNC));
	SYNC[0] = m_bySYNC & 0Xff;
	SYNC[1] = (m_bySYNC >> 8) & 0Xff;

	*pVal = (USHORT)m_bySYNC;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_SYNC(USHORT newVal)
{
	// TODO: Add your implementation code here
	
	AtlTrace("0x%x", newVal & 0xff);
	AtlTrace("0x%x", (newVal >> 8) & 0xff);
	//check SYNC is valid value
	if( 
			(~((newVal ^ 0x01) & ((newVal >> 8) ^ 0xaa))) || //checks Disable Real Time Data Transmission code for SYNC value
			(~((newVal ^ 0x02) & ((newVal >> 8) ^ 0xaa))) || //checks Enable Real Time Data Transmission code for SYNC value
			(~((newVal ^ 0x03) & ((newVal >> 8) ^ 0xaa))) || //checks Configuration Frame2 code for SYNC value
			(~((newVal ^ 0x00) & ((newVal >> 8) ^ 0xaa))) //checks Connection established code for SYNC value
		)
	{
		m_bySYNC = newVal;
		AtlTrace("Success: SYNC value is set properly.\n");
	}
	else
		assert(NULL);

	return S_OK;
}
*/

STDMETHODIMP CTCPCMU1::get_SYNC1(unsigned char* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_chSYNC1;

	return S_OK;
}

STDMETHODIMP CTCPCMU1::put_SYNC1(unsigned char newVal)
{
	// TODO: Add your implementation code here

	AtlTrace("First byte of SYNC is: 0x%02x\n", newVal);
	//check SYNC is valid value
	if( 
			(~((newVal ^ 0xaa))) //checks Disable Real Time Data Transmission code for SYNC value
		)
	{
		m_chSYNC1 = newVal;
		AtlTrace("Success: SYNC value is set properly.\n");
	}
	else
		assert(NULL);

	return S_OK;
}

STDMETHODIMP CTCPCMU1::get_SYNC2(unsigned char* pVal)
{
	// TODO: Add your implementation code here

	*pVal = m_chSYNC2 & 0xff;

	return S_OK;
}

//Binary data returned from PDC Server has a SYNC value that must be
//checked to differentiate between configuration from and Real Time Data
STDMETHODIMP CTCPCMU1::put_SYNC2(unsigned char newVal)
{
	// TODO: Add your implementation code here

	AtlTrace("Second byte of SYNC is: 0x%02x\n", newVal);

	//check SYNC is valid value
	if( 
			(~((newVal ^ 0x01))) || //checks Real Time Data for SYNC value
			(~((newVal ^ 0x31) )) //checks Configuration Frame2 code for SYNC value
		)
	{
		m_chSYNC2 = newVal;
		AtlTrace("Success: SYNC value is set properly.\n");
	}
	else
		assert(NULL);

	return S_OK;
}


// puts binary data vector into this class
DWORD CTCPCMU1::BIN2ASCII(void)
{
	//decode the number of the phasors
	char st[3], *stop;::memset(st, 0, sizeof(st)); 
	::sprintf(st, "%x%x", vec.at(18), vec.at(19));
	USHORT nPMU(0);
	USHORT nIDCODE(0);
	USHORT nFORMAT(0);
	USHORT nPHNMR(0);
	USHORT nANNMR(0);
	USHORT nDGNMR(0);
	USHORT nPHUNIT(0);
	USHORT nANUNIT(0);
	USHORT nDGUNIT(0);
	USHORT nFNOM(0);
	USHORT nCFGCNT(0);
	USHORT nDATA_RATE(0);
	//ICFG2Params object
	HRESULT hr;
	_bstr_t _bstr;

	if(FAILED(hr = m_pInnerUnk->QueryInterface(IID_ICFG2Params, (void**)&pCFG2Params)))
		return 1;
	else
		AtlTrace("Success: ICFG2Params is acquired.\n");

	nPMU = (USHORT)strtol(st,&stop, 16);
	AtlTrace("\n Number of phasors are: %d\n", nPMU);
	
	char stPMU[16];
	char sPMUName[32];
//---------------------------------------------
//iITC	char cc[3];
	char * cc;
	//allocate memory for time stamp decode
	cc = (char*)m_pMalloc->Alloc(6);
	if( !cc )
		assert(NULL);
	::memset(cc, 0, sizeof(cc));
//----------------------------------------------
	::memset(sPMUName, 0, sizeof(sPMUName));
	::memset(stPMU, 0, sizeof(stPMU));
	::memset(cc,0, sizeof(cc));
	int nPos(19);
	ULONG totPHNMR(0);
	ULONG totANNMR(0);

	//nPMU is number of Synchrophasors connected to the PDC
	for(int i = 0; i < nPMU; i++)
	{
		for(int k = 0; k < 16; k++){
				nPos++;
				stPMU[k] = vec.at(nPos);
		}
		//loop to decode the name of the synchrophasor
		for(int j = 0; j < (int)sizeof(stPMU) ;j++)
		{
			if( ::isalpha( stPMU[j]) || ::isdigit(stPMU[j]) )
			{
				sprintf(cc, "%c", stPMU[j]);
				::strcat(sPMUName, cc);
			}
		}
		//insert only PMU name into the PMU name map
		_bstr_t _bstrPMUName = sPMUName;
		if(FAILED( hr = pCFG2Params->InsertPMU(_bstrPMUName.copy())))
			assert(NULL);
		//print PMU Name
		AtlTrace("PURE PMU Name is: %s\n", (const char*)_bstrPMUName);

		//insert / character to the end of the sPMUName string
		::sprintf(cc, "%c", '/');
		::strcat(sPMUName, cc);
		//insert the phasor name into the map
		_bstr = sPMUName;
		_bstr_t _bstr1 = sPMUName;

		AtlTrace("PMU name is: %s\n", sPMUName);
		::memset(stPMU, 0, sizeof(stPMU));
		//read and decode IDCODE
		::memset(sPMUName, 0, sizeof(sPMUName));
		::memset(stPMU, 0, sizeof(stPMU));
		for(int k = 0; k <2; k++){
			nPos++;
			stPMU[k] = vec.at(nPos);
		}
		//loop to decode the name of the synchrophasor
		for(int j = 0; j < 2 ;j++)
		{
			sprintf(cc, "%x", stPMU[j]);
			::strcat(sPMUName, cc);
		}
		nIDCODE = (USHORT)strtol(sPMUName,&stop, 16);
		AtlTrace("IDCODE value is: %d\n", nIDCODE);
		//read & decode FORMAT
		::memset(st, 0, sizeof(st));
		::memset(sPMUName, 0, sizeof(sPMUName));
		for(int k = 0; k < (int)(sizeof(st)-1); k++){
			nPos++;
			st[k] = vec.at(nPos);
		}
		for(int j = 0; j < (int)(sizeof(st)-1) ;j++)
		{
			sprintf(cc, "%x", st[j]);
			::strcat(sPMUName, cc);
		}
		nFORMAT = (USHORT)strtol(cc,&stop, 16);
		AtlTrace("\n FORMAT value is: %d\n", nFORMAT);
		//read & decode PHNMR
		::memset(st, 0, sizeof(st));
		::memset(sPMUName, 0, sizeof(sPMUName));
		for(int k = 0; k < (int)(sizeof(st)-1); k++){
			nPos++;
			st[k] = vec.at(nPos);
		}
		for(int j = 0; j < (int)(sizeof(st)-1) ;j++)
		{
			sprintf(cc, "%x", st[j]);
			::strcat(sPMUName, cc);
		}
		nPHNMR = (USHORT)strtol(sPMUName,&stop, 16);
		AtlTrace("\n PHNMR value is: %d\n", nPHNMR);
		totPHNMR += nPHNMR;	//calculate total number of phasors

		//save the number of phasors for the relevant PMU
		m_retPMU2PHNMR = m_map_PMU2PHNMR.insert(pair<int,int>(i,nPHNMR));
		if(m_retPMU2PHNMR.second == false)
			assert(NULL);

		if(FAILED( hr = pCFG2Params->InsertPMUPHN(_bstrPMUName.copy(), nPHNMR)))
			assert(NULL);

		//read & decode ANNMR
		::memset(st, 0, sizeof(st));
		::memset(sPMUName, 0, sizeof(sPMUName));
		for(int k = 0; k < 2/*(int)(sizeof(st)-1)*/; k++){
			nPos++;
			st[k] = vec.at(nPos);
		}
		for(int j = 0; j < (int)(sizeof(st)-1) ;j++)
		{
			sprintf(cc, "%x", st[j]);
			::strcat(sPMUName, cc);
		}
		nANNMR = (USHORT)strtol(sPMUName,&stop, 16);
		AtlTrace("\n ANNMR value is: %d\n", nANNMR);
		m_retPMU2ANNMR = m_map_PMU2ANNMR.insert(pair<int, int>(i, nANNMR));
		if(m_retPMU2ANNMR.second == false)
			assert(NULL);
		//store number of Analogs for the given PMU			
		if(FAILED( hr = pCFG2Params->InsertPMUANNMR(_bstrPMUName.copy(), nANNMR)))
			assert(NULL);

		//read & decode DGNMR
		::memset(st, 0, sizeof(st));
		::memset(sPMUName, 0, sizeof(sPMUName));
		for(int k = 0; k < 2/*(int)(sizeof(st)-1)*/; k++){
			nPos++;
			st[k] = vec.at(nPos);
		}
		for(int j = 0; j < (int)(sizeof(st)-1) ;j++)
		{
			sprintf(cc, "%x", st[j]);
			::strcat(sPMUName, cc);
		}
		nDGNMR = (USHORT)strtol(sPMUName,&stop, 16);
		AtlTrace("\n DGNMR value is: %d\n", nDGNMR);
		m_retPMU2DGNMR = m_map_PMU2DGNMR.insert(pair<int, int>(i, nDGNMR));
		if(m_retPMU2DGNMR.second == false)
			assert(NULL);

		//store number of Analogs for the given PMU			
		if(FAILED( hr = pCFG2Params->InsertPMUDGNMR(_bstrPMUName.copy(), nDGNMR)))
			assert(NULL);

		//read and decode CHNAMs//--------------------HERE CHANGED 
		for(int n = 0; n < nPHNMR; n++)
		{
			::memset(sPMUName, 0, sizeof(sPMUName));
			::memset(stPMU, 0, sizeof(stPMU));
			for(int k = 0; k < 16; k++){
				nPos++;
 				stPMU[k] = vec.at(nPos);
			}
			//loop to decode the name of the synchrophasor
			for(int j = 0; j < (int)sizeof(stPMU) ;j++)
			{
				if( ::isalpha( stPMU[j]) || ::isdigit(stPMU[j]) )
				{
					sprintf(cc, "%c", stPMU[j]);
					::strcat(sPMUName, cc);
				}
			}
			if(FAILED(hr = pCFG2Params->InsertPhasor( _bstr1.copy())))
				assert(NULL);
			//append the phasor name to the PMU/ name
			_bstr = (LPCSTR)sPMUName;
			//firstly resize the the vector to include the phasor to the end of it
			if(FAILED( hr = pCFG2Params->ResizePhasorAndAppend(_bstr.length(), _bstr.copy())))
				assert(NULL);
			else
				AtlTrace("PMUNAME/PHASOER name is: %s\n", sPMUName);
			_bstr = "";
			::memset(sPMUName, 0, sizeof(sPMUName));
			//secondly move forward in the PMUNAME/PHASOR map
			if(FAILED( hr = pCFG2Params->ResizePhasorAndAppend(0, _bstr.copy())))
				AtlTrace("m_curItem++ is performed.\n");
		}

//**********************************************
		//read and decode Analog CHNAMs
		for(int m = 0; m < nANNMR; m++)
		{
			//loop and extract the names of the analog channels
			::memset(sPMUName, 0, sizeof(sPMUName));
			::memset(stPMU, 0, sizeof(stPMU));
			for(int k = 0; k < 16; k++){
				nPos++;
 				stPMU[k] = vec.at(nPos);
			}
			//loop to decode the name of the synchrophasor
			for(int j = 0; j < (int)sizeof(stPMU) ;j++)
			{
				if( ::isalpha( stPMU[j]) || ::isdigit(stPMU[j]) )
				{
					sprintf(cc, "%c", stPMU[j]);
					::strcat(sPMUName, cc);
				}
			}
			AtlTrace("Analog channel name is: %s\n", sPMUName);
			if(FAILED(hr = pCFG2Params->InsertAnalog( _bstr1.copy())))
				assert(NULL);
			//append the phasor name to the PMU/ name
			_bstr = (LPCSTR)sPMUName;
			//firstly resize the the vector to include the phasor to the end of it
			if(FAILED( hr = pCFG2Params->ResizeAnalogAndAppend(_bstr.length(), _bstr.copy())))
				assert(NULL);
			else
				AtlTrace("PMUNAME/PHASOER name is: %s\n", sPMUName);
			_bstr = "";
			::memset(sPMUName, 0, sizeof(sPMUName));
			//secondly move forward in the PMUNAME/PHASOR map
			if(FAILED( hr = pCFG2Params->ResizeAnalogAndAppend(0, _bstr.copy())))
				AtlTrace("m_curItem++ is performed.\n");
		}

		//read and decode Digital CHNAMs
		for(int m = 0; m < nDGNMR; m++)
		{
			//loop and extract the names of the analog channels
			::memset(sPMUName, 0, sizeof(sPMUName));
			::memset(stPMU, 0, sizeof(stPMU));
			for(int k = 0; k < 16; k++){
				nPos++;
 				stPMU[k] = vec.at(nPos);
			}
			//loop to decode the name of the synchrophasor
			for(int j = 0; j < (int)sizeof(stPMU) ;j++)
			{
				if( ::isalpha( stPMU[j]) || ::isdigit(stPMU[j]) )
				{
					sprintf(cc, "%c", stPMU[j]);
					::strcat(sPMUName, cc);
				}
			}
			AtlTrace("Analog channel name is: %s\n", sPMUName);
			if(FAILED(hr = pCFG2Params->InsertDigital( _bstr1.copy())))
				assert(NULL);
			//append the phasor name to the PMU/ name
			_bstr = (LPCSTR)sPMUName;
			//firstly resize the the vector to include the phasor to the end of it
			if(FAILED( hr = pCFG2Params->ResizeDigitalAndAppend(_bstr.length(), _bstr.copy())))
				assert(NULL);
			else
				AtlTrace("PMUNAME/PHASOER name is: %s\n", sPMUName);
			_bstr = "";
			::memset(sPMUName, 0, sizeof(sPMUName));
			//secondly move forward in the PMUNAME/PHASOR map
			if(FAILED( hr = pCFG2Params->ResizeDigitalAndAppend(0, _bstr.copy())))
				AtlTrace("m_curItem++ is performed.\n");
		}

//**********************************************
		//read and decode PHUNITs
		for(int n = 0; n < nPHNMR; n++)
		{
			::memset(sPMUName, 0, sizeof(sPMUName));
			::memset(stPMU, 0, sizeof(stPMU));
			for(int k = 0; k < 4; k++){
				nPos++;
				stPMU[k] = vec.at(nPos);
			}
			//loop to decode the name of the synchrophasor
			for(int j = 0; j < 4 ;j++)
			{
				sprintf(cc, "%x", stPMU[j]);
				::strcat(sPMUName, cc);
			}
			nPHUNIT = (USHORT)strtol(sPMUName,&stop, 16);
			AtlTrace("PHUNIT value is: %d\n", nPHUNIT);
			vector<int>	phvec(2, 0);
			phvec[0] = n;	//phasor index for given PMU
			phvec[1] = nPHUNIT;	//PMU index
			//insert PHUNIT of related phasor into the map for real-time phasor value calculations
			m_retPH2PHUNIT = m_map_PH2PHUNIT.insert(pair<int, vector<int> >((totPHNMR-nPHNMR+n), phvec));
			if(m_retPH2PHUNIT.second == false)
				assert(NULL);
		}
		//read and decode ANUNITs
		for(int n = 0; n < nANNMR; n++)
		{
			::memset(sPMUName, 0, sizeof(sPMUName));
			::memset(stPMU, 0, sizeof(stPMU));
			for(int k = 0; k < 4; k++){
				nPos++;
				stPMU[k] = vec.at(nPos);
			}
			//loop to decode the name of the synchrophasor
			for(int j = 0; j < 4 ;j++)
			{
				sprintf(cc, "%x", stPMU[j]);
				::strcat(sPMUName, cc);
			}
			nANUNIT = (USHORT)strtol(sPMUName,&stop, 16);
			AtlTrace("ANUNIT value is: %d\n", nANUNIT);

			vector<int> anvec(2,0);
			anvec[0] = n;	//analog signal index for given PMU
			anvec[1] = nANUNIT;	//PMU index
			//insert the ANUNIT value for related analog signal into this map
			m_retAN2ANUNIT = m_map_AN2ANUNIT.insert(pair<int, vector<int> >((totANNMR-nANNMR+n), anvec));
			if(m_retAN2ANUNIT.second == false)
				assert(NULL);
		}
		//read and decode DGUNITs
		for(int n = 0; n < nDGNMR; n++)
		{
			::memset(sPMUName, 0, sizeof(sPMUName));
			::memset(stPMU, 0, sizeof(stPMU));
			for(int k = 0; k < 4; k++){
				nPos++;
				stPMU[k] = vec.at(nPos);
			}
			//loop to decode the name of the synchrophasor
			for(int j = 0; j < 4 ;j++)
			{
				sprintf(cc, "%x", stPMU[j]);
				::strcat(sPMUName, cc);
			}
			nDGUNIT = (USHORT)strtol(sPMUName,&stop, 16);
			AtlTrace("DGUNIT value is: %d\n", nDGUNIT);
		}
		//read and decode FNOM
		::memset(sPMUName, 0, sizeof(sPMUName));
		::memset(stPMU, 0, sizeof(stPMU));
		for(int k = 0; k <2; k++){
			nPos++;
			stPMU[k] = vec.at(nPos);
		}
		//loop to decode the name of the synchrophasor
		for(int j = 0; j < 2 ;j++)
		{
			sprintf(cc, "%x", stPMU[j]);
			::strcat(sPMUName, cc);
		}
		nFNOM = (USHORT)strtol(sPMUName,&stop, 16);
		AtlTrace("FNOM value is: %d\n", nFNOM);
		//read and decode CFGCNT(Configuration change count), check it to update the configuration!!!!!!!
		::memset(sPMUName, 0, sizeof(sPMUName));
		::memset(stPMU, 0, sizeof(stPMU));
		for(int k = 0; k <2; k++){
			nPos++;
			stPMU[k] = vec.at(nPos);
		}
		//loop to decode the name of the synchrophasor
		for(int j = 0; j < 2 ;j++)
		{
			sprintf(cc, "%x", stPMU[j]);
			::strcat(sPMUName, cc);
		}
		nCFGCNT = (USHORT)strtol(sPMUName,&stop, 16);
		AtlTrace("CFGCNT value is: %d\n", nCFGCNT);
		::memset(sPMUName, 0, sizeof(sPMUName));
		::memset(stPMU, 0, sizeof(stPMU));
	}//end-of-PMUs loop
	//read and decode DATA_RATE
	::memset(sPMUName, 0, sizeof(sPMUName));
	::memset(stPMU, 0, sizeof(stPMU));
	for(int k = 0; k <2; k++){
		nPos++;
		stPMU[k] = vec.at(nPos);
	}
	//loop to decode the name of the synchrophasor
	for(int j = 0; j < 2 ;j++)
	{
		sprintf(cc, "%x", stPMU[j]);
		::strcat(sPMUName, cc);
	}
	nDATA_RATE = (USHORT)strtol(sPMUName,&stop, 16);
	AtlTrace("DATA_RATE value is: %d\n", nDATA_RATE);

//-----------------------------------------------------
	m_pMalloc->Free(cc);
//------------------------------------------------------
	return 0;
}

//CTPCMU1: stands for Class TCP Communication Unit1
// puts binary data vector into this class
DWORD CTCPCMU1::BIN2ASCII_RT(void)
{
	HRESULT hr;
	char * st_ts, *stop, *st_phval;
	int n(0);	//the index of the binary data array(vec)
	ULONG u_32_TS_H(0);	//stores SOC
	ULONG u_32_TS_L(0); //stores FRACSEC
	ULARGE_INTEGER u_64_TS;
	SYSTEMTIME u_128_TS;

	//allocate memory for time stamp decode
	st_ts = (char*)m_pMalloc->Alloc(6);
	if( !st_ts )
		assert(NULL);

	//allocate memory for phasor value decode
	st_phval = (char*)m_pMalloc->Alloc(16);
	if( !st_phval )
		assert(NULL);

//	::memset(&st, 0, sizeof(st));
	::memset(&u_64_TS, 0, sizeof(u_64_TS));
	::memset(&u_128_TS, 0, sizeof(u_128_TS));
	::memset(st_ts, 0, sizeof(st_ts));

	//skip part of the following parts of the real-time message
	//1. SYNC 2. FRAMESIZE 3. IDCODE
	n += 6;
	//write two byte into a st and 
	DWORD dwRes = ::sprintf(st_ts, "%02x%02x%02x%02x", (unsigned char)vec1.at(n), (unsigned char)vec1.at(n+1), 
		(unsigned char)vec1.at(n+2), (unsigned char)vec1.at(n+3));
	if( dwRes < 0 )
		assert(NULL);
	
	//SOC conversion to text format
	u_32_TS_H = (ULONG)strtol(st_ts,&stop, 16);
	AtlTrace("\n SOC is: %d\n", u_32_TS_H);
	
	//advance 4 bytes
	n += 4;
	::memset(st_ts, 0, sizeof(st_ts));
	//FRACSEC conversion to text format
	//write two byte into a st and 
	dwRes = ::sprintf(st_ts, "%02x%02x%02x", (unsigned char)vec1.at(n+1), (unsigned char)vec1.at(n+2), 
		(unsigned char)vec1.at(n+3));

	if( dwRes < 0 )
		assert(NULL);

	AtlTrace("===========================FRACSEC BITS0-BITS3 are %02x\n", (unsigned char)vec1.at(n));
	u_32_TS_L = (ULONG)strtol(st_ts,&stop, 16);
	AtlTrace("\n FRACSEC is: %d\n", u_32_TS_L);
	time_t TimeStamp;
	TimeStamp = (u_32_TS_H + u_32_TS_L / 1000000);	//TIME_BASE = 1000000 SO use the CFG2 to fill in this as a parameter not a constant

//--------------------------------------------------------------------
//	::memcpy(&m_ts1, &m_ts2, sizeof(time_t));	//copy the old time stamp to the new time stamp.
//	::memcpy(&m_ts2, &TimeStamp, sizeof(time_t));	//copy the new time stamp to the m_ts2
	//find the diffrence between two time stamps
//	if( !TSDiff(m_ts1, m_ts2) )
//		return -1;

//	m_tsfirst = m_tsSecond;
//	m_tsSecond = (u_32_TS_L/1000);	//this is actual fraction second in millisecond.
//	if( (m_tsSecond - m_tsfirst) > m_u32_RefreshRate )
//	{
//		ContinuePacketReading();
//		return -1;
//	}
//--------------------------------------------------------------------

//--------------------------------------
	SYSTEMTIME syst; ::memset(&syst, 0, sizeof(syst));
	double dwt(0);
	COleDateTime dt(TimeStamp);
	dt.GetAsSystemTime(syst);
	::SystemTimeToVariantTime(&syst, &dwt);
/*	VARIANT var;
	::VariantClear(&var);
	var.vt = VT_DATE;
	var.date = dt;
	if( pVI )
		pVI->SetControlValue("Sample Time", var);
*/
//---------------------------------------------------

	struct tm *newtime;
	char am_pm[] = "AM";

	// time_t long_time=1229521513;
	//Here is where we start the time conversion from seconds and (ms) to readable format, Date, time, etc.
		time_t long_time=TimeStamp;

	newtime = localtime(&long_time);
	if(newtime->tm_hour>12)
	strcpy(am_pm,"PM");
	if(newtime->tm_hour>12)
	newtime->tm_hour-=12;
	if(newtime->tm_hour ==0)
	newtime->tm_hour=12;

	char * pTS(NULL);
	
	AtlTrace("%.19s %s, %i\n", asctime(newtime),am_pm, 1900+newtime->tm_year);
	pTS = asctime(newtime);
	AtlTrace("%s\n", pTS);

	AtlTrace("%d:%d:%d\n", newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
	AtlTrace("%d-%d-%d\n", 1900+newtime->tm_year, 1+newtime->tm_mon, newtime->tm_mday);
	//free allocated memory of the heap.
//take care of it	m_pMalloc->Free(st_ts);
	
	//save time stamp paramters
	if(FAILED( hr = m_pRTDTParams->put_TS_Year(1900+newtime->tm_year)))
		return -1;

	if(FAILED( hr = m_pRTDTParams->put_TS_Month(1+newtime->tm_mon)))
		return -1;

	if(FAILED( hr = m_pRTDTParams->put_TS_Day(newtime->tm_mday)))
		return -1;

	if(FAILED( hr = m_pRTDTParams->put_TS_Hour(newtime->tm_hour)))
		return -1;

	if(FAILED( hr = m_pRTDTParams->put_TS_Minute(newtime->tm_min)))
		return -1;

	if(FAILED( hr = m_pRTDTParams->put_TS_Second(newtime->tm_sec)))
		return -1;

	if(FAILED( hr = m_pRTDTParams->put_Millisecond( u_32_TS_L )))
		return -1;
	
	if(FAILED( hr = m_pRTDTParams->put_TS_OLE_VAR(dwt)))
		return -1;
//----------------------------------Time stamp decoding ends here------------------------------------------

//----------------------------------Phasor decoding starts here -------------------------------------------
	AtlTrace("Decoding of the phasors starts here.\n");
	ULONG	u32_PhNum(100);
	ULONG	nOffset(0);		//this offset to mark the requested phasor
	ULONG	nOffsetFDQANDG(0);	//this offset to mark the FREQ, DFREQ, ANALOG, DIGITAL signals
	ULONG	u32_nPMUs (0), u_32nPMUIndex(0);
	ULONG	u32_nPhasors(0);
	ULONG	u32_MagAng(0);
	int		k(0);
	long	lPHXVAL(0);
	long	lPHYVAL(0);
	float	flPHXVAL(0.0);
	float	flPHYVAL(0.0);
	long	lFQVAL(0);
	long	lDFQVAL(0);
	long	lAngVal(0);
	long	lDigVal(0);
	int		nph(0);	//number of bytes of the real-time data packet to count to reach to the correct phasor value byte
	int		nFDQ(0);	//number of bytes of the real-time data packet to count to reach to the correct FREQ, DFREQ byte
	int		nAD(0), nAD1(0);	//number of bytes of the real-time data packet to count to reach to the correct Analog, Digital Signal byte
	char	FQ[4];
	float	flFQVal(0.0);
	float	flDFQVal(0.0);
	int		nTotAnalogs(0);
	int		nTotDigitals(0);

	//find the number of the PMUS
	if(FAILED(hr = pCFG2Params->get_NoofPMUs(&u32_nPMUs)))
		return -1;

	if( m_map_phdevals.size() > 0)
	{
		for( mphdeIt = m_map_phdevals.begin(); mphdeIt != m_map_phdevals.end(); mphdeIt++)
		{
			nph = n;
			nFDQ = n;
			u32_PhNum = ((*mphdeIt).second)[1];
			u_32nPMUIndex = ((*mphdeIt).second)[2];
			u32_MagAng = ((*mphdeIt).second)[0];
			u32_PhNum += 1;

			AtlTrace("\n\nThe phasor number in list of phasor is: %d\n", u32_PhNum);
			AtlTrace("\n\nPMU Number: %d ---is selected &: %c ---is displayed for phasor number: %d\n\n\n", (u_32nPMUIndex+1), (u32_MagAng == 0) ? 'M' : 'A', u32_PhNum);

			//advance untill to reach to the phasor
			nph += 4; //four bytes FRACSEC
			nFDQ += 4;	//four bytes FRACSEC 
			//advance until you reach to the FRE
			//advance through all previous PMUs to reach the one
			for( k=0; (k < (int)u_32nPMUIndex) && (u_32nPMUIndex > 0); k++)
			{
				nOffset += 2;	//2 bytes of the STAT
				u32_nPhasors = m_map_PMU2PHNMR[k];	//map PMU index to number of phasor in the PMU
				nOffset += u32_nPhasors * 8;		//advanced as much as number of phasor times 8 byte
				nOffset += 4; //FREQ size is 4 byte and advnance 4 bytes
				nOffset += 4; //DFREQ size is 4 bytes and advance 4 bytes
				//subtract the phasors of this PMU from the phasor number of 
				//target phasor that user selected to monitor and display in the VI
				u32_PhNum -= u32_nPhasors;
			}

			//FREQ, DFREQ offset calculations
			nOffsetFDQANDG = nOffset;

			//continue to reach the right phasor if for one PMU case
			if( !k )
			{
				nOffset += 2; //2 bytes of the STAT
				nOffsetFDQANDG += 2;	//2 bytes of the STAT
				//the phasor selected by the user to investigate is in this PMU
				u32_nPhasors = m_map_PMU2PHNMR[k];	//map PMU index to number of phasors in the PMU

				//check whether selected phasor number is less than number of phasors of this PMU
				if( u32_PhNum <= u32_nPhasors )
					for( k = 0; k < (int)(u32_PhNum - 1); k++)
						nOffset += 8;
				else
					AtlTrace("\nThere is no phasor with number : %d does not exit\n", u32_PhNum);

				//advance to the end of the phasor values of this PMU, because the FREQ, DFREQ are after that
				nOffsetFDQANDG += (u32_nPhasors * 8);
			}
			else if( k > 0 )//continue to reach the right phasor if more than one PMU
			{
				nOffset += 2; //2 bytes of the STAT
				nOffsetFDQANDG += 2; //2 bytes of the STAT

				//the phasor selected by the user to investigate is in this PMU
				u32_nPhasors = m_map_PMU2PHNMR[k];	//map PMU index to number of phasors in the PMU

				//check whether selected phasor number is less than number of phasors of this PMU
				if( u32_PhNum <= u32_nPhasors )
					for( k = 0; k < (int)(u32_PhNum -1 ); k++)
						nOffset += 8;
				else
					AtlTrace("\nThere is no phasor with number : %d does not exit\n", u32_PhNum);
				
				//advance to the end of the phasor values of this PMU, because the FREQ, DFREQ are after that
				nOffsetFDQANDG += (u32_nPhasors * 8);
			}
			else
				assert(NULL);
//iITC
			AtlTrace("start of the phasor value for selected phasor: %d of PMU number: %d is: %d\n", u32_PhNum, (u_32nPMUIndex + 1), (nOffset + nph));
			nph += nOffset;
			AtlTrace("\n\n\nPhasor value in binary is: %02x%02x%02x%02x%02x%02x%02x%02x\n", (unsigned char)vec1.at(nph), (unsigned char)vec1.at(nph+1), 
													(unsigned char)vec1.at(nph+2), (unsigned char)vec1.at(nph+3), (unsigned char)vec1.at(nph+4),
													(unsigned char)vec1.at(nph+5), (unsigned char)vec1.at(nph+6), (unsigned char)vec1.at(nph+7));

			AtlTrace("start of the FREQ, DFREQ values for selected phasor: %d of PMU number: %d is: %d\n", u32_PhNum, (u_32nPMUIndex + 1), (nOffsetFDQANDG + nFDQ));
			nFDQ += nOffsetFDQANDG;
			AtlTrace("\n\n\FREQ, DFREQ values in binary is: %02x%02x%02x%02x%02x%02x%02x%02x\n", (unsigned char)vec1[nFDQ], (unsigned char)vec1[nFDQ+1], 
													(unsigned char)vec1[nFDQ+2], (unsigned char)vec1[nFDQ+3], (unsigned char)vec1[nFDQ+4],
													(unsigned char)vec1[nFDQ+5], (unsigned char)vec1[nFDQ+6], (unsigned char)vec1[nFDQ+7]);
	
			//convert the binary to text format
			::memset(st_phval, 0, sizeof(st_phval));
			dwRes = ::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nph), (unsigned char)vec1.at(nph+1), 
				(unsigned char)vec1.at(nph+2), (unsigned char)vec1.at(nph+3));
			if( dwRes < 0 )
				assert(NULL);

			//calculate the phasor value , factor in the conversion factor
			map<int, vector<int> >::iterator it = m_map_PH2PHUNIT.find(u32_PhNum-1);
			int phconvf = ((*it).second)[1];
			AtlTrace("phasor number is: %d-----phasor conversion factor is: %d\n", u32_PhNum, phconvf);

			//convert the x value of the phasor from binary text to long value
			lPHXVAL = strtol(st_phval,&stop, 16);
			flPHXVAL = IEEE_754_to_float(st_phval, nph);

			::memset(st_phval, 0, sizeof(st_phval));
			::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nph+4), (unsigned char)vec1.at(nph+5), 
									(unsigned char)vec1.at(nph+6), (unsigned char)vec1.at(nph+7));
			lPHYVAL = strtol(st_phval,&stop,16);
			flPHYVAL = IEEE_754_to_float(st_phval, nph);

			AtlTrace("\n\n\nPhasor X value is : %16.14f phasor Y value is: %16.14f\n\n\n", flPHXVAL, flPHYVAL);
		
			//set the Phasor X Value
			if(FAILED( hr = m_pRTDTParams->put_PhasorXVal((float)flPHXVAL)))
				return -1;

			//set the Phasor Y Value
			if(FAILED( hr = m_pRTDTParams->put_PhasorYVal((float)flPHYVAL)))
				return -1;

			//set the Phasor Magnitude/Angle phasor type
			if(FAILED( hr = m_pRTDTParams->put_MagAng((float)u32_MagAng)))
				return -1;

			//set all of the values into the phasor value list
			if(FAILED( hr = m_pRTDTParams->SavePhVals(-1, -1, -1)))
				return -1;

			//convert the binary to text format FREQ
			::memset(st_phval, 0, sizeof(st_phval));

			dwRes = ::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nFDQ), (unsigned char)vec1.at(nFDQ+1), 
				(unsigned char)vec1.at(nFDQ+2), (unsigned char)vec1.at(nFDQ+3));
			if( dwRes < 0 )
				assert(NULL);

			//convert FREQ value of the phasor from binary text to long value
			lFQVAL = strtol(st_phval,&stop, 16);

			//convert IEEE hex to float
			flFQVal = IEEE_754_to_float(st_phval, nFDQ);
			AtlTrace(" FREQ value is: %16.14f\n", flFQVal);

			//convert the binary to text format DFREQ
			::memset(st_phval, 0, sizeof(st_phval));
			::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nFDQ+4), (unsigned char)vec1.at(nFDQ+5), 
									(unsigned char)vec1.at(nFDQ+6), (unsigned char)vec1.at(nFDQ+7));

			lDFQVAL = strtol(st_phval,&stop,16);
			flDFQVal = IEEE_754_to_float(st_phval, nFDQ);
			AtlTrace("DFREQ value is: %16.14f\n", flDFQVal);
			//save the FREQ value
			if(FAILED( hr = m_pRTDTParams->put_FREQ((float)flFQVal)))
				return -1;

			//save the FREQ value
			if(FAILED( hr = m_pRTDTParams->put_DFREQ((float)flDFQVal)))
				return -1;
	/*iITC
			//convert the analog and digital values
			nFDQ += 8;	//advance 8 bytes to reach to the end of FREQ/DFREQ
			//convert the binary to text format Analog
			//check the number of the Analog signals
			if( m_map_PMU2ANNMR[k] > 0 )
			{
				::memset(st_phval, 0, sizeof(st_phval));
				dwRes = ::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nFDQ), (unsigned char)vec1.at(nFDQ+1), 
					(unsigned char)vec1.at(nFDQ+2), (unsigned char)vec1.at(nFDQ+3));
				if( dwRes < 0 )
					assert(NULL);
				//convert Analog value of the phasor from binary text to long value
				lAngVal = strtol(st_phval,&stop, 16);
			}
			else
			{
				AtlTrace("\nThere were no analog signals.\n");
				lAngVal = 0.0;
			}

			//convert the binary to text format Digital
			//check the number of digital signals
			if( m_map_PMU2DGNMR[k] > 0 )
			{
				::memset(st_phval, 0, sizeof(st_phval));
				::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nFDQ+4), (unsigned char)vec1.at(nFDQ+5), 
										(unsigned char)vec1.at(nFDQ+6), (unsigned char)vec1.at(nFDQ+7));

				//convert Digital value of the phasor from binary text to long value
				lDigVal = strtol(st_phval,&stop,16);
			}
			else
			{
				AtlTrace("\nThere were no digital signal.\n");
				lDigVal = 0.0;
			}

			//save the Analog value
			if(FAILED( hr = m_pRTDTParams->put_Analog((float)lAngVal)))
				return -1;

			//save the Digital value
			if(FAILED( hr = m_pRTDTParams->put_Digital((float)lDigVal)))
				return -1;
				*/
			//save the FREQ value
			if(FAILED( hr = m_pRTDTParams->SaveFDQANDGVals()))
				return -1;
//--------------------------------------------------------------Analog signal-----------------------------------------------------------------------
	ULONG	u32_AnalogNum(100);
	ULONG	u32_DigitalNum(100);
	ULONG	u32_nAnalogs(0);	//number of analog signals
	ULONG	u32_nDigitals(0);	//number of digital signals
	float	flAnalogVal(0.0);		//value of analog signal 
	float	flDigitalVal(0.0);		//value of analog signal 
	long	lAnalogVal(0);		//presentation of this value is unknown until further discussion
	long	lDigitalVal(0);		//presentation of this value is unknown until further discussion
	
	//advance untill to reach to the analog signal
	nFDQ += 8;
	nAD = nFDQ; //four bytes FRACSEC

	//find the number of the PMUS
	if(FAILED(hr = pCFG2Params->get_NoofPMUs(&u32_nPMUs)))
		return -1;

//-----------------------------------Analog and digital signals encoding starts here--------------------------------------------
	for(m_AnadeIt = m_map_Anadevals.begin(); m_AnadeIt != m_map_Anadevals.end(); m_AnadeIt++)
	{
		u32_AnalogNum = ((*m_AnadeIt).second)[0];
		u32_DigitalNum = ((*m_AnadeIt).second)[1];
		if(u32_AnalogNum == 100000)
			continue;

		u_32nPMUIndex = ((*m_AnadeIt).second)[2];
		u32_AnalogNum += 1;

		AtlTrace("\n\nThe Analog signal number in list of Analog parameters is: %d\n", u32_AnalogNum);
		AtlTrace("\n\nPMU Number: %d ---is selected Analog number: %d\n\n\n", (u_32nPMUIndex+1), u32_PhNum);

		//advance until you reach to the FRE
		//advance through all previous PMUs to reach the one
//		nAD +=1;	//advance to the begginig of the Analog signal

		AtlTrace("\n\n\Analog, DFREQ values in binary is: %02x%02x%02x%02x\n", (unsigned char)vec1[nAD+1], (unsigned char)vec1[nAD+2], 
												(unsigned char)vec1[nAD+3], (unsigned char)vec1[nAD+4]);

		::memset(st_phval, 0, sizeof(st_phval));
		dwRes = ::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nAD+1), (unsigned char)vec1.at(nAD+2), 
			(unsigned char)vec1.at(nAD+3), (unsigned char)vec1.at(nAD+4));
		if( dwRes < 0 )
			assert(NULL);

		lAnalogVal = strtol(st_phval,&stop, 16);
		//convert the binary to floating point value
		if( lAnalogVal != 0)
			flAnalogVal = IEEE_754_to_float(st_phval, nph);
		else
			flAnalogVal = 0.0;

		AtlTrace("\nAnalog signal value is: %16.14f \n\n", flAnalogVal);
		//set the Phasor X Value
		if(FAILED( hr = m_pRTDTParams->put_Analog((float)flAnalogVal)))
			return -1;

		//save the values in the Analog and digital values.
		if(FAILED(hr = m_pRTDTParams->SaveADVals()))
			return -1;

		//reset the real-time data markers to zero
		nAD += 4;
	}//end of analog & digital signals

//-----------------------------------------------------------Digital Signal-----------------------------------------------------
//iITC	if( vec1.size() > (nAD1+7) )
//iITC	{
	ULONG u32_NoofDigitals(0);
	if(FAILED(hr = get_NoofChDigital(&u32_NoofDigitals)))
		return -1;

	//find max number of analogs
	if(FAILED(hr = get_NumAnalogs((long*)&nTotAnalogs)))
		return -1;
	
	//subtract it from m_map_Anadevals.size() value
	//add the result to the nAD to reach to the begginig of the digital signal value
	nAD += (nTotAnalogs - m_map_Anadevals.size() + u32_NoofDigitals) * 4/*size of one analog signal*/;

	for(int analog = 0; analog < u32_NoofDigitals; analog++)
	{
		AtlTrace("\n\n\Digital value in binary is: %02x%02x\n", (unsigned char)vec1[nAD+1], (unsigned char)vec1[nAD+2]);

		::memset(st_phval, 0, sizeof(st_phval));
		dwRes = ::sprintf(st_phval, "%02x%02x", (unsigned char)vec1.at(nAD+1), (unsigned char)vec1.at(nAD+2));
		if( dwRes < 0 )
			assert(NULL);

		lDigitalVal = strtol(st_phval,&stop, 16);
		//convert the binary to floating point value
		if( lDigitalVal != 0)
			flDigitalVal = IEEE_754_to_float(st_phval, nph);
		else
			flDigitalVal = 0.0;

		AtlTrace("\Digital signal value is: %16.14f \n\n", flDigitalVal);
		//set the Phasor X Value
		if(FAILED( hr = m_pRTDTParams->put_Digital((float)flDigitalVal)))
			return -1;

		//save the values in the Analog and digital values.
		if(FAILED(hr = m_pRTDTParams->SaveDigVals()))
			return -1;

		//store first byte of digital signal extracted from c37.118 data packet 
		if( FAILED( hr = m_pRTDTParams->put_HexAnalog(vec1.at(nAD+1))))
			return -1;

		//store second byte of digital signal extraceted from c37.118 data packet
		if(FAILED(hr = m_pRTDTParams->put_HexAnalog(vec1.at(nAD+2))))
			return -1;

		//convert digital signal hex value to binary string
		if(FAILED(hr = m_pRTDTParams->DigHex2BinStr()))
			return -1;

		//save the digital binary string in a list 
		//use this list in the DecodeAngDig
		if(FAILED(hr = m_pRTDTParams->SaveDigHexVals()))
			return -1;

		nAD +=1;
	}
//------------------------------------------------------------------------------------------------------------------------------
			//reset the real-time data markers to zero
			nOffset = 0;
			nph = 0;
		}//end of loop for selected phasors
	}
	else//no phasors are selected
	{
//--------------------------------------------------------------Analog signal-----------------------------------------------------------------------
		ULONG	u32_AnalogNum(100);
		ULONG	u32_DigitalNum(100);
		ULONG	u32_nAnalogs(0);	//number of analog signals
		ULONG	u32_nDigitals(0);	//number of digital signals
		float	flAnalogVal(0.0);		//value of analog signal 
		float	flDigitalVal(0.0);		//value of analog signal 
		long	lAnalogVal(0);		//presentation of this value is unknown until further discussion
		long	lDigitalVal(0);		//presentation of this value is unknown until further discussion
		ULONG	nTotAnalogs(0);		//
//-----------------------------------Analog and digital signals encoding starts here--------------------------------------------

		//find max number of analogs
		if(FAILED(hr = get_NumAnalogs((long*)&nTotAnalogs)))
			return -1;

		if(m_map_Anadevals.size() > 0)
		{
			for(m_AnadeIt = m_map_Anadevals.begin(); m_AnadeIt != m_map_Anadevals.end(); m_AnadeIt++)
			{
				nph = n;
				nFDQ = n;
				nAD = n;
				//advance untill to reach to the phasor
				nAD += 4;

				u32_AnalogNum = ((*m_AnadeIt).second)[0];
				u32_DigitalNum = ((*m_AnadeIt).second)[1];
				if(u32_AnalogNum == 100000)
					continue;

				u_32nPMUIndex = ((*m_AnadeIt).second)[2];
				u_32nPMUIndex += 1;

				AtlTrace("\n\nThe Analog signal number in list of Analog parameters is: %d\n", u32_AnalogNum);
				AtlTrace("\n\nPMU Number: %d ---is selected Analog number: %d\n\n\n", (u_32nPMUIndex+1), u32_PhNum);

				//advance until you reach to the FRE
				//advance through all previous PMUs to reach the one
				for( k=0; (k < (int)u_32nPMUIndex) && (u_32nPMUIndex > 0); k++)
				{
					nOffset += 2;	//2 bytes of the STAT
					u32_nPhasors = m_map_PMU2PHNMR[k];	//map PMU index to number of phasor in the PMU
					nOffset += u32_nPhasors * 8;		//advanced as much as number of phasor times 8 byte
					nOffset += 4; //FREQ size is 4 byte and advnance 4 bytes
					nOffset += 4; //DFREQ size is 4 bytes and advance 4 bytes
		
					nAD += u32_AnalogNum * 4;	//4 bytes the size of analog signal

					AtlTrace("\n\n\Analog, DFREQ values in binary is: %02x%02x%02x%02x\n", (unsigned char)vec1[nAD+nOffset+1], (unsigned char)vec1[nAD+nOffset+2], 
															(unsigned char)vec1[nAD+nOffset+3], (unsigned char)vec1[nAD+nOffset+4]);

					::memset(st_phval, 0, sizeof(st_phval));
					dwRes = ::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nAD+nOffset+1), (unsigned char)vec1.at(nAD+nOffset+2), 
						(unsigned char)vec1.at(nAD+nOffset+3), (unsigned char)vec1.at(nAD+nOffset+4));
					if( dwRes < 0 )
						assert(NULL);

					lAnalogVal = strtol(st_phval,&stop, 16);

					//convert the binary to floating point value
					if( lAnalogVal != 0)
						flAnalogVal = IEEE_754_to_float(st_phval, nph);
					else
						flAnalogVal = 0.0;

					AtlTrace("\nAnalog signal value is: %16.14f \n\n", flAnalogVal);
					//set the Phasor X Value
					if(FAILED( hr = m_pRTDTParams->put_Analog((float)flAnalogVal)))
						return -1;

					//save the values in the Analog and digital values.
					if(FAILED(hr = m_pRTDTParams->SaveADVals()))
						return -1;

					//reset the real-time data markers to zero
					nAD += 4;
					nOffset = 0;
				}
			}//end of analog & digital signals
		}
		else	//no analog signal is in the list of chosen phasors
		{
			//advance as much as number of analog signals
		}
//-----------------------------------------------------------Digital Signal-----------------------------------------------------
		ULONG nADOffset(0);

		ULONG u32_NoofDigitals(0);
		if(FAILED(hr = get_NoofChDigital(&u32_NoofDigitals)))
			return -1;

		//find max number of analogs
		if(FAILED(hr = get_NumAnalogs((long*)&nTotAnalogs)))
			return -1;

		if(m_map_Anadevals.size() > 0)
		{
			for(m_AnadeIt = m_map_Anadevals.begin(); m_AnadeIt != m_map_Anadevals.end(); m_AnadeIt++)
			{
				nph = n;
				nFDQ = n;
				nAD = n;
				//advance untill to reach to the phasor
				nAD += 4;

				u32_AnalogNum = ((*m_AnadeIt).second)[0];
				u32_DigitalNum = ((*m_AnadeIt).second)[1];
				if(u32_DigitalNum == 100000)
					continue;

				u_32nPMUIndex = ((*m_AnadeIt).second)[2];
				u_32nPMUIndex += 1;

				//advance until you reach to the FRE
				//advance through all previous PMUs to reach the one
				for( k=0; (k < (int)u_32nPMUIndex) && (u_32nPMUIndex > 0); k++)
				{
					nADOffset += 2;	//2 bytes of the STAT
					u32_nPhasors = m_map_PMU2PHNMR[k];	//map PMU index to number of phasor in the PMU
					nADOffset += u32_nPhasors * 8;		//advanced as much as number of phasor times 8 byte
					nADOffset += 4; //FREQ size is 4 byte and advnance 4 bytes
					nADOffset += 4; //DFREQ size is 4 bytes and advance 4 bytes
		
					nAD += u32_DigitalNum * 4;	//4 bytes the size of analog signal

					nAD += nTotAnalogs * 4/*size of one analog signal*/;
					
					AtlTrace("Digital signal starts from: %d\n", nAD+nADOffset+1);
	//iITC			for(int analog = 0; analog < u32_DigitalNum; analog++)
	//iITC			{
					AtlTrace("\n\n\Digital value in binary is: %02x%02x\n", (unsigned char)vec1[nAD+nADOffset+1], (unsigned char)vec1[nAD+nADOffset+2]);

					::memset(st_phval, 0, sizeof(st_phval));
					dwRes = ::sprintf(st_phval, "%02x%02x", (unsigned char)vec1.at(nAD+nADOffset+1), (unsigned char)vec1.at(nAD+nADOffset+2));
					if( dwRes < 0 )
						assert(NULL);

					lDigitalVal = strtol(st_phval,&stop, 16);
					//convert the binary to floating point value
					if( lDigitalVal != 0)
						flDigitalVal = IEEE_754_to_float(st_phval, nph);
					else
						flDigitalVal = 0.0;

					AtlTrace("\Digital signal value is: %16.14f \n\n", flDigitalVal);
					//set the Phasor X Value
					if(FAILED( hr = m_pRTDTParams->put_Digital((float)flDigitalVal)))
						return -1;

					//save the values in the Analog and digital values.
					if(FAILED(hr = m_pRTDTParams->SaveDigVals()))
						return -1;

					//store first byte of digital signal extracted from c37.118 data packet 
					if( FAILED( hr = m_pRTDTParams->put_HexAnalog(vec1.at(nAD+nADOffset+1))))
						return -1;

					//store second byte of digital signal extraceted from c37.118 data packet
					if(FAILED(hr = m_pRTDTParams->put_HexAnalog(vec1.at(nAD+nADOffset+2))))
						return -1;

					//convert digital signal hex value to binary string
					if(FAILED(hr = m_pRTDTParams->DigHex2BinStr()))
						return -1;

					//save the digital binary string in a list 
					//use this list in the DecodeAngDig
					if(FAILED(hr = m_pRTDTParams->SaveDigHexVals()))
						return -1;

					nAD +=2;
					nADOffset = 0;
				}
//iITC		}
			}
//iITC		nAD = nFDQ;
		}
//------------------------------------------------------------------------------------------------------------------------------
//----------------------------------This is very important never remove this line unless consulted with software engineer
		//CLEAR THE VEC1 
		vec1.clear();
	}
//----------------------------------phasor decoding ends here-----------------------------------------------
//we need to save the phasor values in to a list and then transport them to the main applicaiton 
//we then insert them into the safearray in order to transfer this data to the PDC_Port.vi VI


//-------Save Decoded phasor values in order to insert into safearrays in PDC_Process_DlgDlg.cpp file starts here---------------
/*
//---------------------------------insert of phasor values into safe arrays ends here-------------------------------------------
	ULONG	u32_AnalogNum(100);
	ULONG	u32_DigitalNum(100);
	ULONG	u32_nAnalogs(0);	//number of analog signals
	ULONG	u32_nDigitals(0);	//number of digital signals
	float	flAnalogVal(0.0);		//value of analog signal 
	float	flDigitalVal(0.0);		//value of analog signal 
	long	lAnalogVal(0);		//presentation of this value is unknown until further discussion
	long	lDigitalVal(0);		//presentation of this value is unknown until further discussion
	
	//advance untill to reach to the analog signal
	nFDQ += 8;
	nAD = nFDQ; //four bytes FRACSEC
//-----------------------------------Analog and digital signals encoding starts here--------------------------------------------
	for(m_AnadeIt = m_map_Anadevals.begin(); m_AnadeIt != m_map_Anadevals.end(); m_AnadeIt++)
	{
		u32_AnalogNum = ((*m_AnadeIt).second)[0];
		u32_DigitalNum = ((*m_AnadeIt).second)[1];
		if(u32_AnalogNum == 100000)
			continue;

		u_32nPMUIndex = ((*m_AnadeIt).second)[2];
		u32_AnalogNum += 1;

		AtlTrace("\n\nThe Analog signal number in list of Analog parameters is: %d\n", u32_AnalogNum);
		AtlTrace("\n\nPMU Number: %d ---is selected Analog number: %d\n\n\n", (u_32nPMUIndex+1), u32_PhNum);

		//advance until you reach to the FRE
		//advance through all previous PMUs to reach the one
//		nAD +=1;	//advance to the begginig of the Analog signal

		AtlTrace("\n\n\Analog, DFREQ values in binary is: %02x%02x%02x%02x\n", (unsigned char)vec1[nAD+1], (unsigned char)vec1[nAD+2], 
												(unsigned char)vec1[nAD+3], (unsigned char)vec1[nAD+4]);

		::memset(st_phval, 0, sizeof(st_phval));
		dwRes = ::sprintf(st_phval, "%02x%02x%02x%02x", (unsigned char)vec1.at(nAD+1), (unsigned char)vec1.at(nAD+2), 
			(unsigned char)vec1.at(nAD+3), (unsigned char)vec1.at(nAD+4));
		if( dwRes < 0 )
			assert(NULL);

		lAnalogVal = strtol(st_phval,&stop, 16);
		//convert the binary to floating point value
		if( lAnalogVal != 0)
			flAnalogVal = IEEE_754_to_float(st_phval, nph);
		else
			flAnalogVal = 0.0;

		AtlTrace("\nAnalog signal value is: %16.14f \n\n", flAnalogVal);
		//set the Phasor X Value
		if(FAILED( hr = m_pRTDTParams->put_Analog((float)flAnalogVal)))
			return -1;

		//save the values in the Analog and digital values.
		if(FAILED(hr = m_pRTDTParams->SaveADVals()))
			return -1;

		//reset the real-time data markers to zero
		nAD += 4;
	}//end of analog & digital signals

//---------------digital signal
//iITC	if( vec1.size() > (nAD1+7) )
//iITC	{
	ULONG u32_NoofDigitals(0);
	if(FAILED(hr = get_NoofChDigital(&u32_NoofDigitals)))
		return -1;

	//find max number of analogs
	if(FAILED(hr = get_NumAnalogs((long*)&nTotAnalogs)))
		return -1;
	
	//subtract it from m_map_Anadevals.size() value
	//add the result to the nAD to reach to the begginig of the digital signal value
	nAD += (nTotAnalogs - m_map_Anadevals.size() + u32_NoofDigitals) * 4;

	for(int analog = 0; analog < u32_NoofDigitals; analog++)
	{
		AtlTrace("\n\n\Digital value in binary is: %02x%02x\n", (unsigned char)vec1[nAD+1], (unsigned char)vec1[nAD+2]);

		::memset(st_phval, 0, sizeof(st_phval));
		dwRes = ::sprintf(st_phval, "%02x%02x", (unsigned char)vec1.at(nAD+1), (unsigned char)vec1.at(nAD+2));
		if( dwRes < 0 )
			assert(NULL);

		lDigitalVal = strtol(st_phval,&stop, 16);
		//convert the binary to floating point value
		if( lDigitalVal != 0)
			flDigitalVal = IEEE_754_to_float(st_phval, nph);
		else
			flDigitalVal = 0.0;

		AtlTrace("\Digital signal value is: %16.14f \n\n", flDigitalVal);
		//set the Phasor X Value
		if(FAILED( hr = m_pRTDTParams->put_Digital((float)flDigitalVal)))
			return -1;

		//save the values in the Analog and digital values.
		if(FAILED(hr = m_pRTDTParams->SaveDigVals()))
			return -1;

		//store first byte of digital signal extracted from c37.118 data packet 
		if( FAILED( hr = m_pRTDTParams->put_HexAnalog(vec1.at(nAD+1))))
			return -1;

		//store second byte of digital signal extraceted from c37.118 data packet
		if(FAILED(hr = m_pRTDTParams->put_HexAnalog(vec1.at(nAD+2))))
			return -1;

		//convert digital signal hex value to binary string
		if(FAILED(hr = m_pRTDTParams->DigHex2BinStr()))
			return -1;

		//save the digital binary string in a list 
		//use this list in the DecodeAngDig
		if(FAILED(hr = m_pRTDTParams->SaveDigHexVals()))
			return -1;

		nAD +=1;
	}
//iITC	}
//iITC	}//end of analog & digital signals
//----------------------------------Analog and digital signals encoding ends here-----------------------------------------------
*/
//----------------------------------This is very important never remove this line unless consulted with software engineer
	//CLEAR THE VEC1 
	vec1.clear();

	return 0;
}

// puts the binary data into this class
DWORD CTCPCMU1::PutBinData(vector<char> * pVecBinData)
{
	vector<char>::iterator it1 = (*pVecBinData).begin();
	vector<char>::iterator it2 = (*pVecBinData).end();
	vector<char>	vec(it1, it2);

	for(int i =0; i < (int)vec.size(); i++)
		AtlTrace("%02x ", vec[i]);
	return 0;
}

STDMETHODIMP CTCPCMU1::get_IDCode(USHORT* pVal)
{
	*pVal = m_u16IDCode;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_IDCode(USHORT newVal)
{
	assert(newVal);

	m_u16IDCode = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::ProcessEventsRT(HWND hwnd, unsigned int msg, HANDLE hEventRT)
{
	// TODO: Add your implementation code here
	// TODO: Add your implementation code here
	DWORD * pu32_Events(0);
	DWORD * pu32_IP(0);
	SOCKET * p_hSocket(0);
	char * ps8_RecvBuf(0);
	DWORD * PU32_Read(0);
	DWORD * pu32_Sent(0);

	
	DWORD u32_Error = ProcessEvents2( hwnd, msg, hEventRT );
	if( u32_Error )
		return S_FALSE;

	return S_OK;
}


DWORD CTCPCMU1::ProcessEvents2(HWND hwnd, unsigned int msg, HANDLE hEventRT)
{
	DWORD u32_Error;
	WSANETWORKEVENTS PDC_Events;

	DWORD u32_Flags(0);
	WSABUF	Buffer;
	Buffer.buf = m_s8_ReadBuffer;
	Buffer.len = READ_BUFFER_SIZE;
	DWORD u32_Read(0);
	char  data[READ_BUFFER_SIZE];
	int iResult(0);
	char c(0), c9(0), c8(0);
	int nCount(0);
	int test_size(0);
	char buf[1500];
	int k = 0;
	HRESULT hr;
	char st[8], *stop;::memset(st, 0, sizeof(st)); 
	long nPktSize(-1);	
//------------------------------------
	char c1(0);
	char sync(0);
//------------------------------------
//---------------------------------------------
	char * lt;
	//allocate memory for time stamp decode
	lt = (char*)m_pMalloc->Alloc(6);
	if( !lt )
		assert(NULL);
	::memset(lt, 0, sizeof(lt));
//----------------------------------------------
	m_map_RTData.insert(pair<int, vector<char> >(1, vec));

	AtlTrace("0x%02x\n", c1 & 0xff);

//get the SYNC word from the ITCPCMU1 object'S Properties
	if(FAILED( hr = get_SYNC1( (unsigned char*)&c1 )))
		return 1;
	else
		AtlTrace("First byte of SYNC is: 0x%02x\n", (unsigned char)c1);

	if(FAILED( hr = get_SYNC2((unsigned char*) &sync )))
		return 1;
	else
		AtlTrace("Second byte of SYNC is: 0x%02x\n", (unsigned char)sync);

	::memset(data, 0, READ_BUFFER_SIZE);
	::memset(buf, 0, 1500);
	//wait for events with calling WSAWaiForMultipleEventsEx
	//WSAWaitForMultipleEventsEx must to design to check other problems with server refer to documentation design!
	//I must think about this one.

	AtlTrace("socket name is: %s\n", ((*m_sockIt).first).c_str());

	
	while(1)
	{
		//check if the event is set (Timeout = 0)
		DWORD u32_Res = ::WaitForSingleObject( m_hEvent, /*0*/INFINITE );
		if( u32_Res == WAIT_OBJECT_0 )
		{
			//get the events sent to this object 
//iITC in Real application we do this line
			if(::WSAEnumNetworkEvents( (*m_sockIt).second, m_hEvent, &PDC_Events ) == SOCKET_ERROR )
			{
				u32_Error = WSAGetLastError();
				//remove the socket with problem
				//socketlist.Remove(m_hSocket);
				//goto EXIT;
				AtlTrace("Error: WSAEnumNetworkEvents failed. error no is: %d\n", u32_Error);
				return u32_Error;
			}
			//check what event has been sent.
			//Read event has occured so read the data sent from server
			//into the buffer
			if ( PDC_Events.lNetworkEvents & FD_READ )
			{
again:
				//( SYNC = AA1 ) then data frame message is started.
				while((iResult = recv((*m_sockIt).second, &c, 1, 0)) > 0)
				{
					c = (unsigned char)c;
					c8= (unsigned char)c;
					if( (c) == c1  )
					{
						if(iResult = recv((*m_sockIt).second, &c, 1, 0))
						{
							c = (unsigned char)c;
							c9 = (unsigned char)c;
							if( c == sync )
								AtlTrace("%02x ", c & 0xff);
								m_bCFG2 = TRUE;
								break;
						}
					}
				}//end-of-while loop

				//it is configuration frame2
				if( m_bCFG2 )
				{
					vec1.push_back(c8 & 0xff);
					vec1.push_back(c9 & 0xff);
				}

				for(int i=0; (iResult > 0); i++){
					iResult = ::recv((*m_sockIt).second, &c, 1, 0);
					if( (iResult > 0 ))
					{
						c = (unsigned char)c;
						if( (c) == c1  )
						{
							//it is configuration frame2
							if( m_bCFG2 )
								vec1.push_back(c & 0xff);
							if(iResult = recv((*m_sockIt).second, &c, 1, 0))
							{
								c = (unsigned char)c;
								//it is configuration frame2
								if( m_bCFG2 )
									vec1.push_back(c & 0xff);
								if(vec1.size() == nPktSize ){
									::ResetEvent(m_hEvent);
									break;
					}
							}
						}
						c = (unsigned char)c;
						//it is configuration frame2
						if( m_bCFG2 )
							vec1.push_back(c & 0xff);
					}
					nCount++;	//total number of the characters read
					if( (nCount >= 4) && (vec1.size() >= 4) )
					{
						::sprintf(st, "%02x%02x", vec1.at(2) & 0xff, vec1.at(3) & 0xff);
						nPktSize = (USHORT)strtol(st,&stop, 16);
					}
					//check for unwanted surge of data if there is one happening interuppt it
					if( vec1.size() > 2048)
					{
						::ResetEvent(m_hEvent);
						vec1.clear();
						::SetEvent(m_hEvent);
						goto again;
					}
					if(vec1.size() == nPktSize ){
						u32_Error = CRC_Check( vec1.size());
						if(u32_Error > 0 )
						{
							::ResetEvent(m_hEvent);
							vec1.clear();
							::SetEvent(m_hEvent);
							goto again;
							break;
						}
						else if ( !u32_Error )
						{
							//packet is good so parse it
							::ResetEvent(m_hEvent);
							break;
						}
						else
							assert(NULL);
					}
				}//end-of-for loop

				AtlTrace("\n");
				AtlTrace("\n");

				AtlTrace("\n");
				AtlTrace("\n");
				
				if( vec1.size() == nPktSize )
				{
					for(int k=0; k < (int)vec1.size(); k++)
					{
						AtlTrace("%02x ", vec1[k] & 0xff);
//iITC						::sprintf(lt, "%02x ",  vec1[k] & 0xff);
//iITC						m_strWarning = lt;
						//write the IP address into the network configuration file
//iITC						if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
//iITC							return -1;
					}
				}
				else
				{
					m_strWarning = "vec1 size is not equal to nPktSize.\n";
					//write the IP address into the network configuration file
//iITC					if( !::fwrite(m_strWarning.c_str(), 1, m_strWarning.size(), f) )
//iITC						return -1;
						::ResetEvent(m_hEvent);
						vec1.clear();
						::SetEvent(m_hEvent);
						goto again;
				}
				AtlTrace("\nsize of CFG2 is: %d\n", vec1.size());

				AtlTrace("\n");
				AtlTrace("\n");
				AtlTrace("\n");
				
				if( (vec1.size() == nPktSize) && (vec1.size() > 16) )
					BIN2ASCII_RT();
				else if( vec1.size() > nPktSize )
					vec1.clear();
				else
				{}
				nCount = 0;
//iITC				::Sleep(6);

//----------------------------------------------------------------------------------------------------------
				Sleep(m_u32_RefreshRate);
//------------------------------------------------------------------------------------------------------------
				//flush the real time data
				//Update the PDC_Port.vi with new data
				//21:45 nov. 5 2012 iITC commented out PostMessageA and replaced it with SetEvent(hPHBOXEvent);
				if( !::SetEvent(hEventRT) )
					assert(NULL);
			}
		}//end-of-waitforsingleobject
	}//end-of-while() loop

	return 0;
}

STDMETHODIMP CTCPCMU1::get_SkipSteps4RT(USHORT* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_bSkip;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_SkipSteps4RT(USHORT newVal)
{
	// TODO: Add your implementation code here
	m_bSkip = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::ContinuePacketReading(void)
{
	// TODO: Add your implementation code here
	if( m_hEvent )
		::SetEvent(m_hEvent);
		
	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_PhMagAng(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = vecphdec[0];

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_PhMagAng(ULONG newVal)
{
	// TODO: Add your implementation code here
	vecphdec[0] = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_PhNumber(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = vecphdec[1];

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_PhNumber(ULONG newVal)
{
	// TODO: Add your implementation code here
	vecphdec[1] = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::InsertPhPosType(void)
{
	// TODO: Add your implementation code here
	ULONG i = m_map_phdevals.size();//find the position at the end of the map

	//
	//this helps the program to recognize which pmu and which phasor is selected in chosen data and is it Magnitude/Angle
	m_retphde = m_map_phdevals.insert(pair<ULONG, vector<ULONG> >(i++, vecphdec));//insert the chosen data decode vector
	if( m_retphde.second == false)
		return -1;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_PMUCount(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = vecphdec[2];

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_PMUCount(ULONG newVal)
{
	// TODO: Add your implementation code here
	vecphdec[2] = newVal;


	return S_OK;
}


STDMETHODIMP CTCPCMU1::ClearDecodeVector(void)
{
	// TODO: Add your implementation code here
	m_map_phdevals.clear();

	return S_OK;
}


// check whether the packet is valid
DWORD CTCPCMU1::CRC_Check(long lPktSize)
{
	unsigned int short	crc = 0xFFFF;
	unsigned int short  temp;
	unsigned int short quick;
	char check[3];

	::memset(check, 0, sizeof(check));
	//compute CRC
	for( int i =0; i < (lPktSize - 2); i++ )
	{
		temp = (crc >> 8) ^ (vec1[i] & 0xff);
		crc <<= 8;
		quick = temp ^ (temp >> 4);
		crc ^= quick;
		quick <<= 5;
		crc ^= quick;
		quick <<= 7;
		crc ^= quick;
	}

	//fill the CRC-CCITT
	check[0] = (crc & 0x0000ff00) >> 8;
	check[1] = crc & 0x000000ff;

	AtlTrace("\nCalculated CRC is: %02x %02x\n", check[0] & 0xff, check[1] & 0xff);
	AtlTrace("\nRecieved Packet CRC is: %02x %02x\n", vec1[lPktSize-2] & 0xff, vec1[lPktSize - 1] & 0xff);

	if( ( (check[0] & 0xff) == (vec1[lPktSize-2] & 0xff) ) && ( ( check[1] & 0xff) == (vec1[lPktSize - 1] & 0xff) ) )
		return 0;

	return 1;
}

float CTCPCMU1::IEEE_754_to_float(char * raw, int pos) {
	int val1(0), val2(0), val(0), iexponent(0); 
	//calculatet the sign of the floating point.
	char * exponent = (char*) m_pMalloc->Alloc(1); 
	::memset(exponent, 0, sizeof(exponent));
	
	*exponent = raw[0];
	val1 = asc1bin(&exponent);
	int sign = (val1 >> 3) ? -1 : 1;
	float flVal(0.0), Mantissa(0.0);
	*exponent = raw[0];
	val1 = asc1bin(&exponent);
	*exponent = raw[1];
	val2 = asc1bin(&exponent);
	val = (val1 << 4) | val2;
	*exponent = raw[2];
	val2 = asc1bin(&exponent);
	val = (val << 4) | val2;
	val = val >> 3;
	////////
	val &= 0x000000ff;
	////////////////////
	iexponent = val - 127;
////////////////////////////////////////////////////////////////////
	//calculate mantissa of the floating point
	*exponent = raw[2];
	val1 = asc1bin(&exponent);
	val1 &= 0x07;//
	*exponent = raw[3];
	val2 = asc1bin(&exponent);
	val = (val1 << 4) | val2;
	*exponent = raw[4];
	val1 = asc1bin(&exponent);
	val = (val << 4) | val1;
	*exponent = raw[5];
	val1 = asc1bin(&exponent);
	val = (val << 4) | val1;
	*exponent = raw[6];
	val1 = asc1bin(&exponent);
	val = (val << 4) | val1;
	*exponent = raw[7];
	val1 = asc1bin(&exponent);
	val = (val << 4) | val1;

		//mantissa is
		Mantissa = 1/::pow(2.0, 23); 
		Mantissa *= val;
		Mantissa += 1.0;
		flVal = sign * ::pow(2.0, iexponent) * Mantissa;
/////////////////////////////////////////////////////////////////////////
		return flVal;
}

/******************************  asc2bin  *************************************
Converts two ASCII hex characters to binary.  Returns binary 0 to 255, or -1 if
error.  Pass in pointer to string pointer, updates string pointer to point at
character after second ASCII hex character.
******************************************************************************/

int CTCPCMU1::asc2bin (

    char **index)          /* points to pointer to MSB ASCII digit */
{
int msb, lsb;

/*--------------------------------------------------------------------------*/

    if ((msb = asc1bin (index)) == -1) return -1;
    if ((lsb = asc1bin (index)) == -1) return -1;
    return (msb << 4) | lsb;
}

/*****************************  asc1bin  **************************************
Converts one ASCII hex character to binary.  Returns binary 0 to 15, or -1 if
error.  Pass in pointer to string pointer, updates string pointer to point at
character after ASCII hex character.
******************************************************************************/

int CTCPCMU1::asc1bin (

    char **index)          /* points to pointer to ASCII digit */
{
char value;

/*--------------------------------------------------------------------------*/

    value = toupper (**index); /* get ASCII hex digit */
    (*index)++;                /* bump pointer */

    if (value >= '0' && value <= '9') 
		return value - '0';
    if (value >= 'A' && value <= 'F') 
		return value - '7';
    return -1;
}


STDMETHODIMP CTCPCMU1::get_AnalogNumber(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = vecADdec[0];

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_AnalogNumber(ULONG newVal)
{
	// TODO: Add your implementation code here
	vecADdec[0] = newVal;
	//one more analog signal, increment the analog signal counter
//	m_u32_NoofAnalogs++;

	return S_OK;
}

STDMETHODIMP CTCPCMU1::get_DigitalNumber(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = vecADdec[1];

	return S_OK;
}

STDMETHODIMP CTCPCMU1::put_DigitalNumber(ULONG newVal)
{
	// TODO: Add your implementation code here
	vecADdec[1] = newVal;
	//one more digital signal is added, increment the number of digital signals counter
//	if( newVal != 100000)
//		m_u32_NoofDigitals++;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::InsertAnaPosType(void)
{
	// TODO: Add your implementation code here
	ULONG i = m_map_Anadevals.size();//find the position at the end of the map

	//
	//this helps the program to recognize which pmu and which phasor is selected in chosen data and is it Magnitude/Angle
	m_retAnade = m_map_Anadevals.insert(pair<ULONG, vector<ULONG> >(++i, vecADdec));//insert the chosen data decode vector
	if( m_retAnade.second == false)
		return -1;

	return S_OK;
}



STDMETHODIMP CTCPCMU1::get_ADPMUCount(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = vecADdec[2];

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_ADPMUCount(ULONG newVal)
{
	// TODO: Add your implementation code here
	vecADdec[2] = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::FindNoofchAD(ULONG* pu32_NoofchAD)
{
	// TODO: Add your implementation code here
	*pu32_NoofchAD = m_map_Anadevals.size();

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_LogFilNm(BSTR* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_bstrLogFil;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_LogFilNm(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_bstrLogFil = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_PhDecSize(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = (ULONG)m_map_phdevals.size();
	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_PhDecSize(ULONG newVal)
{
	// TODO: Add your implementation code here

	return E_NOTIMPL;
}


STDMETHODIMP CTCPCMU1::get_NoofChAnalog(ULONG* pVal)
{
	// TODO: Add your implementation code here
//iITC	*pVal = m_map_ADdevals.size();
	*pVal = m_u32_NoofAnalogs;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_NoofChAnalog(ULONG newVal)
{
	// TODO: Add your implementation code here

	return E_NOTIMPL;
}


STDMETHODIMP CTCPCMU1::get_NoofChDigital(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_u32_NoofDigitals;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_NoofChDigital(ULONG newVal)
{
	// TODO: Add your implementation code here

	return E_NOTIMPL;
}


STDMETHODIMP CTCPCMU1::get_NumAnalogs(LONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_u32_TotAnalogs;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_NumAnalogs(LONG newVal)
{
	// TODO: Add your implementation code here
	m_u32_TotAnalogs = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_NumDigitals(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_u32_TotDigitals;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_NumDigitals(ULONG newVal)
{
	// TODO: Add your implementation code here
	m_u32_TotDigitals = newVal;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::FindTotalNoofPhasors(ULONG* pu32_PHRCount)
{
	// TODO: Add your implementation code here
	*pu32_PHRCount = 0;
	
	for( m_PMU2PHNMRIt = m_map_PMU2PHNMR.begin(); m_PMU2PHNMRIt != m_map_PMU2PHNMR.end(); m_PMU2PHNMRIt++)	
		*pu32_PHRCount += (*m_PMU2PHNMRIt).second;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::AddAnalogCh(void)
{
	// TODO: Add your implementation code here
	m_u32_NoofAnalogs++;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::AddDigitalNumber(void)
{
	// TODO: Add your implementation code here
	m_u32_NoofDigitals++;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::get_RefreshRate(ULONG* pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_u32_RefreshRate;

	return S_OK;
}


STDMETHODIMP CTCPCMU1::put_RefreshRate(ULONG newVal)
{
	// TODO: Add your implementation code here
	m_u32_RefreshRate = newVal;

	return S_OK;
}


// calculates difference between two time stamps
DWORD CTCPCMU1::TSDiff(time_t ts1, time_t ts2)
{
	LARGE_INTEGER	n1, n2;
	SYSTEMTIME syst1, syst2;
	FILETIME	timeTemp;
	float fDiff(0.0f);
	DWORD dwRet(0);

	COleDateTime dt1(ts1);
	COleDateTime dt2(ts2);

	dt1.GetAsSystemTime(syst1);
	dt2.GetAsSystemTime(syst2);

	::SystemTimeToFileTime(&syst1, &timeTemp);
	n1.HighPart = timeTemp.dwHighDateTime;
	n1.LowPart = timeTemp.dwLowDateTime;
	
	::SystemTimeToFileTime(&syst2, &timeTemp);
	n2.HighPart = timeTemp.dwHighDateTime;
	n2.LowPart = timeTemp.dwLowDateTime;

	//calculate difference in milliseconds
	fDiff = (( n2.QuadPart/10000 ) - ( n1.QuadPart/10000));
	AtlTrace("################################################ FDIFF is: %f\n", fDiff);
	if( fDiff <= m_u32_RefreshRate )
		dwRet = -1;
	else
		ContinuePacketReading();

	return dwRet;
}
