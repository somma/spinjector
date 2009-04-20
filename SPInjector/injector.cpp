/*-----------------------------------------------------------------------------
 * code_injector.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include <malloc.h>
#include "injector.h"
#include "AKUtil.h"

// internal function
//	-- never call directly --
//
SECURITY_ATTRIBUTES* _get_null_dacl(BOOL IsInheritable);
BOOL				_free_dacl(SECURITY_ATTRIBUTES *SecurityAttributes);

int		_init_parameter_block(AK_PARAM* pParam, HANDLE hSharedSectionCopy);
int		_size_of_proc(DWORD_PTR FuncBody);



/**	---------------------------------------------------------------------------
	\brief		Target process 에게 command 전송

    \param	
	\return		성공시 0, 실패시 -1
				*Note*
				-1 리턴시 block->IsRunning == FALSE 인 경우 target process 가
				종료된 경우임	
				
				* WARNING *
				If return value is 0 (success), then you can Read data or write data, etc.
				After read/write or anything else you want to shared section, you *MUST*
				send CMD_CONTINUE command.
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int SendCommand(INJECTOR_CONTEXT* block,AK_INJECTOR_COMMAND cmd, DWORD timeout)
{
	_ASSERTE(NULL != block);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	if (
		(NULL == block) || 
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)))
		)
	{
		return -1;
	}
	
	// check target process is running
	//
	DWORD dwExitCode = 0;
	if (FALSE == GetExitCodeProcess(block->hTargetProcHandle, &dwExitCode))
	{
		_ASSERTE(!"GetExitCodeProcess");
		return -1;
	}
	
	if (STILL_ACTIVE != dwExitCode) 
	{
		block->IsRunning = FALSE;	
		return -1;
	}
	else
	{
		// fire command event 
		//
		block->Command = cmd;
		if (FALSE == ResetEvent(block->hRecvReply))	
		{
			_ASSERTE(!"ResetEvent");
			return -1;
		}
		if (FALSE == SetEvent(block->hSendCmd))
		{
			_ASSERTE(!"SetEvent");
			return -1;
		}

		// wait for response
		//
		if (WAIT_OBJECT_0 != WaitForSingleObject(block->hRecvReply, timeout))
		{
			_ASSERTE(!"WaitForSingleObject");
			return -1;
		}

		// check remote thread status 
		// 
		if ( (CMD_QUIT == cmd) && (STATUS_THREAD_EXITED == block->Status) )
		{
			return 0;
		}

		if (CMD_CONTINUE == cmd)
		{
			_ASSERTE(block->Status == STATUS_READY);
			if (STATUS_READY != block->Status)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
		else 
		{
			_ASSERTE(STATUS_WAIT_CONTINUE == block->Status);
			if (STATUS_WAIT_CONTINUE != block->Status)
			{		
				return -1;
			}		
			else
				return 0;	// success 
		}

		// never reach here!
	}

	// never reach here!
}


/**	---------------------------------------------------------------------------
	\brief	

			** WARNING **
			이 함수는 타겟 프로세스에 코드가 주입된다.
			따라서 상대주소 참조 또는 데이터 영역 참조가 불가능 하므로 
			문자열 접근이나 함수 호출시 주의해야 한다. 
			(xp 이전 버전에서는 함수는 그냥 호출해도 되지만 vista 의 경우
			ASLR 때문에 문제가 된다)

			switch-case 구문도 조심해서 사용해야 한다. 
			VS 의 경우 3개 이상의 case 문이 만들어지면 jmp table 을 함수의 
			바깥부분에 만들고 이 테이블을 참조해서 분기를 한다. 
			타겟 프로세스에 프로시저 주입시 테이블에 대한 코드는 주입되지 
			않으므로 당연히 문제가 생긴다. 
			테이블까지 복사한다고 해도 상대주소 참조를 하기 때문에 오류가 
			난다. 

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
#pragma optimize("y", off)			// ensure frame pointers are included
#pragma runtime_checks( "", off )	// disable /RTCx option

#define INJTR_SET_STATUS(block, status)	\
{\
	pInjector->Status = status;\
	if (\
		(TRUE != pParam->pResetEvent(pInjector->hRecvCmd)) ||\
		(TRUE != pParam->pSetEvent(pInjector->hSendReply)))\
	{\
		goto return_thread_procedure;\
	}\
}

#define SET_STATUS_EXIT()\
{\
	INJTR_SET_STATUS(pInjector, STATUS_THREAD_EXITED);\
	goto return_thread_procedure;\
}

#define SET_STATUS_WAIT()	INJTR_SET_STATUS(pInjector, STATUS_WAIT_CONTINUE);
#define SET_STATUS_READY()	INJTR_SET_STATUS(pInjector, STATUS_READY);

__declspec( naked ) 
DWORD __stdcall inject_thread_stub(AK_PARAM* pParam)
{
	__asm      /* prolog */
	{
		push   ebp
		mov      ebp, esp
		sub      esp, __LOCAL_SIZE
	}

	if (NULL == pParam)
	{
		DBG_BREAK
		__asm jmp EPILOG
	}

	// local variables
	//
	DWORD WaitRes; WaitRes = 0;
	INJECTOR_CONTEXT* pInjector; pInjector = NULL;

	//
	// plugin
	//
	HMODULE		hPlugin;	hPlugin = NULL;
	PLUGIN_PROC pProc;		pProc = NULL;

	// open share section
	// 
	pInjector = (INJECTOR_CONTEXT*) pParam->pMapViewOfFile(pParam->hShareSectionHandle, 
								FILE_MAP_WRITE, 
								0,
								0, 
								0);
	if (NULL == pInjector)
	{
		DBG_PRINT_ERROR();
		DBG_BREAK
		__asm jmp EPILOG
	}	

	pInjector->Status = STATUS_READY;

	// wait for command from injector
	//
	for (;;)
	{
		// STEP #1
		// Wait for command (STATUS_READY)
		// 
		if (STATUS_READY != pInjector->Status)
		{
			DBG_BREAK;
		}
		
		WaitRes = pParam->pWaitForSingleObject(pInjector->hRecvCmd, INFINITE);
		switch (WaitRes)
		{
		case WAIT_OBJECT_0:
			{				
				if (CMD_QUIT == pInjector->Command)
				{
					// CMD_QUIT command immediately return thread procedure instead of 
					// change status and wait CMD_CONTINUE command.
					//
					SET_STATUS_EXIT();
				}
				else if (CMD_HELLO == pInjector->Command)
				{
					SET_STATUS_WAIT();
					break;
				}
				else if (CMD_LOAD_PLUGIN == pInjector->Command)
				{
					// load plugin and call procedure
					//
					PCMD_LOAD_PLUGIN_REC p = (PCMD_LOAD_PLUGIN_REC) pInjector->ShareBuffer;
					if (NULL != hPlugin) pParam->pFreeLibrary(hPlugin);
					hPlugin = NULL;
					if (NULL != pProc) pProc = NULL;

					hPlugin = pParam->pLoadLibrary(p->PluginName);
					if (NULL == hPlugin)
					{
						DBG_PRINT_ERROR();
						SET_STATUS_EXIT();
					}

					SET_STATUS_WAIT();
					break;
				}
				else if (CMD_UNLOAD_PLUGIN == pInjector->Command)
				{
					if (NULL != hPlugin) pParam->pFreeLibrary(hPlugin);
					hPlugin = NULL;

					if (NULL != pProc) pProc = NULL;				

					SET_STATUS_WAIT();
					break;
				}
				else if (CMD_CALL_PLUGIN == pInjector->Command)
				{
					if (NULL == hPlugin)
					{
                        INJTR_SET_STATUS(pInjector, STATUS_THREAD_EXITED);
						goto return_thread_procedure;
					}

					PCMD_CALL_PLUGIN_REC p = (PCMD_CALL_PLUGIN_REC) 
												pInjector->ShareBuffer;
					pProc = (PLUGIN_PROC) pParam->pGetProcAddress(hPlugin, 
																  p->Input.ProcName);
					if (NULL == pProc)
					{
						DBG_PRINT_ERROR();
						pParam->pFreeLibrary(hPlugin);
						hPlugin = NULL;				

						SET_STATUS_EXIT();
					}

					// call plugin method
					//
					if ( 0 != pProc(p) )
					{
						SET_STATUS_EXIT();
					}


					SET_STATUS_WAIT();
					break;
				}
				else
				{
					DBG_BREAK
					SET_STATUS_EXIT();
				}
			}			

		//case WAIT_TIMEOUT, WAIT_ABANDONED:	// never reach
		default:
			{
				DBG_BREAK
				SET_STATUS_EXIT();
			}
		}//switch

		// STEP #2
		//	Wait for CMD_CONTINUE 
		//
		if (STATUS_WAIT_CONTINUE != pInjector->Status)
		{
			DBG_BREAK;
		}
		WaitRes = pParam->pWaitForSingleObject(pInjector->hRecvCmd, INFINITE);
		switch (WaitRes)
		{
		case WAIT_OBJECT_0:
			{
				switch(pInjector->Command)
				{
				case CMD_CONTINUE:
					{
						SET_STATUS_READY();
						break;
					}
				default :
					{
						DBG_BREAK
						SET_STATUS_EXIT();
					}
				}// switch				
				break;
			}			

		//case WAIT_TIMEOUT, WAIT_ABANDONED:	// never reach
		default:
			{
				DBG_BREAK
				SET_STATUS_EXIT();
			}
		}//switch


	}//for

