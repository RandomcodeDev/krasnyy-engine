.386
.MODEL FLAT

TITLE x86 Windows function stubs

; I get that code had to be small back then, but fuck stdcall
MAKE_STUB MACRO name, paramSize
	@CatStr(extrnlFwd_, name) TEXTEQU @CatStr(name, _Forwarder)
	PUBLIC @CatStr(_STUB_, name)
	.DATA
	PUBLIC @CatStr(__imp__, name, @, paramSize)
	@CatStr(__imp__, name, @, paramSize) LABEL DWORD
	@CatStr(_STUB_, name) DD 0
	.CODE
	@CatStr(extrnlFwd_, name) PROC
		jmp [DWORD PTR @CatStr(_STUB_, name)]
	@CatStr(extrnlFwd_, name) ENDP
ENDM

MAKE_STUB DbgPrint, 4
MAKE_STUB LdrAddRefDll, 8
MAKE_STUB LdrGetProcedureAddress, 16
MAKE_STUB LdrLoadDll, 16
MAKE_STUB LdrUnloadDll, 4
MAKE_STUB NtAllocateVirtualMemory, 24
MAKE_STUB NtFreeVirtualMemory, 16
MAKE_STUB NtQuerySystemInformation, 16
MAKE_STUB NtRaiseHardError, 0
MAKE_STUB NtTerminateProcess, 8
MAKE_STUB RtlAnsiStringToUnicodeString, 12
MAKE_STUB RtlFreeHeap, 12
MAKE_STUB RtlFreeUnicodeString, 4

END