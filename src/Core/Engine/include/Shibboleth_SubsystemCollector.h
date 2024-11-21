/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Reflection/Shibboleth_Reflection.h"
#include "Shibboleth_ISubsystem.h"

NS_SHIBBOLETH

class SubsystemCollectorBase
{
public:
	SubsystemCollectorBase(const ProxyAllocator& allocator = ProxyAllocator());

	const ISubsystem* getSubsystem(const Refl::IReflectionDefinition& ref_def) const;
	ISubsystem* getSubsystem(const Refl::IReflectionDefinition& ref_def);

	void init(const Refl::IReflectionDefinition& ref_def);
	void destroy(void);

private:
	Vector< UniquePtr<ISubsystem> > _subsystems;
	ProxyAllocator _allocator;
};

template <class T>
class SubsystemCollector final : public SubsystemCollectorBase
{
	static_assert(std::is_base_of_v<ISubsystem, T>, "Type must derive from ISubsystem.");

public:
	template <class U>
	const U* getSubsystem(void) const
	{
		return const_cast<SubsystemCollector*>(this)->template getSubsystem<U>();
	}

	template <class U>
	U* getSubsystem(void)
	{
		static_assert(std::is_base_of_v<T, U>, "Type must derive from collector type.");
		return getSubsystem(Refl::Reflection<U>::GetReflectionDefinition());
	}

	void init(void)
	{
		SubsystemCollectorBase::init(Refl::Reflection<T>::GetReflectionDefinition());
	}
};

NS_END
