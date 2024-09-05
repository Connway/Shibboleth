/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

NS_REFLECTION

template <class T>
VarReflectionDefinition<T>::VarReflectionDefinition(const IReflectionDefinition* T::* ptr):
	IVar<T>(ptr)
{
}

template <class T>
const IReflection& VarReflectionDefinition<T>::getReflection(void) const
{
	GAFF_ASSERT_MSG(false, "VarReflectionDefinition<T>::getReflection: Should not be called.");
	const IReflection* const refl = nullptr;
	return *refl;
}

template <class T>
const void* VarReflectionDefinition<T>::getData(const void* object) const
{
	return const_cast<VarReflectionDefinition<T>*>(this)->getData(const_cast<void*>(object));
}

template <class T>
void* VarReflectionDefinition<T>::getData(void* object)
{
	return IVar<T>::template get<const IReflectionDefinition*>(object);
}

template <class T>
void VarReflectionDefinition<T>::setData(void* object, const void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const IReflectionDefinition** var = IVar<T>::template get<const IReflectionDefinition*>(object);
	*var = *reinterpret_cast<const IReflectionDefinition* const *>(data);
}

template <class T>
void VarReflectionDefinition<T>::setDataMove(void* object, void* data)
{
	setData(object, data);
}

template <class T>
bool VarReflectionDefinition<T>::load(const Shibboleth::ISerializeReader& reader, void* object)
{
	GAFF_ASSERT(reader.isNull() || reader.isString());

	if (reader.isNull()) {
		return true;
	}

	const IReflectionDefinition** var = reinterpret_cast<const IReflectionDefinition**>(object);
	const char8_t* const class_name = reader.readString();

	*var = Shibboleth::GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(class_name));

	reader.freeString(class_name);
	return true;
}

template <class T>
void VarReflectionDefinition<T>::save(Shibboleth::ISerializeWriter& writer, const void* object)
{
	const IReflectionDefinition* const * var = reinterpret_cast<const IReflectionDefinition* const *>(object);

	if (*var) {
		writer.writeString((*var)->getReflectionInstance().getName());
	} else {
		writer.writeNull();
	}
}

template <class T>
bool VarReflectionDefinition<T>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const IReflectionDefinition** var = const_cast<const IReflectionDefinition**>(IVar<T>::template get<const IReflectionDefinition*>(&object));
	return load(reader, var);
}

template <class T>
void VarReflectionDefinition<T>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const IReflectionDefinition* const * var = IVar<T>::template get<const IReflectionDefinition*>(&object);
	save(writer, var);
}

NS_END
