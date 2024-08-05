// RHI device interface

#pragma once

#include "base/compiler.h"
#include "base/types.h"

class IRhiCommandQueue;
enum RhiCommandQueueType_t;
enum RhiCommandQueueFlags_t;
class IRhiCommandList;
enum RhiCommandListFlags_t;
class IRhiFence;
enum RhiMemoryLocation_t;
enum RhiMemoryUsage_t;
class IRhiBuffer;
enum RhiBufferFlags_t;
class IRhiImage;
enum RhiImageType_t;
enum RhiImageFormat_t;
enum RhiImageFlags_t;

class IRhiDevice
{
  public:
	virtual ~IRhiDevice() DEFAULT;

	// Create a command queue
	virtual IRhiCommandQueue* CreateCommandQueue(RhiCommandQueueType_t type, RhiCommandQueueFlags_t flags) = 0;

	// Create a command list
	virtual IRhiCommandList* CreateCommandList(IRhiCommandQueue* queue, RhiCommandListFlags_t flags) = 0;

	// Create a fence
	virtual IRhiFence* CreateFence(u64 initialValue = 0) = 0;

	// Create a buffer
	virtual IRhiBuffer* CreateBuffer(
		ssize size, RhiMemoryLocation_t location, RhiMemoryUsage_t usage, RhiBufferFlags_t flags) = 0;

	// Create an image
	virtual IRhiImage* CreateImage(
		u32 width, u32 height, u32 depth, RhiMemoryLocation_t location, RhiImageType_t type,
		RhiImageFormat_t format, RhiImageFlags_t flags) = 0;
};
