/*-----------------------------------------------------------------------------
 * test_PlgnPE.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * Revision History:
 * Date					Who					What
 * ----------------		----------------	----------------
 * 10.12.2007			somma		birth
**---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "test_PlgnPE.h"
#define STRSAFE_NO_DEPRECATE
#define STRSAFE_LIB
#include <strsafe.h>
#pragma comment(lib, "strsafe.lib")
#include "AKUtil.h"

int __load_plugin(SPINJECTOR_HANDLE hContext);
int __unload_plugin(SPINJECTOR_HANDLE hContext);
int __free_plgn_pe_handle(SPINJECTOR_HANDLE hContext, PLGN_PE_HANDLE hPlugin);

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int test_plgn_pe(SPINJECTOR_HANDLE hContext)
{
	// STEP #1
	// load plugin
	//
	if (0 != __load_plugin(hContext))
	{
		return -1;
	}

	// STEP #2
	// test plugin pe methods
	//
	
	CMD_CALL_PLUGIN_REC cp = {0};
	PLGN_PE_HANDLE hPlugin = NULL;

	// PlgnInitPlgnPe
	RtlZeroMemory(&cp, sizeof(CMD_CALL_PLUGIN_REC));
	if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName , 
								sizeof(cp.Input.ProcName), 
								"%s", 
								"PlgnInitPlgnPe")))
	{
		_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));

		__unload_plugin(hContext);
		return -1;
	}
	cp.Input.Param_1[0] = 0x00;		// analyze current process !

	if (0 != SPCmdCallPlugin(hContext, &cp))
	{
		_ftprintf(stdout, _T("error > PlgnInitPlgnPe() failed \n"));

		__unload_plugin(hContext);
		return -1;
	}

	// assign hPlugin value
	//
	RtlCopyMemory(&hPlugin, &cp.Output, sizeof(PLGN_PE_HANDLE));

	// PlgnGetPEHeaderSize
	RtlZeroMemory(&cp, sizeof(CMD_CALL_PLUGIN_REC));
	if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName , 
							sizeof(cp.Input.ProcName), 
							"%s", 
							"PlgnGetPEHeaderSize")))
	{
		_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}

	RtlCopyMemory(cp.Input.Param_1, &hPlugin, sizeof(PLGN_PE_HANDLE));	
	if (0 != SPCmdCallPlugin(hContext, &cp))
	{
		_ftprintf(stdout, _T("error > PlgnGetPEHeaderSize() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;				
	}
	DWORD PeHeaderSize = 0;
	RtlCopyMemory(&PeHeaderSize, &cp.Output, sizeof(PeHeaderSize));
	
	// PlgnGetPEHeader
	RtlZeroMemory(&cp, sizeof(CMD_CALL_PLUGIN_REC));
	if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName , 
							sizeof(cp.Input.ProcName), 
							"%s", 
							"PlgnGetPEHeader")))
	{
		_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}
	RtlCopyMemory(cp.Input.Param_1, &hPlugin, sizeof(PLGN_PE_HANDLE));	
	if (0 != SPCmdCallPlugin(hContext, &cp))
	{
		_ftprintf(stdout, _T("error > PlgnGetPEHeader() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}

	// putting it all together
	//
	PPE_HEADER peh = (PPE_HEADER) cp.Output;
	_ftprintf(stdout, _T("PE_HEADER size: %u \n"), PeHeaderSize);
	_ftprintf(stdout, _T("IMAGE_DOS_HEADER	: 0x%08p \n"), peh->idh);
	_ftprintf(stdout, _T("IMAGE_NT_HEADER		: 0x%08p \n"), peh->inh);
	_ftprintf(stdout, _T("IMAGE_SECTION_HEADER	: 0x%08p \n"), peh->ish);
	

	// read memory test
	//
	DWORD_PTR	Addr = 0x01000000;
	DWORD		Len = 32;

	RtlZeroMemory(&cp, sizeof(CMD_CALL_PLUGIN_REC));
	if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName , 
							sizeof(cp.Input.ProcName), 
							"%s", 
							"PlgnReadMemory")))
	{
		_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}
	RtlCopyMemory(cp.Input.Param_1, &hPlugin, sizeof(PLGN_PE_HANDLE));	
	RtlCopyMemory(cp.Input.Param_2, &Addr, sizeof(DWORD_PTR));
	RtlCopyMemory(cp.Input.Param_3, &Len, sizeof(DWORD));	
	if (0 != SPCmdCallPlugin(hContext, &cp))
	{
		_ftprintf(stdout, _T("error > PlgnGetPEHeader() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}
	DumpMemory(stdout, Len, cp.Output);

	// write memory test
	//
	Addr = 0x01000000;
	Len = 1;

	RtlZeroMemory(&cp, sizeof(CMD_CALL_PLUGIN_REC));
	if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName , 
							sizeof(cp.Input.ProcName), 
							"%s", 
							"PlgnWriteMemory")))
	{
		_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}
	RtlCopyMemory(cp.Input.Param_1, &hPlugin, sizeof(PLGN_PE_HANDLE));	
	RtlCopyMemory(cp.Input.Param_2, &Addr, sizeof(DWORD_PTR));
	RtlCopyMemory(cp.Input.Param_3, &Len, sizeof(DWORD));	
	cp.Input.Param_4[0] = 'X';
	
	if (0 != SPCmdCallPlugin(hContext, &cp))
	{
		_ftprintf(stdout, _T("error > PlgnGetPEHeader() failed \n"));

		__free_plgn_pe_handle(hContext, hPlugin);
		__unload_plugin(hContext);
		return -1;
	}
	

				// verifiy...
				//
				Addr = 0x01000000;
				Len = 32;

				RtlZeroMemory(&cp, sizeof(CMD_CALL_PLUGIN_REC));
				if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName , 
										sizeof(cp.Input.ProcName), 
										"%s", 
										"PlgnReadMemory")))
				{
					_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));

					__free_plgn_pe_handle(hContext, hPlugin);
					__unload_plugin(hContext);
					return -1;
				}
				RtlCopyMemory(cp.Input.Param_1, &hPlugin, sizeof(PLGN_PE_HANDLE));	
				RtlCopyMemory(cp.Input.Param_2, &Addr, sizeof(DWORD_PTR));
				RtlCopyMemory(cp.Input.Param_3, &Len, sizeof(DWORD));	
				if (0 != SPCmdCallPlugin(hContext, &cp))
				{
					_ftprintf(stdout, _T("error > PlgnGetPEHeader() failed \n"));

					__free_plgn_pe_handle(hContext, hPlugin);
					__unload_plugin(hContext);
					return -1;
				}
				DumpMemory(stdout, Len, cp.Output);




	// STEP #3
	// free pe plugin object
	if (0 != __free_plgn_pe_handle(hContext, hPlugin))
	{
		_ftprintf(stdout, _T("error > __free_plgn_pe_handle() failed \n"));
	}

	// STEP #4
	// unload plugin
	//
	return __unload_plugin(hContext);
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int __load_plugin(SPINJECTOR_HANDLE hContext)
{
    TCHAR cur[MAX_PATH+1]={0};
    GetCurrentDirectory(MAX_PATH, cur);
    TCHAR path[MAX_PATH+1]={0};
    StringCbPrintf(path, sizeof(path), TEXT("%s\\PlgnPE.dll"), cur);

    _tprintf(
        TEXT("trying to load dll (%s) to target process... \n"),
        path
        );

	if (0 != SPCmdLoadPlugin(hContext, path) )
	{
		if ( FALSE == SPIsAlive(hContext) )
		{
			_tprintf(TEXT("[pid : 0x%08x] already terminated\n"), 
				SPGetTargetProcessId(hContext));
		}
		else
			_tprintf(TEXT("[pid : 0x%08x] send [ CMD_LOAD_PLUGIN ] cmd failed\n"), 
				SPGetTargetProcessId(hContext));

		return -1;
	}
	else
		_tprintf(TEXT("[pid : 0x%08x] send [ CMD_LOAD_PLUGIN] success \n"), 
			SPGetTargetProcessId(hContext));
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int __unload_plugin(SPINJECTOR_HANDLE hContext)
{
	if (0 != SPCmdUnloadPlugin(hContext))
	{
		if ( FALSE == SPIsAlive(hContext) )
		{
			_tprintf(TEXT("[pid : 0x%08x] already terminated\n"), 
				SPGetTargetProcessId(hContext));
		}
		else
		{
			_tprintf(TEXT("[pid : 0x%08x] SPCmdUnloadPlugin() failed \n"), 
				SPGetTargetProcessId(hContext));
		}		

		return -1;
	}
	else
		_tprintf(TEXT("[pid : 0x%08x] SPCmdUnloadPlugin() success \n"), 
			SPGetTargetProcessId(hContext));
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int __free_plgn_pe_handle(SPINJECTOR_HANDLE hContext, PLGN_PE_HANDLE hPlugin)
{
	CMD_CALL_PLUGIN_REC cp = {0};
	if (TRUE != SUCCEEDED(StringCbPrintfA(cp.Input.ProcName, 
							sizeof(cp.Input.ProcName), 
							"%s",
							"PlgnFreePlgnPe")))
	{
		_ftprintf(stderr, _T("error > StringCbPrintfA() failed \n"));
		return -1;
	}
	RtlCopyMemory(cp.Input.Param_1, &hPlugin, sizeof(PLGN_PE_HANDLE));

	if (0 != SPCmdCallPlugin(hContext, &cp))
	{
		_ftprintf(stdout, _T("error > PlgnFreePlgnPe() failed \n"));		
		return -1;
	}
	return 0;
}
