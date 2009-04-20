/*-----------------------------------------------------------------------------
 * pe.h
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

#ifndef _PE_H_INCLUDED_
#define _PE_H_INCLUDED_

#include "pe_util.h"

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


//=============================================================================
// base class for PE image 
//=============================================================================
class PEAnalyzer
{
public:
	PEAnalyzer();
	virtual ~PEAnalyzer();

	int Init(LPCTSTR ImagePath);	// for pe file image
	int Init();						// for pe memory

	DWORD GetPEHeaderSize() { return m_HeaderSize; }
	int GetPEHeaders(OUT PPE_HEADER Headers);

	int ReadMemory(IN DWORD_PTR Addr, IN DWORD Len, OUT PBYTE Buf);
	int WriteMemory(IN DWORD_PTR Addr, IN DWORD Len, IN PBYTE Buf);

	virtual int enum_import_modules() = 0;
	virtual int enum_export_apis() = 0;
private:
	IMAGE_BASE	m_Base;
	
	DWORD		m_HeaderSize;
	PPE_HEADER	m_pe;

	int AllocAndCopyHeaders();	
	PIMAGE_SECTION_HEADER 
		GetEnclosingSectionHeader(DWORD_PTR	SectionRVA, PIMAGE_NT_HEADERS pINH);
};

//=============================================================================
// class for PE image on file
//=============================================================================
class PEImage: public PEAnalyzer
{
public:
    PEImage(){};
	/*virtual*/ ~PEImage(){};

	virtual int enum_import_modules(){return 0;};
	virtual int enum_export_apis(){return 0;};
};

//=============================================================================
// class for PE image on memory
//=============================================================================
class PEMemory: public PEAnalyzer
{
public:
    PEMemory(){};
	/*virtual*/ ~PEMemory(){};

	virtual int enum_import_modules(){return 0;};
	virtual int enum_export_apis(){return 0;};
};



#endif//_PE_H_INCLUDED_