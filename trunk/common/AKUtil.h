/*-----------------------------------------------------------------------------
 * AKUtil.h
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>


// Window 플랫폼코드
//
#define OSTYPE_WIN_95		0x0400
#define OSTYPE_WIN_98		0x0410
#define OSTYPE_WIN_SE		0x0410
#define OSTYPE_WIN_ME		0x0490

#define	OSTYPE_WIN_NT		0x0400

#define OSTYPE_WIN_2000		0x0500
#define OSTYPE_WIN_XP		0x0501
#define OSTYPE_WIN_2003		0x0502
#define OSTYPE_WIN_LONGHORN	0x0600


/// 편의상 재정의
// windows 95 family 계열은 0x000F 마스크적용

#define		_WIN95			0x0001
#define		_WIN98			0x0002
#define		_WINSE			0x0004
#define		_WINME			0x0008

// windows nt family 계열은 0x00F0 마스크적용

#define		_WINNT			0x0010
#define		_WIN2000		0x0020
#define		_WINXP			0x0040
#define		_WIN2003		0x0080
#define		_WINLONGHORN	0x00c0

#define		MASK_WIN95		0x000F
#define		MASK_WINNT		0x00F0

/// Windows NT/2000/XP/2003/LONGHORN 패밀리인지를 확인하는 매크로
//
//  < 2006.03.31 노용환 >
//  ISNTFAMILY(GetWinVersion()) 형태로 사용하면 되는데 
//	절대로 
//		if ( ISNTFAMILY(GetWinVersion()) == TRUE ) 
//		{
//			// for ntfamily code 
//			//
//		}
//	형태로 사용하면 안된다. 
//	TRUE 는 windef.h (또는 다른 데서..) 단순히 1 로 정의되어있을 뿐이므로
//	nt family 인 경우라 하더라도 TRUE 를 리턴할 수 없으므로 버그를 만들수 있다.
//	
//		if ( ISNTFAMILY(GetWinVersion()  ) 
//		{
//			// for ntfamily code 
//			//
//		}
//
//	또는 
// 
//		if ( ISNTFAMILY(GetWinVersion() != 0 ) 
//		{
//			// for ntfamily code 
//			//
//		}
// 형태로 사용해야 한다. 
//
#define	ISNTFAMILY(x)	(x & MASK_WINNT)

/// Windows 95/98/SE/ME 패밀리인지를 확인하는 매크로
#define IS95FAMILY(x)	(x & MASK_WIN95)



// 주의 
// new 를 통해서 생성하면 메모리 릭 생김, 꼭 지역변수로만 사용할 것
// 
// RefStringW 가 가변 인자로 사용되는 경우 반드시 WCHAR* 로 명시적으로 
// 타입 캐스팅을 해 주어야 함. (묵시적으로 어케 해야 변환이 되는건지.. -__-)
//
class RefStringW
{
public:
    RefStringW(){};
    RefStringW(WCHAR* null_terminated_string)
    {
        if (NULL == null_terminated_string){m_str = L"";}
        else{m_str = null_terminated_string;}
    }
    ~RefStringW(){}
    RefStringW(const RefStringW& rhs){if(this->m_str!=rhs.m_str) this->m_str = rhs.m_str;}
    RefStringW& operator = (const RefStringW& rhs)
    {
        if(this != &rhs){this->m_str.clear();this->m_str = rhs.m_str;}
        return *this;
    }
    operator wchar_t* () const {return (wchar_t*) m_str.c_str();}
private:
    std::wstring m_str;
};


class UnCopyable
{
protected:
    UnCopyable(){};
    ~UnCopyable(){};
private:
    UnCopyable(const UnCopyable& rhs);
    UnCopyable& operator= (const UnCopyable& rhs);
};


// handle wrapper
//
class SmrtHandle: public UnCopyable
{
public:
    SmrtHandle(HANDLE handle) : m_handle(handle) {}
    ~SmrtHandle(){ CloseHandle(m_handle); m_handle = INVALID_HANDLE_VALUE; }
protected:
private:
	HANDLE m_handle;
};

// simple smart pointer
//
template <typename T>
class SmrtPtr: public UnCopyable
{
public:
    SmrtPtr(T& Ptr): m_ptr(Ptr) {};
    ~SmrtPtr()
    {
        if (NULL != m_ptr)
        {
            free(m_ptr); m_ptr = NULL;
        }
    };
private:
    T& m_ptr;
};



#define FreeAndNil(p)     do { if (NULL != p) { free(p); p = NULL;} } while(p != NULL);



HANDLE advOpenProcess(DWORD dwPid,DWORD dwRights,BOOL RaisePrivilege);
BOOL WriteToFileW(LPCWCH file_path, LPCWCH format,...);
BOOL WriteToFileW(HANDLE hFile,LPCWCH format,...);
BOOL IsFileExistA(const CHAR* lpszFilename);
BOOL IsFileExistW(const WCHAR* lpszFilename);
WCHAR* MbsToWcs(IN const char* mbs);
CHAR* WcsToMbs(IN const WCHAR* wcs);
WORD OsVersion(void);
BOOL SetPrivilege(LPCTSTR Privilege,BOOL bEnablePrivilege);
RefStringW FormatErrorStringW(DWORD errcode);
RefStringW FormatStringW(LPCWCH format,...);
int DumpMemory(FILE* stream,DWORD Length,BYTE* Buf);

#ifdef UNICODE
    #define IsFileExist		IsFileExistW
#else
    #define IsFileExist     IsFileExistA
#endif//UNICODE


