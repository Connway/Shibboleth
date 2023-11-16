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

#include "Shibboleth_SubsystemCollector.h"

namespace
{
	static bool SubsystemSearchPredicate(const Shibboleth::UniquePtr<Shibboleth::ISubsystem>& lhs, const Refl::IReflectionDefinition& rhs)
	{
		return &lhs->getReflectionDefinition() < &rhs;
	}
}

NS_SHIBBOLETH

SubsystemCollectorBase::SubsystemCollectorBase(const ProxyAllocator& allocator):
	_subsystems(allocator), _allocator(allocator)
{
}

const ISubsystem* SubsystemCollectorBase::getSubsystem(const Refl::IReflectionDefinition& ref_def) const
{
	return const_cast<SubsystemCollectorBase*>(this)->getSubsystem(ref_def);
}

ISubsystem* SubsystemCollectorBase::getSubsystem(const Refl::IReflectionDefinition& ref_def)
{
	const auto it = Gaff::LowerBound(_subsystems, ref_def, SubsystemSearchPredicate);
	return (it == _subsystems.end() || &(*it)->getReflectionDefinition() != &ref_def) ? nullptr : it->get();
}

void SubsystemCollectorBase::init(const Refl::IReflectionDefinition& ref_def)
{
	const auto* const type_bucket = GetApp().getReflectionManager().getTypeBucket(ref_def);

	if (!type_bucket) {
		// $TODO: Log error.
		return;
	}

	_subsystems.reserve(type_bucket->size());

	for (const Refl::IReflectionDefinition* ref_def : *type_bucket) {
		const ShouldCreateSubsystemAttribute* const attr = ref_def->getClassAttr<ShouldCreateSubsystemAttribute>();

		if (!attr->shouldCreate()) {
			continue;
		}

		ISubsystem* const subsystem = ref_def->CREATET(Shibboleth::ISubsystem, _allocator);

		const auto it = Gaff::LowerBound(_subsystems, *ref_def, SubsystemSearchPredicate);
		_subsystems.emplace(it, subsystem);
	}

	for (const UniquePtr<ISubsystem>& subsystem : _subsystems) {
		subsystem->init(*this);
	}
}

void SubsystemCollectorBase::destroy(void)
{
	for (const UniquePtr<ISubsystem>& subsystem : _subsystems) {
		subsystem->destroy(*this);
	}

	_subsystems.clear();
}

NS_END