return_thread_procedure:
{
	// Never overwrite data to shared section (especially status field).
	// Injector can try read shared section data after this procedure (thread procedure)
	// returned.
	//
	if (NULL != hPlugin) pParam->pFreeLibrary(hPlugin);
	if (NULL != pProc) pProc = NULL;
				
	if (NULL != pInjector->hRecvCmd) 
	{
		pParam->pCloseHandle(pInjector->hRecvCmd);
		pInjector->hRecvCmd = NULL;
	}
	if (NULL != pInjector->hSendReply) 
	{
		pParam->pCloseHandle(pInjector->hSendReply);
		pInjector->hSendReply = NULL;
	}

	HANDLE hTmp; hTmp = pInjector->hShareSectionHandle;
	pParam->pUnmapViewOfFile(pInjector);
	pInjector = NULL;
	pParam->pCloseHandle(hTmp);	

	__asm jmp EPILOG;
}


	//------------------------ EPILOG -----------------------------------------
	__asm      
	{
EPILOG:		
		xor	eax, eax					// always return 0 (33 C0)
		mov esp, ebp
		pop	ebp

		/* epilog signature */
		xor eax, eax
		xor eax, eax
		
		ret	4							// 0xC3
	}
}


#pragma runtime_checks( "", restore )	// restore /RTCx option
#pragma optimize("", on) // restore compiler settings

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
INJECTOR_CONTEXT* AllocInjectorContext(DWORD dwPid)
{	
	_ASSERTE(0 != dwPid);
	if (0 == dwPid) return NULL;

	SECURITY_ATTRIBUTES* pSecurityAttributes = _get_null_dacl(TRUE);
	if (NULL == pSecurityAttributes)
	{
		_ASSERTE(!"_get_null_dacl");
		return NULL;
	}
	
	HANDLE SectionHandle = CreateFileMapping(NULL, 
								pSecurityAttributes, 
								PAGE_READWRITE,
								0, 
								sizeof(INJECTOR_CONTEXT), 
								NULL);
	if (INVALID_HANDLE_VALUE == SectionHandle)
	{
		_ASSERTE(!"CreateFileMapping");
		_free_dacl(pSecurityAttributes);
		return NULL;
	}
	_free_dacl(pSecurityAttributes);

	INJECTOR_CONTEXT* share_block = (INJECTOR_CONTEXT*)
										MapViewOfFile(SectionHandle, 
														FILE_MAP_WRITE, 
														0,
														0, 
														0);
	if (NULL == share_block)
	{
		_ASSERTE(!"MapViewOfFile");
		goto return_fail;
	}

	// !!! MUST !!! initialize first !! by 0x00
	//
	memset(share_block, 0x00, sizeof(INJECTOR_CONTEXT));

	// 타겟 프로세스 핸들 획득
	//
	share_block->TargetProcId = dwPid;
	share_block->hTargetProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == share_block->hTargetProcHandle)
	{
		share_block->hTargetProcHandle = advOpenProcess(dwPid,PROCESS_ALL_ACCESS,TRUE);
		if (NULL == share_block->hTargetProcHandle)
		{
			_ASSERTE(!"openProcess");
			goto return_fail;
		}
	}
	if (NULL == share_block->hTargetProcHandle)
	{
		_ASSERTE(!"openProcess");
		goto return_fail;
	}
	
	// shared memory section handle 공유를 위해 target process context 로 
	// 핸들을 복사한다. 
	//
	if (FALSE == DuplicateHandle(GetCurrentProcess(), 
								SectionHandle, 
								share_block->hTargetProcHandle, 
								&share_block->hShareSectionHandle, 
								0, 
								FALSE, 
								DUPLICATE_SAME_ACCESS))
	{
		_ASSERTE(!"DuplicateHandle");
		goto return_fail;
	}
	CloseHandle(SectionHandle);	
	SectionHandle = INVALID_HANDLE_VALUE;

	// event 를 생성하고, 마찬가지로 핸들을 복사한다.
	// 
	share_block->hSendCmd = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == share_block->hSendCmd)
	{
		_ASSERTE(!"CreateEvent");
		goto return_fail;
	}
	share_block->hRecvReply =  CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == share_block->hRecvReply)
	{
		_ASSERTE(!"CreateEvent");
		goto return_fail;
	}

	if (FALSE == DuplicateHandle(GetCurrentProcess(), 
								share_block->hSendCmd, 
								share_block->hTargetProcHandle, 
								&share_block->hRecvCmd, 
								0, 
								FALSE, 
								DUPLICATE_SAME_ACCESS))
	{
		_ASSERTE(!"DuplicateHandle");
		goto return_fail;
	}

	if (FALSE == DuplicateHandle(GetCurrentProcess(), 
								share_block->hRecvReply, 
								share_block->hTargetProcHandle, 
								&share_block->hSendReply, 
								0, 
								FALSE, 
								DUPLICATE_SAME_ACCESS))
	{
		_ASSERTE(!"DuplicateHandle");
		goto return_fail;
	}

	// change share block status
	//
	share_block->Status = STATUS_READY;
	share_block->IsRunning = TRUE;

	return share_block;

