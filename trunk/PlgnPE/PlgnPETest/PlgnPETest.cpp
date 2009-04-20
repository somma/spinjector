/*-----------------------------------------------------------------------------
 * PlgnPETest.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by Noh Yong Hwan (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * Revision History:
 * Date                 Who                 What
 * ----------------     ----------------    ----------------
 * 11.04.2008           Noh Yong Hwan       birth
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "winunit.h"
#include "AKUtil.h"
#include "PlgnPE.h"
#include <vector>
using namespace std;


// demo only
// 
#include "hkdmodule.h"
#include "IHKDefender.h"
#include "IHKDefenderUtil.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

/** ---------------------------------------------------------------------------
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
    UNREFERENCED_PARAMETER(ul_reason_for_call);
    UNREFERENCED_PARAMETER(lpReserved);

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


PLGN_HKD_HANDLE hHKD = NULL;

void test_PHKD_MODULE(void);
void test_CheckApiHook(vector<PHOOKED_APIS_PER_MODULE>& HookedApiList);
void test_RestoreApiHook(vector<RFAI>& RestoreFaultApiList);

/** ---------------------------------------------------------------------------
    \brief  SETUP (befor test)

    \param  
    \return         
    \code
    \endcode        
-----------------------------------------------------------------------------*/
FIXTURE(HOOKDefender);
SETUP(HOOKDefender)
{
    LOG_ENTRY le(_T(__FUNCTION__));

    // hook defender 초기화
	//
    hHKD = PlgnInitHookDefender();
    WIN_ASSERT_NOT_NULL(hHKD);
}


/** ---------------------------------------------------------------------------
    \brief  TEARDOWN (after test)

    \param  
    \return         
    \code
    \endcode        
-----------------------------------------------------------------------------*/
TEARDOWN(HOOKDefender)
{
    LOG_ENTRY le(_T(__FUNCTION__));
    
    WIN_ASSERT_NOT_NULL(hHKD);
    PlgnFreeHookDefender(hHKD);
    WIN_ASSERT_NULL(hHKD);
}

/** ---------------------------------------------------------------------------
    \brief  

    \param  
    \return         
    \code
    \endcode        
-----------------------------------------------------------------------------*/
BEGIN_TESTF(CheckApiHook_and_Restore, HOOKDefender)
{
    for (int i = 0; i < 2; ++i )
	{
		WIN_ASSERT_TRUE( PlgnCheckApiHook(hHKD) );		
        WIN_ASSERT_TRUE( PlgnRestoreAllApiHook(hHKD) );
		
        // demo only
        //
        PINTERNAL_HOOK_DEFENDER p = (PINTERNAL_HOOK_DEFENDER) hHKD;
        
        test_CheckApiHook( p->rm_hapm->get() );
        test_RestoreApiHook( p->rm_rfai->get() );

        DebugString(TRUE, _T("Press any key to terminate test..	\n"));
        getchar();
    
	}
}
END_TESTF















void print_hai_list(vector<HAI>& hai_list);

