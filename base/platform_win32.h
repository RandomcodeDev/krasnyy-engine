// Implementation details of Windows DLL shenanigans

#pragma once

#include "base/loader.h"
#include "base/types.h"

// Get the address of NTDLL, find LdrGetProcedureAddress manually, find LdrLoadDll properly, load any other system DLLs/functions
// using those
extern bool Base_InitLoader();

class CWin32Library : public ILibrary
{
  public:
	CWin32Library(void* base);
	~CWin32Library();

	void* GetSymbol(cstr name);

  private:
	void* m_base;
};

extern SYSTEM_BASIC_INFORMATION g_systemInfo;
