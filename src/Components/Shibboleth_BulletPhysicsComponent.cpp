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

#ifndef USE_PHYSX

#include "Shibboleth_BulletPhysicsComponent.h"
#include <Shibboleth_IBulletPhysicsManager.h>
#include <Shibboleth_PhysicsResource.h>
#include <Shibboleth_ISchemaManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

NS_SHIBBOLETH

REF_IMPL_REQ(BulletPhysicsComponent);
SHIB_REF_IMPL(BulletPhysicsComponent)
.addBaseClassInterfaceOnly<BulletPhysicsComponent>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IBulletPhysicsComponent)
;

BulletPhysicsComponent::BulletPhysicsComponent(void):
	_rigid_body(nullptr), _mass(0.0f)
{
}

BulletPhysicsComponent::~BulletPhysicsComponent(void)
{
}

const Gaff::JSON& BulletPhysicsComponent::getSchema(void) const
{
	static const Gaff::JSON& schema = GetApp().getManagerT<ISchemaManager>().getSchema("BulletPhysicsComponent.schema");
	return schema;
}

bool BulletPhysicsComponent::load(const Gaff::JSON& json)
{
	g_ref_def.read(json, this);

	_phys_res = GetApp().getManagerT<IResourceManager>().requestResource(json["Physics File"].getString());
	_mass = Gaff::Max(0.0f, static_cast<float>(json["Mass"].getNumber()));

	auto callback = Gaff::Bind(this, &BulletPhysicsComponent::collisionShapeLoaded);
	_phys_res.getResourcePtr()->addCallback(callback);

	return true;
}

bool BulletPhysicsComponent::save(Gaff::JSON& json)
{
	return Component::save(json);
}

void BulletPhysicsComponent::addToWorld(void)
{
	if (_phys_res.getResourcePtr()->isLoaded()) {
		GetApp().getManagerT<IBulletPhysicsManager>().addToMainWorld(_rigid_body);
	}
}

void BulletPhysicsComponent::removeFromWorld(void)
{
	if (_phys_res.getResourcePtr()->isLoaded()) {
		auto& phys_mgr = GetApp().getManagerT<IBulletPhysicsManager>();
		phys_mgr.removeFromMainWorld(_rigid_body);
	}
}

void BulletPhysicsComponent::setActive(bool active)
{
	Component::setActive(active);
	
	if (_rigid_body) {
		// Set to saved mass properties.
		if (active) {
			_rigid_body->setMassProps(_mass, _inertia);

		// Save mass properties and set to static.
		} else {
			_mass = 1.0f / _rigid_body->getInvMass();
			_inertia = _rigid_body->getLocalInertia();

			btVector3 zero(0.0f, 0.0f, 0.0f);
			_rigid_body->setMassProps(0.0f, zero);
		}
	}
}

const btRigidBody* BulletPhysicsComponent::getRigidBody(void) const
{
	return _rigid_body;
}

btRigidBody* BulletPhysicsComponent::getRigidBody(void)
{
	return _rigid_body;
}

void BulletPhysicsComponent::collisionShapeLoaded(ResourceContainer*)
{
	if (_phys_res.getResourcePtr()->hasFailed()) {
		// log error
		return;
	}

	_rigid_body = GetApp().getManagerT<IBulletPhysicsManager>().createRigidBody(getOwner(), _phys_res->collision_shape, _mass);

	if (getOwner()->isInWorld()) {
		addToWorld();
	}
}

NS_END

#endif
