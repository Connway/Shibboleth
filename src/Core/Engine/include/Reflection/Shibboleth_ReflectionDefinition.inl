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

NS_REFLECTION

template <class T>
bool ReflectionDefinition<T>::load(const Shibboleth::ISerializeReader& reader, void* object, Gaff::Flags<LoadFlags> flags) const
{
	return load(reader, *reinterpret_cast<T*>(object), flags);
}

template <class T>
void ReflectionDefinition<T>::save(Shibboleth::ISerializeWriter& writer, const void* object, Gaff::Flags<SaveFlags> flags) const
{
	save(writer, *reinterpret_cast<const T*>(object), flags);
}

template <class T>
bool ReflectionDefinition<T>::load(const Shibboleth::ISerializeReader& reader, T& object, Gaff::Flags<LoadFlags> flags) const
{
	if (_data.serialize_load && !flags.testAny(LoadFlags::ReflectionLoad)) {
		return _data.serialize_load(reader, object);

	} else {
		for (auto& entry : _data.vars) {
			if (entry.second.var->canSerialize()) {
				const char8_t* const name = entry.first.getBuffer();

				if (!reader.exists(name)) {
					// Skip over optional variables.
					if (entry.second.var->isOptional()) {
						continue;

					} else {
						// $TODO: Log error.
						return false;
					}
				}

				Shibboleth::ScopeGuard scope = reader.enterElementGuard(name);
				entry.second.var->load(reader, object);
			}
		}
	}

	return true;
}

template <class T>
void ReflectionDefinition<T>::save(Shibboleth::ISerializeWriter& writer, const T& object, Gaff::Flags<SaveFlags> flags) const
{
	if (_data.serialize_save && !flags.testAny(SaveFlags::ReflectionSave)) {
		_data.serialize_save(writer, object);

	} else {
		uint32_t writable_vars = 0;

		// Count how many vars we're actually writing to the object.
		for (auto& entry : _data.vars) {
			if (entry.second.var->canSerialize()) {
				++writable_vars;
			}
		}

		// Write out the object.
		writer.startObject(writable_vars + 1);

		writer.writeUInt64(u8"version", getReflectionInstance().getVersion().getHash());

		for (auto& entry : _data.vars) {
			if (entry.second.var->canSerialize()) {
				writer.writeKey(entry.first.getBuffer());

				if (entry.second.var->isSerializingDefaultValue()) {
					writer.writeNull();
				} else {
					entry.second.var->save(writer, object);
				}
			}
		}

		writer.endObject();
	}
}

template <class T>
Gaff::Hash64 ReflectionDefinition<T>::getInstanceHash(const void* object, Gaff::Hash64 init) const
{
	return getInstanceHash(*reinterpret_cast<const T*>(object), init);
}

template <class T>
Gaff::Hash64 ReflectionDefinition<T>::getInstanceHash(const T& object, Gaff::Hash64 init) const
{
	return (_data.instance_hash) ? _data.instance_hash(object, init) : Gaff::FNV1aHash64T(object, init);
}

template <class T>
const void* ReflectionDefinition<T>::getInterface(Gaff::Hash64 class_hash, const void* object) const
{
	if (class_hash == Reflection<T>::GetNameHash()) {
		return object;
	}

	const auto it = _data.base_classes.find(class_hash);

	if (it == _data.base_classes.end()) {
		return nullptr;
	}

	return reinterpret_cast<const int8_t*>(object) + it->second.offset;
}

template <class T>
void* ReflectionDefinition<T>::getInterface(Gaff::Hash64 class_hash, void* object) const
{
	return const_cast<void*>(getInterface(class_hash, const_cast<const void*>(object)));
}

template <class T>
const void* ReflectionDefinition<T>::getInterface(const IReflectionDefinition& ref_def, const void* object) const
{
	return getInterface(ref_def.getReflectionInstance().getNameHash(), object);
}

template <class T>
void* ReflectionDefinition<T>::getInterface(const IReflectionDefinition& ref_def, void* object) const
{
	return getInterface(ref_def.getReflectionInstance().getNameHash(), object);
}

template <class T>
bool ReflectionDefinition<T>::hasInterface(Gaff::Hash64 class_hash) const
{
	if (class_hash == Reflection<T>::GetNameHash()) {
		return true;
	}

	const auto it = _data.base_classes.find(class_hash);
	return it != _data.base_classes.end();
}

template <class T>
bool ReflectionDefinition<T>::hasInterface(const IReflectionDefinition& ref_def) const
{
	return hasInterface(ref_def.getReflectionInstance().getNameHash());
}

template <class T>
const IReflection& ReflectionDefinition<T>::getReflectionInstance(void) const
{
	return Reflection<T>::GetInstance();
}

