// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

// Includes
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceExtensionAttribute.h>

#include <Gaff_ReflectionInterfaces.h>
#include <Gaff_EnumReflection.h>
#include <Gaff_Reflection.h>

namespace Gen
{
	// Using namespaces.
	using namespace Shibboleth;

	void InitReflection(void)
	{
		// Initialize Enums.

		Gaff::IEnumReflection* enum_head = Gaff::GetEnumReflectionChainHead();

		while(enum_head) {
			enum_head->init();
			enum_head = enum_head->next;
		}

		// Initialize Attributes.
		Reflection<ResExtAttribute>::Init();

		Gaff::IReflection* head = Gaff::GetAttributeReflectionChainHead();

		while (head) {
			head->init();
			head = head->attr_next;
		}

		Reflection<ResourceManager>::Init();

		// Initialize any other reflection that we reference, but isn't owned by our module.
		head = Gaff::GetReflectionChainHead();

		while (head) {
			head->init();
			head = head->next;
		}
	}
}