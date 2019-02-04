/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#pragma once

#include <Gaff_Platform.h>
#include "Gleam_IncludeVulkan.h"

namespace Gaff
{
	class DynamicModule;
}

namespace Gleam
{
	bool WrangleVulkanFunctions(Gaff::DynamicModule& mod);
	void WrangleVulkanFunctions(VkInstance instance);
}

#define VULKAN_FUNC_DECL(name, ...) extern PFN_##name name
#define VULKAN_FUNC_IMPL(name, ...) PFN_##name name = nullptr
#define VULKAN_FUNC_IMPORT(name, so) name = so.getFunc<PFN_##name>(#name)
#define VULKAN_FUNC_SUCCESS(name, success) success = success && name
#define VULKAN_FUNC_IMPORT_GET_INST(name, instance) name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(instance, #name))

#define VULKAN_GET_PROC_LIST(macro, ...) \
	macro(vkCreateInstance, __VA_ARGS__); \
	macro(vkEnumerateInstanceExtensionProperties, __VA_ARGS__); \
	macro(vkEnumerateInstanceLayerProperties, __VA_ARGS__)


#define VULKAN_FUNC_LIST(macro, ...) \
	macro(vkDestroyInstance, __VA_ARGS__); \
	macro(vkEnumeratePhysicalDevices, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceFeatures, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceFormatProperties, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceImageFormatProperties, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceProperties, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceQueueFamilyProperties, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceMemoryProperties, __VA_ARGS__); \
	macro(vkCreateDevice, __VA_ARGS__); \
	macro(vkDestroyDevice, __VA_ARGS__); \
	macro(vkEnumerateDeviceExtensionProperties, __VA_ARGS__); \
	macro(vkEnumerateDeviceLayerProperties, __VA_ARGS__); \
	macro(vkGetDeviceQueue, __VA_ARGS__); \
	macro(vkQueueSubmit, __VA_ARGS__); \
	macro(vkQueueWaitIdle, __VA_ARGS__); \
	macro(vkDeviceWaitIdle, __VA_ARGS__); \
	macro(vkAllocateMemory, __VA_ARGS__); \
	macro(vkFreeMemory, __VA_ARGS__); \
	macro(vkMapMemory, __VA_ARGS__); \
	macro(vkUnmapMemory, __VA_ARGS__); \
	macro(vkFlushMappedMemoryRanges, __VA_ARGS__); \
	macro(vkInvalidateMappedMemoryRanges, __VA_ARGS__); \
	macro(vkGetDeviceMemoryCommitment, __VA_ARGS__); \
	macro(vkBindBufferMemory, __VA_ARGS__); \
	macro(vkBindImageMemory, __VA_ARGS__); \
	macro(vkGetBufferMemoryRequirements, __VA_ARGS__); \
	macro(vkGetImageMemoryRequirements, __VA_ARGS__); \
	macro(vkGetImageSparseMemoryRequirements, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSparseImageFormatProperties, __VA_ARGS__); \
	macro(vkQueueBindSparse, __VA_ARGS__); \
	macro(vkCreateFence, __VA_ARGS__); \
	macro(vkDestroyFence, __VA_ARGS__); \
	macro(vkResetFences, __VA_ARGS__); \
	macro(vkGetFenceStatus, __VA_ARGS__); \
	macro(vkWaitForFences, __VA_ARGS__); \
	macro(vkCreateSemaphore, __VA_ARGS__); \
	macro(vkDestroySemaphore, __VA_ARGS__); \
	macro(vkCreateEvent, __VA_ARGS__); \
	macro(vkDestroyEvent, __VA_ARGS__); \
	macro(vkGetEventStatus, __VA_ARGS__); \
	macro(vkSetEvent, __VA_ARGS__); \
	macro(vkResetEvent, __VA_ARGS__); \
	macro(vkCreateQueryPool, __VA_ARGS__); \
	macro(vkDestroyQueryPool, __VA_ARGS__); \
	macro(vkGetQueryPoolResults, __VA_ARGS__); \
	macro(vkCreateBuffer, __VA_ARGS__); \
	macro(vkDestroyBuffer, __VA_ARGS__); \
	macro(vkCreateBufferView, __VA_ARGS__); \
	macro(vkDestroyBufferView, __VA_ARGS__); \
	macro(vkCreateImage, __VA_ARGS__); \
	macro(vkDestroyImage, __VA_ARGS__); \
	macro(vkGetImageSubresourceLayout, __VA_ARGS__); \
	macro(vkCreateImageView, __VA_ARGS__); \
	macro(vkDestroyImageView, __VA_ARGS__); \
	macro(vkCreateShaderModule, __VA_ARGS__); \
	macro(vkDestroyShaderModule, __VA_ARGS__); \
	macro(vkCreatePipelineCache, __VA_ARGS__); \
	macro(vkDestroyPipelineCache, __VA_ARGS__); \
	macro(vkGetPipelineCacheData, __VA_ARGS__); \
	macro(vkMergePipelineCaches, __VA_ARGS__); \
	macro(vkCreateGraphicsPipelines, __VA_ARGS__); \
	macro(vkCreateComputePipelines, __VA_ARGS__); \
	macro(vkDestroyPipeline, __VA_ARGS__); \
	macro(vkCreatePipelineLayout, __VA_ARGS__); \
	macro(vkDestroyPipelineLayout, __VA_ARGS__); \
	macro(vkCreateSampler, __VA_ARGS__); \
	macro(vkDestroySampler, __VA_ARGS__); \
	macro(vkCreateDescriptorSetLayout, __VA_ARGS__); \
	macro(vkDestroyDescriptorSetLayout, __VA_ARGS__); \
	macro(vkCreateDescriptorPool, __VA_ARGS__); \
	macro(vkDestroyDescriptorPool, __VA_ARGS__); \
	macro(vkResetDescriptorPool, __VA_ARGS__); \
	macro(vkAllocateDescriptorSets, __VA_ARGS__); \
	macro(vkFreeDescriptorSets, __VA_ARGS__); \
	macro(vkUpdateDescriptorSets, __VA_ARGS__); \
	macro(vkCreateFramebuffer, __VA_ARGS__); \
	macro(vkDestroyFramebuffer, __VA_ARGS__); \
	macro(vkCreateRenderPass, __VA_ARGS__); \
	macro(vkDestroyRenderPass, __VA_ARGS__); \
	macro(vkGetRenderAreaGranularity, __VA_ARGS__); \
	macro(vkCreateCommandPool, __VA_ARGS__); \
	macro(vkDestroyCommandPool, __VA_ARGS__); \
	macro(vkResetCommandPool, __VA_ARGS__); \
	macro(vkAllocateCommandBuffers, __VA_ARGS__); \
	macro(vkFreeCommandBuffers, __VA_ARGS__); \
	macro(vkBeginCommandBuffer, __VA_ARGS__); \
	macro(vkEndCommandBuffer, __VA_ARGS__); \
	macro(vkResetCommandBuffer, __VA_ARGS__); \
	macro(vkCmdBindPipeline, __VA_ARGS__); \
	macro(vkCmdSetViewport, __VA_ARGS__); \
	macro(vkCmdSetScissor, __VA_ARGS__); \
	macro(vkCmdSetLineWidth, __VA_ARGS__); \
	macro(vkCmdSetDepthBias, __VA_ARGS__); \
	macro(vkCmdSetBlendConstants, __VA_ARGS__); \
	macro(vkCmdSetDepthBounds, __VA_ARGS__); \
	macro(vkCmdSetStencilCompareMask, __VA_ARGS__); \
	macro(vkCmdSetStencilWriteMask, __VA_ARGS__); \
	macro(vkCmdSetStencilReference, __VA_ARGS__); \
	macro(vkCmdBindDescriptorSets, __VA_ARGS__); \
	macro(vkCmdBindIndexBuffer, __VA_ARGS__); \
	macro(vkCmdBindVertexBuffers, __VA_ARGS__); \
	macro(vkCmdDraw, __VA_ARGS__); \
	macro(vkCmdDrawIndexed, __VA_ARGS__); \
	macro(vkCmdDrawIndirect, __VA_ARGS__); \
	macro(vkCmdDrawIndexedIndirect, __VA_ARGS__); \
	macro(vkCmdDispatch, __VA_ARGS__); \
	macro(vkCmdDispatchIndirect, __VA_ARGS__); \
	macro(vkCmdCopyBuffer, __VA_ARGS__); \
	macro(vkCmdCopyImage, __VA_ARGS__); \
	macro(vkCmdBlitImage, __VA_ARGS__); \
	macro(vkCmdCopyBufferToImage, __VA_ARGS__); \
	macro(vkCmdCopyImageToBuffer, __VA_ARGS__); \
	macro(vkCmdUpdateBuffer, __VA_ARGS__); \
	macro(vkCmdFillBuffer, __VA_ARGS__); \
	macro(vkCmdClearColorImage, __VA_ARGS__); \
	macro(vkCmdClearDepthStencilImage, __VA_ARGS__); \
	macro(vkCmdClearAttachments, __VA_ARGS__); \
	macro(vkCmdResolveImage, __VA_ARGS__); \
	macro(vkCmdSetEvent, __VA_ARGS__); \
	macro(vkCmdResetEvent, __VA_ARGS__); \
	macro(vkCmdWaitEvents, __VA_ARGS__); \
	macro(vkCmdPipelineBarrier, __VA_ARGS__); \
	macro(vkCmdBeginQuery, __VA_ARGS__); \
	macro(vkCmdEndQuery, __VA_ARGS__); \
	macro(vkCmdResetQueryPool, __VA_ARGS__); \
	macro(vkCmdWriteTimestamp, __VA_ARGS__); \
	macro(vkCmdCopyQueryPoolResults, __VA_ARGS__); \
	macro(vkCmdPushConstants, __VA_ARGS__); \
	macro(vkCmdBeginRenderPass, __VA_ARGS__); \
	macro(vkCmdNextSubpass, __VA_ARGS__); \
	macro(vkCmdEndRenderPass, __VA_ARGS__); \
	macro(vkCmdExecuteCommands, __VA_ARGS__)

