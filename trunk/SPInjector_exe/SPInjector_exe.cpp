/*-----------------------------------------------------------------------------
 * SPInjector_exe.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * Revision History:
 * Date					Who					What
 * ----------------		----------------	----------------
 * 26.11.2007			somma		        birth
**---------------------------------------------------------------------------*/

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "SPInjector.h"
#include "test_PlgnPE.h"

#define STRSAFE_NO_DEPRECATE
#define STRSAFE_LIB
#include <strsafe.h>
#pragma comment(lib, "strsafe.lib")

//
// internal functions
//
AK_INJECTOR_COMMAND _show_prompt();

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwPid = 0;
	SPINJECTOR_HANDLE hContext = NULL;	

	_tprintf(TEXT("enter pid: "));
	scanf_s("%d", &dwPid);
	if ((0x00000000 > dwPid) || (0xffffffff < dwPid))
	{
		_ASSERTE(!"invalid pid");
		return -1;
	}


    _ftprintf(stdout, _T("[*] enabl debug privilege...\n"));
	if (TRUE != SPSetPrivilege(SE_DEBUG_NAME, TRUE))
	{
		_ASSERTE(!"SPSetPrivilege");
		return -1;
	}
    _ftprintf(stdout, _T("[*] enabl debug privilege...[OK] \n"));

	//
	// allocate injector context
	//
    _ftprintf(stdout, _T("[*] allocate injector context for target process...\n"));
	hContext = SPAllocInjectorContext(dwPid);
	if (NULL == hContext)
	{
		_ASSERTE(!"InjectThread");
		SPSetPrivilege(SE_DEBUG_NAME, FALSE);
		return -1;
	}
    _ftprintf(stdout, _T("[*] allocate injector context for target process...[OK]\n"));

	//
	//  inject thread and command loop
	//
    _ftprintf(stdout, _T("[*] inject thread to target process...\n"));
	if (0 != SPInjectThread(hContext))
	{
		_ASSERTE(!"SPInjectThread");

		SPSetPrivilege(SE_DEBUG_NAME, FALSE);
		SPFreeInjectorContext(hContext);
		return -1;
	}
    _ftprintf(stdout, _T("[*] inject thread to target process...[OK]\n"));


    _ftprintf(stdout, _T("[*] load plugin and call methods of plugin from remote...\n"));
    test_plgn_pe(hContext);
    _ftprintf(stdout, _T("[*] load plugin and call methods of plugin from remote...[OK]\n"));


	//
	// free ctx
	//
	SPCmdQuit(hContext);
	SPSetPrivilege(SE_DEBUG_NAME, FALSE);
	SPFreeInjectorContext(hContext);

	_tprintf(TEXT("press any key to terminate.....\n"));
	_getch();
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
AK_INJECTOR_COMMAND _show_prompt()
{
	_tprintf(TEXT("[*] please enter command --\n"));
	_tprintf(TEXT(" x -- to quit\n")
			 TEXT(" h -- to send hello command\n")
			 TEXT(" l -- load plugin \n")
			 TEXT(" u -- unload plugin \n")
			 TEXT(" c -- call plugin method \n")
			 );
	char c = _getch();
	_ftprintf(stdout, TEXT("[*] > %c\n\n"), c);

	switch (c)
	{
	case 'x':
		return CMD_QUIT;
	case 'h':
		return CMD_HELLO;
	case 'l':
		return CMD_LOAD_PLUGIN;
	case 'u':
		return CMD_UNLOAD_PLUGIN;
	case 'c':
		return CMD_CALL_PLUGIN;
	default :
		return CMD_UNKNOWN;	
	}
}
