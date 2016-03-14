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

#include "Shibboleth_BulletPhysicsComponent.h"
#include <Shibboleth_BulletPhysicsManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

NS_SHIBBOLETH

REF_IMPL_REQ(BulletPhysicsComponent);
SHIB_REF_IMPL(BulletPhysicsComponent)
.addBaseClassInterfaceOnly<BulletPhysicsComponent>()
;

BulletPhysicsComponent::BulletPhysicsComponent(void):
	_collision_shape(nullptr), _rigid_body(nullptr),
	_mass(0.0f)
{
}

BulletPhysicsComponent::~BulletPhysicsComponent(void)
{
}

const Gaff::JSON& BulletPhysicsComponent::getSchema(void) const
{
	return Component::getSchema();
}

bool BulletPhysicsComponent::load(const Gaff::JSON& json)
{
	return Component::load(json);
}

bool BulletPhysicsComponent::save(Gaff::JSON& json)
{
	return Component::save(json);
}

void BulletPhysicsComponent::addToWorld(void)
{
	auto& phys_mgr = GetApp().getManagerT<BulletPhysicsManager>();
	_collision_shape = phys_mgr.createCollisionShapeCapsule(1.0f, 1.0f);

	if (!_collision_shape) {
		// log error
		return;
	}

	_rigid_body = phys_mgr.createRigidBody(getOwner(), _collision_shape, 10.0f);

	phys_mgr.addToMainWorld(_rigid_body);
}

void BulletPhysicsComponent::removeFromWorld(void)
{
	auto& phys_mgr = GetApp().getManagerT<BulletPhysicsManager>();
	phys_mgr.addToMainWorld(_rigid_body);
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

NS_END
