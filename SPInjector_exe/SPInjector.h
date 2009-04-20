/*-----------------------------------------------------------------------------
 * SPInjector.h
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

#ifdef SPINJECTOR_EXPORTS
	#ifdef __cplusplus
		#define SPINJECTOR_API			extern "C" __declspec(dllexport)
		#define SPINJECTOR_CLASS		__declspec(dllexport)
	#else
		#define SPINJECTOR_API			__declspec(dllexport)	
	#endif//__cplusplus
#else
	#ifdef __cplusplus
		#define SPINJECTOR_API			extern "C" __declspec(dllimport)
		#define SPINJECTOR_CLASS		__declspec(dllimport) 
	#else
		#define SPINJECTOR_API			__declspec(dllimport)
	#endif//__cplusplus		
#endif//SPINJECTOR_EXPORTS

#include "injector_common.h"


typedef		void*	SPINJECTOR_HANDLE;


SPINJECTOR_API 
LPCWSTR
__stdcall 
SPInjectorCommandToString(
    AK_INJECTOR_COMMAND cmd
    );

SPINJECTOR_API 
BOOL	
__stdcall 
SPSetPrivilege(
    LPCTSTR Privilege, 
    BOOL bEnablePrivilege
    );


SPINJECTOR_API
SPINJECTOR_HANDLE	
__stdcall 
SPAllocInjectorContext(
    DWORD dwPid
    );


SPINJECTOR_API
void	
__stdcall 
SPFreeInjectorContext(
    SPINJECTOR_HANDLE& hInjector
    );


SPINJECTOR_API
int 	
__stdcall 
SPInjectThread(
    SPINJECTOR_HANDLE hInjector
    );


SPINJECTOR_API
int		
__stdcall 
SPSendInjectorCommand(
    SPINJECTOR_HANDLE			hInjector, 
	AK_INJECTOR_COMMAND			cmd, 
	DWORD						dwTimeout
    );

//
// apis for access context record
//
SPINJECTOR_API
BOOL	
__stdcall 
SPIsAlive(
    SPINJECTOR_HANDLE hInjector
    );


SPINJECTOR_API
DWORD	
__stdcall 
SPGetTargetProcessId(
    SPINJECTOR_HANDLE hInjector
    );


//
// command wrapper apis
//
SPINJECTOR_API
int 
__stdcall 
SPCmdQuit(
    SPINJECTOR_HANDLE hInjector
    );


SPINJECTOR_API
int 
__stdcall 
SPCmdHello(
    SPINJECTOR_HANDLE hInjector
    );


SPINJECTOR_API
int	
__stdcall 
SPCmdLoadPlugin(
    SPINJECTOR_HANDLE hInjector, 
    LPCTSTR PluginPath
    );


SPINJECTOR_API
int 
__stdcall 
SPCmdUnloadPlugin(
    SPINJECTOR_HANDLE hInjector
    );


SPINJECTOR_API
int 
__stdcall 
SPCmdCallPlugin(
    SPINJECTOR_HANDLE		hInjector,
    PCMD_CALL_PLUGIN_REC	Params
    );