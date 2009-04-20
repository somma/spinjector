/*-----------------------------------------------------------------------------
 * GhostAPI.inc
 *-----------------------------------------------------------------------------
 * Ghost 에서 제공하는 API 의 타입 정의
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// kernel32.dll
//-----------------------------------------------------------------------------
#ifdef UNICODE
	#define str_GetModuleHandle			"GetModuleHandleW"	
	#define str_LoadLibrary				"LoadLibraryW"
	#define str_OutputDebugString		"OutputDebugStringW"
	#define str_CreateFile				"CreateFileW"
	#define str_CreateFileMapping		"CreateFileMappingW"	
	#define str_FormatMessage			"FormatMessageW"
	#define str_CreateEvent				"CreateEventW"
	#define str_CreateProcess			"CreateProcessW"
	#define str_CreateMutex				"CreateMutexW"
#else
	#define str_GetModuleHandle			"GetModuleHandleA"	
	#define str_LoadLibrary				"LoadLibraryA"	
	#define str_OutputDebugString		"OutputDebugStringA"
	#define str_CreateFile				"CreateFileA"
	#define str_CreateFileMapping		"CreateFileMappingA"
	#define str_FormatMessage			"FormatMessageA"
	#define str_CreateEvent				"CreateEventA"
	#define str_CreateProcess			"CreateProcessA"
	#define str_CreateMutex				"CreateMutexA"
#endif // !UNICODE

#define str_FreeLibrary					"FreeLibrary"
#define str_WaitForSingleObject			"WaitForSingleObject"
#define str_CloseHandle					"CloseHandle"
#define str_CreateThread				"CreateThread"
#define str_CreateRemoteThread			"CreateRemoteThread"
#define str_SuspendThread				"SuspendThread"
#define str_ResumeThread				"ResumeThread"
#define str_GetCurrentProcessId			"GetCurrentProcessId"
#define str_OpenProcess					"OpenProcess"
#define str_VirtualAlloc				"VirtualAlloc"
#define str_VirtualFree					"VirtualFree"
#define str_VirtualAllocEx				"VirtualAllocEx"
#define str_VirtualFreeEx				"VirtualFreeEx"
#define str_VirtualProtect				"VirtualProtect"
#define str_VirtualProtectEx			"VirtualProtectEx"
#define str_ReadProcessMemory			"ReadProcessMemory"
#define str_WriteProcessMemory			"WriteProcessMemory"
#define str_GetProcAddress				"GetProcAddress"
#define str_TerminateProcess			"TerminateProcess"
#define str_MapViewOfFile				"MapViewOfFile"
#define str_UnmapViewOfFile				"UnmapViewOfFile"
#define str_GetLastError				"GetLastError"
#define	str_SetEvent					"SetEvent"
#define str_ResetEvent					"ResetEvent"
#define str_DeviceIoControl				"DeviceIoControl"
#define str_DuplicateHandle				"DuplicateHandle"


// CreateEvent
typedef HANDLE (__stdcall *TCREATEEVENTA)(
    IN LPSECURITY_ATTRIBUTES lpEventAttributes,
    IN BOOL bManualReset,
    IN BOOL bInitialState,
    IN LPCSTR lpName
    );

typedef HANDLE (__stdcall *TCREATEEVENTW)(
    IN LPSECURITY_ATTRIBUTES lpEventAttributes,
    IN BOOL bManualReset,
    IN BOOL bInitialState,
    IN LPCWSTR lpName
    );
#ifdef UNICODE
#define TCREATEEVENT  TCREATEEVENTW
#else
#define TCREATEEVENT  TCREATEEVENTA
#endif // !UNICODE

// DuplicateHandle
typedef BOOL (__stdcall *TDUPLICATEHANDLE) (  
	HANDLE hSourceProcessHandle,
	HANDLE hSourceHandle,
	HANDLE hTargetProcessHandle,
	LPHANDLE lpTargetHandle,
	DWORD dwDesiredAccess,
	BOOL bInheritHandle,
	DWORD dwOptions);

// CreateMutex
typedef HANDLE (__stdcall *TCREATEMUTEXA) (
	IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
	IN BOOL bInitialOwner,
	IN LPCSTR lpName);

typedef HANDLE (__stdcall *TCREATEMUTEXW) (
	IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
	IN BOOL bInitialOwner,
	IN LPCWSTR lpName);

#ifdef UNICODE
#define TCREATEMUTEX  TCREATEMUTEXW
#else
#define TCREATEMUTEX  TCREATEMUTEXA
#endif // !UNICODE

// InterlockedIncrement
typedef LONG (__stdcall *TINTERLOCKEDINCREMENT)(IN LONG volatile* Addend);

// SetEvent
typedef BOOL (__stdcall *TSETEVENT)(IN HANDLE hEvent);

// ResetEvent
typedef BOOL (__stdcall *TRESETEVENT)(IN HANDLE hEvent);

// WaitForSingleObject
typedef DWORD (__stdcall *TWAITFORSINGLEOBJECT)(IN HANDLE hHandle, IN DWORD dwMilliseconds);

// CloseHandle
typedef BOOL (__stdcall *TCLOSEHANDLE)(IN OUT HANDLE hObject);

// GetModuleHandle
typedef HMODULE (__stdcall * TGETMODULEHANDLEA) (IN LPCSTR lpModuleName);
typedef HMODULE (__stdcall * TGETMODULEHANDLEW) (IN LPCWSTR lpModuleName);
#ifdef UNICODE
	#define TGETMODULEHANDLE  TGETMODULEHANDLEW
#else
	#define TGETMODULEHANDLE  TGETMODULEHANDLEA
#endif // !UNICODE

// LoadLibrary API
typedef HMODULE (__stdcall *TLOADLIBRARYA) (IN LPCSTR lpLibFileName);
typedef HMODULE (__stdcall *TLOADLIBRARYW) (IN LPCWSTR lpLibFileName);

#ifdef UNICODE
	#define TLOADLIBRARY  TLOADLIBRARYW
#else
	#define TLOADLIBRARY  TLOADLIBRARYA
#endif // !UNICODE

// FreeLibrary 
typedef BOOL (__stdcall *TFREELIBRARY)(IN OUT HMODULE hLibModule);

// CreateThread
typedef HANDLE (__stdcall *TCREATETHREAD) (
	IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN SIZE_T dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId
    );

// CreateRemoteThrread
typedef HANDLE (__stdcall *TCREATEREMOTETHREAD) (
    IN HANDLE hProcess,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN SIZE_T dwStackSize,
    IN LPTHREAD_START_ROUTINE lpStartAddress,
    IN LPVOID lpParameter,
    IN DWORD dwCreationFlags,
    OUT LPDWORD lpThreadId
    );

// SuspendThread
typedef DWORD (__stdcall *TSUSPENDTHREAD)(
    IN HANDLE hThread
    );

// ResumeThread
typedef DWORD (__stdcall *TRESUMETHREAD)(
    IN HANDLE hThread
    );

// GetCurrentProcessId
typedef DWORD (__stdcall *TGETCURRENTPROCESSID)(VOID);

// OpenProcess
typedef HANDLE (__stdcall *TOPENPROCESS)(
    IN DWORD dwDesiredAccess,
    IN BOOL bInheritHandle,
    IN DWORD dwProcessId
    );

// VirtualAlloc
typedef LPVOID (__stdcall *TVIRTUALALLOC)(
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD flAllocationType,
    IN DWORD flProtect
    );

// VirtualFree
typedef BOOL (__stdcall *TVIRTUALFREE)(
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD dwFreeType
    );

// VirtualAllocEx
typedef LPVOID (__stdcall *TVIRTUALALLOCEX)(
    IN HANDLE hProcess,
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD flAllocationType,
    IN DWORD flProtect
    );

// VirtualFreeEx
typedef BOOL (__stdcall *TVIRTUALFREEEX)(
    IN HANDLE hProcess,
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD dwFreeType
    );

// VirtualProtect
typedef BOOL (__stdcall *TVIRTUALPROTECT)(
    IN  LPVOID lpAddress,
    IN  SIZE_T dwSize,
    IN  DWORD flNewProtect,
    OUT PDWORD lpflOldProtect
    );

// VirtualProtectEx
typedef BOOL (__stdcall *TVIRTUALPROTECTEX)(
    IN  HANDLE hProcess,
    IN  LPVOID lpAddress,
    IN  SIZE_T dwSize,
    IN  DWORD flNewProtect,
    OUT PDWORD lpflOldProtect
    );

// ReadProcessMemory
typedef BOOL (__stdcall *TREADPROCESSMEMORY)(
    IN HANDLE hProcess,
    IN LPCVOID lpBaseAddress,
    OUT LPVOID lpBuffer,
    IN SIZE_T nSize,
    OUT SIZE_T * lpNumberOfBytesRead
    );

// WriteProcessMemory
typedef BOOL (__stdcall *TWRITEPROCESSMEMORY)(
    IN HANDLE hProcess,
    IN LPVOID lpBaseAddress,
    IN LPCVOID lpBuffer,
    IN SIZE_T nSize,
    OUT SIZE_T * lpNumberOfBytesWritten
    );

// GetProcAddress
typedef FARPROC (__stdcall *TGETPROCADDRESS)(
    IN HMODULE hModule,
    IN LPCSTR lpProcName
    );

// TerminateProcess
typedef BOOL (__stdcall *TTERMINATEPROCESS)(
    IN HANDLE hProcess,
    IN UINT uExitCode
    );

// OutputDebugString
typedef VOID (__stdcall *TOUTPUTDEBUGSTRINGA)(
    IN LPCSTR lpOutputString
    );
typedef VOID (__stdcall *TOUTPUTDEBUGSTRINGW)(
    IN LPCWSTR lpOutputString
    );
#ifdef UNICODE
	#define TOUTPUTDEBUGSTRING  TOUTPUTDEBUGSTRINGW
#else
	#define TOUTPUTDEBUGSTRING  TOUTPUTDEBUGSTRINGA
#endif

// CreateFile
typedef HANDLE (__stdcall *TCREATEFILEA)(
    IN LPCSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
    );

typedef HANDLE (__stdcall *TCREATEFILEW)(
    IN LPCWSTR lpFileName,
    IN DWORD dwDesiredAccess,
    IN DWORD dwShareMode,
    IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    IN DWORD dwCreationDisposition,
    IN DWORD dwFlagsAndAttributes,
    IN HANDLE hTemplateFile
    );
#ifdef UNICODE
	#define TCREATEFILE			TCREATEFILEW
#else
	#define TCREATEFILE			TCREATEFILEA
#endif

typedef HANDLE (__stdcall *TCREATEFILEMAPPINGA)(
    IN HANDLE hFile,
    IN LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    IN DWORD flProtect,
    IN DWORD dwMaximumSizeHigh,
    IN DWORD dwMaximumSizeLow,
    IN LPCSTR lpName
    );


typedef HANDLE (__stdcall *TCREATEFILEMAPPINGW)(
    IN HANDLE hFile,
    IN LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    IN DWORD flProtect,
    IN DWORD dwMaximumSizeHigh,
    IN DWORD dwMaximumSizeLow,
    IN LPCWSTR lpName
    );
#ifdef UNICODE
	#define TCREATEFILEMAPPING		TCREATEFILEMAPPINGW
#else
	#define TCREATEFILEMAPPING		TCREATEFILEMAPPINGA
#endif

// MapViewOfFile
typedef LPVOID (__stdcall *TMAPVIEWOFFILE)(
    IN HANDLE hFileMappingObject,
    IN DWORD dwDesiredAccess,
    IN DWORD dwFileOffsetHigh,
    IN DWORD dwFileOffsetLow,
    IN SIZE_T dwNumberOfBytesToMap
    );

// UnmapViewOfFile
typedef BOOL (__stdcall *TUNMAPVIEWOFFILE)(
    IN LPCVOID lpBaseAddress
    );

typedef DWORD (__stdcall *TGETLASTERROR)(VOID);


typedef DWORD (__stdcall *TFORMATMESSAGEA)(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    LPSTR lpBuffer,
    DWORD nSize,
    va_list *Arguments
    );

typedef DWORD (__stdcall *TFORMATMESSAGEW)(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    LPWSTR lpBuffer,
    DWORD nSize,
    va_list *Arguments
    );

#ifdef UNICODE
#define TFORMATMESSAGE  TFORMATMESSAGEW
#else
#define TFORMATMESSAGE  TFORMATMESSAGEA
#endif // !UNICODE


typedef BOOL (__stdcall *TCREATEPROCESSA)(
    IN LPCSTR lpApplicationName,
    IN LPSTR lpCommandLine,
    IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN BOOL bInheritHandles,
    IN DWORD dwCreationFlags,
    IN LPVOID lpEnvironment,
    IN LPCSTR lpCurrentDirectory,
    IN LPSTARTUPINFOA lpStartupInfo,
    OUT LPPROCESS_INFORMATION lpProcessInformation
    );

typedef BOOL (__stdcall *TCREATEPROCESSW)(
    IN LPCWSTR lpApplicationName,
    IN LPWSTR lpCommandLine,
    IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
    IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
    IN BOOL bInheritHandles,
    IN DWORD dwCreationFlags,
    IN LPVOID lpEnvironment,
    IN LPCWSTR lpCurrentDirectory,
    IN LPSTARTUPINFOW lpStartupInfo,
    OUT LPPROCESS_INFORMATION lpProcessInformation
    );

#ifdef UNICODE
#define TCREATEPROCESS  TCREATEPROCESSW
#else
#define TCREATEPROCESS  TCREATEPROCESSA
#endif // !UNICODE


typedef BOOL (__stdcall *TDEVICEIOCONTROL)(
    IN HANDLE hDevice,
    IN DWORD dwIoControlCode,
    IN LPVOID lpInBuffer,
    IN DWORD nInBufferSize,
    OUT LPVOID lpOutBuffer,
    IN DWORD nOutBufferSize,
    OUT LPDWORD lpBytesReturned,
    IN LPOVERLAPPED lpOverlapped
    );


////-----------------------------------------------------------------------------
//// msvcrXX.dll
////-----------------------------------------------------------------------------
//#ifdef _DEBUG
//#define str_Msvcr71_dll					"msvcr71d.dll"
////#define str_Msvcr80_dll					"msvcr80d.dll"
//#define str_Msvcr80_dll					"msvcr80.dll"
//#else
//#define str_Msvcr71_dll					"msvcr71.dll"
//#define str_Msvcr80_dll					"msvcr80.dll"
//#endif//_DEBUG
//#define str_beginthreadex				"_beginthreadex"
//
//// _beginthreadex
//typedef uintptr_t (__cdecl *TBEGINTHREADEX) (
//	void *security,
//    unsigned stacksize,
//    unsigned (__stdcall * initialcode) (void *),
//    void * argument,
//    unsigned createflag,
//    unsigned *thrdaddr
//);