template <class T>
int32_t ReflectionDefinition<T>::size(void) const
{
	return sizeof(T);
}

template <class T>
bool ReflectionDefinition<T>::isBuiltIn(void) const
{
	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumVars(void) const
{
	return static_cast<int32_t>(_data.vars.size());
}

template <class T>
Shibboleth::HashStringView32<> ReflectionDefinition<T>::getVarName(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < getNumVars());
	return Shibboleth::HashStringView32<>{ _data.vars.at(index).first };
}

template <class T>
IReflectionVar* ReflectionDefinition<T>::getVar(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_data.vars.size()));
	return getVarT(index);
}

template <class T>
IReflectionVar* ReflectionDefinition<T>::getVar(Gaff::Hash32 name) const
{
	return getVarT(name);
}

template <class T>
int32_t ReflectionDefinition<T>::getNumFuncs(void) const
{
	return static_cast<int32_t>(_data.funcs.size());
}

template <class T>
int32_t ReflectionDefinition<T>::getNumFuncOverrides(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_data.funcs.size()));
	return static_cast<int32_t>(_data.funcs.at(index).second.overrides.size());
}

template <class T>
Shibboleth::HashStringView32<> ReflectionDefinition<T>::getFuncName(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < getNumFuncs());
	return Shibboleth::HashStringView32<>{ _data.funcs.at(index).first };
}

template <class T>
int32_t ReflectionDefinition<T>::getNumStaticFuncs(void) const
{
	return static_cast<int32_t>(_data.static_funcs.size());
}

template <class T>
int32_t ReflectionDefinition<T>::getNumStaticFuncOverrides(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_data.static_funcs.size()));
	return static_cast<int32_t>(_data.static_funcs.at(index).second.overrides.size());
}

template <class T>
Shibboleth::HashStringView32<> ReflectionDefinition<T>::getStaticFuncName(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < getNumStaticFuncs());
	return Shibboleth::HashStringView32<>{ _data.static_funcs.at(index).first };
}

