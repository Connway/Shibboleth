/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Containers/Shibboleth_InstancedArray.h"

NS_SHIBBOLETH

InstancedArrayAny::Iterator::Iterator(InstancedArrayAny& owner, int32_t index):
	_owner(&owner), _index(index)
{
}

InstancedArrayAny::Iterator InstancedArrayAny::Iterator::operator+(int32_t offset) const
{
	return (_owner) ? Iterator(*_owner, _index + offset) : Iterator();
}

InstancedArrayAny::Iterator& InstancedArrayAny::Iterator::operator+=(int32_t offset)
{
	_index += offset;
	return *this;
}

InstancedArrayAny::Iterator& InstancedArrayAny::Iterator::operator++(void)
{
	++_index;
	return *this;
}

InstancedArrayAny::Iterator InstancedArrayAny::Iterator::operator++(int)
{
	return (_owner) ? Iterator(*_owner, _index++) : Iterator();
}

InstancedArrayAny::Iterator InstancedArrayAny::Iterator::operator-(int32_t offset) const
{
	return (_owner) ? Iterator(*_owner, _index - offset) : Iterator();
}

InstancedArrayAny::Iterator& InstancedArrayAny::Iterator::operator-=(int32_t offset)
{
	_index -= offset;
	return *this;
}

InstancedArrayAny::Iterator& InstancedArrayAny::Iterator::operator--(void)
{
	--_index;
	return *this;
}

InstancedArrayAny::Iterator InstancedArrayAny::Iterator::operator--(int)
{
	return (_owner) ? Iterator(*_owner, _index--) : Iterator();
}

/*T& InstancedArrayAny::Iterator::operator*(void) const
{
	T* const entry = get();
	GAFF_ASSERT(entry);

	return *entry;
}

T* InstancedArrayAny::Iterator::operator->(void) const
{
	return get();
}*/

int32_t InstancedArrayAny::Iterator::getIndex(void) const
{
	return _index;
}

void* InstancedArrayAny::Iterator::get(void) const
{
	return (_owner) ? _owner->at(_index) : nullptr;
}

bool InstancedArrayAny::Iterator::operator==(const Iterator& rhs) const
{
	return _owner == rhs._owner && _index == rhs._index;
}



InstancedArrayAny::ConstIterator::ConstIterator(const InstancedArrayAny& owner, int32_t index):
	_iterator(const_cast<InstancedArrayAny&>(owner), index)
{
}

InstancedArrayAny::ConstIterator::ConstIterator(const Iterator& it):
	_iterator(it)
{
}


InstancedArrayAny::ConstIterator& InstancedArrayAny::ConstIterator::operator=(const Iterator& rhs)
{
	_iterator = rhs;
	return *this;
}

InstancedArrayAny::ConstIterator InstancedArrayAny::ConstIterator::operator+(int32_t offset) const
{
	return _iterator + offset;
}

InstancedArrayAny::ConstIterator& InstancedArrayAny::ConstIterator::operator+=(int32_t offset)
{
	_iterator += offset;
	return *this;
}

InstancedArrayAny::ConstIterator& InstancedArrayAny::ConstIterator::operator++(void)
{
	++_iterator;
	return *this;
}

InstancedArrayAny::ConstIterator InstancedArrayAny::ConstIterator::operator++(int)
{
	return ConstIterator(_iterator++);
}

InstancedArrayAny::ConstIterator InstancedArrayAny::ConstIterator::operator-(int32_t offset) const
{
	return _iterator - offset;
}

InstancedArrayAny::ConstIterator& InstancedArrayAny::ConstIterator::operator-=(int32_t offset)
{
	_iterator -= offset;
	return *this;
}

InstancedArrayAny::ConstIterator& InstancedArrayAny::ConstIterator::operator--(void)
{
	--_iterator;
	return *this;
}

InstancedArrayAny::ConstIterator InstancedArrayAny::ConstIterator::operator--(int)
{
	return ConstIterator(_iterator--);
}

/*const T& operator*(void) const
{
	const T* const entry = get();
	GAFF_ASSERT(entry);

	return *entry;
}

const T* operator->(void) const
{
	return get();
}*/

int32_t InstancedArrayAny::ConstIterator::getIndex(void) const
{
	return _iterator.getIndex();
}

const void* InstancedArrayAny::ConstIterator::get(void) const
{
	return _iterator.get();
}

bool InstancedArrayAny::ConstIterator::operator==(const ConstIterator& rhs) const
{
	return *this == rhs._iterator;
}

bool InstancedArrayAny::ConstIterator::operator==(const Iterator& rhs) const
{
	return _iterator == rhs;
}



InstancedArrayAny::InstancedArrayAny(const Refl::IReflectionDefinition& ref_def, const ProxyAllocator& allocator):
	_metadata(allocator),
	_instances(allocator),
	_base_ref_def(&ref_def)
{
}

InstancedArrayAny::InstancedArrayAny(const ProxyAllocator& allocator):
	_metadata(allocator), _instances(allocator)
{
}

const void* InstancedArrayAny::operator[](int32_t index) const
{
	return at(index);
}

void* InstancedArrayAny::operator[](int32_t index)
{
	return at(index);
}

const void* InstancedArrayAny::at(int32_t index) const
{
	return const_cast<InstancedArrayAny*>(this)->at(index);
}

void* InstancedArrayAny::at(int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_metadata.size()));
	const Metadata& metadata = _metadata[index];
	return (metadata.ref_def) ? _instances.data() + metadata.start + metadata.offset : nullptr;
}

const Refl::IReflectionDefinition* InstancedArrayAny::getReflectionDefinition(int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_metadata.size()));
	return _metadata[index].ref_def;
}