/**	---------------------------------------------------------------------------
	\brief	PHKD_MODULE class 테스트

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
void test_PHKD_MODULE(void)
{
	DebugString(TRUE, _T("[*] PHKD_MODULE class test - press enter to continue...\n"));
	getchar();

	// method test
	//
	HKD_MODULE_LIST hkd_module_list;
	if (0 != hkd_module_list.Init(TRUE, TRUE))
	{
		_ASSERTE(!"Init");
		return;
	}	
	
	// print result 
	//
	PIMPORTED_API pIAPI = NULL;
	PEXPORTED_API pEAPI = NULL;
	vector<PHKD_MODULE>::iterator it = hkd_module_list.Get().begin();
	for (int i = 0; it != hkd_module_list.Get().end(); it++)
	{
		PHKD_MODULE pHKDM = *it;
		DebugString(TRUE, _T("[%04d] %s (base: 0x%08x, size: 0x%x)\n"), 
			i, 
			pHKDM->FullPathName,
			pHKDM->ImageBase, 
			pHKDM->SizeOfImage);

		// import api information
		//
		DebugString(TRUE, _T("\tIMPORTED API COUNT - %03d	\n"), pHKDM->ImportedApis->size());
		vector<IMPORTED_API>::iterator it_imp = pHKDM->ImportedApis->begin();
		for (; it_imp != pHKDM->ImportedApis->end(); it_imp++)
		{
			pIAPI = &(*it_imp);
			if (NULL != pIAPI)
			{
				if (0x00 == pIAPI->Name[0])
				{
					DebugString(TRUE, _T("\t[0x%08x] Ord: %04d \n"), 
						pIAPI->EntryPtr, pIAPI->uImportType.Ordinal);
					DebugString(TRUE, _T("\t%s\n\n"), pIAPI->Opcode);
				}
				else
				{
					DebugString(TRUE, _T("\t[0x%08x] %s (HINT: %04d)\n"), 
						pIAPI->EntryPtr, pIAPI->Name, pIAPI->uImportType.Hint);
					DebugString(TRUE, _T("\t%s\n\n"), pIAPI->Opcode);
				}			
			}
		}

		// exported api - from virtual memeory
		// 
		DebugString(TRUE, _T("\tEXPORTED API COUNT from MEMORY - %03d	\n"), pHKDM->ExportedApisFromMem->size());
		vector<EXPORTED_API>::iterator it_exp_vm = pHKDM->ExportedApisFromMem->begin();
		for (; it_exp_vm != pHKDM->ExportedApisFromMem->end(); it_exp_vm++)
		{
			pEAPI = &(*it_exp_vm);
			if (NULL != pEAPI)
			{				
				DebugString(TRUE, _T("\t[0x%08x] Ord: %04d %s\n"), 
					pEAPI->AddressOfApi, pEAPI->Ordinal, pEAPI->name);
				DebugString(TRUE, _T("\t%s\n\n"), pEAPI->Opcodes);
			}
		}

		// exproted api - from disk 
		//
		DebugString(TRUE, _T("\tEXPORTED API COUNT from MEMORY - %03d	\n"), pHKDM->ExportedApisFromDisk->size());
		vector<EXPORTED_API>::iterator it_exp_dsk = pHKDM->ExportedApisFromDisk->begin();
		for (; it_exp_dsk != pHKDM->ExportedApisFromDisk->end(); it_exp_dsk++)
		{
			pEAPI = &(*it_exp_dsk);
			if (NULL != pEAPI)
			{				
				DebugString(TRUE, _T("\t[0x%08x] Ord: %04d %s\n"), 
					pEAPI->AddressOfApi, pEAPI->Ordinal, pEAPI->name);
				DebugString(TRUE, _T("\t%s\n\n"), pEAPI->Opcodes);
			}
		}			

	}


	DebugString(TRUE, _T("[*] PHKD_MODULE class test - success...\n\n"));
	getchar();
}


/**	---------------------------------------------------------------------------
	\brief	IHKDefender::CheckApiHook() test

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
void test_CheckApiHook(vector<PHOOKED_APIS_PER_MODULE>& HookedApiList)
{
	PHOOKED_APIS_PER_MODULE	pHAPM = NULL;
	vector<PHOOKED_APIS_PER_MODULE>::iterator it = HookedApiList.begin();
	for(; it != HookedApiList.end(); it++)
	{
		pHAPM = *it;
		_ASSERTE(NULL != pHAPM);
		if (NULL != pHAPM)
		{
			DebugString(TRUE, _T("%s (0x%08x - 0x%08x) - %d api hooked\n"), 
				pHAPM->DllName, 
				pHAPM->RangeStart, 
				pHAPM->RangeEnd, 
				pHAPM->HookedApiList.size());
			print_hai_list(pHAPM->HookedApiList);
		}
	}	
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
void print_hai_list(vector<HAI>& hai_list)
{
	PHAI pHAI = NULL;
	vector<HAI>::iterator it = hai_list.begin();
	for (; it != hai_list.end(); it++)
	{
		pHAI = &(*it);
		_ASSERTE(NULL != pHAI);
		if (NULL != pHAI)
		{
            DebugStringA(TRUE, "\t%s  ", pHAI->ApiName);
			DebugString(FALSE, _T("(ord: %03d, org_addr: 0x%08x)\n"), 
				pHAI->Ordinal_or_Hint, pHAI->OrgAddr);
			DebugString(FALSE, _T("\t\thook_type: %s\n"), hook_type_to_string(pHAI->HookType));
			DebugString(FALSE, _T("\t\tcur_addr: 0x%08x, detour: 0x%08x\n"), 
				pHAI->CurAddr, pHAI->DetourAddr);
			DebugString(FALSE, _T("\t\torg_op: %s\n\t\tcur_op: %s\n"), 
				pHAI->OrgOpcode,
				pHAI->CurOpcode);
		}
	}
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
void test_RestoreApiHook(vector<RFAI>& RestoreFaultApiList)
{
	PRFAI	pRFAI = NULL;

	if (0 == RestoreFaultApiList.size())
	{
		DebugString(TRUE, _T("\t\tno restore fault api - all restored !!\n"));	
	}
	else
	{
		vector<RFAI>::iterator it = RestoreFaultApiList.begin();
		for(; it != RestoreFaultApiList.end(); it++)
		{
			pRFAI = &(*it);
			_ASSERTE(NULL != pRFAI);
			if (NULL != pRFAI)
			{
				DebugString(FALSE, _T("\t\trestore_fail [%d]: %s (0x%08x)\n"), 
					pRFAI->ErrorCode, 
					pRFAI->HookedApiInfo.ApiName, 
					pRFAI->HookedApiInfo.CurAddr);
			}
		}
	}
}

