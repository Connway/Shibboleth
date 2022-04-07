/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include <Shibboleth_Reflection.h>
//#include <Shibboleth_ECSEntity.h>
//#include <Gaff_IncludeEASTLAtomic.h>
//#include <Gaff_RefPtr.h>
//#include <Gaff_Hash.h>

NS_SHIBBOLETH

class Entity;

class IEntityComponent : public Refl::IReflectionObject
{
	GAFF_NO_COPY(IEntityComponent);

public:
	virtual ~IEntityComponent(void) {}

	virtual void init(void) {}
	virtual void destroy(void) {}

	Entity* getOwner(void) const { return _owner; }

private:
	Entity* _owner = nullptr;

	friend class Entity;
};

class Entity : public Refl::IReflectionObject
{
	GAFF_NO_COPY(Entity);

public:
	template <class T>
	bool removeComponents(void)
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");
		return removeComponents(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool removeComponent(void)
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");
		return removeComponent(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	void addComponent(void)
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");
		addComponent(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool hasComponent(void) const
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");
		return has(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	Vector<const T*> getComponents(void) const
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");
		return const_cast<Entity*>(this)->getComponents<T>();
	}

	template <class T>
	Vector<T*> getComponents(void)
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");

		Vector<IEntityComponent*> comps = getComponents(Refl::Reflection<T>::GetReflectionDefinition());
		Vector<T*> cast_comps(comps.size());

		for (int32_t i = 0; i < static_cast<int32_t>(comps.size()); ++i) {
			cast_comps[i] = static_cast<T*>(comps[i]);
		}

		return cast_comps;
	}

	template <class T>
	const T* getComponent(void) const
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");
		return const_cast<Entity*>(this)->getComponent<T>();
	}

	template <class T>
	T* getComponent(void)
	{
		static_assert(std::is_base_of<IEntityComponent, T>::value, "T must be derived from IEntityComponent.");

		IEntityComponent* const comp = getComponent(Refl::Reflection<T>::GetReflectionDefinition());
		return (comp) ? static_cast<T*>(comp) : nullptr;
	}

	Entity(void) = default;
	~Entity(void) = default;

	void addComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs);
	void addComponent(const Refl::IReflectionDefinition& ref_def);
	void addComponent(const Vector<IEntityComponent*>& components);
	void addComponent(IEntityComponent& component);

	int32_t removeComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs);
	bool removeComponent(const Refl::IReflectionDefinition& ref_def);
	void removeComponent(int32_t index);

	int32_t removeComponents(const Vector<const Refl::IReflectionDefinition*>& ref_defs);
	int32_t removeComponents(const Refl::IReflectionDefinition& ref_def);

	Vector<const IEntityComponent*> getComponents(const Refl::IReflectionDefinition& ref_def) const;
	Vector<IEntityComponent*> getComponents(const Refl::IReflectionDefinition& ref_def);

	const IEntityComponent* getComponent(const Refl::IReflectionDefinition& ref_def) const;
	IEntityComponent* getComponent(const Refl::IReflectionDefinition& ref_def);
	const IEntityComponent& getComponent(int32_t index) const;
	IEntityComponent& getComponent(int32_t index);

	bool hasComponent(const Refl::IReflectionDefinition& ref_def) const;

	int32_t getNumComponents(void) const;

private:
	Vector< UniquePtr<IEntityComponent> > _components;

	SHIB_REFLECTION_CLASS_DECLARE(Entity);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::Entity)