int32_t InstancedArrayAny::size(void) const
{
	return static_cast<int32_t>(_metadata.size());
}

bool InstancedArrayAny::empty(void) const
{
	return size() <= 0;
}

InstancedArrayAny::ConstIterator InstancedArrayAny::begin(void) const
{
	return ConstIterator{ *this };
}

InstancedArrayAny::ConstIterator InstancedArrayAny::end(void) const
{
	return ConstIterator{ *this, size() };
}

InstancedArrayAny::Iterator InstancedArrayAny::begin(void)
{
	return Iterator{ *this };
}

InstancedArrayAny::Iterator InstancedArrayAny::end(void)
{
	return Iterator{ *this, size() };
}

void InstancedArrayAny::pushEmpty(void)
{
	Metadata metadata = { nullptr, 0, 0, 0 };

	if (!_metadata.empty()) {
		const Metadata& last_metadata = _metadata.back();
		metadata.start = last_metadata.start + last_metadata.size;
	}

	_metadata.emplace_back(metadata);
}

void InstancedArrayAny::pop(void)
{
	GAFF_ASSERT(!empty());
	erase(size() - 1);
}

void InstancedArrayAny::erase(const ConstIterator& iterator)
{
	eraseInternal(iterator.getIndex(), true);
}

void InstancedArrayAny::erase(int32_t index)
{
	eraseInternal(index, true);
}

void InstancedArrayAny::resize(int32_t new_size)
{
	new_size = Gaff::Max(0, new_size);

	const int32_t old_size = size();

	if (new_size > old_size) {
		for (int32_t i = old_size; i < new_size; ++i) {
			pushEmpty();
		}

	} else if (new_size < old_size) {
		for (int32_t i = old_size; i > new_size; --i) {
			pop();
		}
	}
}

void InstancedArrayAny::swap(int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a >= 0 && index_a < size());
	GAFF_ASSERT(index_b >= 0 && index_b < size());

	if (index_a == index_b) {
		return;
	}

	// Ensure index_a < index_b.
	if (index_a > index_b) {
		eastl::swap(index_a, index_b);
	}

	// Copying as we're going to erase the entries later.
	const Metadata metadata_a = _metadata[index_a];
	const Metadata metadata_b = _metadata[index_b];

	Vector<uint8_t> value_a(metadata_a.size, 0, _metadata.get_allocator());
	Vector<uint8_t> value_b(metadata_b.size, 0, _metadata.get_allocator());

	// Copy bytes into temp value buffers.
	for (int32_t i = 0; i < metadata_a.size; ++i) {
		value_a[i] = _instances[metadata_a.start + i];
	}

	for (int32_t i = 0; i < metadata_b.size; ++i) {
		value_b[i] = _instances[metadata_b.start + i];
	}

	eraseInternal(index_a, false);
	insertInternal(index_a, metadata_b, value_b);

	eraseInternal(index_b, false);
	insertInternal(index_b, metadata_a, value_a);
}

void InstancedArrayAny::clone(InstancedArrayAny& out) const
{
	// For now, out array must be empty.
	GAFF_ASSERT(out._instances.empty());
	GAFF_ASSERT(out._metadata.empty());

	out._instances.resize(_instances.size());
	out._base_ref_def = _base_ref_def;
	out._metadata = _metadata;

	for (const Metadata& metadata : _metadata) {
		Gaff::Hash64 ctor_hash = CLASS_HASH("const");
		ctor_hash = Gaff::FNV1aHash64String(metadata.ref_def->getReflectionInstance().getName(), ctor_hash);
		ctor_hash = Gaff::FNV1aHash64String("&", ctor_hash);

		const auto ctor = reinterpret_cast<void (*)(void*)>(metadata.ref_def->getConstructor(ctor_hash));

		if (!ctor) {
			// $TODO: Log error.

			// Attempt to default construct.
			if (!metadata.ref_def->construct(out._instances.data() +  metadata.start)) {
				// $TODO: Log error.
			}

			continue;
		}
	}
}

void* InstancedArrayAny::pushInternal(const Refl::IReflectionDefinition& ref_def)
{
	Metadata metadata =
	{
		&ref_def,
		(_base_ref_def) ? static_cast<int32_t>(ref_def.getBasePointerOffset(_base_ref_def->getReflectionInstance().getNameHash())) : 0,
		0,
		ref_def.size()
	};

	if (!_metadata.empty()) {
		const Metadata& last_metadata = _metadata.back();
		metadata.start = last_metadata.start + last_metadata.size;
	}

	_metadata.emplace_back(metadata);
	_instances.resize(_instances.size() + metadata.size);

	return _instances.data() + metadata.start;
}

void InstancedArrayAny::eraseInternal(int32_t index, bool destroy_instance)
{
	GAFF_ASSERT(index >= 0 && index < size());

	const Metadata& metadata = _metadata[index];

	if (metadata.ref_def) {
		if (destroy_instance) {
			metadata.ref_def->destroyInstance(_instances.data() + metadata.start);
		}

		const auto start = _instances.begin() + metadata.start;
		_instances.erase(start, start + metadata.size);
	}

	for (int32_t i = index + 1; i < size(); ++i) {
		_metadata[i].start -= metadata.size;
	}

	_metadata.erase(_metadata.begin() + index);
}

void InstancedArrayAny::insertInternal(int32_t index, const Metadata& metadata, const Vector<uint8_t>& value)
{
	const int32_t start = _metadata[index].start;

	for (int32_t i = index; i < size(); ++i) {
		_metadata[i].start += metadata.size;
	}

	_metadata.insert(_metadata.begin() + index, metadata);
	_metadata[index].start = start;

	_instances.insert(_instances.begin() + start, value.begin(), value.end());
}

NS_END
