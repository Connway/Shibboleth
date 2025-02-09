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

#include "Shibboleth_IReflectionDefinition.h"
#include "Shibboleth_ReflectionBuilder.h"
#include <Gaff_ArrayString.h>

NS_REFLECTION

template <class T>
class ReflectionDefinition final : public IReflectionDefinition
{
public:
	// Add to this list as needed.
	using IReflectionDefinition::getClassAttr;
	using IReflectionDefinition::getVarAttr;

	GAFF_STRUCTORS_DEFAULT(ReflectionDefinition);
	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);

	constexpr static bool IsBuiltIn(void) { return false; }

	bool load(const Shibboleth::ISerializeReader& reader, void* object, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const override;
	bool load(const Shibboleth::ISerializeReader& reader, T& object, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const;
	void save(Shibboleth::ISerializeWriter& writer, const T& object, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const;

	Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const override;
	Gaff::Hash64 getInstanceHash(const T& object, Gaff::Hash64 init = Gaff::k_init_hash64) const;

	const void* getInterface(Gaff::Hash64 class_hash, const void* object) const override;
	void* getInterface(Gaff::Hash64 class_hash, void* object) const override;
	const void* getInterface(const IReflectionDefinition& ref_def, const void* object) const override;
	void* getInterface(const IReflectionDefinition& ref_def, void* object) const override;
	bool hasInterface(Gaff::Hash64 class_hash) const override;
	bool hasInterface(const IReflectionDefinition& ref_def) const override;

	const IReflection& getReflectionInstance(void) const override;

	int32_t size(void) const override;

	bool isBuiltIn(void) const override;

	int32_t getNumVars(void) const override;
	Shibboleth::HashStringView32<> getVarName(int32_t index) const override;
	IReflectionVar* getVar(int32_t index) const override;
	IReflectionVar* getVar(Gaff::Hash32 name) const override;

	int32_t getNumFuncs(void) const override;
	int32_t getNumFuncOverrides(int32_t index) const override;
	Shibboleth::HashStringView32<> getFuncName(int32_t index) const override;

	int32_t getNumStaticFuncs(void) const override;
	int32_t getNumStaticFuncOverrides(int32_t index) const override;
	Shibboleth::HashStringView32<> getStaticFuncName(int32_t) const override;

	int32_t getNumClassAttrs(void) const override;
	const IAttribute* getClassAttr(Gaff::Hash64 attr_name) const override;
	const IAttribute* getClassAttr(int32_t index) const override;
	bool hasClassAttr(Gaff::Hash64 attr_name) const override;
	void addClassAttr(IAttribute& attribute) override;

	int32_t getNumVarAttrs(Gaff::Hash32 name) const override;
	int32_t getNumVarAttrs(int32_t var_index) const override;
	const IAttribute* getVarAttr(Gaff::Hash32 name, Gaff::Hash64 attr_name) const override;
	const IAttribute* getVarAttr(Gaff::Hash32 name, int32_t index) const override;
	const IAttribute* getVarAttr(int32_t var_index, Gaff::Hash64 attr_name) const override;
	const IAttribute* getVarAttr(int32_t var_index, int32_t attr_index) const override;
	bool hasVarAttr(Gaff::Hash64 attr_name) const override;

	int32_t getNumFuncAttrs(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash) const override;
	const IAttribute* getFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, Gaff::Hash64 attr_name) const override;
	const IAttribute* getFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, int32_t index) const override;
	bool hasFuncAttr(Gaff::Hash64 attr_name) const override;

	int32_t getNumFuncAttrs(int32_t func_index, int32_t override_index) const override;
	const IAttribute* getFuncAttr(int32_t func_index, int32_t override_index, Gaff::Hash64 attr_name) const override;
	const IAttribute* getFuncAttr(int32_t func_index, int32_t override_index, int32_t attr_index) const override;

	int32_t getNumStaticFuncAttrs(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash) const override;
	const IAttribute* getStaticFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, Gaff::Hash64 attr_name) const override;
	const IAttribute* getStaticFuncAttr(Gaff::Hash32 name_hash, Gaff::Hash64 args_hash, int32_t index) const override;
	bool hasStaticFuncAttr(Gaff::Hash64 attr_name) const override;

	int32_t getNumStaticFuncAttrs(int32_t func_index, int32_t override_index) const override;
	const IAttribute* getStaticFuncAttr(int32_t func_index, int32_t override_index, Gaff::Hash64 attr_name) const override;
	const IAttribute* getStaticFuncAttr(int32_t func_index, int32_t override_index, int32_t attr_index) const override;

	int32_t getNumConstructors(void) const override;
	IReflectionStaticFunctionBase* getConstructor(int32_t index) const override;
	VoidFunc getConstructor(Gaff::Hash64 ctor_hash) const override;

	VoidFunc getFactory(Gaff::Hash64 ctor_hash) const override;

	IReflectionStaticFunctionBase* getStaticFunc(int32_t name_index, int32_t override_index) const override;
	IReflectionStaticFunctionBase* getStaticFunc(Gaff::Hash32 name, Gaff::Hash64 args) const override;
	IReflectionFunctionBase* getFunc(int32_t name_index, int32_t override_index) const override;
	IReflectionFunctionBase* getFunc(Gaff::Hash32 name, Gaff::Hash64 args) const override;

	void* duplicate(const void* object, Gaff::IAllocator& allocator) const override;

	void destroyInstance(void* data) const override;

	bool isCopyConstructible(void) const override;
	bool isCopyAssignable(void) const override;
	bool isConstructible(void) const override;
	bool isDestructible(void) const override;

	IVar<T>* getVarT(int32_t index) const;
	IVar<T>* getVarT(Gaff::Hash32 name) const;

	ptrdiff_t getBasePointerOffset(Gaff::Hash64 interface_name) const override;

