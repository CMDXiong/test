
//#ifndef _DEBUGHEAP_H_HASBEENINCLUDED

#pragma once
#include <CRTDBG.H>

/********************************************************
 Author:  Christian Mueller-Planitz
		  cmp@cs.utah.edu

 Simplified by Olly Ansell
 ********************************************************/


//

// now add the following to the top of each CPP file in your project that 
// allocates mem with a call to new()
// Do NOT add this to *.H files !!!
//
/*

  #include "MemLeak.h"
  #ifdef _DEBUG 
	#define new	   DEBUG_NEW  
	#define malloc DEBUG_MALLOC  
    static char THIS_FILE[] = __FILE__;  
  #endif

*/
//-----------------------------------------------------------------------------------------------------



// Prototypes of functions defined in 'DebugHeap.cpp'




#ifdef _DEBUG  
  #define DEBUG_NEW				new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #define DEBUG_MALLOC(size)	_malloc_dbg(size, _NORMAL_BLOCK, THIS_FILE, __LINE__)   
//  #define strdup(str)			_strdup_dbg(str)
  #define wcsdup(str)			_wcsdup_dbg(str)
  #undef THIS_FILE
#else
  #define DEBUG_NEW		new
  #define DEBUG_MALLOC  malloc   
#endif




//
// if you want to allocate memory in *.H files, use the following macros:
// The reason for this is that 'THIS_FILE' is not defined in *.h files.
//

#ifdef _DEBUG  
  #define __new			 new(_NORMAL_BLOCK,  __FILE__, __LINE__)   
  #define __malloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__)   
  #define __free(o)		 _free_dbg(o, _NORMAL_BLOCK)
#else
  #define __new			 new
  #define __malloc(size) malloc(size)
  #define __free(o)		 free(o)
#endif

//#define _DEBUGHEAP_H_HASBEENINCLUDED
//#endif