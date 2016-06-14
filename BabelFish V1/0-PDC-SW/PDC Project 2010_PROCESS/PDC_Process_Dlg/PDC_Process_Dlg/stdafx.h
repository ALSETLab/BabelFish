
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


//**********************************Activex server related settings*************************************
// Acquire.cpp : This source code is an Automation client. It calls the
// LabVIEW 7.1's automation server. 
//C:\Program Files (x86)\National Instruments\LabVIEW 2011\resource\labview.tlb
//#define LABVIEW_TLB_PATH "c:\Program Files\National Instruments\Labview 7.1\RESOURCE\labview.tlb"
//#define LABVIEW_TLB_PATH "C:\\Program Files (x86)\\National Instruments\\LabVIEW 2011\\resource\\labview.tlb"
#define LABVIEW_TLB_PATH "C:\\KTH software to check\\VI TUTORIAL PROJECTS\\labview.tlb"
//iITC #define VI_PATH "C:\\PMU Project\\202516\\Calling_LabVIEW_7_1_from_C_plus_plus_using_ActiveX\\var_test.vi"
#define VI_PATH "C:\\KTH software to check\\VI TUTORIAL PROJECTS\\PDC_Port.vi"
//iITC #define FILE_PATH  "C:\\Users\\iitc1\\Documents\\statfile2.txt"

// Include the LabVIEW Automation Server Type Libraries
#import LABVIEW_TLB_PATH

// generate a  namespace declaration to and identify and assign a name to a declarative region.
// In this case we are assigning the LabVIEW.
using namespace LabVIEW;


//************************************************************************************************************







#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