return_fail:
	{
		if(NULL != share_block)
		{			
			if (NULL != share_block->hTargetProcHandle) 
				CloseHandle(share_block->hTargetProcHandle);
			if (NULL != share_block->hSendCmd)
				CloseHandle(share_block->hSendCmd);
			if (NULL != share_block->hRecvReply)
				CloseHandle(share_block->hRecvReply);			
			if (NULL != share_block->hShareSectionHandle)
				CloseHandle(share_block->hShareSectionHandle);
			if (NULL != share_block->hRecvCmd)
				CloseHandle(share_block->hRecvCmd);
			if (NULL != share_block->hSendReply)
				CloseHandle(share_block->hSendReply);
			
			RtlFillMemory(share_block, sizeof(INJECTOR_CONTEXT), 0xcc);
			UnmapViewOfFile(share_block);
		}

		if (INVALID_HANDLE_VALUE != SectionHandle)
		{
			CloseHandle(SectionHandle);
		}
	}
	return NULL;
}


/**	---------------------------------------------------------------------------
	\brief	injected 된 스레드의 parameter 로 넘겨줄 data block 을 생성하는
			함수

	\param	pParam	parameter block 포인터
			hSharedSectionCopy	duplicate 한 공유메모리 핸들
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int _init_parameter_block(AK_PARAM* pParam, HANDLE hSharedSectionCopy)
{
	_ASSERTE(NULL != pParam);
	_ASSERTE(TRUE != IsBadWritePtr(pParam, sizeof(AK_PARAM)));
	
	if ((NULL != pParam) && 
		(TRUE != IsBadWritePtr(pParam, sizeof(AK_PARAM))))
	{
#ifdef _DEBUG
		memset(pParam, 0xCC, sizeof(AK_PARAM));
#else
		memset(pParam, 0x90, sizeof(AK_PARAM));
#endif//_DEBUG

		pParam->pGetLastError = GetLastError;
		pParam->pFormatMessage = FormatMessage;
		pParam->pLoadLibrary = LoadLibrary;
		pParam->pGetProcAddress = GetProcAddress;
		pParam->pFreeLibrary = FreeLibrary;
		pParam->pOutputDebugString = OutputDebugString;
		pParam->pGetModuleHandle = GetModuleHandle;
		pParam->pMapViewOfFile = MapViewOfFile;
		pParam->pUnmapViewOfFile = UnmapViewOfFile;
		pParam->pResetEvent = ResetEvent;
		pParam->pSetEvent = SetEvent;
		pParam->pWaitForSingleObject = WaitForSingleObject;
		pParam->pCloseHandle = CloseHandle;

		pParam->lastErrorCode = 0;	
		pParam->hShareSectionHandle = hSharedSectionCopy;
		                              
		return 0;
	}

	return -1;
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
void FreeInjectorContext(INJECTOR_CONTEXT* ptr)
{
	_ASSERTE(NULL != ptr);

	INJECTOR_CONTEXT* tmp = ptr;
	ptr = NULL;

	if (NULL != tmp)
	{	
		if (NULL != tmp->hTargetProcHandle)
		{
			if (NULL != tmp->DataPtr)
				VirtualFreeEx(tmp->hTargetProcHandle, tmp->DataPtr, 
						tmp->DataPtrLen, MEM_RELEASE);	
				
			if (NULL != tmp->CodePtr)
				VirtualFreeEx(tmp->hTargetProcHandle, tmp->CodePtr, 
						tmp->CodePtrLen, MEM_RELEASE);
		}

		// Handles was duplicated must closed by target process context.
		// Never close these handles here !
		//
		//if (NULL != tmp->hShareSectionHandle) CloseHandle(tmp->hShareSectionHandle);
		//if (NULL != tmp->hRecvCmd) CloseHandle(tmp->hRecvCmd);
		//if (NULL != tmp->hSendReply) CloseHandle(tmp->hSendReply);

		// close handles in injector process context
		//
		if (NULL != tmp->hSendCmd) CloseHandle(tmp->hSendCmd);
		if (NULL != tmp->hRecvReply) CloseHandle(tmp->hRecvReply);
		if (NULL != tmp->hTargetProcHandle) CloseHandle(tmp->hTargetProcHandle);
		
		RtlFillMemory(tmp, sizeof(INJECTOR_CONTEXT), 0xcc);
		UnmapViewOfFile(tmp);			
	}
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int	InjectThread(INJECTOR_CONTEXT* pCtx, RMT_THREAD_PROC pRmtThrdProc)
{
	_ASSERTE(NULL != pCtx);
	_ASSERTE(TRUE != IsBadWritePtr(pCtx, sizeof(INJECTOR_CONTEXT)));
	if ((NULL == pCtx) || (TRUE == IsBadWritePtr(pCtx, sizeof(INJECTOR_CONTEXT))))
	{
		return -1;
	}
	_ASSERTE(NULL != pRmtThrdProc);
	_ASSERTE(TRUE != IsBadCodePtr((FARPROC) pRmtThrdProc));
	if ((NULL == pRmtThrdProc) || (TRUE == IsBadCodePtr((FARPROC) pRmtThrdProc) ))
	{
		return -1;
	}

	// #1
	// Target 과 Injector 가 공유할 데이터 할당 
	// 
	AK_PARAM ParamBlock = {0, };
	if (0 != _init_parameter_block(&ParamBlock, pCtx->hShareSectionHandle))
	{
		_ASSERTE(!"_init_parameter_block");
		return -1;
	}


	// #2 
	// ready to inject thread and thread parameter
	//
	DWORD_PTR ProcBody = GetFunctionRVA(pRmtThrdProc);
	pCtx->CodePtrLen = _size_of_proc(ProcBody);
	if(0 == pCtx->CodePtrLen)
	{
		_ASSERTE(!"_size_of_proc");
		return -1;
	}


	// alloc parameter block 
	//
	pCtx->DataPtrLen = sizeof(AK_PARAM);
	pCtx->DataPtr = (char*) VirtualAllocEx(
										pCtx->hTargetProcHandle,
										NULL, 
										pCtx->DataPtrLen, 
										MEM_COMMIT, 
										PAGE_EXECUTE_READWRITE);
	if (NULL == pCtx->DataPtr)
	{
		_ASSERTE(!"pCtx->DataPtr");
		return -1;
	}

	pCtx->CodePtr = (char *) VirtualAllocEx( 
										pCtx->hTargetProcHandle,
										NULL, 
										pCtx->CodePtrLen, 
										MEM_COMMIT, 
										PAGE_EXECUTE_READWRITE);
	if (NULL == pCtx->CodePtr)
	{
		_ASSERTE(!"pCtx->CodePtr");
		return -1;
	}

	// write data block and code to remote process
	//
	DWORD dwBytesWritten = 0;
	if (FALSE == WriteProcessMemory(
							pCtx->hTargetProcHandle,
							pCtx->DataPtr,
							(LPCVOID)&ParamBlock,
							pCtx->DataPtrLen, 
							&dwBytesWritten))
	{
		_ASSERTE(!"write prcs memory");
		return -1;
	}

	if (FALSE == WriteProcessMemory(
							pCtx->hTargetProcHandle,
							pCtx->CodePtr,
							(LPCVOID)ProcBody, 
							pCtx->CodePtrLen, 
							&dwBytesWritten))
	{
		_ASSERTE(!"write prcs memory");
		return -1;
	}

	// create remote thread
	//
	// According to the MSDN, we should call VirtualProtectEx explicitly for dynamically 
	// generated code, even though already passed PAGE_EXECUTE_READWRITE to VirtualAlloc
	// It probably doesn't make any different whether VirtualAllocEx is passed
	// PAGE_READWRITE or PAGE_EXECUTE_READWRITE.
	//
	DWORD OldProtection = 0;
	if (!VirtualProtectEx(pCtx->hTargetProcHandle,
								pCtx->CodePtr, 
								pCtx->CodePtrLen, 
								PAGE_EXECUTE_READWRITE, 
								&OldProtection))
	{
		_ASSERTE(!"VirtualProtectEx"); 
		return -1;
	}

	if (!FlushInstructionCache(pCtx->hTargetProcHandle, 
									pCtx->CodePtr, 
									pCtx->CodePtrLen))
	{
		_ASSERTE(!"FlushInstructionCache"); 
		return -1;
	}

	DWORD dwTid = 0;
	HANDLE hThread = CreateRemoteThread( 
						pCtx->hTargetProcHandle,
						NULL,
						0,
						(LPTHREAD_START_ROUTINE)pCtx->CodePtr,
						pCtx->DataPtr,	
						CREATE_SUSPENDED,
						&dwTid);
	if (NULL == hThread)
	{
		_ASSERTE(!"hTherad");
		return -1;
	}

	// start remote thread
	// 
	ResumeThread(hThread);	
	
	return 0;	// success !~
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int CMD_Quit(INJECTOR_CONTEXT* block)
{
	_ASSERTE(NULL != block);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	if (
		(NULL == block) ||
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)))
		)
	{
		return -1;
	}

	if (block->Status == STATUS_THREAD_EXITED) return 0;
	if (block->Status != STATUS_READY) return -1;

	if (0 != SendCommand(block, CMD_QUIT, 5000))
	{
		WriteToFileW(INJECTOR_LOG,
			L"injecotr > SendCommand(0x%08x, CMD_QUIT, 5000) failed",
			block);
		return -1;							
	}

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int CMD_Continue(INJECTOR_CONTEXT* block)
{
	_ASSERTE(NULL != block);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	if (
		(NULL == block) ||
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)))
		)
	{
		return -1;
	}

	_ASSERTE(block->Status == STATUS_WAIT_CONTINUE);
	if (block->Status != STATUS_WAIT_CONTINUE) return -1;

	if (0 != SendCommand(block, CMD_CONTINUE, 5000))
	{
		WriteToFileW(INJECTOR_LOG,
			L"injecotr > SendCommand(0x%08x, CMD_CONTINUE, 5000) failed",
			block);
		return -1;							
	}

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int CMD_Hello(INJECTOR_CONTEXT* block)
{
	_ASSERTE(NULL != block);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	if (
		(NULL == block) ||
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)))
		)
	{
		return -1;
	}

	_ASSERTE(block->Status == STATUS_READY);
	if (block->Status != STATUS_READY) return -1;

	if (0 != SendCommand(block, CMD_HELLO, 5000))
	{
		WriteToFileW(INJECTOR_LOG,
			L"injecotr > SendCommand(0x%08x, CMD_HELLO, 5000) failed",
			block);
		return -1;							
	}

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int	CMD_LoadPlugin(INJECTOR_CONTEXT* block, PCMD_LOAD_PLUGIN_REC rec)
{
	_ASSERTE(NULL != block);
	_ASSERTE(NULL != rec);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	_ASSERTE(TRUE != IsBadReadPtr(rec, sizeof(CMD_LOAD_PLUGIN_REC)));
	if (
		(NULL == block) || (NULL == rec) ||
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT))) ||
		(TRUE == IsBadReadPtr(rec, sizeof(CMD_LOAD_PLUGIN_REC)))
		)
	{
		return -1;
	}

	if (TRUE != IsFileExist(rec->PluginName))
	{
		WriteToFileW(INJECTOR_LOG, L"injecotr > can not load %s plugin dll",rec->PluginName);		
		_ASSERTE(!"no plugin files");
		return -1;
	}
	
	_ASSERTE(block->Status == STATUS_READY);
	if (block->Status != STATUS_READY) return -1;
	
	// only debug
	_ASSERTE(sizeof(block->ShareBuffer) >= sizeof(CMD_LOAD_PLUGIN_REC));

	RtlZeroMemory(block->ShareBuffer, sizeof(block->ShareBuffer));	
	RtlCopyMemory(block->ShareBuffer, rec, sizeof(CMD_LOAD_PLUGIN_REC));
	if (0 != SendCommand(block, CMD_LOAD_PLUGIN, 5000))
	{
		WriteToFileW(INJECTOR_LOG,
			L"injecotr > SendCommand(0x%08x, CMD_LOAD_PLUGIN, 5000) failed",
			block);
		return -1;							
	}

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int CMD_UnloadPlugin(INJECTOR_CONTEXT* block)
{
	_ASSERTE(NULL != block);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	if (
		(NULL == block) ||
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)))
		)
	{
		return -1;
	}

	_ASSERTE(block->Status == STATUS_READY);
	if (block->Status != STATUS_READY) return -1;

	if (0 != SendCommand(block, CMD_UNLOAD_PLUGIN, 5000))
	{
		WriteToFileW(INJECTOR_LOG,
			L"injecotr > SendCommand(0x%08x, CMD_UNLOAD_PLUGIN, 5000) failed",
			block);
		return -1;							
	}

	return 0;

}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int CMD_CallPlugin(INJECTOR_CONTEXT* block,PCMD_CALL_PLUGIN_REC rec)
{
	_ASSERTE(NULL != block);
	_ASSERTE(NULL != rec);
	_ASSERTE(TRUE != IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT)));
	_ASSERTE(TRUE != IsBadReadPtr(rec, sizeof(CMD_CALL_PLUGIN_REC)));
	if (
		(NULL == block) || (NULL == rec) ||
		(TRUE == IsBadReadPtr(block, sizeof(INJECTOR_CONTEXT))) ||
		(TRUE == IsBadReadPtr(rec, sizeof(CMD_CALL_PLUGIN_REC)))
		)
	{
		return -1;
	}

	_ASSERTE(block->Status == STATUS_READY);
	if (block->Status != STATUS_READY) return -1;
	
	RtlZeroMemory(block->ShareBuffer, sizeof(block->ShareBuffer));
	RtlCopyMemory(block->ShareBuffer, rec, sizeof(CMD_CALL_PLUGIN_REC));
	if (0 != SendCommand(block, CMD_CALL_PLUGIN, 5000))
	{
		WriteToFileW(INJECTOR_LOG,
			L"injecotr > SendCommand(0x%08x, CMD_CALL_PLUGIN, 5000) failed",
			block);
		return -1;							
	}

	// copy result
	//
	RtlCopyMemory(rec->Output, block->ShareBuffer, SHARE_BUFFER_SIZE);
	return 0;
}


/**	---------------------------------------------------------------------------
	\brief	

		DEBUG 모드인경우

		push argument
		call my_thread_proc
		...
		my_thread_proc :
		jmp RVA
		형태로 코드가 생성됨
		따라서 실제 구현코드는 jmp 인스트럭션 다음 4바이트값이다.
		
		RELEASE 모드인 경우 
		funcName 은 실제 func 의 body 부분임

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
DWORD_PTR GetFunctionRVA(void* FuncName)
{
	_ASSERTE(NULL != FuncName);
	if (NULL == FuncName)
	{
		return 0;
	}

#ifdef _DEBUG
	void *_tempFuncName = FuncName;
	char *ptempFuncName = PCHAR(_tempFuncName);
	DWORD_PTR _jmpdwRVA, dwRVA;
	CopyMemory(&_jmpdwRVA, ptempFuncName + 1, 4);
	dwRVA = (DWORD_PTR)ptempFuncName + _jmpdwRVA + 5;
#else
	DWORD_PTR dwRVA = (DWORD_PTR)FuncName;
#endif // _DEBUG
	return (dwRVA);
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int _size_of_proc(DWORD_PTR FuncBody)
{
	_ASSERTE(0 != FuncBody);
	if (0 == FuncBody)
	{
		return 0;
	}


	int i = 0;
	BOOL bBreak = FALSE;
	BYTE *tmp = (BYTE *)FuncBody;
	do
	{
		// prolog code bytes		
		// 55					push        ebp  // push   ebp		
		// 8B EC				mov         ebp,esp // mov      ebp, esp		
		// 81 EC 94 00 00 00 sub         esp,94h  // sub      esp, __LOCAL_SIZE
		// --> 9 byte
		//
		// epilog code bytes (epilog signature)
		// [33][C0][33][C0][C2][04][00]
		//  xor eax xor eax ret 4 (RET imm16)
		//
		if(0xC2 == tmp[i])
		{
			if (10 > i)
			{
				// never reach here
				//
				_ASSERTE(!"invalid function body");
				return -1;
			}

			if ((0xC0 == tmp[i - 1]) &&
				(0x33 == tmp[i - 2]) &&
				(0xC0 == tmp[i - 3]) &&
				(0x33 == tmp[i - 4]))
			{
				// ok valid epilog
				//
				i += 2;		// for imm16 
				bBreak = TRUE;
			}
		}	

		// bBreak = TRUE 로 설정한 경우에도 아래의 코드는 실행됨
		// 인덱스는 0 부터 시작 하고, 이 함수는 길이를 리턴해야 하므로
		// ++i 코드는 항상 실행되어야 한다.
		//
		++i;		
	} while (bBreak != TRUE);

	return i;
}


/**	---------------------------------------------------------------------------
	\brief	This returns the infamous NULL DACL
			The caller must free the SECURITY_ATTRIBUTES returned

			original function from security.c (mconover@gmail.com)

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SECURITY_ATTRIBUTES* _get_null_dacl(BOOL IsInheritable)
{
	SECURITY_DESCRIPTOR *pSD = NULL;
	SECURITY_ATTRIBUTES *pSecurityAttributes = NULL;

	if (NULL == (pSD = (SECURITY_DESCRIPTOR *)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH))) return NULL;
	if (NULL == InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) goto abort;
	if (NULL == (pSecurityAttributes = (SECURITY_ATTRIBUTES *)malloc(sizeof(SECURITY_ATTRIBUTES)))) goto abort;

	if (!SetSecurityDescriptorDacl(pSD, TRUE, NULL, FALSE))
	{
#ifdef DEBUG
		fprintf(stderr, "SetSecurityDescriptorDacl failed: error code 0x%08lx\n", GetLastError());
#endif
		goto abort;
	}

	pSecurityAttributes->bInheritHandle = IsInheritable;
	pSecurityAttributes->lpSecurityDescriptor = pSD;
	pSecurityAttributes->nLength = sizeof(SECURITY_ATTRIBUTES);
	return pSecurityAttributes;

abort:
	if (pSD) free(pSD);
	if (pSecurityAttributes) free(pSecurityAttributes);
	return NULL;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
BOOL _free_dacl(SECURITY_ATTRIBUTES *SecurityAttributes)
{
	if (!SecurityAttributes) return FALSE;
	if (SecurityAttributes->lpSecurityDescriptor)
	{
		SECURITY_DESCRIPTOR *pSD = (SECURITY_DESCRIPTOR *)SecurityAttributes->lpSecurityDescriptor;
		if (pSD->Dacl) free(pSD->Dacl);
		if (pSD->Sacl) free(pSD->Sacl);
		if (pSD->Owner) FreeSid(pSD->Owner);
		if (pSD->Group) FreeSid(pSD->Group);
		free(pSD);
		SecurityAttributes->lpSecurityDescriptor = NULL;
	}

	free(SecurityAttributes);
	return TRUE;
}