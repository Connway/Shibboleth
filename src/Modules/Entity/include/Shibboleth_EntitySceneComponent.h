/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include <Gleam_Transform.h>

NS_SHIBBOLETH

class EntitySceneComponent : public EntityComponent
{
	GAFF_NO_COPY(EntitySceneComponent);

public:
	EntitySceneComponent(void) = default;

	void setTransformRelative(const Gleam::Transform& transform);
	const Gleam::Transform& getTransformRelative(void) const;

	const Gleam::Vec3& getPositionRelative(void) const;
	void setPositionRelative(const Gleam::Vec3& pos);

	const Gleam::Quat& getRotationRelative(void) const;
	void setRotationRelative(const Gleam::Quat& rot);

	const Gleam::Vec3& getScaleRelative(void) const;
	void setScaleRelative(const Gleam::Vec3& scale);

	void setTransformWorld(const Gleam::Transform& transform);
	const Gleam::Transform& getTransformWorld(void) const;

	const Gleam::Vec3& getPositionWorld(void) const;
	void setPositionWorld(const Gleam::Vec3& pos);

	const Gleam::Quat& getRotationWorld(void) const;
	void setRotationWorld(const Gleam::Quat& rot);

	const Gleam::Vec3& getScaleWorld(void) const;
	void setScaleWorld(const Gleam::Vec3& scale);

	void setParent(EntitySceneComponent& component);
	void addChild(EntitySceneComponent& component);
	void removeFromParent(bool update_to_world = false);

	virtual Gleam::Transform getTransformFromGenerator(Gaff::Hash32 generator_name);
	virtual bool hasTransformGenerator(Gaff::Hash32 generator_name) const;

private:
	Gleam::Transform _transform_relative;
	Gleam::Transform _transform_world;

	EntitySceneComponent* _parent = nullptr;
	EntitySceneComponent* _first_child = nullptr;
	EntitySceneComponent* _last_child = nullptr;
	EntitySceneComponent* _next_sibling = nullptr;
	EntitySceneComponent* _prev_sibling = nullptr;

	void updateChildrenToWorld(void);
	void updateToWorld(void);

	friend class Entity;

	SHIB_REFLECTION_CLASS_DECLARE(EntitySceneComponent);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntitySceneComponent)
