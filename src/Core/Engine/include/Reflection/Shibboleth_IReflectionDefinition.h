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

#include "Shibboleth_IReflectionFunction.h"
#include "Containers/Shibboleth_Vector.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_IAttribute.h"
#include <Gaff_Hashable.h>
#include <Gaff_Hash.h>

NS_SHIBBOLETH
	class ISerializeReader;
	class ISerializeWriter;
NS_END

NS_GAFF
	class IAllocator;
NS_END

NS_REFLECTION

class IReflectionVar;
class IReflection;
class IAttribute;

class IReflectionDefinition
{
public:
	template <class T, class... Args>
	using ConstructFuncT = void (*)(T*, Args&&...);

	template <class... Args>
	using ConstructFunc = void (*)(void*, Args&&...);

	template <class T, class... Args>
	using FactoryFuncT = T* (*)(Gaff::IAllocator&, Args&&...);

	template <class... Args>
	using FactoryFunc = void* (*)(Gaff::IAllocator&, Args&&...);

	template <class Ret, class... Args>
	using TemplateFunc = Ret (*)(Args...);

	using VoidFunc = void (*)(void);

	template <class T>
	ptrdiff_t getBasePointerOffset(void) const
	{
		return getBasePointerOffset(Hash::template ClassHashable<T>::GetName());
	}

	const void* getBasePointer(Gaff::Hash64 interface_name, const void* object) const
	{
		return const_cast<IReflectionDefinition*>(this)->getBasePointer(interface_name, const_cast<void*>(object));
	}

	void* getBasePointer(Gaff::Hash64 interface_name, void* object) const
	{
		const ptrdiff_t offset = getBasePointerOffset(interface_name);
		return reinterpret_cast<int8_t*>(object) - offset;
	}

	template <class T>
	const void* getBasePointer(const T* object) const
	{
		return getBasePointer(Hash::template ClassHashable<T>::GetHash(), object);
	}

	template <class T>
	void* getBasePointer(T* object) const
	{
		return getBasePointer(Hash::template ClassHashable<T>::GetHash(), object);
	}

	template <class T>
	const T* getInterface(const void* object) const
	{
		return reinterpret_cast<const T*>(getInterface(Hash::template ClassHashable<T>::GetHash(), object));
	}

	template <class T>
	T* getInterface(void* object) const
	{
		return reinterpret_cast<T*>(getInterface(Hash::template ClassHashable<T>::GetHash(), object));
	}

	template <class T>
	const T* getInterface(Gaff::Hash64 interface_hash, const void* object) const
	{
		return reinterpret_cast<const T*>(getInterface(interface_hash, object));
	}

	template <class T>
	T* getInterface(Gaff::Hash64 interface_hash, void* object) const
	{
		return reinterpret_cast<T*>(getInterface(interface_hash, object));
	}

