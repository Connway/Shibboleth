/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_InputMapping.h"
#include <Shibboleth_LocalPlayerSubsystem.h>
#include <gtc/constants.hpp>

NS_SHIBBOLETH

class InputMappingResource;

class PlayerInputSubsystem final : public LocalPlayerSubsystem
{
public:
	void init(const SubsystemCollectorBase& /*collector*/) override;
	void destroy(const SubsystemCollectorBase& /*collector*/) override;

	void addInputMapping(const InputMappingResource& input_mapping, int32_t priority);
	void removeInputMapping(const InputMappingResource& input_mapping);

private:
	struct BoundInputMapping final
	{
		Vector<InputMapping> instances{ ProxyAllocator("Input") };

		const InputMappingResource* source;
		int32_t priority;

		bool operator==(const InputMappingResource* rhs) const
		{
			return source == rhs;
		}

		std::strong_ordering operator<=>(int32_t rhs) const
		{
			if (priority == rhs) {
				return std::strong_ordering::equal;
			} else if (priority < rhs) {
				return std::strong_ordering::less;
			} else {
				return std::strong_ordering::greater;
			}
		}
	};

	struct AliasData final
	{
		Gleam::Vec3 value = glm::zero<Gleam::Vec3>();
	};

	VectorMap< HashString32<>, Gleam::Vec3 > _alias_values{ ProxyAllocator("Input") };
	Vector<BoundInputMapping> _mappings{ ProxyAllocator("Input") };

	Gleam::Vec3& getOrAddAlias(const HashString32<>& alias);

	SHIB_REFLECTION_CLASS_DECLARE(PlayerInputSubsystem);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::PlayerInputSubsystem)
