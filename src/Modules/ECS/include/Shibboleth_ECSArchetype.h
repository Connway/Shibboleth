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

#pragma once

#include <Shibboleth_Vector.h>
#include <Gaff_Hash.h>

NS_GAFF
	class IReflectionDefinition;
	class JSON;
NS_END

NS_SHIBBOLETH

class IECSVarAttribute;

class ECSArchetype
{
public:
	void setSharedName(Gaff::Hash64 name);
	void setSharedName(const char* name);

	void addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void addShared(const Gaff::IReflectionDefinition& ref_def);
	void removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void removeShared(const Gaff::IReflectionDefinition& ref_def);
	void removeShared(int32_t index);

	void initShared(const Gaff::JSON& json);

	void add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void add(const Gaff::IReflectionDefinition* ref_def);
	void remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void remove(const Gaff::IReflectionDefinition* ref_def);
	void remove(int32_t index);

	int32_t sharedSize(void) const;
	int32_t size(void) const;

	Gaff::Hash64 getHash(void) const;

	bool fromJSON(const Gaff::JSON& json);

private:
	struct VarData final
	{
		const IECSVarAttribute& var_attr;
		int32_t offset;
	};

	struct SharedVarData final
	{
		const Gaff::IReflectionDefinition& ref_def;
		Vector<VarData> var_attrs;
	};

	Vector<SharedVarData> _shared_vars;
	Vector<const Gaff::IReflectionDefinition*> _vars;

	mutable Gaff::Hash64 _hash = Gaff::INIT_HASH64;
	Gaff::Hash64 _shared_name = Gaff::INIT_HASH64;

	int32_t _shared_alloc_size = 0;
	int32_t _alloc_size = 0;

	void* _shared_instances = nullptr;

	void add(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void add(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition* ref_def);
	void remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition* ref_def);
	void remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, int32_t index);
};

NS_END
