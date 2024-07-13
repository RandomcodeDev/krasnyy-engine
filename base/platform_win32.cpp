#include "platform_win32.h"
#include "base.h"
#include "base/compiler.h"
#include "base/platform.h"
#include "base/types.h"

extern "C" uptr(__cdecl* STUB_NAME(DbgPrint))(...);
extern "C" uptr(__stdcall* STUB_NAME(NtTerminateProcess))(...);

SYSTEM_BASIC_INFORMATION g_systemInfo;

NORETURN void Base_QuitImpl(s32 code, cstr msg)
{
	if (code == 1)
	{
		// LastErrorValue/LastStatusValue aren't reliable enough
		code = STATUS_FATAL_APP_EXIT;
	}

	if (STUB_NAME(DbgPrint))
	{
		DbgPrint("%s\n", msg);
	}

	// TODO: use NtRaiseHardError

	BREAKPOINT();
	if (STUB_NAME(NtTerminateProcess))
	{
		NtTerminateProcess(NtCurrentProcess(), static_cast<NTSTATUS>(code));
	}
	else
	{
		BREAKPOINT();
	}

	ASSUME(0);
}

bool Base_GetSystemMemory(usize size)
{
	// Linked list nodes, can contain any size of allocation, but there's a limit to the number of OS allocations (this should be
	// changed in the future, this is based on a vacuum currently)
	static LinkedNode_t<SystemAllocation_t> memoryNodes[64];

	size = ALIGN(size, g_systemInfo.PageSize);

	ASSERT_MSG(
		g_memInfo.allocations.Size() < ARRAY_SIZE(memoryNodes),
		"OS allocation nodes exhausted, increase the size of the memory nodes array");

	g_memInfo.size += size;
	LinkedNode_t<SystemAllocation_t>* node = &memoryNodes[g_memInfo.allocations.Size()];
	node->data.size = size;
	NTSTATUS status =
		NtAllocateVirtualMemory(NtCurrentProcess(), &node->data.memory, 0, (PSIZE_T)&node->data.size, MEM_COMMIT, PAGE_READWRITE);
	if (!NT_SUCCESS(status))
	{
		NtCurrentTeb()->LastStatusValue = status;
		return false;
	}

	g_memInfo.allocations.Append(node);

	return true;
}

void Base_ReleaseSystemMemory(LinkedNode_t<SystemAllocation_t>* allocation)
{
	usize size = 0;
	NtFreeVirtualMemory(NtCurrentProcess(), &allocation->data.memory, (PSIZE_T)&size, MEM_RELEASE);
	g_memInfo.allocations.Remove(allocation);
}

void Base_ReleaseAllMemory()
{
	for (LinkedNode_t<SystemAllocation_t>* cur = g_memInfo.allocations.GetHead(); cur; cur = cur->GetNext())
	{
		Base_ReleaseSystemMemory(cur);
	}
}