	template <class T>
	bool hasInterface(void) const
	{
		return hasInterface(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T, class... Args>
	T* createT(Gaff::Hash64 interface_hash, Gaff::Hash64 factory_hash, Gaff::IAllocator& allocator, Args&&... args) const
	{
		GAFF_ASSERT(hasInterface(interface_hash));
		void* const data = create(factory_hash, allocator, std::forward<Args>(args)...);

		if (data) {
			return reinterpret_cast<T*>(getInterface(interface_hash, data));
		}

		return nullptr;
	}

	template <class T, class... Args>
	T* createT(Gaff::Hash64 interface_hash, Gaff::IAllocator& allocator, Args&&... args) const
	{
		constexpr Gaff::Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
		return createT<T>(interface_hash, ctor_hash, allocator, std::forward<Args>(args)...);
	}

	template <class T, class... Args>
	T* createT(Gaff::IAllocator& allocator, Args&&... args) const
	{
		constexpr Gaff::Hash64 interface_hash = Hash::template ClassHashable<T>::GetHash();
		constexpr Gaff::Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
		return createT<T>(interface_hash, ctor_hash, allocator, std::forward<Args>(args)...);
	}

	template <class... Args>
	void* create(Gaff::Hash64 factory_hash, Gaff::IAllocator& allocator, Args&&... args) const
	{
		FactoryFunc<Args...> factory_func = reinterpret_cast< FactoryFunc<Args...> >(getFactory(factory_hash));

		if (factory_func) {
			void* const object = factory_func(allocator, std::forward<Args>(args)...);
			instantiated(object);
			return object;
		}

		return nullptr;
	}

	template <class... Args>
	void* create(Gaff::IAllocator& allocator, Args&&... args) const
	{
		constexpr Gaff::Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
		return create(ctor_hash, allocator, std::forward<Args>(args)...);
	}

	template <class T>
	const T* getClassAttr(Gaff::Hash64 attr_name) const
	{
		const auto* const attr = getClassAttr(attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getVarAttr(Gaff::Hash32 var_name, Gaff::Hash64 attr_name) const
	{
		const auto* const attr = getVarAttr(var_name, attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const
	{
		const auto* const attr = getFuncAttr(name_arg_hash, attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getStaticFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const
	{
		const auto* const attr = getStaticFuncAttr(name_arg_hash, attr_name);
		return (attr) ? static_cast<const T*>(attr) : nullptr;
	}

	template <class T>
	const T* getClassAttr(void) const
	{
		return getClassAttr<T>(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	const T* getVarAttr(Gaff::Hash32 name) const
	{
		return getVarAttr<T>(name, Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	const T* getFuncAttr(Gaff::Hash32 name) const
	{
		return getFuncAttr<T>(name, Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	const T* getStaticFuncAttr(Gaff::Hash32 name) const
	{
		return getStaticFuncAttr<T>(name, Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	bool hasClassAttr(void) const
	{
		return hasClassAttr(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	bool hasVarAttr(void) const
	{
		return hasVarAttr(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	bool hasFuncAttr(void) const
	{
		return hasFuncAttr(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	bool hasStaticFuncAttr(void) const
	{
		return hasStaticFuncAttr(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	eastl::pair<Shibboleth::HashStringView32<>, const T*> getVarAttr(void) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Shibboleth::HashStringView32<> name = getVarName(j);
			const int32_t size = getNumVarAttrs(name.getHash());

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name.getHash(), i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					return eastl::make_pair(name, attr);
				}
			}
		}

		return eastl::pair<Gaff::Hash32, const T*>(0, nullptr);
	}

	template <class T>
	eastl::pair<Shibboleth::HashStringView32<>, const T*> getVarAttr(Gaff::Hash64 attr_name) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Shibboleth::HashStringView32<> name = getVarName(j);
			const int32_t size = getNumVarAttrs(name.getHash());

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name.getHash(), i);
				const void* attr = attribute->getReflectionDefinition().getInterface(
					attr_name, attribute->getBasePointer()
				);

				if (attr) {
					return eastl::make_pair(name, reinterpret_cast<const T*>(attr));
				}
			}
		}

		return eastl::pair<Shibboleth::HashStringView32<>, const T*>(Shibboleth::HashStringView32<>(), nullptr);
	}

	template <class T, class Ret, class... Args>
	eastl::pair<Shibboleth::HashStringView32<>, const T*> getFuncAttr(void) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const int32_t num_funcs = getNumFuncs();

		for (int32_t j = 0; j < num_funcs; ++j) {
			const Shibboleth::HashStringView32<> name = getFuncName(j);
			const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));
			const int32_t size = getNumFuncAttrs(func_hash);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getFuncAttr(func_hash, i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					return eastl::make_pair(name, attr);
				}
			}
		}

		return eastl::pair<Shibboleth::HashStringView32<>, const T*>(Shibboleth::HashStringView32<>(), nullptr);
	}

	template <class T, class Ret, class... Args>
	eastl::pair<Shibboleth::HashStringView32<>, const T*> getFuncAttr(Gaff::Hash64 attr_name) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const int32_t num_funcs = getNumFuncs();

		for (int32_t j = 0; j < num_funcs; ++j) {
			const Shibboleth::HashStringView32<> name = getFuncName(j);
			const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));
			const int32_t size = getNumFuncAttrs(func_hash);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getFuncAttr(func_hash, i);
				const void* attr = attribute->getReflectionDefinition().getInterface(
					attr_name, attribute->getBasePointer()
				);

				if (attr) {
					return eastl::make_pair(name, reinterpret_cast<const T*>(attr));
				}
			}
		}

		return eastl::pair<Shibboleth::HashStringView32<>, const T*>(Shibboleth::HashStringView32<>(), nullptr);
	}

	template <class T, class Ret, class... Args>
	eastl::pair<Shibboleth::HashStringView32<>, const T*> getStaticFuncAttr(void) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const int32_t num_funcs = getNumStaticFuncs();

		for (int32_t j = 0; j < num_funcs; ++j) {
			const Shibboleth::HashStringView32<> name = getStaticFuncName(j);
			const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));
			const int32_t size = getNumStaticFuncAttrs(func_hash);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getStaticFuncAttr(func_hash, i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					return eastl::make_pair(name, attr);
				}
			}
		}

		return eastl::pair<Shibboleth::HashStringView32<>, const T*>(Shibboleth::HashStringView32<>(), nullptr);
	}

	template <class T, class Ret, class... Args>
	eastl::pair<Shibboleth::HashStringView32<>, const T*> getStaticFuncAttr(Gaff::Hash64 attr_name) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const int32_t num_funcs = getNumStaticFuncs();

		for (int32_t j = 0; j < num_funcs; ++j) {
			const Shibboleth::HashStringView32<> name = getStaticFuncName(j);
			const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));
			const int32_t size = getNumStaticFuncAttrs(func_hash);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getStaticFuncAttr(func_hash, i);
				const void* attr = attribute->getReflectionDefinition().getInterface(
					attr_name, attribute->getBasePointer()
				);

				if (attr) {
					return eastl::make_pair(name, reinterpret_cast<const T*>(attr));
				}
			}
		}

		return eastl::pair<Shibboleth::HashStringView32<>, const T*>(Shibboleth::HashStringView32<>(), nullptr);
	}

