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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <Gaff_DefaultAlignedAllocator.h>
#include <Gaff_DefaultAllocator.h>
#include <Shibboleth_ProxyAllocator.h>
#include <Shibboleth_Memory.h>

#include <EASTL/vector.h>

template <class Allocator>
void AllocatorTestHelper(const Allocator& allocator = Allocator())
{
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

TEST_CASE("Gaff Allocator with vector test", "[gaff_vector]")
{
	AllocatorTestHelper<Gaff::DefaultAllocator>();
	AllocatorTestHelper<Gaff::DefaultAlignedAllocator<16> >();
}

TEST_CASE("Shibboleth Allocator with vector test", "[shibboleth_vector]")
{
	Shibboleth::ProxyAllocator allocatorA("Pool A");
	Shibboleth::ProxyAllocator allocatorB("Pool B");

	AllocatorTestHelper(allocatorA);
	AllocatorTestHelper(allocatorB);

	// Leak test. Uncomment and check project/vs2015/tests/logs/LeakLog*.txt.
	//allocatorB.alloc(4, __FILE__, __LINE__);
}

TEST_CASE("new/delete overrides test", "[shibboleth_newdelete]")
{
	AllocatorTestHelper<eastl::allocator>();
}