#define VULKAN_FUNC_EXT_LIST(macro, ...) \
	macro(vkDestroySurfaceKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfaceSupportKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfaceFormatsKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfacePresentModesKHR, __VA_ARGS__); \
	macro(vkCreateSwapchainKHR, __VA_ARGS__); \
	macro(vkDestroySwapchainKHR, __VA_ARGS__); \
	macro(vkGetSwapchainImagesKHR, __VA_ARGS__); \
	macro(vkAcquireNextImageKHR, __VA_ARGS__); \
	macro(vkQueuePresentKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceDisplayPropertiesKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceDisplayPlanePropertiesKHR, __VA_ARGS__); \
	macro(vkGetDisplayPlaneSupportedDisplaysKHR, __VA_ARGS__); \
	macro(vkGetDisplayModePropertiesKHR, __VA_ARGS__); \
	macro(vkCreateDisplayModeKHR, __VA_ARGS__); \
	macro(vkGetDisplayPlaneCapabilitiesKHR, __VA_ARGS__); \
	macro(vkCreateDisplayPlaneSurfaceKHR, __VA_ARGS__); \
	macro(vkCreateSharedSwapchainsKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceFeatures2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceProperties2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceFormatProperties2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceImageFormatProperties2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceQueueFamilyProperties2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceMemoryProperties2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSparseImageFormatProperties2KHR, __VA_ARGS__); \
	macro(vkTrimCommandPoolKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceExternalBufferPropertiesKHR, __VA_ARGS__); \
	macro(vkGetMemoryFdKHR, __VA_ARGS__); \
	macro(vkGetMemoryFdPropertiesKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceExternalSemaphorePropertiesKHR, __VA_ARGS__); \
	macro(vkImportSemaphoreFdKHR, __VA_ARGS__); \
	macro(vkGetSemaphoreFdKHR, __VA_ARGS__); \
	macro(vkCmdPushDescriptorSetKHR, __VA_ARGS__); \
	macro(vkCreateDescriptorUpdateTemplateKHR, __VA_ARGS__); \
	macro(vkDestroyDescriptorUpdateTemplateKHR, __VA_ARGS__); \
	macro(vkUpdateDescriptorSetWithTemplateKHR, __VA_ARGS__); \
	macro(vkCmdPushDescriptorSetWithTemplateKHR, __VA_ARGS__); \
	macro(vkGetSwapchainStatusKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceExternalFencePropertiesKHR, __VA_ARGS__); \
	macro(vkImportFenceFdKHR, __VA_ARGS__); \
	macro(vkGetFenceFdKHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfaceCapabilities2KHR, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfaceFormats2KHR, __VA_ARGS__); \
	macro(vkGetImageMemoryRequirements2KHR, __VA_ARGS__); \
	macro(vkGetBufferMemoryRequirements2KHR, __VA_ARGS__); \
	macro(vkGetImageSparseMemoryRequirements2KHR, __VA_ARGS__); \
	macro(vkCreateSamplerYcbcrConversionKHR, __VA_ARGS__); \
	macro(vkDestroySamplerYcbcrConversionKHR, __VA_ARGS__); \
	macro(vkBindBufferMemory2KHR, __VA_ARGS__); \
	macro(vkBindImageMemory2KHR, __VA_ARGS__); \
	macro(vkCreateDebugReportCallbackEXT, __VA_ARGS__); \
	macro(vkDestroyDebugReportCallbackEXT, __VA_ARGS__); \
	macro(vkDebugReportMessageEXT, __VA_ARGS__); \
	macro(vkDebugMarkerSetObjectTagEXT, __VA_ARGS__); \
	macro(vkDebugMarkerSetObjectNameEXT, __VA_ARGS__); \
	macro(vkCmdDebugMarkerBeginEXT, __VA_ARGS__); \
	macro(vkCmdDebugMarkerEndEXT, __VA_ARGS__); \
	macro(vkCmdDebugMarkerInsertEXT, __VA_ARGS__); \
	macro(vkCmdDrawIndirectCountAMD, __VA_ARGS__); \
	macro(vkCmdDrawIndexedIndirectCountAMD, __VA_ARGS__); \
	macro(vkGetShaderInfoAMD, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceExternalImageFormatPropertiesNV, __VA_ARGS__); \
	macro(vkGetDeviceGroupPeerMemoryFeaturesKHX, __VA_ARGS__); \
	macro(vkCmdSetDeviceMaskKHX, __VA_ARGS__); \
	macro(vkCmdDispatchBaseKHX, __VA_ARGS__); \
	macro(vkGetDeviceGroupPresentCapabilitiesKHX, __VA_ARGS__); \
	macro(vkGetDeviceGroupSurfacePresentModesKHX, __VA_ARGS__); \
	macro(vkGetPhysicalDevicePresentRectanglesKHX, __VA_ARGS__); \
	macro(vkAcquireNextImage2KHX, __VA_ARGS__); \
	macro(vkEnumeratePhysicalDeviceGroupsKHX, __VA_ARGS__); \
	macro(vkCmdProcessCommandsNVX, __VA_ARGS__); \
	macro(vkCmdReserveSpaceForCommandsNVX, __VA_ARGS__); \
	macro(vkCreateIndirectCommandsLayoutNVX, __VA_ARGS__); \
	macro(vkDestroyIndirectCommandsLayoutNVX, __VA_ARGS__); \
	macro(vkCreateObjectTableNVX, __VA_ARGS__); \
	macro(vkDestroyObjectTableNVX, __VA_ARGS__); \
	macro(vkRegisterObjectsNVX, __VA_ARGS__); \
	macro(vkUnregisterObjectsNVX, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX, __VA_ARGS__); \
	macro(vkCmdSetViewportWScalingNV, __VA_ARGS__); \
	macro(vkReleaseDisplayEXT, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceSurfaceCapabilities2EXT, __VA_ARGS__); \
	macro(vkDisplayPowerControlEXT, __VA_ARGS__); \
	macro(vkRegisterDeviceEventEXT, __VA_ARGS__); \
	macro(vkRegisterDisplayEventEXT, __VA_ARGS__); \
	macro(vkGetSwapchainCounterEXT, __VA_ARGS__); \
	macro(vkGetRefreshCycleDurationGOOGLE, __VA_ARGS__); \
	macro(vkGetPastPresentationTimingGOOGLE, __VA_ARGS__); \
	macro(vkCmdSetDiscardRectangleEXT, __VA_ARGS__); \
	macro(vkSetHdrMetadataEXT, __VA_ARGS__); \
	macro(vkCmdSetSampleLocationsEXT, __VA_ARGS__); \
	macro(vkGetPhysicalDeviceMultisamplePropertiesEXT, __VA_ARGS__); \
	macro(vkCreateValidationCacheEXT, __VA_ARGS__); \
	macro(vkDestroyValidationCacheEXT, __VA_ARGS__); \
	macro(vkMergeValidationCachesEXT, __VA_ARGS__); \
	macro(vkGetValidationCacheDataEXT, __VA_ARGS__)

