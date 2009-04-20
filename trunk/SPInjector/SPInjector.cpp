#include "stdafx.h"
#include "SPInjector.h"
#include "injector.h"		
#include "AKUtil.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

/**	---------------------------------------------------------------------------
	\brief	just wrapper 	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
#ifndef CASE_RETURN_STR
#define CASE_RETURN_STR(s)	case s: return L#s
#endif
SPINJECTOR_API 
LPCWSTR
__stdcall 
SPInjectorCommandToString(
    AK_INJECTOR_COMMAND cmd
    )
{
    switch (cmd)
    {
    CASE_RETURN_STR(CMD_UNKNOWN); 
    CASE_RETURN_STR(CMD_QUIT);
    CASE_RETURN_STR(CMD_CONTINUE);
    CASE_RETURN_STR(CMD_HELLO);
    CASE_RETURN_STR(CMD_LOAD_PLUGIN);
    CASE_RETURN_STR(CMD_UNLOAD_PLUGIN);
    CASE_RETURN_STR(CMD_CALL_PLUGIN);
    }
    return L"Unknown";
}

/**	---------------------------------------------------------------------------
	\brief	just wrapper 	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API 
BOOL __stdcall SPSetPrivilege(LPCTSTR Privilege, BOOL bEnablePrivilege)
{
	return ( SetPrivilege(Privilege, bEnablePrivilege) );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
SPINJECTOR_HANDLE __stdcall SPAllocInjectorContext(DWORD dwPid)
{
	return SPINJECTOR_HANDLE(AllocInjectorContext(dwPid));
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
void __stdcall SPFreeInjectorContext(SPINJECTOR_HANDLE& hInjector)
{
	if (NULL != hInjector)
	{
		FreeInjectorContext((INJECTOR_CONTEXT*) hInjector);
		hInjector = NULL;
	}
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
int __stdcall SPInjectThread(SPINJECTOR_HANDLE hInjector)
{
	return ( InjectThread((INJECTOR_CONTEXT*)hInjector, inject_thread_stub) );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API 
int __stdcall SPSendInjectorCommand(SPINJECTOR_HANDLE			hInjector, 
						AK_INJECTOR_COMMAND				cmd, 
						DWORD							dwTimeout)
{
	return ( SendCommand( (INJECTOR_CONTEXT*) hInjector, cmd, dwTimeout) );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
BOOL __stdcall SPIsAlive(SPINJECTOR_HANDLE hInjector)
{
	return ( ((INJECTOR_CONTEXT*)hInjector)->IsRunning );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
DWORD __stdcall SPGetTargetProcessId(SPINJECTOR_HANDLE hInjector)
{
	return ( ((INJECTOR_CONTEXT*) hInjector)->TargetProcId );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
int __stdcall SPCmdQuit(SPINJECTOR_HANDLE hInjector)
{
	return ( CMD_Quit((INJECTOR_CONTEXT*) hInjector) );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
int __stdcall SPCmdHello(SPINJECTOR_HANDLE hInjector)
{
	if (0 != CMD_Hello((INJECTOR_CONTEXT*) hInjector) )
	{
		return -1;
	}
	else
	{
		CMD_Continue((INJECTOR_CONTEXT*) hInjector);
		return 0;
	}
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
int __stdcall SPCmdLoadPlugin(SPINJECTOR_HANDLE hInjector, LPCTSTR PluginPath)
{
	CMD_LOAD_PLUGIN_REC rec = {0,};
	if(!SUCCEEDED(StringCbPrintf(rec.PluginName, sizeof(rec.PluginName), TEXT("%s"), PluginPath))) return -1;
	
	if (0 != CMD_LoadPlugin((INJECTOR_CONTEXT*) hInjector, &rec))
	{
		return -1;
	}
	else
	{
		CMD_Continue((INJECTOR_CONTEXT*) hInjector);
		return 0;
	}
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
int __stdcall SPCmdUnloadPlugin(SPINJECTOR_HANDLE hInjector)
{
	if (0 != CMD_UnloadPlugin((INJECTOR_CONTEXT*) hInjector))
	{
		return -1;
	}
	else
	{
		CMD_Continue((INJECTOR_CONTEXT*) hInjector);
		return 0;
	}
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
SPINJECTOR_API
int __stdcall SPCmdCallPlugin(SPINJECTOR_HANDLE		hInjector, 					
					PCMD_CALL_PLUGIN_REC	Params)
{
	_ASSERTE(NULL != Params);
	_ASSERTE(TRUE != IsBadReadPtr(Params, sizeof(CMD_CALL_PLUGIN_REC)));
	if (
			(NULL == Params) ||
			(TRUE == IsBadReadPtr(Params, sizeof(CMD_CALL_PLUGIN_REC)))
		)
	{
		return -1;
	}

	if (0 != CMD_CallPlugin((INJECTOR_CONTEXT*) hInjector, Params) )
	{
		return -1;
	}
	else
	{
		CMD_Continue((INJECTOR_CONTEXT*) hInjector);
		return 0;
	}
}