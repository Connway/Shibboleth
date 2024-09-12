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

#include "Shibboleth_AngelScriptManager.h"
#include "Shibboleth_AngelScriptString.h"
#include "Shibboleth_AngelScriptMath.h"
#include "Shibboleth_ScriptDefines.h"
#include <Attributes/Shibboleth_EngineAttributesCommon.h>
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Gaff_ContainerAlgorithm.h>
#include "Shibboleth_IncludeAngelScript.h"

#define CHECK_RESULT(r) if (r < 0) { return false; }

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::AngelScriptManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::AngelScriptManager)

namespace
{
	static constexpr const char8_t* k_op_names[] = {
		u8"opAdd",
		u8"opSub",
		u8"opMul",
		u8"opDiv",
		u8"opMod",
		u8"opAnd",
		u8"opOr",
		u8"opXor",
		u8"opCom",
		u8"opShl",
		u8"opShr",
		nullptr, // OP_LOGIC_AND_NAME
		nullptr, // OP_LOGIC_OR_NAME
		u8"opEquals",
		nullptr, // OP_LESS_THAN_NAME
		nullptr, // OP_GREATER_THAN_NAME
		nullptr, // OP_LESS_THAN_OR_EQUAL_NAME
		nullptr, //OP_GREATER_THAN_OR_EQUAL_NAME
		u8"opNeg",
		nullptr, // OP_PLUS_NAME
		u8"opCall",
		u8"opIndex",
		nullptr, // OP_TO_STRING_NAME
		u8"opCmp",
		u8"opPreInc",
		u8"opPostInc",
		u8"opPreDec",
		u8"opPostDec",
		u8"opAssign",
		u8"opAddAssign",
		u8"opSubAssign",
		u8"opMulAssign",
		u8"opDivAssign",
		u8"opModAssign",
		u8"opAndAssign",
		u8"opOrAssign",
		u8"opXorAssign",
		u8"opShlAssign",
		u8"opShrAssign",
	};
	static_assert(std::size(k_op_names) == static_cast<size_t>(Gaff::Operator::Count));

