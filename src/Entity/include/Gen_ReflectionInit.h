// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

// Includes
#include <Shibboleth_ComponentManager.h>

#include <Gaff_ReflectionInterfaces.h>
#include <Gaff_Reflection.h>

namespace Gen
{
	// Using namespaces.
	using namespace Shibboleth;

	void InitReflection(void)
	{
		// Initialize Attributes first.

		Gaff::IReflection* head = Gaff::GetAttributeReflectionChainHead();

		while (head) {
			head->init();
			head = head->attr_next;
		}

		Reflection<ComponentManager>::Init();

		// Initialize any other reflection that we reference, but isn't owned by our module.
		head = Gaff::GetReflectionChainHead();

		while (head) {
			head->init();
			head = head->next;
		}
	}
}