template <class T>
int32_t ReflectionDefinition<T>::getNumClassAttrs(void) const
{
	return static_cast<int32_t>(_data.class_attrs.size());
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getClassAttr(Gaff::Hash64 attr_name) const
{
	return getAttribute(_data.class_attrs, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getClassAttr(int32_t index) const
{
	GAFF_ASSERT(index < getNumClassAttrs());
	return _data.class_attrs[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasClassAttr(Gaff::Hash64 attr_name) const
{
	return getClassAttr(attr_name) != nullptr;
}

template <class T>
void ReflectionDefinition<T>::addClassAttr(IAttribute& attribute)
{
	ReflectionBuilder<T, T> builder{ _data, *this };
	builder.classAttrs(attribute);
}

template <class T>
int32_t ReflectionDefinition<T>::getNumVarAttrs(Gaff::Hash32 name) const
{
	const auto it = _data.vars.find(name);
	GAFF_ASSERT(it != _data.vars.end());

	return static_cast<int32_t>(it->second.attrs.size());
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getVarAttr(Gaff::Hash32 name, Gaff::Hash64 attr_name) const
{
	const auto it = _data.vars.find(name);
	GAFF_ASSERT(it != _data.vars.end());

	return getAttribute(it->second.attrs, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getVarAttr(Gaff::Hash32 name, int32_t index) const
{
	const auto it = _data.vars.find(name);

	GAFF_ASSERT(it != _data.vars.end());
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(it->second.attrs.size()));

	return it->second.attrs[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasVarAttr(Gaff::Hash64 attr_name) const
{
	for (const auto& entry : _data.vars) {
		if (getAttribute(entry.second.attrs, attr_name)) {
			return true;
		}
	}

	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumFuncAttrs(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash) const
{
	const auto it = _data.funcs.find(name_hash);
	GAFF_ASSERT(it != _data.funcs.end());

	const auto it_override = it->second.overrides.find(args_hash);
	GAFF_ASSERT(it_override != it->second.overrides.end());

	return static_cast<int32_t>(it_override->second.attrs.size());
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, Gaff::Hash64 attr_name) const
{
	const auto it = _data.funcs.find(name_hash);
	GAFF_ASSERT(it != _data.funcs.end());

	const auto it_override = it->second.overrides.find(args_hash);
	GAFF_ASSERT(it_override != it->second.overrides.end());

	return getAttribute(it_override->second.attrs, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, int32_t index) const
{
	const auto it = _data.funcs.find(name_hash);
	GAFF_ASSERT(it != _data.funcs.end());

	const auto it_override = it->second.overrides.find(args_hash);
	GAFF_ASSERT(it_override != it->second.overrides.end());

	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(it_override->second.attrs.size()));
	return it_override->second.attrs[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasFuncAttr(Gaff::Hash64 attr_name) const
{
	for (const auto& entry : _data.funcs) {
		for (const auto& override : entry.second.overrides) {
			if (getAttribute(override.second.attrs, attr_name)) {
				return true;
			}
		}
	}

	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumStaticFuncAttrs(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash) const
{
	const auto it = _data.static_funcs.find(name_hash);
	GAFF_ASSERT(it != _data.static_funcs.end());

	const auto it_override = it->second.overrides.find(args_hash);
	GAFF_ASSERT(it_override != it->second.overrides.end());

	return static_cast<int32_t>(it_override->second.attrs.size());
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getStaticFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, Gaff::Hash64 attr_name) const
{
	const auto it = _data.static_funcs.find(name_hash);
	GAFF_ASSERT(it != _data.static_funcs.end());

	const auto it_override = it->second.overrides.find(args_hash);
	GAFF_ASSERT(it_override != it->second.overrides.end());

	return getAttribute(it_override->second.attrs, attr_name);
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getStaticFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, int32_t index) const
{
	const auto it = _data.static_funcs.find(name_hash);
	GAFF_ASSERT(it != _data.static_funcs.end());

	const auto it_override = it->second.overrides.find(args_hash);
	GAFF_ASSERT(it_override != it->second.overrides.end());

	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(it_override->second.attrs.size()));
	return it_override->second.attrs[index].get();
}

template <class T>
bool ReflectionDefinition<T>::hasStaticFuncAttr(Gaff::Hash64 attr_name) const
{
	for (const auto& entry : _data.static_funcs) {
		for (const auto& override : entry.second.overrides) {
			if (getAttribute(override.second.attrs, attr_name)) {
				return true;
			}
		}
	}

	return false;
}

template <class T>
int32_t ReflectionDefinition<T>::getNumConstructors(void) const
{
	return static_cast<int32_t>(_data.factories.size());
}

template <class T>
IReflectionStaticFunctionBase* ReflectionDefinition<T>::getConstructor(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < getNumConstructors());
	return _data.factories.at(static_cast<size_t>(index)).second.ctor.get();
}

template <class T>
VoidFunc ReflectionDefinition<T>::getConstructor(Gaff::Hash64 ctor_hash) const
{
	const auto it = _data.factories.find(ctor_hash);
	return (it == _data.factories.end()) ? nullptr : it->second.ctor->getFunc();
}

template <class T>
VoidFunc ReflectionDefinition<T>::getFactory(Gaff::Hash64 ctor_hash) const
{
	const auto it = _data.factories.find(ctor_hash);
	return (it == _data.factories.end()) ? nullptr : it->second.factory;
}

template <class T>
IReflectionStaticFunctionBase* ReflectionDefinition<T>::getStaticFunc(int32_t name_index, int32_t override_index) const
{
	GAFF_ASSERT(name_index >= 0 && name_index < getNumStaticFuncs());

	const auto& static_func_entry = _data.static_funcs.at(name_index);

	GAFF_ASSERT(override_index >= 0 && override_index < static_cast<int32_t>(static_func_entry.second.overrides.size()));

	const auto& override_entry = static_func_entry.second.overrides.at(override_index);
	return override_entry.second.func.get();
}

template <class T>
IReflectionStaticFunctionBase* ReflectionDefinition<T>::getStaticFunc(Gaff::Hash32 name, Gaff::Hash64 args) const
{
	const auto it_static_funcs = _data.static_funcs.find(name);

	if (it_static_funcs != _data.static_funcs.end()) {
		const auto it_override = it_static_funcs->second.overrides.find(args);
		return (it_override == it_static_funcs->second.overrides.end()) ? nullptr : it_override->second.func.get();
	}

	return nullptr;
}

template <class T>
IReflectionFunctionBase* ReflectionDefinition<T>::getFunc(int32_t name_index, int32_t override_index) const
{
	GAFF_ASSERT(name_index >= 0 && name_index < getNumFuncs());

	const auto& func_entry = _data.funcs.at(name_index);

	GAFF_ASSERT(override_index >= 0 && override_index < static_cast<int32_t>(func_entry.second.overrides.size()));

	const auto& override_entry = func_entry.second.overrides.at(override_index);
	return override_entry.second.func.get();
}

template <class T>
IReflectionFunctionBase* ReflectionDefinition<T>::getFunc(Gaff::Hash32 name, Gaff::Hash64 args) const
{
	const auto it_funcs = _data.funcs.find(name);

	if (it_funcs != _data.funcs.end()) {
		const auto it_override = it_funcs->second.overrides.find(args);
		return (it_override == it_funcs->second.overrides.end()) ? nullptr : it_override->second.func.get();
	}

	return nullptr;
}

template <class T>
void* ReflectionDefinition<T>::duplicate(const void* object, Gaff::IAllocator& allocator) const
{
	Gaff::Hash64 copy_ctor_hash = Gaff::FNV1aHash64Const(u8"const ");
	copy_ctor_hash = Gaff::FNV1aHash64String(getReflectionInstance().getName(), copy_ctor_hash);
	copy_ctor_hash = Gaff::FNV1aHash64Const(u8"&", copy_ctor_hash);

	void* copy = create(
		copy_ctor_hash,
		allocator,
		object
	);

	if (!copy) {
		copy = create(allocator);

		const int32_t num_vars = getNumVars();

		for (int32_t i = 0; i < num_vars; ++i) {
			Refl::IReflectionVar* const var = getVar(i);

			if (var->isNoCopy()) {
				continue;
			}

			const void* const orig_data = var->getData(object);
			var->setData(copy, orig_data);
		}
	}

	return copy;
}

template <class T>
void ReflectionDefinition<T>::destroyInstance(void* data) const
{
	T* const instance = reinterpret_cast<T*>(data);
	Gaff::Deconstruct(instance);
}

template <class T>
bool ReflectionDefinition<T>::isCopyConstructible(void) const
{
	return std::is_copy_constructible_v<T> && !std::is_trivially_copy_constructible_v<T>;
}

template <class T>
bool ReflectionDefinition<T>::isCopyAssignable(void) const
{
	return std::is_copy_assignable_v<T> && !std::is_trivially_copy_assignable_v<T>;
}

template <class T>
bool ReflectionDefinition<T>::isConstructible(void) const
{
	return std::is_default_constructible_v<T> && !std::is_trivially_default_constructible_v<T>;
}

template <class T>
bool ReflectionDefinition<T>::isDestructible(void) const
{
	return std::is_destructible_v<T> && !std::is_trivially_destructible_v<T>;
}

template <class T>
IVar<T>* ReflectionDefinition<T>::getVarT(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < getNumVars());
	return _data.vars.at(index).second.var.get();
}

template <class T>
IVar<T>* ReflectionDefinition<T>::getVarT(Gaff::Hash32 name) const
{
	const auto it = _data.vars.find(name);
	return (it == _data.vars.end()) ? nullptr : it->second.var.get();
}

template <class T>
ptrdiff_t ReflectionDefinition<T>::getBasePointerOffset(Gaff::Hash64 interface_name) const
{
	const auto it = _data.base_classes.find(interface_name);
	return (it != _data.base_classes.end()) ? it->second.offset : -1;
}

template <class T>
void ReflectionDefinition<T>::instantiated(void* object) const
{
	for (auto& attr : _data.class_attrs) {
		attr->instantiated(object, *this);
	}

	for (auto& var_entry : _data.vars) {
		for (auto& attr : var_entry.second.attrs) {
			attr->instantiated(object, *this);
		}
	}

	for (auto& func_entry : _data.funcs) {
		for (auto& override_entry : func_entry.second.overrides) {
			for (auto& attr : override_entry.second.attrs) {
				attr->instantiated(object, *this);
			}
		}
	}

	for (auto& static_func_entry : _data.funcs) {
		for (auto& override_entry : static_func_entry.second.overrides) {
			for (auto& attr : override_entry.second.attrs) {
				attr->instantiated(object, *this);
			}
		}
	}
}

template <class T>
const IAttribute* ReflectionDefinition<T>::getAttribute(const AttributeList& attributes, Gaff::Hash64 attr_name) const
{
	for (const auto& attr : attributes) {
		if (attr->getReflectionDefinition().hasInterface(attr_name)) {
			return attr.get();
		}
	}

	return nullptr;
}

template <class T>
ReflectionBuilder<T, T> ReflectionDefinition<T>::getInitialBuilder(void)
{
	return ReflectionBuilder<T, T>{ _data, *this };
}

REF_DEF_BUILTIN(int8_t, Int8);
REF_DEF_BUILTIN(int16_t, Int16);
REF_DEF_BUILTIN(int32_t, Int32);
REF_DEF_BUILTIN(int64_t, Int64);
REF_DEF_BUILTIN(uint8_t, UInt8);
REF_DEF_BUILTIN(uint16_t, UInt16);
REF_DEF_BUILTIN(uint32_t, UInt32);
REF_DEF_BUILTIN(uint64_t, UInt64);
REF_DEF_BUILTIN(float, Float);
REF_DEF_BUILTIN(double, Double);
REF_DEF_BUILTIN(bool, Bool);

NS_END
