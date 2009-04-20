/*-----------------------------------------------------------------------------
 * peb.h
 *-----------------------------------------------------------------------------
 *
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#ifndef _PEB_INCLUDE_H_
#define _PEB_INCLUDE_H_


#include <windows.h>
#include <Ntsecapi.h>


//kd> dt _PEB_LDR_DATA
//   +0x000 Length           : Uint4B
//   +0x004 Initialized      : UChar
//   +0x008 SsHandle         : Ptr32 Void
//   +0x00c InLoadOrderModuleList : _LIST_ENTRY
//   +0x014 InMemoryOrderModuleList : _LIST_ENTRY
//   +0x01c InInitializationOrderModuleList : _LIST_ENTRY
//   +0x024 EntryInProgress  : Ptr32 Void
//
typedef struct _PEB_LDR_DATA 
{
  ULONG                   Length;
  ULONG					  Initialized;
  PVOID                   SsHandle;
  LIST_ENTRY              InLoadOrderModuleList;				// _LDR_MODULE 구조체 리스트
  LIST_ENTRY              InMemoryOrderModuleList;				// _LDR_MODULE 구조체 리스트
  LIST_ENTRY              InInitializationOrderModuleList;		// _LDR_MODULE 구조체 리스트
} PEB_LDR_DATA, *PPEB_LDR_DATA;


typedef struct _LDR_MODULE 
{
  LIST_ENTRY              InLoadOrderModuleList;
  LIST_ENTRY              InMemoryOrderModuleList;
  LIST_ENTRY              InInitializationOrderModuleList;

  PVOID                   BaseAddress;
  PVOID                   EntryPoint;
  ULONG                   SizeOfImage;
  UNICODE_STRING          FullDllName;
  UNICODE_STRING          BaseDllName;
  ULONG                   Flags;
  SHORT                   LoadCount;
  SHORT                   TlsIndex;
  LIST_ENTRY              HashTableEntry;
  ULONG                   TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;


//-----------------------------------------------------------------------------
// 프로세스의 PEB 포인터를 구하고, PEB 의 정보를 관리하기 위한 클래스
//-----------------------------------------------------------------------------
class CPeb
{
public:
	CPeb();
    ~CPeb();

	BOOL initialized() {return m_init;};

	// 초기화/소멸 메소드
	//
	BOOL init();
	BOOL terminate();
	
	// _PEB 포인터를 얻어오는 메소드
	//
	PBYTE	getPeb();
	
	// _PEB.Ldr 관련 메소드
	//
	PPEB_LDR_DATA	getPebLdr();	
	PLIST_ENTRY		getmodule_by_loadorder();
	PLIST_ENTRY		getmodule_by_memorder();
	PLIST_ENTRY		getmodule_by_initorder();
protected:	
private:
	BOOL			m_init;
	PBYTE			m_pPeb;				// _PEB 주소
	PPEB_LDR_DATA	m_pPebLdrData;		// _PEB_LDR_DATA 주소
	
	//static DWORD HashBKDR(const std::string str);	

};


#endif // _PEB_INCLUDE_H_
