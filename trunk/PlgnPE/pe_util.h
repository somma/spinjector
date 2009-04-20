/*-----------------------------------------------------------------------------
 * pe_util.h
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

#ifndef _PE_UTIL_H_INCLUDED_
#define _PE_UTIL_H_INCLUDED_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct
{
	WORD	flag;
	LPCTSTR	name;
} WORD_FLAG_DESCRIPTIONS;

typedef struct
{
	DWORD	flag;
	LPCTSTR	name;
} DWORD_FLAG_DESCRIPTIONS;

#define NUMBER_IMAGE_HEADER_FLAGS \
    (sizeof(ImageFileHeaderCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

#define NUMBER_DLL_CHARACTERISTICS \
	(sizeof(DllCharacteristics) / sizeof(WORD_FLAG_DESCRIPTIONS))

#define NUMBER_IMAGE_DIRECTORY_ENTRYS \
	(sizeof(ImageDirectoryNames)/sizeof(char *))

#define NUMBER_SECTION_CHARACTERISTICS \
	(sizeof(SectionCharacteristics) / sizeof(DWORD_FLAG_DESCRIPTIONS))

// from PEDUMP - Matt Pietrek 1997
//
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))


//
//
// UTILITY FUNCTIONS
//


LPCTSTR _machinecode_to_string(WORD MachineCode);

//=============================================================================
// class for manage image base address of PE image on file / on memory)
//=============================================================================
class IMAGE_BASE
{
public:
	IMAGE_BASE();
	~IMAGE_BASE();

	int Init(LPCTSTR ImagePath);
	int Init();

	// return image base address
	//
	LPBYTE GetImageBase() 
	{ 
		return m_pe_imagebase; 
	};

	// return TRUE if m_pe_imagebase is file pointer
	//
	BOOLEAN		IsImage()	
	{
		if (INVALID_HANDLE_VALUE != m_hFile)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
private:
	LPBYTE	m_pe_imagebase;	// IMAGE_DOS_HEADER pointer

	// additional variables for disk image 
	//
	HANDLE	m_hFile;
	HANDLE	m_hImageMap;
	LPBYTE	m_ImageView;
};


#endif//_PE_UTIL_H_INCLUDED_