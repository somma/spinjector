/*-----------------------------------------------------------------------------
 * pe_util.cpp
 *-----------------------------------------------------------------------------
 * 
 *-----------------------------------------------------------------------------
 * All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
 *-----------------------------------------------------------------------------
 * Revision History:
 * Date					Who					What
 * ----------------		----------------	----------------
 * 04.12.2007			somma		birth
**---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "pe_util.h"
#include "AKUtil.h"

// Bitfield values and names for the IMAGE_FILE_HEADER flags
//
WORD_FLAG_DESCRIPTIONS ImageFileHeaderCharacteristics[] =
{
	{ IMAGE_FILE_RELOCS_STRIPPED, _T("RELOCS_STRIPPED") },
	{ IMAGE_FILE_EXECUTABLE_IMAGE, _T("EXECUTABLE_IMAGE") },
	{ IMAGE_FILE_LINE_NUMS_STRIPPED, _T("LINE_NUMS_STRIPPED") },
	{ IMAGE_FILE_LOCAL_SYMS_STRIPPED, _T("LOCAL_SYMS_STRIPPED") },
	{ IMAGE_FILE_AGGRESIVE_WS_TRIM, _T("AGGRESSIVE_WS_TRIM") },
	{ IMAGE_FILE_LARGE_ADDRESS_AWARE, _T("LARGE_ADDRESS_AWARE") },
	//	{ IMAGE_FILE_16BIT_MACHINE, _T("16BIT_MACHINE") },
	{ IMAGE_FILE_BYTES_REVERSED_LO, _T("BYTES_REVERSED_LO") },
	{ IMAGE_FILE_32BIT_MACHINE, _T("32BIT_MACHINE") },
	{ IMAGE_FILE_DEBUG_STRIPPED, _T("DEBUG_STRIPPED") },
	{ IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, _T("REMOVABLE_RUN_FROM_SWAP") },
	{ IMAGE_FILE_NET_RUN_FROM_SWAP, _T("NET_RUN_FROM_SWAP") },
	{ IMAGE_FILE_SYSTEM, _T("SYSTEM") },
	{ IMAGE_FILE_DLL, _T("DLL") },
	{ IMAGE_FILE_UP_SYSTEM_ONLY, _T("UP_SYSTEM_ONLY") },
	{ IMAGE_FILE_BYTES_REVERSED_HI, _T("BYTES_REVERSED_HI") }
};

// Bitfield values and names for the DllCharacteritics flags
//
WORD_FLAG_DESCRIPTIONS DllCharacteristics[] =
{
	//  IMAGE_LIBRARY_PROCESS_INIT				0x0001     // Reserved.
	//  IMAGE_LIBRARY_PROCESS_TERM				0x0002     // Reserved.
	//  IMAGE_LIBRARY_THREAD_INIT				0x0004     // Reserved.
	//  IMAGE_LIBRARY_THREAD_TERM				0x0008     // Reserved.
	{IMAGE_DLLCHARACTERISTICS_NO_ISOLATION, _T("NO_ISOLATION")},
	{IMAGE_DLLCHARACTERISTICS_NO_SEH, _T("NO_SEH")},
	{IMAGE_DLLCHARACTERISTICS_NO_BIND, _T("NO_BIND")},
	//											0x1000     // Reserved.
	{IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, _T("WDM_DRIVER")},
	//											0x4000     // Reserved.
	{IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, _T("SERVER_AWARE")}
};


// Names of the data directory elements that are defined
//
TCHAR* ImageDirectoryNames[] = 
{
	_T("EXPORT"),
	_T("IMPORT"),
	_T("RESOURCE"),
	_T("EXCEPTION"),
	_T("SECURITY"),
	_T("BASERELOC"),
	_T("DEBUG"),
	//	COPYRIGHT       7   // (X86 usage)
	_T("ARCHITECTURE"),
	_T("GLOBALPTR"),
	_T("TLS"),
	_T("LOAD_CONFIG"),
	_T("BOUND_IMPORT"),
	_T("IAT"),
	_T("DELAY_IMPORT"),
	_T("COM_DESCRIPTOR")
};

// Bitfield values and names for the IMAGE_SECTION_HEADER flags
//
DWORD_FLAG_DESCRIPTIONS SectionCharacteristics[] = 
{
	//      IMAGE_SCN_TYPE_REG                   0x00000000  // Reserved.
	//      IMAGE_SCN_TYPE_DSECT                 0x00000001  // Reserved.
	//      IMAGE_SCN_TYPE_NOLOAD                0x00000002  // Reserved.
	//      IMAGE_SCN_TYPE_GROUP                 0x00000004  // Reserved.
	{IMAGE_SCN_TYPE_NO_PAD, _T("TYPE_NO_PAD")},
	//      IMAGE_SCN_TYPE_COPY                  0x00000010  // Reserved.
	{IMAGE_SCN_CNT_CODE, _T("CNT_CODE")},
	{IMAGE_SCN_CNT_INITIALIZED_DATA, _T("INITIALIZED_DATA")},
	{IMAGE_SCN_CNT_UNINITIALIZED_DATA, _T("UNINITIALIZED_DATA")},
	{IMAGE_SCN_LNK_OTHER, _T("LNK_OTHER")},
	{IMAGE_SCN_LNK_INFO, _T("LNK_INFO")},
	//      IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
	{IMAGE_SCN_LNK_REMOVE, _T("LNK_REMOVE")},
	{IMAGE_SCN_LNK_COMDAT, _T("LNK_COMDAT")},
	//                                           0x00002000  // Reserved.
	//      IMAGE_SCN_MEM_PROTECTED - Obsolete   0x00004000
	{IMAGE_SCN_NO_DEFER_SPEC_EXC, _T("NO_DEFER_SPEC_EXC")},
	{IMAGE_SCN_GPREL, _T("GPREL")},
	{IMAGE_SCN_MEM_FARDATA, _T("MEM_FARDATA")},
	//      IMAGE_SCN_MEM_SYSHEAP  - Obsolete    0x00010000
	{IMAGE_SCN_MEM_PURGEABLE, _T("MEM_PURGEABLE")},
	{IMAGE_SCN_MEM_16BIT, _T("MEM_16BIT")},
	{IMAGE_SCN_MEM_LOCKED, _T("MEM_LOCKED")},
	{IMAGE_SCN_MEM_PRELOAD, _T("MEM_PRELOAD")},
	{IMAGE_SCN_ALIGN_1BYTES, _T("ALIGN_1BYTES")},
	{IMAGE_SCN_ALIGN_2BYTES, _T("ALIGN_2BYTES")},
	{IMAGE_SCN_ALIGN_4BYTES, _T("ALIGN_4BYTES")},
	{IMAGE_SCN_ALIGN_8BYTES, _T("ALIGN_8BYTES")},
	{IMAGE_SCN_ALIGN_16BYTES, _T("ALIGN_16BYTES")},
	{IMAGE_SCN_ALIGN_32BYTES, _T("ALIGN_32BYTES")},
	{IMAGE_SCN_ALIGN_64BYTES, _T("ALIGN_64YTES")},
	{IMAGE_SCN_ALIGN_128BYTES, _T("ALIGN_128BYTES")},
	{IMAGE_SCN_ALIGN_256BYTES, _T("ALIGN_256BYTES")},
	{IMAGE_SCN_ALIGN_512BYTES, _T("ALIGN_512BYTES")},
	{IMAGE_SCN_ALIGN_1024BYTES, _T("ALIGN_1024BYTES")},
	{IMAGE_SCN_ALIGN_2048BYTES, _T("ALIGN_2048BYTES")},
	{IMAGE_SCN_ALIGN_4096BYTES, _T("ALIGN_4096BYTES")},
	{IMAGE_SCN_ALIGN_8192BYTES, _T("ALIGN_8192BYTES")},
	// Unused                                    0x00F00000
	{IMAGE_SCN_ALIGN_MASK, _T("ALIGN_MASK")},
	{IMAGE_SCN_LNK_NRELOC_OVFL, _T("LNK_NRELOC_OVFL")},
	{IMAGE_SCN_MEM_DISCARDABLE, _T("MEM_DISCARDABLE")},
	{IMAGE_SCN_MEM_NOT_CACHED, _T("MEM_NOT_CACHED")},
	{IMAGE_SCN_MEM_NOT_PAGED, _T("MEM_NOT_PAGED")},
	{IMAGE_SCN_MEM_SHARED, _T("MEM_SHARED")},
	{IMAGE_SCN_MEM_EXECUTE, _T("MEM_EXECUTE")},
	{IMAGE_SCN_MEM_READ, _T("MEM_READ")},
	{IMAGE_SCN_MEM_WRITE, _T("MEM_WRITE")}
};

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
LPCTSTR _machinecode_to_string(WORD MachineCode)
{
	switch (MachineCode)
	{
	case IMAGE_FILE_MACHINE_UNKNOWN:     
		return _T("UNKNOWN");
	case IMAGE_FILE_MACHINE_I386:
		return _T("I386");
	case IMAGE_FILE_MACHINE_R3000:
		return _T("R3000");
	case IMAGE_FILE_MACHINE_R4000:
		return _T("R4000");
	case IMAGE_FILE_MACHINE_R10000:
		return _T("R10000");
	case IMAGE_FILE_MACHINE_WCEMIPSV2:
		return _T("WCEMIPSV2");
	case IMAGE_FILE_MACHINE_ALPHA:
		return _T("ALPHA");
	case IMAGE_FILE_MACHINE_SH3:
		return _T("SH3");
	case IMAGE_FILE_MACHINE_SH3DSP:
		return _T("SH3DSP");
	case IMAGE_FILE_MACHINE_SH3E:
		return _T("SH3E");
	case IMAGE_FILE_MACHINE_SH4:
		return _T("SH4");
	case IMAGE_FILE_MACHINE_SH5:
		return _T("SH5");
	case IMAGE_FILE_MACHINE_ARM:
		return _T("ARM");
	case IMAGE_FILE_MACHINE_THUMB:
		return _T("THUMB");
	case IMAGE_FILE_MACHINE_AM33:
		return _T("AM33");
	case IMAGE_FILE_MACHINE_POWERPC:
		return _T("POWERPC");
	case IMAGE_FILE_MACHINE_POWERPCFP:
		return _T("POWERPCFP");
	case IMAGE_FILE_MACHINE_IA64:
		return _T("IA64");
	case IMAGE_FILE_MACHINE_MIPS16:
		return _T("MIPS16");
	case IMAGE_FILE_MACHINE_ALPHA64:
		return _T("ALPHA64");
	case IMAGE_FILE_MACHINE_MIPSFPU:
		return _T("MIPSFPU");
	case IMAGE_FILE_MACHINE_MIPSFPU16:
		return _T("MIPSFPU16");
	/*
	IMAGE_FILE_MACHINE_ALPHA64 µ¿ÀÏ
	case IMAGE_FILE_MACHINE_AXP64:
		return _T("AXP64");*/
	case IMAGE_FILE_MACHINE_TRICORE:
		return _T("TRICORE");
	case IMAGE_FILE_MACHINE_CEF:
		return _T("CEF");
	case IMAGE_FILE_MACHINE_EBC:
		return _T("EBC");
	case IMAGE_FILE_MACHINE_AMD64:
		return _T("AMD64");
	case IMAGE_FILE_MACHINE_M32R:
		return _T("M32R");
	case IMAGE_FILE_MACHINE_CEE:
		return _T("CEE");	
	default:
		return _T("UNKNOWN");
	}//switch	
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
IMAGE_BASE::IMAGE_BASE()
:	m_pe_imagebase(NULL),
	m_hFile(INVALID_HANDLE_VALUE),
	m_hImageMap(NULL),
	m_ImageView(NULL)
{	
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
IMAGE_BASE::~IMAGE_BASE()
{
	m_pe_imagebase = NULL;

	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		if ((NULL != m_hImageMap) && (NULL != m_ImageView))
		{
			UnmapViewOfFile(m_ImageView);
			m_ImageView = NULL;

			CloseHandle(m_hImageMap);
			m_hImageMap = NULL;
		}
		else
		{
			_ASSERTE(!"oops!");
		}

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}	
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int IMAGE_BASE::Init(LPCTSTR ImagePath)
{
	_ASSERTE(NULL != ImagePath);
	_ASSERTE(TRUE != IsBadStringPtr(ImagePath, sizeof(MAX_PATH * sizeof(TCHAR))));
	if (
			(NULL == ImagePath) ||
			(TRUE == IsBadReadPtr(ImagePath, sizeof(MAX_PATH * sizeof(TCHAR))))
		)
	{
		return -1;
	}
	
	if (TRUE != IsFileExist(ImagePath))
	{
		return -1;
	}

	m_hFile = CreateFile(ImagePath, 
						 GENERIC_READ, 
						 FILE_SHARE_READ, 
						 NULL, 
						 OPEN_EXISTING, 
						 0, 
						 NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		_ASSERTE(!"CreateFile");
		return -1;
	}

	m_hImageMap = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (NULL == m_hImageMap)
	{
		_ASSERTE(!"CreateFileMapping");

		CloseHandle(m_hFile); m_hFile = INVALID_HANDLE_VALUE;
		return -1;
	}

	m_ImageView = (LPBYTE) MapViewOfFile(m_hImageMap, FILE_MAP_READ, 0, 0, 0);
	if(m_ImageView == NULL)
	{
		_ASSERTE(!"MapViewOfFile");

		CloseHandle(m_hImageMap); m_hImageMap = NULL;
		CloseHandle(m_hFile); m_hFile = INVALID_HANDLE_VALUE;
		return -1;
	}

	PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)m_ImageView;
	if(pIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		_ASSERTE(!"IDH_SIG");		

		UnmapViewOfFile(m_ImageView);	m_ImageView = NULL;
		CloseHandle(m_hImageMap);		m_hImageMap = NULL;
		CloseHandle(m_hFile);			m_hFile = INVALID_HANDLE_VALUE;
		return -1;
	}

	m_pe_imagebase = reinterpret_cast<PBYTE>(pIDH);
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int IMAGE_BASE::Init()
{
	HMODULE hModule = GetModuleHandle(NULL);
	if (NULL == hModule)
	{
		_ASSERTE(!"GetModuleHandle");
		return -1;
	}

	PIMAGE_DOS_HEADER pIDH = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
	if(pIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		_ASSERTE(!"IDH_SIG");		

		return -1;
	}

	m_pe_imagebase = reinterpret_cast<LPBYTE>(hModule);
	return 0;	
}