#ifdef PLATFORM_WINDOWS
	#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
		macro(vkCreateWin32SurfaceKHR, __VA_ARGS__); \
		macro(vkGetPhysicalDeviceWin32PresentationSupportKHR, __VA_ARGS__); \
		macro(vkGetMemoryWin32HandleKHR, __VA_ARGS__); \
		macro(vkGetMemoryWin32HandlePropertiesKHR, __VA_ARGS__); \
		macro(vkImportSemaphoreWin32HandleKHR, __VA_ARGS__); \
		macro(vkGetSemaphoreWin32HandleKHR, __VA_ARGS__); \
		macro(vkImportFenceWin32HandleKHR, __VA_ARGS__); \
		macro(vkGetFenceWin32HandleKHR, __VA_ARGS__); \
		macro(vkGetMemoryWin32HandleNV, __VA_ARGS__)

#elif defined(PLATFORM_LINUX)
	#ifdef VULKAN_USE_WAYLAND
		#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
			macro(vkCreateWaylandSurfaceKHR, __VA_ARGS__); \
			macro(vkGetPhysicalDeviceWaylandPresentationSupportKHR, __VA_ARGS__)
	#elif defined(VULKAN_USE_XRANDR)
		#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
			macro(vkCreateXlibSurfaceKHR, __VA_ARGS__); \
			macro(vkGetPhysicalDeviceXlibPresentationSupportKHR, __VA_ARGS__); \
			macro(vkAcquireXlibDisplayEXT, __VA_ARGS__); \
			macro(vkGetRandROutputDisplayEXT, __VA_ARGS__)
	#else
		#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
			macro(vkCreateXlibSurfaceKHR, __VA_ARGS__); \
			macro(vkGetPhysicalDeviceXlibPresentationSupportKHR, __VA_ARGS__)
	#endif

