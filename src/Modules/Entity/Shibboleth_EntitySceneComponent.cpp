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

#include "Shibboleth_EntitySceneComponent.h"
#include "Shibboleth_EntityManager.h"
#include <Shibboleth_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntitySceneComponent)
	.base<Shibboleth::EntityComponent>()

	.var("transformRelative", &Shibboleth::EntitySceneComponent::getTransformRelative, &Shibboleth::EntitySceneComponent::setTransformRelative)
	.var("transformWorld", &Shibboleth::EntitySceneComponent::getTransformWorld, &Shibboleth::EntitySceneComponent::setTransformWorld)
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntitySceneComponent)



NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntitySceneComponent)

void EntitySceneComponent::setTransformRelative(const Gleam::Transform& transform)
{
	_transform_relative = transform;

	if (_parent) {
		_transform_world = _parent->getTransformWorld().concat(transform);
	} else {
		_transform_world = _transform_relative;
	}

	updateChildrenToWorld();
}

const Gleam::Transform& EntitySceneComponent::getTransformRelative(void) const
{
	return _transform_relative;
}

const Gleam::Vec3& EntitySceneComponent::getPositionRelative(void) const
{
	return _transform_relative.getTranslation();
}

const Gleam::Quat& EntitySceneComponent::getRotationRelative(void) const
{
	return _transform_relative.getRotation();
}

void EntitySceneComponent::setTransformWorld(const Gleam::Transform& transform)
{
	_transform_world = transform;

	if (_parent) {
		const Gleam::Transform& parent_transform = _parent->getTransformWorld();
		_transform_relative = parent_transform.inverse().concat(transform);

	} else {
		_transform_relative = transform;
	}

	updateChildrenToWorld();
}

const Gleam::Transform& EntitySceneComponent::getTransformWorld(void) const
{
	return _transform_world;
}

const Gleam::Vec3& EntitySceneComponent::getPositionWorld(void) const
{
	return _transform_world.getTranslation();
}

const Gleam::Quat& EntitySceneComponent::getRotationWorld(void) const
{
	return _transform_world.getRotation();
}

void EntitySceneComponent::addChild(EntitySceneComponent& component)
{
	// This component should not already be part of a hierarchy.
	GAFF_ASSERT(!component._parent);
	GAFF_ASSERT(!component._prev_sibling);
	GAFF_ASSERT(!component._next_sibling);

#if _DEBUG
	for (EntitySceneComponent* child = _first_child; child; child = child->_next_sibling) {
		GAFF_ASSERT(child != &component);
	}
#endif

	if (_last_child) {
		component._prev_sibling = _last_child;
		_last_child->_next_sibling = &component;

	} else {
		_first_child = &component;
	}

	component._parent = this;
	_last_child = &component;

	component.updateToWorld();
	component.updateChildrenToWorld();
}

void EntitySceneComponent::removeFromParent(bool update_to_world)
{
	if (!_parent) {
		return;
	}

	if (_prev_sibling) {
		_prev_sibling->_next_sibling = _next_sibling;
	}

	if (_next_sibling) {
		_next_sibling->_prev_sibling = _prev_sibling;
	}

	_next_sibling = nullptr;
	_prev_sibling = nullptr;
	_parent = nullptr;

	// Here just in case, but most scenarios that call this are
	// likely re-adding somewhere else in the hierarchy.
	if (update_to_world) {
		updateToWorld();
		updateChildrenToWorld();
	}
}

void EntitySceneComponent::updateChildrenToWorld(void)
{
	for (EntitySceneComponent* child = _first_child; child; child = child->_next_sibling) {
		child->updateToWorld();
		child->updateChildrenToWorld();
	}
}

void EntitySceneComponent::updateToWorld(void)
{
	// Forces _transform_world to update to latest parent transform.
	setTransformRelative(_transform_relative);
}

NS_END
