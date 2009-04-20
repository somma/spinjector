/*-----------------------------------------------------------------------------
 * peb.cpp
 *-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "peb.h"
#include <crtdbg.h>

//kd> dt _NT_TIB
//   +0x000 ExceptionList    : Ptr32 _EXCEPTION_REGISTRATION_RECORD
//   +0x004 StackBase        : Ptr32 Void
//   +0x008 StackLimit       : Ptr32 Void
//   +0x00c SubSystemTib     : Ptr32 Void
//   +0x010 FiberData        : Ptr32 Void
//   +0x010 Version          : Uint4B
//   +0x014 ArbitraryUserPointer : Ptr32 Void
//   +0x018 Self             : Ptr32 _NT_TIB			<<==
#define XPSP2_NT_TIB_SELF_OFFSET		0x018

//kd> dt _TEB
//   +0x000 NtTib            : _NT_TIB
//   +0x01c EnvironmentPointer : Ptr32 Void
//   +0x020 ClientId         : _CLIENT_ID
//   +0x028 ActiveRpcHandle  : Ptr32 Void
//   +0x02c ThreadLocalStoragePointer : Ptr32 Void
//   +0x030 ProcessEnvironmentBlock : Ptr32 _PEB		<<==
//   +0x034 LastErrorValue   : Uint4B
//   +0x038 CountOfOwnedCriticalSections : Uint4B
//.....
#define XPSP2_NT_TEB_PEB_OFFSET			0x030

//kd> dt _PEB
//   +0x000 InheritedAddressSpace : UChar
//   +0x001 ReadImageFileExecOptions : UChar
//   +0x002 BeingDebugged    : UChar
//   +0x003 SpareBool        : UChar
//   +0x004 Mutant           : Ptr32 Void
//   +0x008 ImageBaseAddress : Ptr32 Void
//   +0x00c Ldr              : Ptr32 _PEB_LDR_DATA		<<==
//   +0x010 ProcessParameters : Ptr32 _RTL_USER_PROCESS_PARAMETERS
//   +0x014 SubSystemData    : Ptr32 Void
//.....
#define XPSP2_PEB_LDR_OFFSET			0x00C


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
CPeb::CPeb()
:	m_init(FALSE),
	m_pPeb(NULL),
	m_pPebLdrData(NULL)
{		
};


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
CPeb::~CPeb()
{	
	if (TRUE == initialized())
		terminate();
};


// ntdll.dll, kernel32.dll ���� ���ڿ��� �̸� �ؽ��� �ΰ�, Ldr �� ������ 
// Hash ���� ���ؼ� ����� ���̽� �ּҸ� �����Ϸ� ������ Ldr �� DllBaseName
// ���� UNICODE_STRING �� ��ȯ�ϸ鼭 ���ڿ��� �߰ߵǱ� �� ����. 
// 
// ���� �ش� DllBaseName �� Ư�� �������� ���� ��� ���ϴ� ����� ����Ѵ�.
//
///**	---------------------------------------------------------------------------
//	\brief	���ڿ� �ؽø� ���ϴ� �Լ�
//			this hash function is described in "The C Programming Language"
//			by Brian Kernighan and Donald Ritchie, Prentice Hall
//
//	\param	
//	\return			
//	\code
//	\endcode		
//-----------------------------------------------------------------------------*/
//DWORD CPeb::HashBKDR(int buffer_length, const char *modname_buf)
//{
//	int i = 0;
//	DWORD ret = 0;
//
//	if (0 >= buffer_length)
//	{
//		_ASSERTE(!"invalid buffer length");
//		return 0;
//	}
//	if (TRUE == IsBadReadPtr(modname_buf, buffer_length))
//	{
//		_ASSERTE(!"BadReadPtr()");
//		return 0;
//	}
//
//
//	for (i = 0; i < buffer_length; i ++)
//	{		
//		ret = (ret * 31) + modname_buf[i];
//	}
//	return ret;
//}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
BOOL CPeb::init()
{
	// _PEB �ּ� ��� 
	// 
	PBYTE pPeb = NULL;
	
	__asm
	{
		mov eax, FS:XPSP2_NT_TIB_SELF_OFFSET			// _NT_TIB.Self
		mov eax, dword ptr[eax + XPSP2_NT_TEB_PEB_OFFSET]		// _TEB.ProcessEnvironmentBlock	(_PEB)
		mov pPeb, eax
	}

	m_pPeb = pPeb;


	//
	// ���� ���� �� �ڵ� ����µ� �� �νð� �ŷȴ�. -_-;; �� ~ ���
	// ���� ���̻� �ڵ����� ���ƾ� �ڴ�... 
	//
	//DWORD_PTR ldr;
	//memmove(&ldr, m_pPeb + XPSP2_PEB_LDR_OFFSET, sizeof(DWORD_PTR));
	//
	m_pPebLdrData = (PPEB_LDR_DATA)*(DWORD_PTR *)(m_pPeb + XPSP2_PEB_LDR_OFFSET );

	m_init = TRUE;
	return TRUE;
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
BOOL CPeb::terminate()
{
	m_init = FALSE;
	return TRUE;
}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PBYTE CPeb::getPeb()
{
	if (TRUE == initialized())
		return m_pPeb;
	else
	{
		_ASSERTE(!"not initialized");
		return NULL;
	}	
};


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PPEB_LDR_DATA CPeb::getPebLdr()
{
	if (TRUE == initialized())
		return m_pPebLdrData;
	else
	{
		_ASSERTE(!"not initialized");
		return NULL;
	}	
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLIST_ENTRY CPeb::getmodule_by_loadorder()
{
	if (TRUE != initialized())
	{
		_ASSERTE(!"not initialized");
		return NULL;
	}

	return &m_pPebLdrData->InLoadOrderModuleList;		
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLIST_ENTRY CPeb::getmodule_by_memorder()
{
	if (TRUE != initialized())
	{
		_ASSERTE(!"not initialized");
		return NULL;
	}

	return &m_pPebLdrData->InMemoryOrderModuleList;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PLIST_ENTRY CPeb::getmodule_by_initorder()
{
	if (TRUE != initialized())
	{
		_ASSERTE(!"not initialized");
		return NULL;
	}

	return &m_pPebLdrData->InInitializationOrderModuleList;
}

