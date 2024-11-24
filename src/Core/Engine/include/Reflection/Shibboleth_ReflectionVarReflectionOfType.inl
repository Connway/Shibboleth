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

#include "Shibboleth_ReflectionOfType.h" // Including here to avoid include order issues.

NS_REFLECTION

template <class T, class U>
VarDeferredReflectionOfType<T, U>::VarDeferredReflectionOfType(DeferredReflectionOfType<U> T::* ptr):
	IVar<T>(ptr)
{
}

template <class T, class U>
const IReflection& VarDeferredReflectionOfType<T, U>::getReflection(void) const
{
	GAFF_ASSERT_MSG(false, "VarDeferredReflectionOfType<T, U>::getReflection: Should not be called.");
	const IReflection* const refl = nullptr;
	return *refl;
}

template <class T, class U>
const void* VarDeferredReflectionOfType<T, U>::getData(const void* object) const
{
	return const_cast<VarDeferredReflectionOfType<T, U>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class U>
void* VarDeferredReflectionOfType<T, U>::getData(void* object)
{
	return IVar<T>::template get< DeferredReflectionOfType<U> >(object);
}

template <class T, class U>
void VarDeferredReflectionOfType<T, U>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	DeferredReflectionOfType<U>* const var = IVar<T>::template get< DeferredReflectionOfType<U> >(object);
	*var = *reinterpret_cast<const DeferredReflectionOfType<U>*>(data);
}

template <class T, class U>
void VarDeferredReflectionOfType<T, U>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	DeferredReflectionOfType<U>* const var = IVar<T>::template get< DeferredReflectionOfType<U> >(object);
	*var = std::move(*reinterpret_cast<DeferredReflectionOfType<U>*>(data));
}

template <class T, class U>
bool VarDeferredReflectionOfType<T, U>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isString());

	if (reader.isNull()) {
		return true;
	}

	DeferredReflectionOfType<U>* const var = reinterpret_cast<DeferredReflectionOfType<U>*>(object);
	const char8_t* const class_name = reader.readString();

	var->_class_name = class_name;
	var->_ref_def = nullptr; // Just in case this was default constructed.

	reader.freeString(class_name);
	return true;
}

template <class T, class U>
void VarDeferredReflectionOfType<T, U>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	const DeferredReflectionOfType<U>* const var = reinterpret_cast<const DeferredReflectionOfType<U>*>(object);

	if (var->_class_name.empty()) {
		writer.writeNull();
	} else {
		writer.writeString(var->_class_name.getBuffer());
	}
}

template <class T, class U>
bool VarDeferredReflectionOfType<T, U>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	DeferredReflectionOfType<U>* const var = IVar<T>::template get< DeferredReflectionOfType<U> >(&object);
	return load(reader, var);
}

template <class T, class U>
void VarDeferredReflectionOfType<T, U>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const DeferredReflectionOfType<U>* const var = IVar<T>::template get< DeferredReflectionOfType<U> >(&object);
	save(writer, var);
}



template <class T, class U>
VarReflectionOfType<T, U>::VarReflectionOfType(ReflectionOfType<U> T::* ptr):
	IVar<T>(ptr)
{
}

template <class T, class U>
const IReflection& VarReflectionOfType<T, U>::getReflection(void) const
{
	GAFF_ASSERT_MSG(false, "VarReflectionOfType<T, U>::getReflection: Should not be called.");
	const IReflection* const refl = nullptr;
	return *refl;
}

template <class T, class U>
const void* VarReflectionOfType<T, U>::getData(const void* object) const
{
	return const_cast<VarReflectionOfType<T, U>*>(this)->getData(const_cast<void*>(object));
}

template <class T, class U>
void* VarReflectionOfType<T, U>::getData(void* object)
{
	return IVar<T>::template get< ReflectionOfType<U> >(object);
}

template <class T, class U>
void VarReflectionOfType<T, U>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	ReflectionOfType<U>* const var = IVar<T>::template get< ReflectionOfType<U> >(object);
	*var = *reinterpret_cast<ReflectionOfType<U>*>(data);
}

template <class T, class U>
void VarReflectionOfType<T, U>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T, class U>
bool VarReflectionOfType<T, U>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isString());

	if (reader.isNull()) {
		return true;
	}

	ReflectionOfType<U>* const var = reinterpret_cast<ReflectionOfType<U>*>(object);
	const char8_t* const class_name = reader.readString();

	*var = Shibboleth::GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(class_name));

	reader.freeString(class_name);
	return true;
}

template <class T, class U>
void VarReflectionOfType<T, U>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	const ReflectionOfType<U>* const var = reinterpret_cast<const ReflectionOfType<U>*>(object);

	if (*var) {
		writer.writeString((*var)->getReflectionInstance().getName());
	} else {
		writer.writeNull();
	}
}

template <class T, class U>
bool VarReflectionOfType<T, U>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	ReflectionOfType<U>* const var = IVar<T>::template get< ReflectionOfType<U> >(&object);
	return load(reader, var);
}

template <class T, class U>
void VarReflectionOfType<T, U>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const ReflectionOfType<U>* const var = IVar<T>::template get< ReflectionOfType<U> >(&object);
	save(writer, var);
}

NS_END
