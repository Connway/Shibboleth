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

#include <Shibboleth_ProxyAllocator.h>
#include <Gaff_DefaultAlignedAllocator.h>
#include <Gaff_DefaultAllocator.h>
#include <EASTL/vector.h>

MSVC_DISABLE_WARNING_PUSH(4244 4996)
#include <catch_amalgamated.hpp>
MSVC_DISABLE_WARNING_POP()


template <class Allocator>
void AllocatorTestHelper(const Allocator& allocator = Allocator())
{
	// $TODO: Rework this function. Instead of checking the vector,
	// check the allocator to see if we've allocated the appropriate amount of memory.

	eastl::vector<int32_t, Allocator> int_vector(allocator);

	int_vector.reserve(20);

	REQUIRE(int_vector.size() == 0);
	REQUIRE(int_vector.capacity() == 20);

	for (size_t i = 0; i < 10; ++i) {
		int_vector.push_back(static_cast<int32_t>(i));
	}

	REQUIRE(int_vector.size() == 10);
	REQUIRE(int_vector.capacity() == 20);

	for (size_t i = 0; i < int_vector.size(); ++i) {
		REQUIRE(int_vector[i] == static_cast<int32_t>(i));
	}
}

TEST_CASE("gaff_allocator_vector")
{
	AllocatorTestHelper<Gaff::DefaultAllocator>();
	AllocatorTestHelper<Gaff::DefaultAlignedAllocator<16> >();
}

TEST_CASE("shibboleth_allocator_vector")
{
	Shibboleth::ProxyAllocator allocatorA("Pool A");
	Shibboleth::ProxyAllocator allocatorB("Pool B");

	AllocatorTestHelper(allocatorA);
	AllocatorTestHelper(allocatorB);

	// Leak test. Uncomment and check tests/logs/AllocatorTest64d/LeakLog*.txt.
	//allocatorB.alloc(4, __FILE__, __LINE__);
}

TEST_CASE("shibboleth_newdelete_override")
{
	AllocatorTestHelper<eastl::allocator>();
}
