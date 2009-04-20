/*-----------------------------------------------------------------------------
 * PlgnPE.cpp
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

#include "stdafx.h"
#include "PlgnPE.h"
#include "pe.h"

#include "AKUtil.h"

#define STRSAFE_NO_DEPRECATE
#define STRSAFE_LIB
#include <strsafe.h>
#pragma comment(lib, "strsafe.lib")



/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(ul_reason_for_call);
    UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

/**	---------------------------------------------------------------------------
	\brief	

	\param	
				Param->Input.Param_1	: Plugin path ( e.g c:\plgn_pe.dll )
	\return			
				Param->Output			: PLGN_PE_HANDLE
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int __stdcall PlgnInitPlgnPe(PCMD_CALL_PLUGIN_REC Param)
{
    _ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}
	
	PLGN_PE_HANDLE p = InitPlgnPe((LPCTSTR) Param->Input.Param_1);
	if (NULL == p)
	{
		return -1;
	}
	else
	{
		RtlZeroMemory(Param->Output, sizeof(CMD_CALL_PLUGIN_REC));	
		
		RtlCopyMemory(Param->Output, &p, sizeof(PLGN_PE_HANDLE));		
	}
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
				Param->Input.Param_1	: PLGN_PE_HANDLE
	\return			
				Param->Output			: null
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int __stdcall PlgnFreePlgnPe(PCMD_CALL_PLUGIN_REC Param)
{
	_ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}
	
	PLGN_PE_HANDLE handle = *(PLGN_PE_HANDLE*)Param->Input.Param_1;	

	FreePlgnPe(handle);
	RtlZeroMemory(Param->Output, sizeof(CMD_CALL_PLUGIN_REC));	

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API 
int __stdcall PlgnGetPluginName(PCMD_CALL_PLUGIN_REC Param)
{
	_ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}

	if (! SUCCEEDED(StringCbPrintf((LPTSTR)Param->Output, 
								   sizeof(Param->Output), 
								   _T("%s"), 
								   _T("PE analyzer"))))
	{
		return -1;
	}

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param		
				Param->Input.Param_1	: PLGN_PE_HANDLE
	\return			
				Param->Output			: PE_HEADER 구조체의 전체 크기 
										(section 갯수가 PE 마다 다르므로 동적 사이즈임)
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int __stdcall PlgnGetPEHeaderSize(PCMD_CALL_PLUGIN_REC Param)
{
	_ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}

	PLGN_PE_HANDLE handle = *(DWORD*)Param->Input.Param_1;
	
	DWORD size = GetPEHeaderSize(handle);
	RtlZeroMemory(Param->Output, sizeof(CMD_CALL_PLUGIN_REC));
	RtlCopyMemory(Param->Output, &size, sizeof(DWORD));
	
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	
			
	\param		
				Param->Input.Param_1	: PLGN_PE_HANDLE
	\return			
				Param->Output			: PE_HEADER 구조체 복사본
									(Output 의 정확한 사이즈는 PlgnGetPEHeaderSize() )
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int __stdcall PlgnGetPEHeader(PCMD_CALL_PLUGIN_REC Param)
{
	_ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}

	PLGN_PE_HANDLE handle = *(PLGN_PE_HANDLE*)Param->Input.Param_1;	

	RtlZeroMemory(Param->Output, sizeof(CMD_CALL_PLUGIN_REC));
	int ret = GetPEHeader( handle, sizeof(Param->Output), (PPE_HEADER)Param->Output);

	return (ret);
}

/**	---------------------------------------------------------------------------
	\brief	

	\param		
				Param->Input.Param_1	: PLGN_PE_HANDLE
				Param->Input.Param_2	: Address to read
				Param->Input.Param_3	: Length to read
	\return			
				Param->Output			: Address 의 메모리 (Length 만큼)
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int	__stdcall PlgnReadMemory(PCMD_CALL_PLUGIN_REC Param)
{
	_ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}

	PLGN_PE_HANDLE handle = *(PLGN_PE_HANDLE*)Param->Input.Param_1;	
	DWORD_PTR Addr = *(DWORD_PTR*)&Param->Input.Param_2;
	DWORD Len = *(DWORD*)&Param->Input.Param_3;

	RtlZeroMemory(Param->Output, sizeof(CMD_CALL_PLUGIN_REC));
	int ret = ReadMemory(handle, Addr, Len, Param->Output);

	return (ret);
}

/**	---------------------------------------------------------------------------
	\brief	

	\param		
				Param->Input.Param_1	: PLGN_PE_HANDLE
				Param->Input.Param_2	: Address to write
				Param->Input.Param_3	: Length to write
				Param->Input.Param_4	: Data to write
	\return			
				Param->Output			: nil
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int	__stdcall PlgnWriteMemory(PCMD_CALL_PLUGIN_REC Param)
{
	_ASSERTE(NULL != Param);
	_ASSERTE(TRUE != IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)));
	if (TRUE == IsBadWritePtr(Param, sizeof(CMD_CALL_PLUGIN_REC)))
	{
		return -1;
	}

	PLGN_PE_HANDLE handle = *(PLGN_PE_HANDLE*)Param->Input.Param_1;	
	DWORD_PTR Addr = *(DWORD_PTR*)&Param->Input.Param_2;
	DWORD Len = *(DWORD*)&Param->Input.Param_3;
	PBYTE Buf = (PBYTE) Param->Input.Param_4;


	int ret = WriteMemory(handle, Addr, Len, Buf);
	RtlZeroMemory(Param->Output, sizeof(CMD_CALL_PLUGIN_REC));

	return (ret);
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API 
PLGN_PE_HANDLE __stdcall InitPlgnPe(LPCTSTR ImagePath)
{
	PEAnalyzer* p = NULL;

	if ((NULL == ImagePath) || (0x00 == ImagePath[0]))
	{
		// PE image on memory
		//
		p = new PEMemory();
		if (0 != p->Init())
		{
			delete p; p = NULL;
			return NULL;
		}		
	}
	else
	{
		// PE image on file
		//
		p = new PEImage();
		if (0 != p->Init(ImagePath))
		{
			delete p; p = NULL;
			return NULL;
		}
	}

	return (PLGN_PE_HANDLE)p;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API void __stdcall FreePlgnPe(PLGN_PE_HANDLE handle)
{
	_ASSERTE(NULL != handle);
	if (NULL != handle)
	{
		PEAnalyzer* p = (PEAnalyzer*) handle;
		handle = NULL;
		
		delete p;
	}
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API DWORD __stdcall GetPEHeaderSize(IN PLGN_PE_HANDLE handle)
{
	_ASSERTE(NULL != handle);
	if (NULL == handle) return 0;

	PEAnalyzer* pea = (PEAnalyzer*) handle;
	DWORD ret = pea->GetPEHeaderSize();

	return (ret);
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int __stdcall GetPEHeader( IN  PLGN_PE_HANDLE	handle, 
							IN  DWORD			SizeOfDest, 
							OUT PPE_HEADER		Dest)
{
	_ASSERTE(NULL != handle);
	_ASSERTE(NULL != Dest);
	_ASSERTE(TRUE != IsBadWritePtr(Dest, SizeOfDest));
	if (
		(NULL == handle) ||
		(NULL == Dest) || 
		(TRUE == IsBadWritePtr(Dest, SizeOfDest))
		)
	{
		return -1;
	}

	PEAnalyzer* pea = (PEAnalyzer*) handle;
	if (pea->GetPEHeaderSize() > SizeOfDest)
	{
		_ASSERTE(!"too small buffer");
		return -1;
	}

	int ret = pea->GetPEHeaders(Dest);

	return (ret);
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int		__stdcall ReadMemory(IN	PLGN_PE_HANDLE	handle, 
							   IN	DWORD_PTR		Addr, 
							   IN	DWORD			Len, 
							   OUT	PBYTE			Buf)
{
	_ASSERTE(NULL != handle);
	PEAnalyzer* pea = (PEAnalyzer*) handle;
	
	int ret = pea->ReadMemory(Addr, Len, Buf);

	return (ret);
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLGNPE_API int		__stdcall WriteMemory(IN	PLGN_PE_HANDLE	handle, 
								IN	DWORD_PTR		Addr, 
								IN	DWORD			Len, 
								IN	PBYTE			Buf)
{
	_ASSERTE(NULL != handle);
	PEAnalyzer* pea = (PEAnalyzer*) handle;
	
	int ret = pea->WriteMemory(Addr, Len, Buf);

	return (ret);
}

///** ---------------------------------------------------------------------------
//    \brief  
//
//    \param  
//    \return         
//    \code
//    \endcode        
//-----------------------------------------------------------------------------*/
//PLGNPE_API 
//PLGN_HKD_HANDLE
//__stdcall 
//PlgnInitHookDefender()
//{
//    PINTERNAL_HOOK_DEFENDER phkd = (PINTERNAL_HOOK_DEFENDER)malloc(sizeof(INTERNAL_HOOK_DEFENDER));
//    if (NULL == phkd)
//    {
//        DBG_OP2 _T("insufficient resources") DBG_END
//        return NULL;
//    }
//    RtlZeroMemory(phkd, sizeof(INTERNAL_HOOK_DEFENDER));
//
//    phkd->rm_hapm = new RM_HAPM;
//	phkd->rm_rfai = new RM_RFAI;
//    phkd->hkd = IHKDefender::CreateInstance();
//    if ( (NULL == phkd->rm_hapm) || (NULL == phkd->rm_rfai) ||
//         (NULL == phkd->hkd) )
//    {
//        DBG_OP2 _T("insufficient resources") DBG_END
//
//        _free_internal_hook_defender(phkd);
//        _ASSERTE(NULL == phkd);
//
//        return NULL;
//    }
//
//    if (0 != phkd->hkd->Init(TRUE, TRUE) )
//    {
//        DBG_OP2 _T("init failed") DBG_END
//
//        _free_internal_hook_defender(phkd);
//        _ASSERTE(NULL == phkd);
//
//        return NULL;
//    }
//
//    return (PLGN_HKD_HANDLE) phkd;
//}
//
//
//
///** ---------------------------------------------------------------------------
//    \brief  
//
//    \param  
//    \return         
//    \code
//    \endcode        
//-----------------------------------------------------------------------------*/
//PLGNPE_API 
//void		
//__stdcall 
//PlgnFreeHookDefender(
//    PLGN_HKD_HANDLE& handle
//    )
//{
//    _ASSERTE(NULL != handle);
//    if (NULL != handle)
//    {
//        PINTERNAL_HOOK_DEFENDER p = (PINTERNAL_HOOK_DEFENDER) handle;
//        handle = NULL;
//
//        _free_internal_hook_defender(p);
//        _ASSERTE(NULL == p);
//    }
//}
//
///** ---------------------------------------------------------------------------
//    \brief  
//
//    \param  
//    \return         
//    \code
//    \endcode        
//-----------------------------------------------------------------------------*/
//PLGNPE_API 
//BOOL
//__stdcall 
//PlgnCheckApiHook(
//    PLGN_HKD_HANDLE& handle
//    )
//{
//    _ASSERTE(NULL != handle);
//    if (NULL == handle) 
//    {
//        return FALSE;
//    }
//
//    PINTERNAL_HOOK_DEFENDER p = (PINTERNAL_HOOK_DEFENDER) handle;
//    if (0 != p->hkd->CheckApiHook(p->rm_hapm->get()))
//    {
//        return FALSE;
//    }
//    else
//    {
//        return TRUE;
//    }
//}
//
///** ---------------------------------------------------------------------------
//    \brief  
//
//    \param  
//    \return         
//    \code
//    \endcode        
//-----------------------------------------------------------------------------*/
//PLGNPE_API 
//BOOL
//__stdcall 
//PlgnRestoreAllApiHook(
//    PLGN_HKD_HANDLE& handle
//    )
//{
//    _ASSERTE(NULL != handle);
//    if (NULL == handle) 
//    {
//        return FALSE;
//    }
//
//    PINTERNAL_HOOK_DEFENDER p = (PINTERNAL_HOOK_DEFENDER) handle;
//    if (0 != p->hkd->RestoreApiHook(p->rm_hapm->get(), p->rm_rfai->get()))
//    {
//        return FALSE;
//    }
//    else
//    {
//        return TRUE;
//    }
//}
//
//#pragma TODO("인덱스를 통해서 한개만 복원할 수 있는 함수 추가")



