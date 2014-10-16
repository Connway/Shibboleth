/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_IComponent.h"
#include <Shibboleth_MessageBroadcaster.h>
#include <Shibboleth_Watcher.h>
#include <Shibboleth_Array.h>
#include <Gaff_INamedObject.h>
#include <Gaff_WeakObject.h>
#include <Gaff_Function.h>
#include <Gaff_SmartPtr.h>
#include <Gleam_Quaternion.h>
#include <Gleam_AABB.h>

#define MAX_OBJ_NAME_LENGTH 64

NS_SHIBBOLETH

class ComponentManager;
class IApp;

class Object : public Gaff::INamedObject, public Gaff::WeakObject<ProxyAllocator>
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
				components.push(interface);
			}
		}
	}

	template <class Interface>
	void getComponentsWithInterface(Array<Interface*>& components)
	{
		for (auto it = _components.begin(); it != _components.end(); ++it) {
			Interface* interface = (*it)->requestInterface<Interface>();

			if (interface) {
				components.push(interface);
			}
		}
	}

	typedef Gaff::FunctionBinder<void, double> UpdateCallback;

	Object(IApp& app, unsigned int id);
	~Object(void);

	bool init(const Gaff::JSON& json);
	INLINE bool init(const char* file_name);
	void destroy(void);

	const char* getName(void) const;

	unsigned int getID(void) const;
	void setID(unsigned int id);

	void registerForPrePhysicsUpdate(const UpdateCallback& callback);
	void registerForPostPhysicsUpdate(const UpdateCallback& callback);

	void prePhysicsUpdate(double dt);
	void postPhysicsUpdate(double dt);

	INLINE MessageBroadcaster& getBroadcaster(void);

	// Should I move the AABB and Position into it's own component?
	// That way we're not partially reliant on Gleam's math library?
	INLINE Gaff::WatchReceipt watchRotation(const Watcher<Gleam::Quaternion>::Callback& callback);
	INLINE Gaff::WatchReceipt watchPosition(const Watcher<Gleam::Vec4>::Callback& callback);
	INLINE Gaff::WatchReceipt watchScale(const Watcher<Gleam::Vec4>::Callback& callback);
	INLINE Gaff::WatchReceipt watchAABB(const Watcher<Gleam::AABB>::Callback& callback);

	INLINE const Gleam::Quaternion& getRotation(void) const;
	INLINE void setRotation(const Gleam::Quaternion& rot);
	INLINE const Gleam::Vec4& getPosition(void) const;
	INLINE void setPosition(const Gleam::Vec4& pos);
	INLINE const Gleam::Vec4& getScale(void) const;
	INLINE void setScale(const Gleam::Vec4& scale);
	INLINE const Gleam::AABB& getAABB(void) const;
	INLINE void setAABB(const Gleam::AABB& aabb);

private:
	MessageBroadcaster _broadcaster;

	char _name[MAX_OBJ_NAME_LENGTH];

	Watcher<Gleam::AABB> _aabb;
	Watcher<Gleam::Quaternion> _rotation;
	Watcher<Gleam::Vec4> _position;
	Watcher<Gleam::Vec4> _scale;

	Array<IComponent*> _components;

	ComponentManager& _comp_mgr;
	IApp& _app;

	unsigned int _id;

	bool createComponents(const Gaff::JSON& json);

	GAFF_NO_COPY(Object);
	GAFF_NO_MOVE(Object);
};

NS_END