	static bool CheckMemberFunctionPointerSize(size_t mem_func_size)
	{
		return mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+1*sizeof(int)) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+2*sizeof(int)) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+3*sizeof(int)) ||
			mem_func_size == static_cast<size_t>(SINGLE_PTR_SIZE+4*sizeof(int));
	}

	static Shibboleth::U8String GetFunctionDeclaration(const Refl::FunctionSignature& sig, const char8_t* name)
	{
		Shibboleth::U8String decl{ SCRIPT_ALLOCATOR };

		decl = sig.return_value.getArgString(true) + u8' ' + name + u8'(';

		for (const Refl::FunctionArg& arg : sig.args) {
			decl += arg.getArgString(false);

			if (arg.isReference()) {
				if (arg.isConst()) {
					const size_t index = decl.find_last_of(u8'&');
					GAFF_ASSERT(index != Shibboleth::U8String::npos);

					decl.insert(index + 1, u8"in");
				}
				// $TODO: Mark up non-const references to be out or inout.
			}

			if (&arg != &sig.args.back()) {
				decl += u8", ";
			}
		}

		decl += u8')';

		if (sig.isConst()) {
			decl += u8" const";
		}

		return decl;
	}

	static bool RegisterEnum(asIScriptEngine& engine, const Refl::IEnumReflectionDefinition& enum_ref_def)
	{
		const Shibboleth::U8String name = enum_ref_def.getReflectionInstance().getName();
		const size_t last_scope_delimeter = name.rfind(u8"::");
		int result = 0;

		GAFF_ASSERT(last_scope_delimeter != Shibboleth::U8String::npos);

		result = engine.SetDefaultNamespace(reinterpret_cast<const char*>(name.data() + last_scope_delimeter));

		if (result < 0) {
			// $TODO: Log error.
			return false;
		}

		const Gaff::U8StringView enum_name(name.data() + last_scope_delimeter + 2);

		result = engine.RegisterEnum(reinterpret_cast<const char*>(enum_name.data()));

		if (result < 0) {
			// $TODO: Log error.
			return false;
		}

		const int32_t num_entries = enum_ref_def.getNumEntries();

		for (int32_t i = 0; i < num_entries; ++i) {
			const Shibboleth::HashStringView32<> entry_name = enum_ref_def.getEntryNameFromIndex(i);
			const int32_t entry_value = enum_ref_def.getEntryValue(i);

			result = engine.RegisterEnumValue(
				reinterpret_cast<const char*>(enum_name.data()),
				reinterpret_cast<const char*>(entry_name.getBuffer()),
				entry_value
			);

			if (result < 0) {
				// $TODO: Log error.
				return false;
			}
		}

		return true;
	}

	static bool RegisterType(asIScriptEngine& engine, const Refl::IReflectionDefinition& ref_def)
	{
		// We do not need to register attributes or built-in types.
		if (ref_def.hasInterface<Refl::IAttribute>() || ref_def.isBuiltIn()) {
			return true;
		}

		const Shibboleth::ScriptFlagsAttribute* const script_flags = ref_def.getClassAttr<Shibboleth::ScriptFlagsAttribute>();

		if (script_flags && !script_flags->canRegister()) {
			return true;
		}

		Shibboleth::U8String class_name = ref_def.getReflectionInstance().getName();

		// Treat default templated types as just a normal type.
		if (Gaff::EndsWith(class_name.data(), u8"<>")) {
			class_name.erase(class_name.size() - 2);
		}

		const size_t last_scope_delimeter = class_name.rfind(u8"::");
		int result = 0;

		GAFF_ASSERT(last_scope_delimeter != Shibboleth::U8String::npos);

		const Gaff::U8StringView class_name_view(class_name.data() + last_scope_delimeter + 2);

		result = engine.SetDefaultNamespace(reinterpret_cast<const char*>(class_name.substr(0, last_scope_delimeter).data()));

		if (result < 0) {
			// $TODO: Log error.
			return false;
		}

		asQWORD flags = asOBJ_APP_CLASS;

		if (script_flags && script_flags->isValueType()) {
			flags |= asOBJ_VALUE | asOBJ_APP_CLASS_DESTRUCTOR;

			int32_t ctor_count = 0;

			if (ref_def.isConstructible()) {
				flags |= asOBJ_APP_CLASS_CONSTRUCTOR;
				++ctor_count;
			}

			if (ref_def.isDestructible()) {
				flags |= asOBJ_APP_CLASS_DESTRUCTOR;
			}

			if (ref_def.isCopyConstructible()) {
				flags |= asOBJ_APP_CLASS_COPY_CONSTRUCTOR;
				++ctor_count;
			}

			if (ref_def.isCopyAssignable()) {
				flags |= asOBJ_APP_CLASS_ASSIGNMENT;
			}

			if (ref_def.getNumConstructors() > ctor_count) {
				flags |= asOBJ_APP_CLASS_MORE_CONSTRUCTORS;
			}

		} else {
			// $TODO: Support for ref-counted classes.
			flags |= asOBJ_REF | asOBJ_NOCOUNT;
		}

		result = engine.RegisterObjectType(reinterpret_cast<const char*>(class_name_view.data()), ref_def.size(), flags);

		if (result < 0) {
			// $TODO: Log error.
			return false;
		}



		// Register static functions and operators.
		const int32_t num_static_funcs = ref_def.getNumStaticFuncs();

		for (int32_t i = 0; i < num_static_funcs; ++i) {
			const Shibboleth::HashStringView32<> func_name = ref_def.getStaticFuncName(i);
			const char8_t* raw_name = func_name.getBuffer();
			asECallConvTypes call_conv = asCALL_CDECL;

			// Is an operator function
			if (Gaff::Find(func_name.getBuffer(), u8"__") == 0) {
				// $TODO: Register operator function.
				const Gaff::Hash32 op_hash = Gaff::FNV1aHash32String(func_name.getBuffer());
				const int32_t index = Gaff::IndexOfArray(Gaff::k_op_hashes, op_hash);

				if (index == -1) {
					continue;
				}

				if (!k_op_names[index]) {
					continue;
				}

				call_conv = asCALL_CDECL_OBJFIRST;
				raw_name = k_op_names[index];
			}

			const int32_t num_overrides = ref_def.getNumStaticFuncOverrides(i);

			for (int32_t j = 0; j < num_overrides; ++j) {
				const Refl::IReflectionStaticFunctionBase* const func = ref_def.getStaticFunc(i, j);
				const Refl::FunctionSignature sig = func->getSignature();
				const Shibboleth::U8String decl = GetFunctionDeclaration(sig, raw_name);

				result = engine.RegisterObjectMethod(
					reinterpret_cast<const char*>(class_name_view.data()),
					reinterpret_cast<const char*>(decl.data()),
					asFunctionPtr(func->getFunc()),
					call_conv
				);
			}
		}



		// Register regular functions.
		const int32_t num_funcs = ref_def.getNumFuncs();

		for (int32_t i = 0; i < num_funcs; ++i) {
			const Shibboleth::HashStringView32<> func_name = ref_def.getFuncName(i);
			const int32_t num_overrides = ref_def.getNumFuncOverrides(i);

			for (int32_t j = 0; j < num_overrides; ++j) {
				const Refl::IReflectionFunctionBase* const func = ref_def.getFunc(i, j);
				const Refl::FunctionSignature sig = func->getSignature();
				const Shibboleth::U8String decl = GetFunctionDeclaration(sig, func_name.getBuffer());

				if (func->isExtensionFunction()) {
					result = engine.RegisterObjectMethod(
						reinterpret_cast<const char*>(class_name_view.data()),
						reinterpret_cast<const char*>(decl.data()),
						asFunctionPtr(*reinterpret_cast<const Refl::IReflectionStaticFunctionBase::VoidFunc*>(func->getFunctionPointer())),
						asCALL_CDECL_OBJFIRST
					);

				} else {
					const int32_t func_ptr_size = func->getFunctionPointerSize();

					if (!CheckMemberFunctionPointerSize(func_ptr_size)) {
						// $TODO: Log error.
						return false;
					}

					// Mark this as a class method
					asSFuncPtr func_ptr(3);
					func_ptr.CopyMethodPtr(func->getFunctionPointer(), func_ptr_size);

					result = engine.RegisterObjectMethod(
						reinterpret_cast<const char*>(class_name_view.data()),
						reinterpret_cast<const char*>(decl.data()),
						func_ptr,
						asCALL_THISCALL
					);
				}

				if (result < 0) {
					// $TODO: Log error.
					return false;
				}
			}
		}



		// Register properties.
		const int32_t num_vars = ref_def.getNumVars();

		for (int32_t i = 0; i < num_vars; ++i) {
			const Shibboleth::HashStringView32<> var_name = ref_def.getVarName(i);
			const Refl::IReflectionVar* const var = ref_def.getVar(i);

			if (const int32_t offset = var->getOffset(); offset >= 0) {
				Shibboleth::U8String decl = ref_def.getReflectionInstance().getName();
				decl += Shibboleth::U8String{ u8' ' } + var_name.getBuffer();

				result = engine.RegisterObjectProperty(
					reinterpret_cast<const char*>(class_name_view.data()),
					reinterpret_cast<const char*>(decl.data()),
					offset
				);

				if (result < 0) {
					// $TODO: Log error.
					return false;
				}

			// Uses getter/setter functions.
			} else {
				// Getter
				if (const void* getter_function = var->getGetterFunctionPointer()) {
					const int32_t func_ptr_size = var->getGetterFunctionPointerSize();

					if (!CheckMemberFunctionPointerSize(func_ptr_size)) {
						// $TODO: Log error.
						return false;
					}

					const Refl::FunctionSignature sig = var->getGetterSignature();
					const Shibboleth::U8String decl = GetFunctionDeclaration(sig, (Shibboleth::U8String{ u8"get_" } + var_name.getBuffer() + Shibboleth::U8String{ u8" property" }).data());

					// Mark this as a class method
					asSFuncPtr func_ptr(3);
					func_ptr.CopyMethodPtr(getter_function, func_ptr_size);

					result = engine.RegisterObjectMethod(
						reinterpret_cast<const char*>(class_name_view.data()),
						reinterpret_cast<const char*>(decl.data()),
						func_ptr,
						asCALL_THISCALL
					);
				}

				// Setter
				if (const void* setter_function = var->getSetterFunctionPointer()) {
					const int32_t func_ptr_size = var->getSetterFunctionPointerSize();

					if (!CheckMemberFunctionPointerSize(func_ptr_size)) {
						// $TODO: Log error.
						return false;
					}

					const Refl::FunctionSignature sig = var->getSetterSignature();
					const Shibboleth::U8String decl = GetFunctionDeclaration(sig, (Shibboleth::U8String{ u8"set_" } + var_name.getBuffer() + Shibboleth::U8String{ u8" property" }).data());

					// Mark this as a class method
					asSFuncPtr func_ptr(3);
					func_ptr.CopyMethodPtr(setter_function, func_ptr_size);

					result = engine.RegisterObjectMethod(
						reinterpret_cast<const char*>(class_name_view.data()),
						reinterpret_cast<const char*>(decl.data()),
						func_ptr,
						asCALL_THISCALL
					);
				}
			}
		}

		return true;
	}
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(AngelScriptManager)

