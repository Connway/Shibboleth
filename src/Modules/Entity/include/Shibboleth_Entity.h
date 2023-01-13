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

#include "Shibboleth_EntityComponent.h"
//#include <Shibboleth_ECSEntity.h>
//#include <Gaff_IncludeEASTLAtomic.h>
//#include <Gaff_RefPtr.h>
//#include <Gaff_Hash.h>

NS_SHIBBOLETH

class EntitySceneComponent;
class EntityManager;

class Entity : public IEntityUpdateable
{
public:
	template <class T>
	bool removeComponents(void)
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");
		return removeComponents(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool removeComponent(void)
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");
		return removeComponent(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	void addComponent(void)
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");
		addComponent(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool hasComponent(void) const
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");
		return has(Refl::Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	Vector<const T*> getComponents(void) const
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");
		return const_cast<Entity*>(this)->getComponents<T>();
	}

	template <class T>
	Vector<T*> getComponents(void)
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");

		Vector<EntityComponent*> comps = getComponents(Refl::Reflection<T>::GetReflectionDefinition());
		Vector<T*> cast_comps(comps.size());

		for (int32_t i = 0; i < static_cast<int32_t>(comps.size()); ++i) {
			cast_comps[i] = static_cast<T*>(comps[i]);
		}

		return cast_comps;
	}

	template <class T>
	const T* getComponent(void) const
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");
		return const_cast<Entity*>(this)->getComponent<T>();
	}

	template <class T>
	T* getComponent(void)
	{
		static_assert(std::is_base_of<EntityComponent, T>::value, "T must be derived from EntityComponent.");

		EntityComponent* const comp = getComponent(Refl::Reflection<T>::GetReflectionDefinition());
		return (comp) ? static_cast<T*>(comp) : nullptr;
	}

	Entity(EntityManager& entity_mgr);
	~Entity(void) = default;

	virtual bool init(void);
	void update(float dt) override;

	void addComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs);
	void addComponent(const Refl::IReflectionDefinition& ref_def);
	void addComponent(const Vector<EntityComponent*>& components);
	void addComponent(EntityComponent& component);

	int32_t removeComponent(const Vector<const Refl::IReflectionDefinition*>& ref_defs);
	bool removeComponent(const Refl::IReflectionDefinition& ref_def);
	void removeComponent(int32_t index);

	int32_t removeComponents(const Vector<const Refl::IReflectionDefinition*>& ref_defs);
	int32_t removeComponents(const Refl::IReflectionDefinition& ref_def);

	Vector<const EntityComponent*> getComponents(const Refl::IReflectionDefinition& ref_def) const;
	Vector<EntityComponent*> getComponents(const Refl::IReflectionDefinition& ref_def);

	const EntityComponent* getComponent(const Refl::IReflectionDefinition& ref_def) const;
	EntityComponent* getComponent(const Refl::IReflectionDefinition& ref_def);
	const EntityComponent& getComponent(int32_t index) const;
	EntityComponent& getComponent(int32_t index);

	bool hasComponent(const Refl::IReflectionDefinition& ref_def) const;
	int32_t getNumComponents(void) const;

	void updateAfter(IEntityUpdateable& after);

	void setEnableUpdate(bool enabled);
	bool canUpdate(void) const;

	const U8String& getName(void) const;
	void setName(const U8String& name);
	void setName(U8String&& name);

protected:
	EntityManager& _entity_mgr;

private:
	enum class Flag
	{
		UpdateEnabled,

		Count
	};

	Vector< UniquePtr<EntityComponent> > _components;
	EntitySceneComponent* _root_scene_comp = nullptr;

	U8String _name;

	Gaff::Flags<Flag> _flags;

	friend class EntityComponent;
	friend class EntityManager;

	SHIB_REFLECTION_CLASS_DECLARE(Entity);

	GAFF_NO_MOVE(Entity);
	GAFF_NO_COPY(Entity);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::Entity)
