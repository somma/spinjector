/*-----------------------------------------------------------------------------
 * pe_image_on_memory.h
 *-----------------------------------------------------------------------------
 * pe analyze library for loaded image
 *-----------------------------------------------------------------------------
 * All rights reserved by Noh Yong Hwan (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/

#ifndef _pe_on_memory_h_
#define _pe_on_memory_h_

#include "pe.h"

//
// class for PE image on memory
//
class PEMemory: public PEAnalyzer
{
public:
    PEMemory(){};
	virtual ~PEMemory(){};

	virtual BOOL enum_imported_modules(){return TRUE;};
    virtual BOOL enum_imported_api_from_module() {return TRUE;};
	virtual BOOL enum_export_apis(){return TRUE;};
protected:
	virtual BOOLEAN OnInit(                
						LPCTSTR ImagePath, 
						DWORD_PTR ImageBase, 
						DWORD ImageSize
						) 
    { 
        UNREFERENCED_PARAMETER(ImagePath);
        UNREFERENCED_PARAMETER(ImageBase);
        UNREFERENCED_PARAMETER(ImageSize);
        return TRUE; 
    }
};

#endif//_pe_in_memory_h_