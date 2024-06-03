#include "framework/IPlatform.h"
#include "framework/Util.h"

#include "render/IRenderSystem.h"
#include "render/impl/CNVRHIRenderSystem.h"

extern "C" KR_EXPORT ISystem* CreateInterface()
{
	KR_QUIT("No render system implementation!");
	return nullptr;
}

extern "C" KR_EXPORT u32 GetSystemVersion()
{
	return IRenderSystem::VERSION;
}
