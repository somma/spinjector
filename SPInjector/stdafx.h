/*-----------------------------------------------------------------------------
 * stdafx.h
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
                        
#pragma once

#ifndef WINVER				
#define WINVER 0x0500		// windows 2000
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0500
#endif						

#ifndef _WIN32_IE			// IE 6.0 이상에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0600	// 다른 버전의 IE에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#define STRSAFE_NO_DEPRECATE
#define STRSAFE_LIB
#include <strsafe.h>
#pragma comment(lib, "strsafe.lib")

