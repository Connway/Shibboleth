/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_AngelScriptBaseTypes.h"
#include "Shibboleth_AngelScriptComponent.h"
#include <Shibboleth_Object.h>
#include <Shibboleth_IncludeAngelScript.h>
#include <gtc/quaternion.hpp>
#include <gtc/vec1.hpp>
#include <vec4.hpp>
#include <vec3.hpp>
#include <vec2.hpp>

#ifdef PLATFORM_64_BIT
	#define SIZE_T_STRING "uint64"
#elif defined(PLATFORM_32_BIT)
	#define SIZE_T_STRING "uint32"
#else
static_assert(false, "size_t is not 32 or 64-bits!");
#endif


#define REGISTER_VEC(name, type, vec_len) \
	REGISTER_BASE_VEC_TYPE(name, type); \
	engine->RegisterObjectBehaviour(#name, asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::##type, float>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name "& opAddAssign(float)", asMETHODPR(glm::##type, operator+=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opSubAssign(float)", asMETHODPR(glm::##type, operator-=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opDivAssign(const " #name "& in)", asMETHODPR(glm::##type, operator/=, (const glm::##type&), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opMulAssign(float)", asMETHODPR(glm::##type, operator*=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opAdd(float) const", asFUNCTIONPR(glm::operator+, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opSub(const " #name "& in) const", asFUNCTIONPR(glm::operator-, (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opSub(float) const", asFUNCTIONPR(glm::operator-, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opDiv(const " #name "& in) const", asFUNCTIONPR(glm::operator/, (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opDiv(float) const", asFUNCTIONPR(glm::operator/, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMul(float) const", asFUNCTIONPR(glm::operator*, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMod(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::mod<vec_len, float, glm::highp>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMod(float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::mod<vec_len, float, glm::highp>), (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opPow(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::pow<vec_len, float, glm::highp>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opAdd_r(float) const", asFUNCTIONPR(glm::operator+, (float, const glm::##type&), glm::##type), asCALL_CDECL_OBJLAST); \
	engine->RegisterObjectMethod(#name, #name " opSub_r(float) const", asFUNCTIONPR(glm::operator-, (float, const glm::##type&), glm::##type), asCALL_CDECL_OBJLAST); \
	engine->RegisterObjectMethod(#name, #name " opDiv_r(float) const", asFUNCTIONPR(glm::operator/, (float, const glm::##type&), glm::##type), asCALL_CDECL_OBJLAST); \
	engine->RegisterObjectMethod(#name, #name " opMul_r(float) const", asFUNCTIONPR(glm::operator*, (float, const glm::##type&), glm::##type), asCALL_CDECL_OBJLAST); \
	engine->RegisterObjectMethod(#name, #name "& opPreInc()", asMETHODPR(glm::##type, operator++, (void), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opPreDec()", asMETHODPR(glm::##type, operator--, (void), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opPostInc() const", asMETHODPR(glm::##type, operator++, (int32_t), glm::##type), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opPostDec() const", asMETHODPR(glm::##type, operator--, (int32_t), glm::##type), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, "float distance(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::distance<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " reflect(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::reflect<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " refract(const " #name "& in, float) const", asFUNCTION(GAFF_SINGLE_ARG(glm::refract<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " abs() const", asFUNCTION(GAFF_SINGLE_ARG(glm::abs<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " sign() const", asFUNCTION(GAFF_SINGLE_ARG(glm::sign<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " floor() const", asFUNCTION(GAFF_SINGLE_ARG(glm::floor<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " trunc() const", asFUNCTION(GAFF_SINGLE_ARG(glm::trunc<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " round() const", asFUNCTION(GAFF_SINGLE_ARG(glm::round<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " roundEven() const", asFUNCTION(GAFF_SINGLE_ARG(glm::roundEven<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " ceil() const", asFUNCTION(GAFF_SINGLE_ARG(glm::ceil<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " fract() const", asFUNCTION(GAFF_SINGLE_ARG(glm::fract<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " min(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::min<vec_len, float, glm::highp>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " min(float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::min<vec_len, float, glm::highp>), (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " max(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::max<vec_len, float, glm::highp>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " max(float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::max<vec_len, float, glm::highp>), (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " clamp(const " #name "& in, const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::clamp<vec_len, float, glm::highp>), (const glm::##type&, const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " clamp(float, float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::clamp<vec_len, float, glm::highp>), (const glm::##type&, float, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, "float dot(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::dot<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " normalize(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::normalize<vec_len, float, glm::highp>)), asCALL_CDECL_OBJFIRST)

#define REGISTER_BASE_VEC_TYPE(name, type) \
	engine->RegisterObjectBehaviour(#name, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Gaff::ConstructExact<glm::##type>), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectBehaviour(#name, asBEHAVE_CONSTRUCT, "void f(const " #name "& in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::##type, const glm::##type&>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name "& opAssign(const " #name "& in)", asMETHODPR(glm::##type, operator=, (const glm::##type&), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opAddAssign(const " #name "& in)", asMETHODPR(glm::##type, operator+=, (const glm::##type&), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opSubAssign(const " #name "& in)", asMETHODPR(glm::##type, operator-=, (const glm::##type&), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opMulAssign(const " #name "& in)", asMETHODPR(glm::##type, operator*=, (const glm::##type&), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opDivAssign(float)", asMETHODPR(glm::##type, operator/=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opAdd(const " #name "& in) const", asFUNCTIONPR(glm::operator+, (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMul(const " #name "& in) const", asFUNCTIONPR(glm::operator*, (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opNeg() const", asFUNCTIONPR(glm::operator-, (const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, "bool opEquals(const " #name "& in) const", asFUNCTIONPR(glm::operator==, (const glm::##type&, const glm::##type&), bool), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, "const float& opIndex(int32) const", asMETHODPR(glm::##type, operator[], (int32_t) const, const float&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, "float& opIndex(int32)", asMETHODPR(glm::##type, operator[], (int32_t), float&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, "int length() const", asFUNCTION(GetLength<glm::##type>), asCALL_CDECL_OBJFIRST)


NS_SHIBBOLETH

template <class T>
static int GetLength(void*)
{
	return T::length();
}

template <class T>
static void ObjectDestructor(T* instance)
{
	// Because VS2015 complains that this function does't reference "instance" ...
	GAFF_REF(instance);
	instance->~T();
}

template <class HS>
static void HashStringConstructor(HS* object, const U8String& string)
{
	Gaff::ConstructExact(object, string.data(), string.size(), nullptr, ProxyAllocator("AngelScript String"));
}

template <class HS>
static void HashStringConstructor(HS* object, const HS& hs)
{
	Gaff::ConstructExact(object, hs);
}

template <class HS>
static void HashStringConstructor(HS* object)
{
	Gaff::ConstructExact(object, nullptr, ProxyAllocator("AngelScript String"));
}

template <class HS, class HS2>
static int32_t CompareHashStrings(const HS& lhs, const HS2& rhs)
{
	if (lhs < rhs) {
		return -1;
	}
	else if (lhs > rhs) {
		return 1;
	}
	else {
		return 0;
	}
}

static void GetComponentFromObject(asIScriptGeneric* generic)
{
	Object* const object = reinterpret_cast<Object*>(generic->GetObject());
	void** const out = reinterpret_cast<void**>(generic->GetArgAddress(0));
	const int type_id = generic->GetArgTypeId(0);

	GAFF_ASSERT(type_id > asTYPEID_DOUBLE);
	GAFF_ASSERT(type_id & asTYPEID_OBJHANDLE);
	GAFF_ASSERT(!(generic->GetObjectTypeId() & asTYPEID_HANDLETOCONST) || type_id & asTYPEID_HANDLETOCONST);

	const asITypeInfo* const type_info = generic->GetEngine()->GetTypeInfoById(type_id);

	// Find the first script component that has a script class with this name.
	if (type_id & asTYPEID_SCRIPTOBJECT) {
		const Vector<Component*>& components = object->getComponents();

		for (Component* component : components) {
			AngelScriptComponent* const script = Gaff::ReflectionCast<AngelScriptComponent>(*component);

			if (script) {
				asIScriptObject* const script_object = script->getObject();

				if (type_info == script_object->GetObjectType()) {
					*out = script_object;
				}
			}
		}

	// Get the first component that implements this interface.
	} else {
		const char* const name = type_info->GetName();
		const Gaff::Hash64 class_hash = Gaff::FNV1aHash64String(name);
		*out = object->getComponent(class_hash);
	}
}

static void GetInterfaceFromComponent(asIScriptGeneric* generic)
{
	Component* const component = reinterpret_cast<Component*>(generic->GetObject());
	void** const out = reinterpret_cast<void**>(generic->GetArgAddress(0));
	const int type_id = generic->GetArgTypeId(0);

	GAFF_ASSERT(type_id > asTYPEID_DOUBLE);
	GAFF_ASSERT(type_id & asTYPEID_OBJHANDLE);
	GAFF_ASSERT(!(generic->GetObjectTypeId() & asTYPEID_HANDLETOCONST) || type_id & asTYPEID_HANDLETOCONST);

	AngelScriptComponent* const script = Gaff::ReflectionCast<AngelScriptComponent>(*component);
	asIScriptEngine* const engine = generic->GetEngine();
	asITypeInfo* const type_info = engine->GetTypeInfoById(type_id);

	// Try and cast the held script to the associated type.
	if (script) {
		asIScriptObject* const object = script->getObject();
		engine->RefCastObject(object, object->GetObjectType(), type_info, out);

	// Use reflection definition to cast.
	} else {
		*out = component->getReflectionDefinition().getInterface(Gaff::FNV1aHash64String(type_info->GetName()), component->getBasePointer());
	}
}

class StringFactory : public asIStringFactory
{
public:
	const void* GetStringConstant(const char* str, asUINT length)
	{
		const Gaff::Hash64 hash = Gaff::FNV1aHash64(str, length);
		auto it = eastl::lower_bound(_string_cache.begin(), _string_cache.end(), hash);

		if (it != _string_cache.end() && it->hash == hash) {
			++it->count;

		} else {
			CacheData data;
			data.hash = hash;
			data.string = U8String(str, length, ProxyAllocator("AngelScript String"));
			data.count = 1;

			it = _string_cache.insert(it, data);
		}

		return reinterpret_cast<const void*>(&it->string);
	}

	int ReleaseStringConstant(const void* str)
	{
		if (!str) {
			return asERROR;
		}

		const U8String* const string = reinterpret_cast<const U8String*>(str);
		const Gaff::Hash64 hash = Gaff::FNV1aHash64(string->data(), string->length());
		auto it = eastl::lower_bound(_string_cache.begin(), _string_cache.end(), hash);

		if (it == _string_cache.end() || it->hash != hash) {
			return asERROR;
		}

		--it->count;

		if (!it->count) {
			_string_cache.erase(it);
		}

		return asSUCCESS;
	}
	
	int GetRawStringData(const void* str, char* data, asUINT* length) const
	{
		if (!str) {
			return asERROR;
		}

		if (length) {
			*length = static_cast<asUINT>(reinterpret_cast<const U8String*>(str)->length());
		}

		if (data) {
			memcpy(data, reinterpret_cast<const U8String*>(str)->c_str(), reinterpret_cast<const U8String*>(str)->length());
		}

		return asSUCCESS;
	}

private:
	struct CacheData
	{
	public:
		Gaff::Hash64 hash;
		U8String string;
		int count = 0;

		bool operator<(Gaff::Hash64 rhs) const
		{
			return hash < rhs;
		}
	};
	
	Vector<CacheData> _string_cache;
};

static StringFactory g_string_factory;

//static U8String StringFactory(asUINT size, const char* str)
//{
//	return U8String (str, size, ProxyAllocator("AngelScript String"));
//}

void DeclareTypes(asIScriptEngine* engine)
{
	engine->RegisterObjectType("Object", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("Component", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("String", sizeof(U8String), asOBJ_VALUE | asGetTypeTraits<U8String>());
	engine->RegisterObjectType("HashString64", sizeof(HashString64), asOBJ_VALUE | asGetTypeTraits<HashString64>());
	engine->RegisterObjectType("HashString32", sizeof(HashString32), asOBJ_VALUE | asGetTypeTraits<HashString32>());
	engine->RegisterObjectType("Vec2", sizeof(glm::vec2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::vec2>());
	engine->RegisterObjectType("Vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::vec3>());
	engine->RegisterObjectType("Vec4", sizeof(glm::vec4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::vec4>());
	engine->RegisterObjectType("Quat", sizeof(glm::quat), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::quat>());
}

void RegisterObject(asIScriptEngine* engine)
{
	engine->RegisterObjectMethod("Object", "const HashString64& get_name() const", asMETHOD(Object, getName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "int32 get_num_components() const", asMETHOD(Object, getNumComponents), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Component@ getComponent(int32) const", asMETHODPR(Object, getComponent, (int32_t) const, const Component*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Component@ getComponent(int32)", asMETHODPR(Object, getComponent, (int32_t), Component*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void getComponent(?& out) const", asFUNCTION(GetComponentFromObject), asCALL_GENERIC);
	//engine->RegisterObjectMethod("Object", "void getComponent(?& out)", asFUNCTION(GetComponentFromObject), asCALL_GENERIC);
	engine->RegisterObjectMethod("Object", "const Vec3& get_local_position() const", asMETHOD(Object, getLocalPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Vec3& get_world_position() const", asMETHOD(Object, getWorldPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Quat& get_local_rotation() const", asMETHOD(Object, getLocalRotation), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Quat& get_world_rotation() const", asMETHOD(Object, getWorldRotation), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Vec3& get_local_scale() const", asMETHOD(Object, getLocalScale), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Vec3& get_world_scale() const", asMETHOD(Object, getWorldScale), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void set_local_position(const Vec3&)", asMETHOD(Object, setLocalPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void set_world_position(const Vec3&)", asMETHOD(Object, setWorldPosition), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void set_local_rotation(const Quat&)", asMETHOD(Object, setLocalRotation), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void set_world_rotation(const Quat&)", asMETHOD(Object, setWorldRotation), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void set_local_scale(const Vec3&)", asMETHOD(Object, setLocalScale), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void set_world_scale(const Vec3&)", asMETHOD(Object, setWorldScale), asCALL_THISCALL);

	engine->RegisterObjectMethod("Object", "void addChild(Object@)", asMETHOD(Object, addChild), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void removeFromParent()", asMETHOD(Object, removeFromParent), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void removeChildren()", asMETHOD(Object, removeChildren), asCALL_THISCALL);

	engine->RegisterObjectMethod("Object", "bool get_in_world() const", asMETHOD(Object, isInWorld), asCALL_THISCALL);
}

void RegisterComponent(asIScriptEngine* engine)
{
	engine->RegisterObjectMethod("Component", "const Object@ get_owner() const", asMETHODPR(Component, getOwner, (void) const, const Object*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "Object@ get_owner()", asMETHODPR(Component, getOwner, (void), Object*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "const String& get_name() const", asMETHOD(Component, getName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "bool get_active() const", asMETHOD(Component, isActive), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "void set_active(bool)", asMETHOD(Component, setActive), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "void opCast(?& out) const", asFUNCTION(GetInterfaceFromComponent), asCALL_GENERIC);
	//engine->RegisterObjectMethod("Component", "void opCast(?& out)", asFUNCTION(GetInterfaceFromComponent), asCALL_GENERIC);
}

void RegisterMath(asIScriptEngine* engine)
{
	// Vec2
	REGISTER_VEC(Vec2, vec2, 2);

	engine->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec2, float, float>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectProperty("Vec2", "float x", asOFFSET(glm::vec2, x));
	engine->RegisterObjectProperty("Vec2", "float y", asOFFSET(glm::vec2, y));
	engine->RegisterObjectProperty("Vec2", "float r", asOFFSET(glm::vec2, r));
	engine->RegisterObjectProperty("Vec2", "float g", asOFFSET(glm::vec2, g));
	engine->RegisterObjectProperty("Vec2", "float s", asOFFSET(glm::vec2, s));
	engine->RegisterObjectProperty("Vec2", "float t", asOFFSET(glm::vec2, t));

	// Vec3
	REGISTER_VEC(Vec3, vec3, 3);

	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec3, float, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(const Vec2& in, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec3, const glm::vec2&, float>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Vec3", "Vec3 cross(const Vec3& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::cross<float, glm::highp>), (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectProperty("Vec3", "float x", asOFFSET(glm::vec3, x));
	engine->RegisterObjectProperty("Vec3", "float y", asOFFSET(glm::vec3, y));
	engine->RegisterObjectProperty("Vec3", "float z", asOFFSET(glm::vec3, z));
	engine->RegisterObjectProperty("Vec3", "float r", asOFFSET(glm::vec3, r));
	engine->RegisterObjectProperty("Vec3", "float g", asOFFSET(glm::vec3, g));
	engine->RegisterObjectProperty("Vec3", "float b", asOFFSET(glm::vec3, b));
	engine->RegisterObjectProperty("Vec3", "float s", asOFFSET(glm::vec3, s));
	engine->RegisterObjectProperty("Vec3", "float t", asOFFSET(glm::vec3, t));
	engine->RegisterObjectProperty("Vec3", "float p", asOFFSET(glm::vec3, p));

	// Vec4
	REGISTER_VEC(Vec4, vec4, 4);

	engine->RegisterObjectBehaviour("Vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec4, float, float, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vec4", asBEHAVE_CONSTRUCT, "void f(const Vec2& in, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec4, const glm::vec2&, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vec4", asBEHAVE_CONSTRUCT, "void f(const Vec3& in, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec4, const glm::vec3&, float>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectProperty("Vec4", "float x", asOFFSET(glm::vec4, x));
	engine->RegisterObjectProperty("Vec4", "float y", asOFFSET(glm::vec4, y));
	engine->RegisterObjectProperty("Vec4", "float z", asOFFSET(glm::vec4, z));
	engine->RegisterObjectProperty("Vec4", "float w", asOFFSET(glm::vec4, w));
	engine->RegisterObjectProperty("Vec4", "float r", asOFFSET(glm::vec4, r));
	engine->RegisterObjectProperty("Vec4", "float g", asOFFSET(glm::vec4, g));
	engine->RegisterObjectProperty("Vec4", "float b", asOFFSET(glm::vec4, b));
	engine->RegisterObjectProperty("Vec4", "float a", asOFFSET(glm::vec4, a));
	engine->RegisterObjectProperty("Vec4", "float s", asOFFSET(glm::vec4, s));
	engine->RegisterObjectProperty("Vec4", "float t", asOFFSET(glm::vec4, t));
	engine->RegisterObjectProperty("Vec4", "float p", asOFFSET(glm::vec4, p));
	engine->RegisterObjectProperty("Vec4", "float q", asOFFSET(glm::vec4, q));

	// Quaternion
	REGISTER_BASE_VEC_TYPE(Quat, quat);
	engine->RegisterObjectMethod("Quat", "float dot(const Quat& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::dot<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat normalize(const Quat& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::normalize<float, glm::highp>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterGlobalFunction("Quat FromAxisAngle(const float& in, const Vec3& in)", asFUNCTION(GAFF_SINGLE_ARG(glm::angleAxis<float, glm::highp>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::quat, float, float, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_CONSTRUCT, "void f(const Vec3& in, const Vec3& in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::quat, const glm::vec3&, const glm::vec3&>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_CONSTRUCT, "void f(float, const Vec3& in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::quat, float, const glm::vec3&>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Quat", "Quat opDiv(const float& in) const", asFUNCTIONPR(glm::operator/, (const glm::quat&, const float&), glm::quat), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Vec3 opMul(const Vec3& in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Vec4 opMul(const Vec4& in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::vec4&), glm::vec4), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Vec3 opMul_r(const Vec3& in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Quat", "Vec4 opMul_r(const Vec4& in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::vec4&), glm::vec4), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("Quat", "Quat lerp(const Quat& in, float) const", asFUNCTION(GAFF_SINGLE_ARG(glm::lerp<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat slerp(const Quat& in, float) const", asFUNCTION(GAFF_SINGLE_ARG(glm::slerp<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat conjugate() const", asFUNCTION(GAFF_SINGLE_ARG(glm::conjugate<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat inverse() const", asFUNCTION(GAFF_SINGLE_ARG(glm::inverse<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat rotate(const float& in, const Vec3& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::rotate<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Quat eulerAngles() const", asFUNCTION(GAFF_SINGLE_ARG(glm::eulerAngles<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "float roll() const", asFUNCTION(GAFF_SINGLE_ARG(glm::roll<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "float pitch() const", asFUNCTION(GAFF_SINGLE_ARG(glm::pitch<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "float yaw() const", asFUNCTION(GAFF_SINGLE_ARG(glm::yaw<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "float angle() const", asFUNCTION(GAFF_SINGLE_ARG(glm::angle<float, glm::highp>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Vec3 axis() const", asFUNCTION(GAFF_SINGLE_ARG(glm::axis<float, glm::highp>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectProperty("Quat", "float x", asOFFSET(glm::quat, x));
	engine->RegisterObjectProperty("Quat", "float y", asOFFSET(glm::quat, y));
	engine->RegisterObjectProperty("Quat", "float z", asOFFSET(glm::quat, z));
	engine->RegisterObjectProperty("Quat", "float w", asOFFSET(glm::quat, w));
}

void RegisterString(asIScriptEngine* engine)
{
	engine->RegisterGlobalProperty("const " SIZE_T_STRING " npos", const_cast<size_t*>(&U8String::npos));

	//engine->RegisterStringFactory("String", asFUNCTION(StringFactory), asCALL_CDECL);
	engine->RegisterStringFactory("String", &g_string_factory);
	engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Gaff::ConstructExact<U8String>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f(const String& in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<U8String, const U8String&>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("String", asBEHAVE_CONSTRUCT, "void f(const String& in, " SIZE_T_STRING ", " SIZE_T_STRING " n = npos)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<U8String, const U8String&, size_t, size_t>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("String", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(ObjectDestructor<U8String>), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("String", "String& opAssign(const String& in)", asMETHODPR(U8String, operator=, (const U8String&), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAssign(int8)", asMETHODPR(U8String, operator=, (char), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(const String& in)", asMETHODPR(U8String, operator+=, (const U8String&), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& opAddAssign(int8)", asMETHODPR(U8String, operator+=, (char), U8String&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "String& opAdd(const String& in) const", asFUNCTIONPR(eastl::operator+, (const U8String&, const U8String&), U8String), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "String& opAdd(int8) const", asFUNCTIONPR(eastl::operator+, (const U8String&, char), U8String), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("String", "String& opAdd_r(int8) const", asFUNCTIONPR(eastl::operator+, (char, const U8String&), U8String), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("String", "bool opEquals(const String& in) const", asFUNCTIONPR(eastl::operator==, (const U8String&, const U8String&), bool), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("String", "int32 opCmp(const String& in) const", asMETHODPR(U8String, compare, (const U8String&) const, int32_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "const int8& opIndex(" SIZE_T_STRING ") const", asMETHODPR(U8String, operator[], (size_t) const, const char&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int8& opIndex(" SIZE_T_STRING ")", asMETHODPR(U8String, operator[], (size_t), char&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "bool get_empty() const", asMETHOD(U8String, empty), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " get_size() const", asMETHOD(U8String, size), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void set_size(" SIZE_T_STRING ")", asMETHODPR(U8String, resize, (size_t), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " get_capacity() const", asMETHOD(U8String, capacity), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void set_capacity(" SIZE_T_STRING ")", asMETHOD(U8String, set_capacity), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void resize(" SIZE_T_STRING ", int8)", asMETHODPR(U8String, resize, (size_t, char), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void reserve(" SIZE_T_STRING ")", asMETHOD(U8String, reserve), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "const int8& front() const", asMETHODPR(U8String, front, (void) const, const char&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int8& front()", asMETHODPR(U8String, front, (void), char&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "const int8& back() const", asMETHODPR(U8String, back, (void) const, const char&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int8& back()", asMETHODPR(U8String, back, (void), char&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "void push(int8)", asMETHOD(U8String, push_back), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void pop()", asMETHOD(U8String, pop_back), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "String& append(const String& in)", asMETHODPR(U8String, append, (const U8String&), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& append(const String& in, " SIZE_T_STRING ", " SIZE_T_STRING ")", asMETHODPR(U8String, append, (const U8String&, size_t, size_t), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& append(" SIZE_T_STRING ", int8)", asMETHODPR(U8String, append, (size_t, char), U8String&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "String& insert(" SIZE_T_STRING ", const String& in)", asMETHODPR(U8String, insert, (size_t, const U8String&), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& insert(" SIZE_T_STRING ", const String& in, " SIZE_T_STRING ", " SIZE_T_STRING ")", asMETHODPR(U8String, insert, (size_t, const U8String&, size_t, size_t), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& insert(" SIZE_T_STRING ", " SIZE_T_STRING ",  int8)", asMETHODPR(U8String, insert, (size_t, size_t, char), U8String&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "String& erase(" SIZE_T_STRING " pos = 0, " SIZE_T_STRING " n = npos)", asMETHODPR(U8String, erase, (size_t, size_t), U8String&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "String& replace(" SIZE_T_STRING ", " SIZE_T_STRING ", const String& in)", asMETHODPR(U8String, replace, (size_t, size_t, const U8String&), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& replace(" SIZE_T_STRING ", " SIZE_T_STRING ", const String& in, " SIZE_T_STRING ", " SIZE_T_STRING ")", asMETHODPR(U8String, replace, (size_t, size_t, const U8String&, size_t, size_t), U8String&), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String& replace(" SIZE_T_STRING ", " SIZE_T_STRING ", " SIZE_T_STRING ", int8)", asMETHODPR(U8String, replace, (size_t, size_t, size_t, char), U8String&), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", SIZE_T_STRING " find(const String& in, " SIZE_T_STRING " pos = 0) const", asMETHODPR(U8String, find, (const U8String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " find(int8, " SIZE_T_STRING " pos = 0) const", asMETHODPR(U8String, find, (char, size_t) const, size_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", SIZE_T_STRING " rfind(const String& in, " SIZE_T_STRING " pos = npos) const", asMETHODPR(U8String, rfind, (const U8String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " rfind(int8, " SIZE_T_STRING " pos = npos) const", asMETHODPR(U8String, rfind, (char, size_t) const, size_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_first_of(const String& in, " SIZE_T_STRING " pos = 0) const", asMETHODPR(U8String, find_first_of, (const U8String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_first_of(int8, " SIZE_T_STRING " pos = 0) const", asMETHODPR(U8String, find_first_of, (char, size_t) const, size_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_last_of(const String& in, " SIZE_T_STRING " pos = npos) const", asMETHODPR(U8String, find_last_of, (const U8String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_last_of(int8, " SIZE_T_STRING " pos = npos) const", asMETHODPR(U8String, find_last_of, (char, size_t) const, size_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_first_not_of(const String& in, " SIZE_T_STRING " pos = 0) const", asMETHODPR(U8String, find_first_not_of, (const U8String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_first_not_of(int8, " SIZE_T_STRING " pos = 0) const", asMETHODPR(U8String, find_first_not_of, (char, size_t) const, size_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_last_not_of(const String& in, " SIZE_T_STRING " pos = npos) const", asMETHODPR(U8String, find_last_not_of, (const U8String&, size_t) const, size_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", SIZE_T_STRING " find_last_not_of(int8, " SIZE_T_STRING " pos = npos) const", asMETHODPR(U8String, find_last_not_of, (char, size_t) const, size_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "String substr(" SIZE_T_STRING " pos = 0, " SIZE_T_STRING " n = npos) const", asMETHOD(U8String, substr), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "int32 compare(const String& in) const", asMETHODPR(U8String, compare, (const U8String&) const, int32_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int32 compare(" SIZE_T_STRING ", " SIZE_T_STRING ", const String& in) const", asMETHODPR(U8String, compare, (size_t, size_t, const U8String&) const, int32_t), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "int32 compare(" SIZE_T_STRING ", " SIZE_T_STRING ", const String& in, " SIZE_T_STRING ", " SIZE_T_STRING ") const", asMETHODPR(U8String, compare, (size_t, size_t, const U8String&, size_t, size_t) const, int32_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "int32 comparei(const String& in) const", asMETHODPR(U8String, comparei, (const U8String&) const, int32_t), asCALL_THISCALL);

	engine->RegisterObjectMethod("String", "void make_lower()", asMETHOD(U8String, make_lower), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void make_upper()", asMETHOD(U8String, make_upper), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void ltrim()", asMETHOD(U8String, ltrim), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void rtrim()", asMETHOD(U8String, rtrim), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void trim()", asMETHOD(U8String, trim), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String left(" SIZE_T_STRING ") const", asMETHOD(U8String, left), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "String right(" SIZE_T_STRING ") const", asMETHOD(U8String, right), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "bool validate() const", asMETHOD(U8String, validate), asCALL_THISCALL);
	engine->RegisterObjectMethod("String", "void swap(const String& in)", asMETHOD(U8String, swap), asCALL_THISCALL);
}

void RegisterStringHash(asIScriptEngine* engine)
{
	// HashString64
	engine->RegisterObjectBehaviour("HashString64", asBEHAVE_CONSTRUCT, "void f(const HashString64& in)", asFUNCTIONPR(HashStringConstructor<HashString64>, (HashString64*, const HashString64&), void), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("HashString64", asBEHAVE_CONSTRUCT, "void f(const String& in)", asFUNCTIONPR(HashStringConstructor<HashString64>, (HashString64*, const U8String&), void), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("HashString64", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(HashStringConstructor<HashString64>, (HashString64*), void), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("HashString64", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(ObjectDestructor<HashString64>), asCALL_CDECL_OBJFIRST);

	// Should we support HashStringTemp64?
	engine->RegisterObjectMethod("HashString64", "HashString64& opAssign(const HashString64& in)", asMETHODPR(HashString64, operator=, (const HashString64&), HashString64&), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString64", "HashString64& opAssign(const String& in)", asMETHODPR(HashString64, operator=, (const U8String&), HashString64&), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString64", "bool opEquals(const HashString64& in) const", asMETHODPR(HashString64, operator==, (const HashString64&) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString64", "bool opEquals(uint64) const", asMETHODPR(HashString64, operator==, (uint64_t) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString64", "int32 opCmp(uint64) const", asFUNCTION(GAFF_SINGLE_ARG(CompareHashStrings<HashString64, uint64_t>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("HashString64", "const String& get_string() const", asMETHOD(HashString64, getString), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString64", "uint64 get_hash() const", asMETHOD(HashString64, getHash), asCALL_THISCALL);


	// HashString32
	engine->RegisterObjectBehaviour("HashString32", asBEHAVE_CONSTRUCT, "void f(const HashString32& in)", asFUNCTIONPR(HashStringConstructor<HashString32>, (HashString32*, const HashString32&), void), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("HashString32", asBEHAVE_CONSTRUCT, "void f(const String& in)", asFUNCTIONPR(HashStringConstructor<HashString32>, (HashString32*, const U8String&), void), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("HashString32", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(HashStringConstructor<HashString32>, (HashString32*), void), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("HashString32", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(ObjectDestructor<HashString32>), asCALL_CDECL_OBJFIRST);

	// Should we support HashStringTemp32?
	engine->RegisterObjectMethod("HashString32", "HashString32& opAssign(const HashString32& in)", asMETHODPR(HashString32, operator=, (const HashString32&), HashString32&), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString32", "HashString32& opAssign(const String& in)", asMETHODPR(HashString32, operator=, (const U8String&), HashString32&), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString32", "bool opEquals(const HashString32& in) const", asMETHODPR(HashString32, operator==, (const HashString32&) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString32", "bool opEquals(uint32) const", asMETHODPR(HashString32, operator==, (uint32_t) const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString32", "int32 opCmp(uint32) const", asFUNCTION(GAFF_SINGLE_ARG(CompareHashStrings<HashString32, uint32_t>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("HashString32", "const String& get_string() const", asMETHOD(HashString32, getString), asCALL_THISCALL);
	engine->RegisterObjectMethod("HashString32", "uint32 get_hash() const", asMETHOD(HashString32, getHash), asCALL_THISCALL);
}

NS_END