void AngelScriptManager::InitModuleThread(void)
{
	ManagerRef<AngelScriptManager> as_mgr;
	as_mgr->initModuleThread();
}

AngelScriptManager::~AngelScriptManager(void)
{
	_engine->ShutDownAndRelease();
	_engine = nullptr;

	asUnprepareMultithread();
}

bool AngelScriptManager::initAllModulesLoaded(void)
{
	return true;
}

bool AngelScriptManager::init(void)
{
	asPrepareMultithread();
	_thread_mgr = asGetThreadManager();

	_engine = asCreateScriptEngine();

	if (!_engine) {
		// $TODO: Log error.
		return false;
	}

	int result = _engine->SetMessageCallback(asMETHOD(AngelScriptManager, messageCallback), this, asCALL_THISCALL);
	CHECK_RESULT(result)

	result = _engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, true);
	CHECK_RESULT(result)

	result = _engine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true);
	CHECK_RESULT(result)

	// Treat warnings as errors.
	result = _engine->SetEngineProperty(asEP_COMPILER_WARNINGS, 2);
	CHECK_RESULT(result)

	if (!RegisterScriptMath_Native(_engine)) {
		// $TODO: Log error.
		return false;
	}

	if (!RegisterScriptString_Native(_engine)) {
		// $TODO: Log error.
		return false;
	}

	// $TODO: Register array type.
	// $TODO: Register resource ptr.


	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const auto* const ref_defs = refl_mgr.getTypeBucket(CLASS_HASH(*));
	const auto enum_ref_defs = refl_mgr.getEnumReflection();

	bool success = true;

	for (const Refl::IEnumReflectionDefinition* enum_ref_def : enum_ref_defs) {
		if (!RegisterEnum(*_engine, *enum_ref_def)) {
			success = false;
		}
	}

	if (ref_defs) {
		for (const Refl::IReflectionDefinition* ref_def : *ref_defs) {
			if (!RegisterType(*_engine, *ref_def)) {
				success = false;
			}
		}
	}

	return success;
}

void AngelScriptManager::initModuleThread(void)
{
	asPrepareMultithread(_thread_mgr);
}

void AngelScriptManager::messageCallback(const asSMessageInfo* msg, void* param)
{
	GAFF_REF(msg, param);

/*	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING )
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";

	printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);*/
}

NS_END