	template <class T>
	void getClassAttrs(Shibboleth::Vector<const T*>& out) const
	{
		for (int32_t i = 0; i < getNumClassAttrs(); ++i) {
			const IAttribute* const attribute = getClassAttr(i);
			const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T>
	void getClassAttrs(Gaff::Hash64 attr_name, Shibboleth::Vector<const T*>& out) const
	{
		for (int32_t i = 0; i < getNumClassAttrs(); ++i) {
			const IAttribute* const attribute = getClassAttr(i);
			const void* attr = attribute->getReflectionDefinition().getInterface(
				attr_name, attribute->getBasePointer()
			);

			if (attr) {
				out.emplace_back(reinterpret_cast<const T*>(attr));
			}
		}
	}

	template <class T>
	Shibboleth::Vector<const T*> getClassAttrs(Gaff::Hash64 attr_name) const
	{
		Shibboleth::Vector<const T*> out;
		getClassAttrs(attr_name, out);

		return out;
	}

	template <class T>
	void getVarAttrs(Gaff::Hash32 name, Shibboleth::Vector<const T*>& out) const
	{
		const int32_t size = getNumVarAttrs(name);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getVarAttr(name, i);
			const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class T>
	void getVarAttrs(Shibboleth::Vector< eastl::pair<Shibboleth::HashStringView32<>, const T*> >& out) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Shibboleth::HashStringView32<> name = getVarName(j);
			const int32_t size = getNumVarAttrs(name.getHash());

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name.getHash(), i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					out.emplace_back(eastl::make_pair(name, attr));
				}
			}
		}
	}

	template <class T>
	void getVarAttrs(Shibboleth::Vector< eastl::pair<int32_t, const T*> >& out) const
	{
		const int32_t num_vars = getNumVars();

		for (int32_t j = 0; j < num_vars; ++j) {
			const Shibboleth::HashStringView32<> name = getVarName(j);
			const int32_t size = getNumVarAttrs(name.getHash());

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getVarAttr(name.getHash(), i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					out.emplace_back(eastl::make_pair(j, attr));
				}
			}
		}
	}

	template <class Ret, class... Args, class T>
	void getFuncAttrs(Gaff::Hash32 name, Shibboleth::Vector<const T*>& out) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));
		const int32_t size = getNumFuncAttrs(func_hash);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getFuncAttr(func_hash, i);
			const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class Ret, class... Args, class T>
	void getFuncAttrs(Shibboleth::Vector< eastl::pair<Shibboleth::HashStringView32<>, const T*> >& out) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const int32_t num_funcs = getNumFuncs();

		for (int32_t j = 0; j < num_funcs; ++j) {
			const Shibboleth::HashStringView32<> name = getFuncName(j);
			const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));

			const int32_t size = getNumFuncAttrs(func_hash);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getFuncAttr(func_hash, i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					out.emplace_back(eastl::make_pair(name, attr));
				}
			}
		}
	}

	template <class Ret, class... Args, class T>
	void getStaticFuncAttrs(Gaff::Hash32 name, Shibboleth::Vector<const T*>& out) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name));
		const int32_t size = getNumStaticFuncAttrs(func_hash);

		for (int32_t i = 0; i < size; ++i) {
			const IAttribute* const attribute = getStaticFuncAttr(func_hash, i);
			const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

			if (attr) {
				out.emplace_back(attr);
			}
		}
	}

	template <class Ret, class... Args, class T>
	void getStaticFuncAttrs(Shibboleth::Vector< eastl::pair<Shibboleth::HashStringView32<>, const T*> >& out) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		const int32_t num_funcs = getNumStaticFuncs();

		for (int32_t j = 0; j < num_funcs; ++j) {
			const Shibboleth::HashStringView32<> name = getStaticFuncName(j);
			const Gaff::Hash64 func_hash = Gaff::FNV1aHash64T(arg_hash, Gaff::FNV1aHash64T(name.getHash()));
			const int32_t size = getNumStaticFuncAttrs(func_hash);

			for (int32_t i = 0; i < size; ++i) {
				const IAttribute* const attribute = getStaticFuncAttr(func_hash, i);
				const T* const attr = attribute->getReflectionDefinition().template getInterface<T>(attribute->getBasePointer());

				if (attr) {
					out.emplace_back(eastl::make_pair(name, attr));
				}
			}
		}
	}

	template <class... Args>
	ConstructFunc<Args...> getConstructor(void) const
	{
		constexpr Gaff::Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
		return reinterpret_cast< ConstructFunc<Args...> >(getConstructor(ctor_hash));
	}

	template <class... Args>
	FactoryFunc<Args...> getFactory(void) const
	{
		constexpr Gaff::Hash64 ctor_hash = Gaff::CalcTemplateHash<Args...>(Gaff::k_init_hash64);
		return reinterpret_cast< FactoryFunc<Args...> >(getFactory(ctor_hash));
	}

	template <class Ret, class... Args>
	IReflectionFunction<Ret, Args...>* getFunc(Gaff::Hash32 name) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		IReflectionFunctionBase* const functor = getFunc(name, arg_hash);

		if (functor) {
			return static_cast<IReflectionFunction<Ret, Args...>*>(functor);
		}

		return nullptr;
	}

	template <class Ret, class... Args>
	IReflectionStaticFunction<Ret, Args...>* getStaticFunc(Gaff::Hash32 name) const
	{
		constexpr Gaff::Hash64 arg_hash = Gaff::CalcTemplateHash<Ret, Args...>(Gaff::k_init_hash64);
		IReflectionStaticFunctionBase* const func = getStaticFunc(name, arg_hash);

		if (func) {
			return static_cast<IReflectionStaticFunction<Ret, Args...>*>(func);
		}

		return nullptr;
	}

	template <class T>
	T* duplicateT(const void* object, Gaff::IAllocator& allocator) const
	{
		void* const copy = duplicate(object, allocator);
		return (copy) ? getInterface<T>(copy) : nullptr;
	}

	virtual ~IReflectionDefinition(void) {}

	virtual const IReflection& getReflectionInstance(void) const = 0;
	virtual int32_t size(void) const = 0;

	virtual bool isPolymorphic(void) const = 0;
	virtual bool isBuiltIn(void) const = 0;

	virtual const char8_t* getFriendlyName(void) const = 0;

	virtual bool load(const Shibboleth::ISerializeReader& reader, void* object, bool refl_load = false) const = 0;
	virtual void save(Shibboleth::ISerializeWriter& writer, const void* object, bool refl_save = false) const = 0;

	virtual Gaff::Hash64 getInstanceHash(const void* object, Gaff::Hash64 init = Gaff::k_init_hash64) const = 0;

	virtual const void* getInterface(Gaff::Hash64 class_id, const void* object) const = 0;
	virtual void* getInterface(Gaff::Hash64 class_id, void* object) const = 0;
	virtual bool hasInterface(Gaff::Hash64 class_hash) const = 0;

	virtual Shibboleth::ProxyAllocator& getAllocator(void) = 0;

	virtual int32_t getNumVars(void) const = 0;
	virtual Shibboleth::HashStringView32<> getVarName(int32_t index) const = 0;
	virtual IReflectionVar* getVar(int32_t index) const = 0;
	virtual IReflectionVar* getVar(Gaff::Hash32 name) const = 0;

	virtual int32_t getNumFuncs(void) const = 0;
	virtual int32_t getNumFuncOverrides(int32_t index) const = 0;
	virtual Shibboleth::HashStringView32<> getFuncName(int32_t index) const = 0;
	virtual int32_t getFuncIndex(Gaff::Hash32 name) const = 0;

	virtual int32_t getNumStaticFuncs(void) const = 0;
	virtual int32_t getNumStaticFuncOverrides(int32_t index) const = 0;
	virtual Shibboleth::HashStringView32<> getStaticFuncName(int32_t) const = 0;
	virtual int32_t getStaticFuncIndex(Gaff::Hash32 name) const = 0;

	virtual int32_t getNumClassAttrs(void) const = 0;
	virtual const IAttribute* getClassAttr(Gaff::Hash64 attr_name) const = 0;
	virtual const IAttribute* getClassAttr(int32_t index) const = 0;
	virtual void addClassAttr(IAttribute& attribute) = 0;
	virtual bool hasClassAttr(Gaff::Hash64 attr_name) const = 0;

	virtual int32_t getNumVarAttrs(Gaff::Hash32 name) const = 0;
	virtual const IAttribute* getVarAttr(Gaff::Hash32 name, Gaff::Hash64 attr_name) const = 0;
	virtual const IAttribute* getVarAttr(Gaff::Hash32 name, int32_t index) const = 0;
	virtual bool hasVarAttr(Gaff::Hash64 attr_name) const = 0;

	virtual int32_t getNumFuncAttrs(Gaff::Hash64 name_arg_hash) const = 0;
	virtual const IAttribute* getFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const = 0;
	virtual const IAttribute* getFuncAttr(Gaff::Hash64 name_arg_hash, int32_t index) const = 0;
	virtual bool hasFuncAttr(Gaff::Hash64 attr_name) const = 0;

	virtual int32_t getNumStaticFuncAttrs(Gaff::Hash64 name_arg_hash) const = 0;
	virtual const IAttribute* getStaticFuncAttr(Gaff::Hash64 name_arg_hash, Gaff::Hash64 attr_name) const = 0;
	virtual const IAttribute* getStaticFuncAttr(Gaff::Hash64 name_arg_hash, int32_t index) const = 0;
	virtual bool hasStaticFuncAttr(Gaff::Hash64 attr_name) const = 0;

	virtual int32_t getNumConstructors(void) const = 0;
	virtual IReflectionStaticFunctionBase* getConstructor(int32_t index) const = 0;
	virtual VoidFunc getConstructor(Gaff::Hash64 ctor_hash) const = 0;

	virtual VoidFunc getFactory(Gaff::Hash64 ctor_hash) const = 0;

	virtual IReflectionStaticFunctionBase* getStaticFunc(int32_t name_index, int32_t override_index) const = 0;
	virtual IReflectionStaticFunctionBase* getStaticFunc(Gaff::Hash32 name, Gaff::Hash64 args) const = 0;
	virtual IReflectionFunctionBase* getFunc(int32_t name_index, int32_t override_index) const = 0;
	virtual IReflectionFunctionBase* getFunc(Gaff::Hash32 name, Gaff::Hash64 args) const = 0;

	virtual void* duplicate(const void* object, Gaff::IAllocator& allocator) const = 0;

	virtual void destroyInstance(void* data) const = 0;

private:
	virtual ptrdiff_t getBasePointerOffset(Gaff::Hash64 interface_name) const = 0;
	virtual void instantiated(void* object) const = 0;
};

NS_END
