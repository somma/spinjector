/*-----------------------------------------------------------------------------
 * PlgnPE.h
 *-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * Revision History:
 * Date					Who					What
 * ----------------		----------------	----------------
 * 28.11.2007			somma		birth
**---------------------------------------------------------------------------*/

#ifdef PLGNPE_EXPORTS
	#ifdef __cplusplus
		#define PLGNPE_API			extern "C" __declspec(dllexport)
		#define PLGNPE_CLASS		__declspec(dllexport)
	#else
		#define PLGNPE_API			__declspec(dllexport)	
	#endif//__cplusplus
#else
	#ifdef __cplusplus
		#define PLGNPE_API			extern "C" __declspec(dllimport)
		#define PLGNPE_CLASS		__declspec(dllimport) 
	#else
		#define PLGNPE_API			__declspec(dllimport)
	#endif//__cplusplus		
#endif//PLGNPE_EXPORTS

#include "injector_common.h"




///////////////////////////////////////////////////////////////////////////////
//	PlgnPE.h , pe.h 중복 정의  
//
#ifndef _PE_HEADER_DEFINED_
#define _PE_HEADER_DEFINED_

typedef struct _PE_HEADER
{	
	PIMAGE_DOS_HEADER		idh;
	PIMAGE_NT_HEADERS		inh;
	PIMAGE_SECTION_HEADER	ish;

	IMAGE_DOS_HEADER		idh_copy;
	IMAGE_NT_HEADERS		inh_copy;
	IMAGE_SECTION_HEADER	ish_copy[1];		
	// contigious memroy for ish_copy
	//
} PE_HEADER, *PPE_HEADER;

#define HEADER_SIZE(SectionSize) \
	(sizeof(PE_HEADER) - sizeof(PIMAGE_SECTION_HEADER) + (SectionSize))
	
#endif//_PE_HEADER_DEFINED_
///////////////////////////////////////////////////////////////////////////////



typedef		DWORD_PTR	PLGN_PE_HANDLE;

//==============================================================================
// APIs that called by injector
//==============================================================================

// PE HEADER API
//
PLGNPE_API int	__stdcall PlgnInitPlgnPe(PCMD_CALL_PLUGIN_REC Param);
PLGNPE_API int 	__stdcall PlgnFreePlgnPe(PCMD_CALL_PLUGIN_REC Param);
PLGNPE_API int	__stdcall PlgnGetPluginName(PCMD_CALL_PLUGIN_REC Param);
PLGNPE_API int	__stdcall PlgnGetPEHeaderSize(PCMD_CALL_PLUGIN_REC Param);
PLGNPE_API int	__stdcall PlgnGetPEHeader(PCMD_CALL_PLUGIN_REC Param);
PLGNPE_API int	__stdcall PlgnReadMemory(PCMD_CALL_PLUGIN_REC Param);
PLGNPE_API int	__stdcall PlgnWriteMemory(PCMD_CALL_PLUGIN_REC Param);

// HOOKDefender API
//

//==============================================================================
// APIs that called directly
//==============================================================================

// PE HEADER API
//
PLGNPE_API 
PLGN_PE_HANDLE		
__stdcall 
InitPlgnPe(
    LPCTSTR ImagePath
    );


PLGNPE_API 
void		
__stdcall 
FreePlgnPe(
    PLGN_PE_HANDLE handle
    );


PLGNPE_API 
DWORD	
__stdcall 
GetPEHeaderSize(
    IN PLGN_PE_HANDLE handle
    );


PLGNPE_API 
int		
__stdcall 
GetPEHeader(
    IN	PLGN_PE_HANDLE	handle, 
	IN	DWORD			SizeOfDest,
	OUT PPE_HEADER		Dest
    );


PLGNPE_API 
int		
__stdcall 
ReadMemory(
    IN	PLGN_PE_HANDLE	handle, 
	IN	DWORD_PTR		Addr, 
	IN	DWORD			Len, 
	OUT	PBYTE			Buf
    );


PLGNPE_API 
int		
__stdcall 
WriteMemory(
    IN	PLGN_PE_HANDLE	handle, 
    IN	DWORD_PTR		Addr, 
	IN	DWORD			Len, 
	IN	PBYTE			Buf
    );

// HOOKDefender API
//
typedef     void*   PLGN_HKD_HANDLE;        // INTERNAL_HOOK_DEFENDER 포인터


PLGNPE_API 
PLGN_HKD_HANDLE
__stdcall 
PlgnInitHookDefender();


PLGNPE_API 
void		
__stdcall 
PlgnFreeHookDefender(
    PLGN_HKD_HANDLE& handle
    );

PLGNPE_API 
BOOL
__stdcall 
PlgnCheckApiHook(
    PLGN_HKD_HANDLE& handle
    );

PLGNPE_API 
BOOL
__stdcall 
PlgnRestoreAllApiHook(
    PLGN_HKD_HANDLE& handle
    );











