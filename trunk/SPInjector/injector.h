/*-----------------------------------------------------------------------------
 * injector.h
 *-----------------------------------------------------------------------------
 * thread injector 모듈 
 *	- inject thread/code to target process
 *	- can communicate injected thread (in target process)
 *	- send command / receive result
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#ifndef _CODE_INJECTOR_H_
#define _CODE_INJECTOR_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "injector_common.h"
#include "typedef_GhostAPI.h"


#define INJECTOR_LOG	_T("injector_log.txt")

// remote thread data block 
//
typedef struct _AK_PARAM
{
	TGETLASTERROR			pGetLastError;
	TFORMATMESSAGE			pFormatMessage;
	TLOADLIBRARY			pLoadLibrary;
	TGETPROCADDRESS			pGetProcAddress;
	TFREELIBRARY			pFreeLibrary;
	TOUTPUTDEBUGSTRING		pOutputDebugString;
	TGETMODULEHANDLE		pGetModuleHandle;	
	TMAPVIEWOFFILE			pMapViewOfFile;
	TUNMAPVIEWOFFILE		pUnmapViewOfFile;
	TRESETEVENT				pResetEvent;
	TSETEVENT				pSetEvent;	
	TWAITFORSINGLEOBJECT	pWaitForSingleObject;
	TCLOSEHANDLE			pCloseHandle;
	
	DWORD					lastErrorCode;			// api fail 시 GetLastError() 값
	HANDLE					hShareSectionHandle;	// AK_SHARE_BLOCK 핸들 사본
} AK_PARAM;

typedef struct _INJECTOR_CTX
{
	// remote code, data block pointer 
	// 
	DWORD	DataPtrLen;
	char*	DataPtr;	

	DWORD	CodePtrLen;
	char*	CodePtr;	
	
	// To be used by injector
	//
	BOOL	IsRunning;			// indicate target process is running
	DWORD	TargetProcId;		// target process id
	HANDLE	hTargetProcHandle;	// target process handle 
	HANDLE	hSendCmd;			// used when send command to injected lib
	HANDLE	hRecvReply;			// used when wait response from injected lib
	
	// To be used by injected library 
	//
	HANDLE	hShareSectionHandle;
	HANDLE	hRecvCmd;		// used when wait command from injected lib
	HANDLE	hSendReply;		// used when send response to injector

	AK_INJECTOR_COMMAND		Command;
	AK_INJECTOR_STATUS		Status;

	// buffer for transfer additional information of command or result data of command 
	// between injector and target process 
	// 
	BYTE	ShareBuffer[SHARE_BUFFER_SIZE];
} INJECTOR_CONTEXT;

#ifdef _DEBUG
#define DBG_BREAK __asm int 3
#else
#define DBG_BREAK 
#endif//_DEBUG

#define DBG_PRINT_ERROR() \
{\
	TCHAR msg[512];\
	DWORD msgLen;\
	DWORD dwError;\
	dwError = pParam->pGetLastError();\
\
	msgLen = pParam->pFormatMessage(\
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,\
				NULL, \
				dwError, \
				0, \
				msg, \
				512, \
				NULL);\
	if(0 == msgLen)\
	{\
		__asm int 3\
		__asm jmp EPILOG\
	}\
	msg[msgLen] = 0x00; \
	pParam->lastErrorCode = dwError;\
	pParam->pOutputDebugString(msg);\
}




// prototype for thread procedure executed from target process context
//
typedef 
DWORD (__stdcall *RMT_THREAD_PROC)(AK_PARAM* pParam);

// get real procedure's body
//
DWORD_PTR GetFunctionRVA(void* FuncName);

// remote thread procedure that load dll, call functions, and so on.
// -- This is *most* important routine.
//
DWORD __stdcall inject_thread_stub(AK_PARAM* pParam);

// Used by *only* injector
//
//
int SendCommand(INJECTOR_CONTEXT* block, AK_INJECTOR_COMMAND cmd, DWORD dwTimeout);

// allocate / free context
//
INJECTOR_CONTEXT* AllocInjectorContext(DWORD dwPid);
void	FreeInjectorContext(INJECTOR_CONTEXT* ptr);

// inject method
//
int	InjectThread(INJECTOR_CONTEXT* pCtx, RMT_THREAD_PROC pRmtThrdProc);





//
// command wrapper functions
// [ WARNING ]
//	sizeof(command_structure) < CONTEXT_SHARE_BUFFER_SIZE
//

int CMD_Quit(INJECTOR_CONTEXT* block);
int CMD_Continue(INJECTOR_CONTEXT* block);
int CMD_Hello(INJECTOR_CONTEXT* block);

// CMD_LOAD_PLUGIN
typedef struct _CMD_LOAD_PLUGIN_REC
{
	TCHAR	PluginName[MAX_PATH];		
}CMD_LOAD_PLUGIN_REC, *PCMD_LOAD_PLUGIN_REC;

int	CMD_LoadPlugin(INJECTOR_CONTEXT* block, PCMD_LOAD_PLUGIN_REC rec);
int CMD_UnloadPlugin(INJECTOR_CONTEXT* block);

//
// CMD_CALL_PLUGIN
//
int CMD_CallPlugin(INJECTOR_CONTEXT* block, PCMD_CALL_PLUGIN_REC rec);



#endif//_CODE_INJECTOR_H_