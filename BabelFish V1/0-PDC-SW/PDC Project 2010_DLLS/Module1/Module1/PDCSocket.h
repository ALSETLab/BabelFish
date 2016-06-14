#pragma once


#define READ_BUFFER_SIZE	(65 * 1024) //this is 8K size buffer size

#include <assert.h>

template<class T> class CPDCSocket
{
public:

	CPDCSocket(void)
		: m_bInitialized(FALSE)
//iITC		, m_s8_ReadBuffer(NULL)
	{
	}

	virtual ~CPDCSocket(void)
	{
	}

protected:
	DWORD Initialize1(void);
public:
	// closes the connection between client and server
	DWORD Close(void);
	// disconnect connection between client and server
	DWORD DisconnectClient(SOCKET hSocket);
	// establishes a connection between client and server
	DWORD ConnectTo(DWORD u32_ServIP, USHORT u16_Port, DWORD u32_EventTimeout, DWORD u32_MaxIdleTime);
	// sends data from client to server
	DWORD SendTo(SOCKET hSocket, char* s8_SendBuf, DWORD u32_Len);
protected:
	// creates socket that is used to open a connection between client and server
	DWORD CreateSocket(void);
public:
	// Processes events associted with socket exchanged  between client and server
	DWORD ProcessEvents(DWORD * pu32_Events, DWORD * pu32_IP, SOCKET * ph_Socket, char* ps8_RecvBuf, DWORD * pu32_Read, DWORD * pu32_Sent);
protected:
	// stores whether the sockets library is initialized
	BOOL m_bInitialized;
protected:
	// buffer to read & store stream from server
//	char* m_s8_ReadBuffer;
	// IMalloc allocator
//	LPMALLOC m_pMalloc;
	// socket to establish a connection between client & server
	SOCKET m_hSocket;
	// EVENT handle associated with a SOCKET
	HANDLE m_hEvent;
	// stores socket for header frame retrieval connection
	SOCKET m_hSocket2;
	HANDLE m_hEvent2;
	//stores socket for configuration frame1
	SOCKET m_hSocket3;
	HANDLE  m_hEvent3;
	//stores socket for configuration frame2
	SOCKET  m_hSocket4;
	HANDLE  m_hEvent4;
};

