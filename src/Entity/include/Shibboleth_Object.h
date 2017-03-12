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

#include <Gaff_JSON.h>

NS_SHIBBOLETH

class IComponentManager;
class IObjectManager;
class IFileSystem;
class IApp;

class Object
{
public:
	template <class Interface>
	const Interface* getFirstComponentWithInterface(void) const
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			const Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				return interface;
			}
		}

		return nullptr;
	}

	template <class Interface>
	Interface* getFirstComponentWithInterface(void)
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				return interface;
			}
		}

		return nullptr;
	}

	template <class Interface>
	void getComponentsWithInterface(Vector<const Interface*>& components)
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			const Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				components.emplacePush(interface);
			}
		}
	}

	template <class Interface>
	void getComponentsWithInterface(Vector<Interface*>& components)
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				components.emplacePush(interface);
			}
		}
	}

	using DirtyCallback = Gaff::FunctionBinder<void, Object*, uint64_t>;

	Object(uint32_t id);
	~Object(void);

	bool init(const Gaff::JSON& json);
	bool init(IFileSystem* fs, const char* file_name);
	void destroy(void);

	const HashString64& getName(void) const;

	uint32_t getID(void) const;
	void setID(uint32_t id);

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
	const Vector<Component*>& getComponents(void) const;
	Vector<Component*>& getComponents(void);

	//void addComponent(Component* component);
	//void removeComponent(Component* component);

	const void* getFirstComponentWithInterface(Gaff::Hash64 class_id) const;
	void* getFirstComponentWithInterface(Gaff::Hash64 class_id);

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
	Gaff::SpinLock _children_lock;
	Object* _parent;

	HashString64 _name;

	Vector<Component*> _components;
	//IComponentManager& _comp_mgr;
	//IObjectManager& _obj_mgr;

	uint32_t _id;

	uint8_t _flags;

	bool createComponents(const Gaff::JSON& json);
	void markDirty(void);

	GAFF_NO_COPY(Object);
	GAFF_NO_MOVE(Object);
};

NS_END
