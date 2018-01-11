/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_Component.h"
#include <Shibboleth_HashString.h>
#include <Shibboleth_Vector.h>
#include <Gleam_Transform.h>
#include <Gleam_AABB.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Function.h>

NS_GAFF
	class ISerializeReader;
	class ISerializeWriter;
NS_END

NS_SHIBBOLETH

class IComponentManager;
class IObjectManager;
class IFileSystem;
class IApp;

class Object final
{
public:
	template <class T>
	const T* getComponent(void) const
	{
		const void* const interface = getComponent(Reflection<T>::GetHash());
		return reinterpret_cast<const T*>(interface);
	}

	template <class T>
	T* getComponent(void)
	{
		void* const interface = getComponent(Reflection<T>::GetHash());
		return reinterpret_cast<T*>(interface);
	}

	template <class T>
	Vector<const T*> getComponents(void) const
	{
		Vector<const T*> components;
		getComponents(components);
		return components;
	}

	template <class T>
	Vector<T*> getComponents(void)
	{
		Vector<T*> components;
		getComponents(components);
		return components;
	}

	template <class T>
	Vector<const T*>& getComponents(Vector<const T*>& components) const
	{
		for (const Component* const component : _components) {
			const T* const interface = Gaff::ReflectionCast<T>(component);

			if (interface) {
				components.push_back(interface);
			}
		}

		return components;
	}

	template <class T>
	Vector<T*>& getComponents(Vector<T*>& components)
	{
		for (const Component* const component : _components) {
			T* const interface = ReflectionCast<T>(*it);

			if (interface) {
				components.push_back(interface);
			}
		}

		return components;
	}

	template <class T, class... Args>
	T* addComponent(Args&&... args)
	{
		T* const component = Reflection<T>::GetReflectionDefinition().create(std::forward<Args>(args)...);

		if (!component) {
			// TODO: log error
			return nullptr;
		}

		addComponent(component);
		return component;
	}

	using DirtyCallback = Gaff::FunctionBinder<void, Object*, uint64_t>;

	Object(int32_t id);
	~Object(void);

	bool load(const Gaff::ISerializeReader& reader);
	bool save(Gaff::ISerializeWriter& writer);

	void destroy(void);

	const HashString64& getName(void) const;

	int32_t getID(void) const;
	void setID(int32_t id);

	const Gleam::Transform& getLocalTransform(void) const;
	const Gleam::Transform& getWorldTransform(void) const;
	void setLocalTransform(const Gleam::Transform& transform);
	void setWorldTransform(const Gleam::Transform& transform);
	const glm::quat& getLocalRotation(void) const;
	const glm::quat& getWorldRotation(void) const;
	void setLocalRotation(const glm::quat& rot);
	void setWorldRotation(const glm::quat& rot);
	const glm::vec3& getLocalPosition(void) const;
	const glm::vec3& getWorldPosition(void) const;
	void setLocalPosition(const glm::vec3& pos);
	void setWorldPosition(const glm::vec3& pos);
	const glm::vec3& getLocalScale(void) const;
	const glm::vec3& getWorldScale(void) const;
	void setLocalScale(const glm::vec3& scale);
	void setWorldScale(const glm::vec3& scale);
	const Gleam::AABB& getLocalAABB(void) const;
	const Gleam::AABB& getWorldAABB(void) const;
	void setLocalAABB(const Gleam::AABB& aabb);

	int32_t getNumComponents(void) const;
	const Component* getComponent(int32_t index) const;
	Component* getComponent(int32_t index);
	const void* getComponent(Gaff::Hash64 class_id) const;
	void* getComponent(Gaff::Hash64 class_id);

	Vector<const void*> getComponents(Gaff::Hash64 class_id) const;
	Vector<void*> getComponents(Gaff::Hash64 class_id);
	Vector<const void*>& getComponents(Gaff::Hash64 class_id, Vector<const void*>& components) const;
	Vector<void*>& getComponents(Gaff::Hash64 class_id, Vector<void*>& components);
	const Vector<Component*>& getComponents(void) const;
	Vector<Component*>& getComponents(void);

	void addComponent(Component* component);
	void removeComponent(Component* component, bool destroy = false);

	void addChild(Object* object);
	void removeFromParent(void);
	void removeChildren(void);
	void updateTransforms(void);

	void registerForLocalDirtyCallback(const DirtyCallback& callback, uint64_t user_data = 0);
	void unregisterForLocalDirtyCallback(const DirtyCallback& callback);
	void notifyLocalDirtyCallbacks(void);

	void registerForWorldDirtyCallback(const DirtyCallback& callback, uint64_t user_data = 0);
	void unregisterForWorldDirtyCallback(const DirtyCallback& callback);
	void notifyWorldDirtyCallbacks(void);

	bool isDirty(void) const;
	void clearDirty(void);

	bool isInWorld(void) const;
	void addToWorld(void);
	void removeFromWorld(void);

private:
	Gleam::Transform _local_transform;
	Gleam::Transform _world_transform;
	Gleam::AABB _local_aabb;
	Gleam::AABB _world_aabb;

	Vector< eastl::pair<DirtyCallback, uint64_t> > _local_callbacks;
	Vector< eastl::pair<DirtyCallback, uint64_t> > _world_callbacks;
	Gaff::SpinLock _local_cb_lock;
	Gaff::SpinLock _world_cb_lock;

	Vector<Object*> _children;
	//Gaff::SpinLock _children_lock;
	Object* _parent;

	HashString64 _name;

	Vector<Component*> _components;
	//IObjectManager& _obj_mgr;

	int32_t _id;

	int32_t _flags;

	bool createComponents(const Gaff::ISerializeReader& reader);
	void markDirty(void);

	GAFF_NO_COPY(Object);
	GAFF_NO_MOVE(Object);
};

NS_END