private:
	ReflectionData<T> _data;

	void instantiated(void* object) const override;

	const IAttribute* getAttribute(const AttributeList& attributes, Gaff::Hash64 attr_name) const;

	ReflectionBuilder<T, T> getInitialBuilder(void);

	template <class RefT>
	friend class Reflection;
};



#define REF_DEF_BUILTIN(class_type, serialize_type) \
	template <> \
	class ReflectionDefinition<class_type> final : public IReflectionDefinition \
	{ \
	public: \
		constexpr static bool IsBuiltIn(void) { return true; } \
		const IReflection& getReflectionInstance(void) const override { return Reflection<class_type>::GetInstance();; } \
		int32_t size(void) const override { return static_cast<int32_t>(sizeof(class_type)); } \
		bool isBuiltIn(void) const override { return true; } \
		bool load(const Shibboleth::ISerializeReader& reader, void* object, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const override { return load(reader, *reinterpret_cast<class_type*>(object), flags); } \
		void save(Shibboleth::ISerializeWriter& writer, const void* object, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const override { save(writer, *reinterpret_cast<const class_type*>(object), flags); } \
		bool load(const Shibboleth::ISerializeReader& reader, class_type& out, Gaff::Flags<LoadFlags> flags = Gaff::Flags<LoadFlags>{}) const \
		{ \
			GAFF_REF(flags); \
			if (reader.isNull()) { \
				return true; \
			} \
			if (!reader.is##serialize_type()) { \
				return false; \
			} \
			out = reader.read##serialize_type(); \
			return true; \
		} \
		void save(Shibboleth::ISerializeWriter& writer, const class_type& value, Gaff::Flags<SaveFlags> flags = Gaff::Flags<SaveFlags>{}) const { GAFF_REF(flags); writer.write##serialize_type(value); } \
		Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const override { return Gaff::FNV1aHash64(reinterpret_cast<const char*>(object), sizeof(class_type), init); } \
		const void* getInterface(Gaff::Hash64, const void*) const override { return nullptr; } \
		void* getInterface(Gaff::Hash64, void*) const override { return nullptr; } \
		const void* getInterface(const IReflectionDefinition&, const void*) const override { return nullptr; } \
		void* getInterface(const IReflectionDefinition&, void*) const override { return nullptr; } \
		bool hasInterface(Gaff::Hash64) const override { return false; } \
		bool hasInterface(const IReflectionDefinition&) const override { return false; } \
		int32_t getNumVars(void) const override { return 0; } \
		Shibboleth::HashStringView32<> getVarName(int32_t) const override { return Shibboleth::HashStringView32<>(); } \
		IReflectionVar* getVar(int32_t) const override { return nullptr; } \
		IReflectionVar* getVar(Gaff::Hash32) const override { return nullptr; } \
		int32_t getNumFuncs(void) const override { return 0; } \
		int32_t getNumFuncOverrides(int32_t) const override { return 0; } \
		Shibboleth::HashStringView32<> getFuncName(int32_t) const override { return Shibboleth::HashStringView32<>(); } \
		int32_t getNumStaticFuncs(void) const override { return 0; } \
		Shibboleth::HashStringView32<> getStaticFuncName(int32_t) const override { return Shibboleth::HashStringView32<>(); } \
		int32_t getNumStaticFuncOverrides(int32_t) const override { return 0; } \
		int32_t getNumClassAttrs(void) const override { return 0; } \
		const IAttribute* getClassAttr(Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getClassAttr(int32_t) const override { return nullptr; } \
		bool hasClassAttr(Gaff::Hash64) const override { return false; } \
		void addClassAttr(IAttribute&) override {} \
		int32_t getNumVarAttrs(Gaff::Hash32) const override { return 0; } \
		int32_t getNumVarAttrs(int32_t) const override { return 0; } \
		const IAttribute* getVarAttr(Gaff::Hash32, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getVarAttr(Gaff::Hash32, int32_t) const override { return nullptr; } \
		const IAttribute* getVarAttr(int32_t, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getVarAttr(int32_t, int32_t) const override { return nullptr; } \
		bool hasVarAttr(Gaff::Hash64) const override { return false; } \
		int32_t getNumFuncAttrs(Gaff::Hash32, Gaff::Hash64) const override { return 0; } \
		const IAttribute* getFuncAttr(Gaff::Hash32, Gaff::Hash64, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getFuncAttr(Gaff::Hash32, Gaff::Hash64, int32_t) const override { return nullptr; } \
		int32_t getNumFuncAttrs(int32_t, int32_t) const override { return 0; } \
		const IAttribute* getFuncAttr(int32_t, int32_t, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getFuncAttr(int32_t, int32_t, int32_t) const override { return nullptr; } \
		bool hasFuncAttr(Gaff::Hash64) const override { return false; } \
		int32_t getNumStaticFuncAttrs(Gaff::Hash32, Gaff::Hash64) const override { return 0; } \
		const IAttribute* getStaticFuncAttr(Gaff::Hash32, Gaff::Hash64, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getStaticFuncAttr(Gaff::Hash32, Gaff::Hash64, int32_t) const override { return nullptr; } \
		int32_t getNumStaticFuncAttrs(int32_t, int32_t) const override { return 0; } \
		const IAttribute* getStaticFuncAttr(int32_t, int32_t, Gaff::Hash64) const override { return nullptr; } \
		const IAttribute* getStaticFuncAttr(int32_t, int32_t, int32_t) const override { return nullptr; } \
		bool hasStaticFuncAttr(Gaff::Hash64) const override { return false; } \
		int32_t getNumConstructors(void) const override { return 0; } \
		IReflectionStaticFunctionBase* getConstructor(int32_t) const override { return nullptr; } \
		VoidFunc getConstructor(Gaff::Hash64 ctor_hash) const override \
		{ \
			if (ctor_hash == Gaff::CalcTemplateHash<class_type>(Gaff::k_init_hash64)) { \
				ConstructFuncT<class_type, class_type> construct_func = ConstructFuncImpl<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} else if (ctor_hash == Gaff::CalcTemplateHash<>(Gaff::k_init_hash64)) { \
				ConstructFuncT<class_type> construct_func = ConstructFuncImpl<class_type>; \
				return reinterpret_cast<VoidFunc>(construct_func); \
			} \
			return nullptr; \
		} \
		VoidFunc getFactory(Gaff::Hash64 factory_hash) const override \
		{ \
			if (factory_hash == Gaff::CalcTemplateHash<class_type>(Gaff::k_init_hash64)) { \
				FactoryFuncT<class_type, class_type> factory_func = FactoryFuncImpl<class_type, class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} else if (factory_hash == Gaff::CalcTemplateHash<>(Gaff::k_init_hash64)) { \
				FactoryFuncT<class_type> factory_func = FactoryFuncImpl<class_type>; \
				return reinterpret_cast<VoidFunc>(factory_func); \
			} \
			return nullptr; \
		} \
		IReflectionStaticFunctionBase* getStaticFunc(int32_t, int32_t) const override { return nullptr; } \
		IReflectionStaticFunctionBase* getStaticFunc(Gaff::Hash32, Gaff::Hash64) const override { return nullptr; } \
		IReflectionFunctionBase* getFunc(int32_t, int32_t) const override { return nullptr; } \
		IReflectionFunctionBase* getFunc(Gaff::Hash32, Gaff::Hash64) const override { return nullptr; } \
		void* duplicate(const void*, Gaff::IAllocator&) const override { return nullptr; } \
		void destroyInstance(void* data) const override { class_type* const instance = reinterpret_cast<class_type*>(data); Gaff::Deconstruct(instance); } \
		bool isCopyConstructible(void) const override { return std::is_copy_constructible_v<class_type> && !std::is_trivially_copy_constructible_v<class_type>; } \
		bool isCopyAssignable(void) const override { return std::is_copy_assignable_v<class_type> && !std::is_trivially_copy_assignable_v<class_type>; } \
		bool isConstructible(void) const override { return std::is_default_constructible_v<class_type> && !std::is_trivially_default_constructible_v<class_type>; } \
		bool isDestructible(void) const override { return std::is_destructible_v<class_type> && !std::is_trivially_destructible_v<class_type>; } \
		ReflectionDefinition& getInitialBuilder(void) { return *this; } \
		void finish(void) {} \
		ptrdiff_t getBasePointerOffset(Gaff::Hash64) const override { return 0; } \
	private: \
		void instantiated(void*) const override {} \
	}

// I don't like this, but these are defined in Shibboleth_ReflectionDefinition.inl to avoid some compiler errors.
//REF_DEF_BUILTIN(int8_t, Int8);
//REF_DEF_BUILTIN(int16_t, Int16);
//REF_DEF_BUILTIN(int32_t, Int32);
//REF_DEF_BUILTIN(int64_t, Int64);
//REF_DEF_BUILTIN(uint8_t, UInt8);
//REF_DEF_BUILTIN(uint16_t, UInt16);
//REF_DEF_BUILTIN(uint32_t, UInt32);
//REF_DEF_BUILTIN(uint64_t, UInt64);
//REF_DEF_BUILTIN(float, Float);
//REF_DEF_BUILTIN(double, Double);
//REF_DEF_BUILTIN(bool, Bool);

NS_END

// I don't like this, but because we hard reference built-ins (eg Reflection<bool>) in these files, we have to include it later in Shibboleth_Reflection.h
//#include "Shibboleth_ReflectionDefinition.inl"
//#include "Shibboleth_ReflectionDefinitionFunction.inl"
