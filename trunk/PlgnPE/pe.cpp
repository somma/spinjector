/*-----------------------------------------------------------------------------
 * pe.cpp
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
#include "pe.h"

#include "AKUtil.h"

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PEAnalyzer::PEAnalyzer()
:	m_pe(NULL)
{
};

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
/*virtual*/ PEAnalyzer::~PEAnalyzer()
{
	if (NULL != m_pe)
	{
		free(m_pe); m_pe = NULL;
	}
};

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int PEAnalyzer::Init(LPCTSTR ImagePath)
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

	if (0 != m_Base.Init(ImagePath))
	{
		return -1;
	}

	return ( AllocAndCopyHeaders() );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int PEAnalyzer::Init()
{
	if ( 0 != m_Base.Init() )
	{
		return -1;
	}

	return ( AllocAndCopyHeaders() );
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int PEAnalyzer::GetPEHeaders(OUT PPE_HEADER Headers)
{
	_ASSERTE(NULL != Headers);
	_ASSERTE(TRUE != IsBadWritePtr(Headers, GetPEHeaderSize()));
	if (
		(NULL == Headers) ||
		(TRUE == IsBadWritePtr(Headers, GetPEHeaderSize()))
		)
	{
		return -1;
	}

	if (0 == GetPEHeaderSize() || NULL == m_pe)
	{
		return -1;
	}

	RtlZeroMemory(Headers, GetPEHeaderSize());
	RtlCopyMemory(Headers, m_pe, GetPEHeaderSize());
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int PEAnalyzer::ReadMemory(IN DWORD_PTR Addr,IN DWORD Len,OUT PBYTE Buf)
{
	_ASSERTE(0 != Len);
	_ASSERTE(NULL != Buf);
	_ASSERTE(TRUE != IsBadWritePtr(Buf, Len));
	if ( (0 == Len) || (NULL == Buf) || (TRUE == IsBadWritePtr(Buf, Len)))
	{
		return -1;
	}

	_ASSERTE(TRUE != IsBadReadPtr((void*)Addr, Len));
	if (TRUE == IsBadReadPtr((void*)Addr, Len))
	{
		return -1;
	}

	RtlZeroMemory(Buf, Len);
	RtlCopyMemory(Buf, (void*) Addr, Len);
	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int PEAnalyzer::WriteMemory(IN DWORD_PTR Addr,IN DWORD Len,IN PBYTE Buf)
{
	_ASSERTE(0 != Len);
	_ASSERTE(NULL != Buf);
	_ASSERTE(TRUE != IsBadReadPtr(Buf, Len));
	if ( (0 == Len) || (NULL == Buf) || (TRUE == IsBadReadPtr(Buf, Len)))
	{
		return -1;
	}

	//_ASSERTE(TRUE != IsBadWritePtr((void*)Addr, Len));
	//if (TRUE == IsBadWritePtr((void*)Addr, Len))
	//{
	//	return -1;
	//}

	DWORD old = 0;
	if (TRUE != VirtualProtect((void*)Addr, Len, PAGE_READWRITE, &old))
	{
		_ASSERTE(!"VirtualProtext");
		return -1;
	}	
	RtlCopyMemory((void*)Addr, Buf, Len);
	VirtualProtect((void*)Addr, Len, old, &old);
	
	return 0;
}

///**	---------------------------------------------------------------------------
//	\brief	
//
//	\param	
//	\return			
//	\code
//	\endcode		
//-----------------------------------------------------------------------------*/
//int PEAnalyzer::GetDosHeader(OUT PIMAGE_DOS_HEADER idh)
//{
//	_ASSERTE(NULL != idh);
//	_ASSERTE(TRUE != IsBadWritePtr(idh, sizeof(IMAGE_DOS_HEADER)));
//	if ((NULL == idh) || (TRUE == IsBadWritePtr(idh, sizeof(IMAGE_DOS_HEADER))))
//	{
//		return -1;
//	}
//
//	_ASSERTE(NULL != m_pe);
//	if (NULL == m_pe)	{ return -1; }
//	
//	RtlCopyMemory(idh, m_pe->idh, sizeof(IMAGE_DOS_HEADER));
//	return 0;	
//}
//
///**	---------------------------------------------------------------------------
//	\brief	
//
//	\param	
//	\return			
//	\code
//	\endcode		
//-----------------------------------------------------------------------------*/
//DWORD_PTR PEAnalyzer::GetDosHeaderPtr()
//{
//	_ASSERTE(NULL != m_pe);
//	if (NULL == m_pe)	{ return 0; }
//
//	return (DWORD_PTR) m_pe->idh;
//}
//
///**	---------------------------------------------------------------------------
//	\brief	
//
//	\param	
//	\return			
//	\code
//	\endcode		
//-----------------------------------------------------------------------------*/
//int PEAnalyzer::GetNtHeaders(OUT PIMAGE_NT_HEADERS inh)
//{
//	_ASSERTE(NULL != inh);
//	_ASSERTE(TRUE != IsBadWritePtr(inh, sizeof(IMAGE_NT_HEADERS)));
//	if ((NULL == inh) || (TRUE == IsBadWritePtr(inh, sizeof(IMAGE_NT_HEADERS))))
//	{
//		return -1;
//	}
//	
//	_ASSERTE(NULL != m_pe);
//	if (NULL == m_pe)	{ return -1; }
//
//	RtlCopyMemory(inh, m_pe->inh, sizeof(IMAGE_NT_HEADERS));
//	return 0;
//}
//
///**	---------------------------------------------------------------------------
//	\brief	
//
//	\param	
//	\return			
//	\code
//	\endcode		
//-----------------------------------------------------------------------------*/
//DWORD_PTR PEAnalyzer::GetNtHeadersPtr()
//{
//	_ASSERTE(NULL != m_pe);
//	if (NULL == m_pe)	{ return 0; }
//
//	return (DWORD_PTR) m_pe->inh;
//}


/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
int PEAnalyzer::AllocAndCopyHeaders()
{
	_ASSERTE(NULL == m_pe);
	if (NULL != m_pe)
	{
		return -1;
	}

	PIMAGE_DOS_HEADER idh = (PIMAGE_DOS_HEADER) m_Base.GetImageBase();
	_ASSERTE(NULL != idh);
	_ASSERTE(IMAGE_DOS_SIGNATURE == idh->e_magic);
	if ( (NULL == idh) || (IMAGE_DOS_SIGNATURE != idh->e_magic) )
	{
		_ASSERTE(!"invalid IMAGE_DOS_SIG");
		return -1;
	}

	PIMAGE_NT_HEADERS inh = (PIMAGE_NT_HEADERS) ((DWORD_PTR)idh + idh->e_lfanew);
	if (IMAGE_NT_SIGNATURE != inh->Signature)
	{
		_ASSERTE(!"IMAGE_NT_SIGNATURE");
		return -1;
	}

	DWORD SectionSize = IMAGE_SIZEOF_SECTION_HEADER * inh->FileHeader.NumberOfSections;
	_ASSERTE(0 != SectionSize);

	if (TRUE == IsBadReadPtr(IMAGE_FIRST_SECTION(inh), SectionSize))
	{
		_ASSERTE(!"invalid section size");
		return -1;
	}

	
	DWORD HeaderSize = HEADER_SIZE(SectionSize);
	PPE_HEADER pe = (PPE_HEADER) malloc(HeaderSize);
	_ASSERTE(NULL != pe);
	if (NULL == pe)
	{
		return -1;
	}
	
	RtlZeroMemory(pe, HeaderSize);	

	pe->idh = idh;
	pe->inh = inh;
	pe->ish = IMAGE_FIRST_SECTION(inh);
	RtlCopyMemory(&pe->idh_copy, idh, sizeof(IMAGE_DOS_HEADER));
	RtlCopyMemory(&pe->inh_copy, inh, sizeof(IMAGE_NT_HEADERS));
	RtlCopyMemory(pe->ish_copy, IMAGE_FIRST_SECTION(inh), SectionSize);

	m_HeaderSize = HeaderSize;
	m_pe = pe; pe = NULL;

	return 0;
}

/**	---------------------------------------------------------------------------
	\brief	

	\param	
	\return			
	\code
	\endcode		
-----------------------------------------------------------------------------*/
PIMAGE_SECTION_HEADER 
PEAnalyzer::GetEnclosingSectionHeader(DWORD_PTR			SectionRVA,
									   PIMAGE_NT_HEADERS	pINH)
{
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pINH);	

	for (unsigned i = 0; 
		 i < pINH->FileHeader.NumberOfSections; 
		i++, section++ )
	{
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		//			
		DWORD dwSize = section->Misc.VirtualSize;
		if ( 0 == dwSize ) dwSize = section->SizeOfRawData;

		// Is the RVA within this section?
		//
		if ( 
			(SectionRVA >= section->VirtualAddress) && 
			(SectionRVA < (section->VirtualAddress + dwSize))
			)
		{
			return section;
		}
	}

	return NULL;
}






















