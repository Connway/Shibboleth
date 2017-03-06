/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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
#include <Shibboleth_Object.h>
#include <angelscript.h>
#include <vec4.hpp>
#include <vec3.hpp>
#include <vec2.hpp>

#define REGISTER_VEC(name, type) \
	REGISTER_BASE_VEC_TYPE(name, type); \
	engine->RegisterObjectBehaviour(#name, asBEHAVE_CONSTRUCT, "void f(float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::##type, float>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name "& opAddAssign(float)", asMETHODPR(glm::##type, operator+=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opSubAssign(float)", asMETHODPR(glm::##type, operator-=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opDivAssign(const " #name "& in)", asMETHODPR(glm::##type, operator/=, (const glm::##type&), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opMulAssign(float)", asMETHODPR(glm::##type, operator*=, (float), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opAdd(float) const", asFUNCTIONPR(glm::operator+, (float, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opSub(const " #name "& in) const", asFUNCTIONPR(glm::operator-, (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opSub(float) const", asFUNCTIONPR(glm::operator-, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opDiv(const " #name "& in) const", asFUNCTIONPR(glm::operator/, (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opDiv(float) const", asFUNCTIONPR(glm::operator/, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMul(float) const", asFUNCTIONPR(glm::operator*, (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMod(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::mod<float, glm::highp, glm::t##type>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opMod(float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::mod<float, glm::highp, glm::t##type>), (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " opPow(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::pow<float, glm::highp, glm::t##type>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name "& opPreInc()", asMETHODPR(glm::##type, operator++, (void), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name "& opPreDec()", asMETHODPR(glm::##type, operator--, (void), glm::##type&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opPostInc() const", asMETHODPR(glm::##type, operator++, (int), glm::##type), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, #name " opPostDec() const", asMETHODPR(glm::##type, operator--, (int), glm::##type), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, "float distance(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::distance<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " reflect(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::reflect<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " refract(const " #name "& in, float) const", asFUNCTION(GAFF_SINGLE_ARG(glm::refract<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " abs() const", asFUNCTION(GAFF_SINGLE_ARG(glm::abs<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " sign() const", asFUNCTION(GAFF_SINGLE_ARG(glm::sign<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " floor() const", asFUNCTION(GAFF_SINGLE_ARG(glm::floor<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " trunc() const", asFUNCTION(GAFF_SINGLE_ARG(glm::trunc<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " round() const", asFUNCTION(GAFF_SINGLE_ARG(glm::round<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " roundEven() const", asFUNCTION(GAFF_SINGLE_ARG(glm::roundEven<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " ceil() const", asFUNCTION(GAFF_SINGLE_ARG(glm::ceil<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " fract() const", asFUNCTION(GAFF_SINGLE_ARG(glm::fract<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " min(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::min<float, glm::highp, glm::t##type>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " min(float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::min<float, glm::highp, glm::t##type>), (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " max(const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::max<float, glm::highp, glm::t##type>), (const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " max(float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::max<float, glm::highp, glm::t##type>), (const glm::##type&, float), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " clamp(const " #name "& in, const " #name "& in) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::clamp<float, glm::highp, glm::t##type>), (const glm::##type&, const glm::##type&, const glm::##type&), glm::##type), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " clamp(float, float) const", asFUNCTIONPR(GAFF_SINGLE_ARG(glm::clamp<float, glm::highp, glm::t##type>), (const glm::##type&, float, float), glm::##type), asCALL_CDECL_OBJFIRST)

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
	engine->RegisterObjectMethod(#name, "const float& opIndex(int) const", asMETHODPR(glm::##type, operator[], (int) const, const float&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, "float& opIndex(int)", asMETHODPR(glm::##type, operator[], (int), float&), asCALL_THISCALL); \
	engine->RegisterObjectMethod(#name, "float length() const", asFUNCTION(GAFF_SINGLE_ARG(glm::length<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, "float dot(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::dot<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST); \
	engine->RegisterObjectMethod(#name, #name " normalize(const " #name "& in) const", asFUNCTION(GAFF_SINGLE_ARG(glm::normalize<float, glm::highp, glm::t##type>)), asCALL_CDECL_OBJFIRST)


NS_SHIBBOLETH

void RegisterObject(asIScriptEngine* engine)
{
	//engine->RegisterObjectMethod("Object", "const HashString64@ get_name() const", asMETHOD(Object, getName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "int get_numComponents() const", asMETHOD(Object, getNumComponents), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "const Component@ getComponent(int) const", asMETHODPR(Object, getComponent, (int32_t) const, const Component*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "Component@ getComponent(int)", asMETHODPR(Object, getComponent, (int32_t), Component*), asCALL_THISCALL);
	//engine->RegisterObjectMethod("Object", "void getComponent(?& out) const", asFUNCTION(getComponent), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Object", "void addChild(Object@)", asMETHOD(Object, addChild), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void removeFromParent()", asMETHOD(Object, removeFromParent), asCALL_THISCALL);
	engine->RegisterObjectMethod("Object", "void removeChildren()", asMETHOD(Object, removeChildren), asCALL_THISCALL);
}

void RegisterComponent(asIScriptEngine* engine)
{
	engine->RegisterObjectMethod("Component", "const Object@ get_owner() const", asMETHODPR(Component, getOwner, (void) const, const Object*), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "Object@ get_owner()", asMETHODPR(Component, getOwner, (void), Object*), asCALL_THISCALL);
	//engine->RegisterObjectMethod("Component", "const String@ get_name() const", asMETHOD(Component, getName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "bool get_active() const", asMETHOD(Component, isActive), asCALL_THISCALL);
	engine->RegisterObjectMethod("Component", "void set_active(bool)", asMETHOD(Component, setActive), asCALL_THISCALL);
	//engine->RegisterObjectMethod("Component", "void getInterface(?& out) const", asFUNCTION(), asCALL_CDECL_OBJFIRST);
}

void RegisterMath(asIScriptEngine* engine)
{
	engine->RegisterObjectType("Vec2", sizeof(glm::vec2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::vec2>());
	engine->RegisterObjectType("Vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::vec3>());
	engine->RegisterObjectType("Vec4", sizeof(glm::vec4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::vec4>());
	engine->RegisterObjectType("Quat", sizeof(glm::quat), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS | asGetTypeTraits<glm::quat>());

	// Vec2
	REGISTER_VEC(Vec2, vec2);

	engine->RegisterObjectBehaviour("Vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec2, float, float>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectProperty("Vec2", "float x", asOFFSET(glm::vec2, x));
	engine->RegisterObjectProperty("Vec2", "float y", asOFFSET(glm::vec2, y));
	engine->RegisterObjectProperty("Vec2", "float r", asOFFSET(glm::vec2, r));
	engine->RegisterObjectProperty("Vec2", "float g", asOFFSET(glm::vec2, g));
	engine->RegisterObjectProperty("Vec2", "float s", asOFFSET(glm::vec2, s));
	engine->RegisterObjectProperty("Vec2", "float t", asOFFSET(glm::vec2, t));

	// Vec3
	REGISTER_VEC(Vec3, vec3);

	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec3, float, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(const Vec2& in, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec3, const glm::vec2&, float>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Vec3", "Vec3 opMul(const Quat& in) const", asFUNCTIONPR(glm::operator*, (const glm::vec3&, const glm::quat&), glm::vec3), asCALL_CDECL_OBJFIRST);

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
	REGISTER_VEC(Vec4, vec4);

	engine->RegisterObjectBehaviour("Vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec4, float, float, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vec4", asBEHAVE_CONSTRUCT, "void f(const Vec2& in, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec4, const glm::vec2&, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Vec4", asBEHAVE_CONSTRUCT, "void f(const Vec3& in, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::vec4, const glm::vec3&, float>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Vec4", "Vec4 opMul(const Quat& in) const", asFUNCTIONPR(glm::operator*, (const glm::vec4&, const glm::quat&), glm::vec4), asCALL_CDECL_OBJFIRST);

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

	engine->RegisterGlobalFunction("Quat FromAxisAngle(const float& in, const Vec3& in)", asFUNCTION(GAFF_SINGLE_ARG(glm::angleAxis<float, glm::highp>)), asCALL_CDECL);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::quat, float, float, float, float>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_CONSTRUCT, "void f(const Vec3& in, const Vec3& in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::quat, const glm::vec3&, const glm::vec3&>)), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour("Quat", asBEHAVE_CONSTRUCT, "void f(float, const Vec3& in)", asFUNCTION(GAFF_SINGLE_ARG(Gaff::ConstructExact<glm::quat, float, const glm::vec3&>)), asCALL_CDECL_OBJFIRST);

	engine->RegisterObjectMethod("Quat", "Quat opDiv(const float& in) const", asFUNCTIONPR(glm::operator/, (const glm::quat&, const float&), glm::quat), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Vec3 opMul(const Vec3& in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Quat", "Vec4 opMul(const Vec4& in) const", asFUNCTIONPR(glm::operator*, (const glm::quat&, const glm::vec4&), glm::vec4), asCALL_CDECL_OBJFIRST);

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

NS_END
