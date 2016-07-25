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
#include <Shibboleth_Array.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Function.h>
#include <Gleam_Transform_CPU.h>
#include <Gleam_AABB_CPU.h>

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
	void getComponentsWithInterface(Array<const Interface*>& components)
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			const Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				components.emplacePush(interface);
			}
		}
	}

	template <class Interface>
	void getComponentsWithInterface(Array<Interface*>& components)
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				components.emplacePush(interface);
			}
		}
	}

	using DirtyCallback = Gaff::FunctionBinder<void, Object*, uint64_t>;

	Object(unsigned int id);
	~Object(void);

	bool init(const Gaff::JSON& json);
	INLINE bool init(IFileSystem* fs, const char* file_name);
	void destroy(void);

	uint32_t getNameHash(void) const;
	const char* getName(void) const;

	unsigned int getID(void) const;
	void setID(unsigned int id);

	INLINE const Gleam::TransformCPU& getLocalTransform(void) const;
	INLINE const Gleam::TransformCPU& getWorldTransform(void) const;
	INLINE void setLocalTransform(const Gleam::TransformCPU& transform);
	void setWorldTransform(const Gleam::TransformCPU& transform);
	INLINE const Gleam::QuaternionCPU& getLocalRotation(void) const;
	INLINE const Gleam::QuaternionCPU& getWorldRotation(void) const;
	INLINE void setLocalRotation(const Gleam::QuaternionCPU& rot);
	void setWorldRotation(const Gleam::QuaternionCPU& rot);
	INLINE const Gleam::Vector4CPU& getLocalPosition(void) const;
	INLINE const Gleam::Vector4CPU& getWorldPosition(void) const;
	INLINE void setLocalPosition(const Gleam::Vector4CPU& pos);
	void setWorldPosition(const Gleam::Vector4CPU& pos);
	INLINE const Gleam::Vector4CPU& getLocalScale(void) const;
	INLINE const Gleam::Vector4CPU& getWorldScale(void) const;
	INLINE void setLocalScale(const Gleam::Vector4CPU& scale);
	void setWorldScale(const Gleam::Vector4CPU& scale);
	INLINE const Gleam::AABBCPU& getLocalAABB(void) const;
	INLINE const Gleam::AABBCPU& getWorldAABB(void) const;
	INLINE void setLocalAABB(const Gleam::AABBCPU& aabb);

	INLINE size_t getNumComponents(void) const;
	INLINE const Component* getComponent(unsigned int index) const;
	INLINE Component* getComponent(unsigned int index);
	INLINE const Array<Component*>& getComponents(void) const;
	INLINE Array<Component*>& getComponents(void);

	const void* getFirstComponentWithInterface(Gaff::ReflectionHash class_id) const;
	void* getFirstComponentWithInterface(Gaff::ReflectionHash class_id);

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

	INLINE bool isDirty(void) const;
	INLINE void clearDirty(void);

	INLINE bool isInWorld(void) const;
	void addToWorld(void);
	void removeFromWorld(void);

private:
	Gleam::TransformCPU _local_transform;
	Gleam::TransformCPU _world_transform;
	Gleam::AABBCPU _local_aabb;
	Gleam::AABBCPU _world_aabb;

	Array< Gaff::Pair<DirtyCallback, uint64_t> > _local_callbacks;
	Array< Gaff::Pair<DirtyCallback, uint64_t> > _world_callbacks;
	Gaff::SpinLock _local_cb_lock;
	Gaff::SpinLock _world_cb_lock;

	Array<Object*> _children;
	Gaff::SpinLock _children_lock;
	Object* _parent;

	AHashString _name;

	Array<Component*> _components;
	IComponentManager& _comp_mgr;
	IObjectManager& _obj_mgr;

	unsigned int _id;;

	uint8_t _flags;

	bool createComponents(const Gaff::JSON& json);
	void markDirty(void);

	GAFF_NO_COPY(Object);
	GAFF_NO_MOVE(Object);
};

NS_END