#elif defined(PLATFORM_IOS)
	#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
		macro(vkCreateIOSSurfaceMVK, __VA_ARGS__)

#elif defined(PLATFORM_MAC)
	#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
		macro(vkCreateMacOSSurfaceMVK, __VA_ARGS__)

#elif defined(PLATFORM_ANDROID)
	#define VULKAN_FUNC_OS_EXT_LIST(macro, ...) \
		macro(vkCreateAndroidSurfaceKHR, __VA_ARGS__)
#endif

extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

//macro(vkCreateViSurfaceNN, __VA_ARGS__);

#define VULKAN_DECL VULKAN_GET_PROC_LIST(VULKAN_FUNC_DECL); VULKAN_FUNC_LIST(VULKAN_FUNC_DECL)
#define VULKAN_IMPL VULKAN_GET_PROC_LIST(VULKAN_FUNC_IMPL); VULKAN_FUNC_LIST(VULKAN_FUNC_IMPL)
#define VULKAN_IMPORT(instance) VULKAN_FUNC_LIST(VULKAN_FUNC_IMPORT_GET_INST, instance)
#define VULKAN_SUCCESS(success) VULKAN_FUNC_LIST(VULKAN_FUNC_SUCCESS, success)

#define VULKAN_DECL_EXT VULKAN_FUNC_EXT_LIST(VULKAN_FUNC_DECL)
#define VULKAN_IMPL_EXT VULKAN_FUNC_EXT_LIST(VULKAN_FUNC_IMPL)
#define VULKAN_IMPORT_EXT(instance) VULKAN_FUNC_EXT_LIST(VULKAN_FUNC_IMPORT_GET_INST, instance)
#define VULKAN_SUCCESS_EXT(success) VULKAN_FUNC_EXT_LIST(VULKAN_FUNC_SUCCESS, success)

#define VULKAN_DECL_OS VULKAN_FUNC_OS_EXT_LIST(VULKAN_FUNC_DECL)
#define VULKAN_IMPL_OS VULKAN_FUNC_OS_EXT_LIST(VULKAN_FUNC_IMPL)
#define VULKAN_IMPORT_OS(instance) VULKAN_FUNC_OS_EXT_LIST(VULKAN_FUNC_IMPORT_GET_INST, instance)
#define VULKAN_SUCCESS_OS(success) VULKAN_FUNC_OS_EXT_LIST(VULKAN_FUNC_SUCCESS, success)

VULKAN_DECL;
VULKAN_DECL_EXT;
VULKAN_DECL_OS;
