/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_RigidBodyComponent.h"
#include <Shibboleth_ResourceManager.h>
// #include <PxPhysicsAPI.h>

// SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RigidBody)
// 	.classAttrs(
// 		Shibboleth::ECSClassAttribute(nullptr, u8"Physics")
// 	)

// 	.template base< Shibboleth::ECSComponentBaseBoth<Shibboleth::RigidBody, Shibboleth::RigidBody&> >()
// 	.template ctor<>()

// 	.staticFunc("Constructor", Shibboleth::RigidBody::Constructor)
// 	.staticFunc("Destructor", Shibboleth::RigidBody::Destructor)

// 	.var("shape", &Shibboleth::RigidBody::shape)
// 	.var("density", &Shibboleth::RigidBody::density, Shibboleth::OptionalAttribute())
// 	.var("static", &Shibboleth::RigidBody::is_static, Shibboleth::OptionalAttribute())
// SHIB_REFLECTION_DEFINE_END(Shibboleth::RigidBody)

NS_SHIBBOLETH

// RigidBody::~RigidBody(void)
// {
// 	if (body.body_dynamic) {
// 		if (is_static) {
// 			GAFF_SAFE_RELEASE(body.body_static);
// 		} else {
// 			GAFF_SAFE_RELEASE(body.body_dynamic);
// 		}
// 	}
// }

// void RigidBody::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
// {
// 	const auto* const old_sm = reinterpret_cast<const RigidBody*>(old_begin) + old_index;
// 	auto* const new_sm = reinterpret_cast<RigidBody*>(new_begin) + new_index;

// 	*new_sm = *old_sm;
// }

// void RigidBody::SetInternal(void* component, int32_t comp_index, const RigidBody& value)
// {
// 	GetInternal(component, comp_index) = value;
// }

// RigidBody& RigidBody::GetInternal(const void* component, int32_t comp_index)
// {
// 	return *(reinterpret_cast<RigidBody*>(const_cast<void*>(component)) + comp_index);
// }

// void RigidBody::Constructor(ECSEntityID, void* component, int32_t comp_index)
// {
// 	new(&GetInternal(component, comp_index)) RigidBody();
// }

// void RigidBody::Destructor(ECSEntityID, void* component, int32_t comp_index)
// {
// 	GetInternal(component, comp_index).~RigidBody();
// }

// bool RigidBody::Load(ECSManager& ecs_mgr, ECSEntityID id, const ISerializeReader& reader)
// {
// 	RigidBody& rb = GetInternal(ecs_mgr.getComponent<RigidBody>(id), ecs_mgr.getComponentIndex(id));
// 	return Refl::Reflection<RigidBody>::GetInstance().load(reader, rb);
// }

// bool RigidBody::Load(const ISerializeReader& reader, RigidBody& out)
// {
// 	return Refl::Reflection<RigidBody>::GetInstance().load(reader, out);
// }

// RigidBody& RigidBody::operator=(const RigidBody& rhs)
// {
// 	if (shape != rhs.shape) {
// 		// Body has an instance. Release it.
// 		if (body.body_dynamic) {
// 			if (is_static) {
// 				GAFF_SAFE_RELEASE(body.body_static);
// 			} else {
// 				GAFF_SAFE_RELEASE(body.body_dynamic);
// 			}
// 		}

// 		is_static = rhs.is_static;
// 		shape = rhs.shape;

// 		// Let RigidBody system handle instantiating the new body shape.

// 		//// Body has an instance. Clone it.
// 		//if (rhs.body.body_dynamic) {
// 		//	if (rhs.is_static) {
// 		//		//value.body.body_static;
// 		//	} else {
// 		//	}
// 		//}
// 	}

// 	return *this;
// }

NS_END
