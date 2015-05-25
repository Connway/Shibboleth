/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

VAR_CONTAINER_CONSTRUCTOR(DoubleContainer);
VAR_CONTAINER_CONSTRUCTOR(FloatContainer);
VAR_CONTAINER_CONSTRUCTOR(UIntContainer);
VAR_CONTAINER_CONSTRUCTOR(IntContainer);
VAR_CONTAINER_CONSTRUCTOR(UShortContainer);
VAR_CONTAINER_CONSTRUCTOR(ShortContainer);
VAR_CONTAINER_CONSTRUCTOR(UCharContainer);
VAR_CONTAINER_CONSTRUCTOR(CharContainer);
VAR_CONTAINER_CONSTRUCTOR(BoolContainer);
VAR_CONTAINER_CONSTRUCTOR(StringContainer);

VAR_CONTAINER_VAL_TYPE(DoubleContainer, VT_DOUBLE);
VAR_CONTAINER_VAL_TYPE(FloatContainer, VT_FLOAT);
VAR_CONTAINER_VAL_TYPE(UIntContainer, VT_UINT);
VAR_CONTAINER_VAL_TYPE(IntContainer, VT_INT);
VAR_CONTAINER_VAL_TYPE(UShortContainer, VT_USHORT);
VAR_CONTAINER_VAL_TYPE(ShortContainer, VT_SHORT);
VAR_CONTAINER_VAL_TYPE(UCharContainer, VT_UCHAR);
VAR_CONTAINER_VAL_TYPE(CharContainer, VT_CHAR);
VAR_CONTAINER_VAL_TYPE(BoolContainer, VT_BOOL);
VAR_CONTAINER_VAL_TYPE(StringContainer, VT_STRING);

VAR_CONTAINER_SET(DoubleContainer);
VAR_CONTAINER_SET(FloatContainer);
VAR_CONTAINER_SET(UIntContainer);
VAR_CONTAINER_SET(IntContainer);
VAR_CONTAINER_SET(UShortContainer);
VAR_CONTAINER_SET(ShortContainer);
VAR_CONTAINER_SET(UCharContainer);
VAR_CONTAINER_SET(CharContainer);
VAR_CONTAINER_SET(BoolContainer);
VAR_CONTAINER_SET(StringContainer);

VAR_CONTAINER_GET(DoubleContainer);
VAR_CONTAINER_GET(FloatContainer);
VAR_CONTAINER_GET(UIntContainer);
VAR_CONTAINER_GET(IntContainer);
VAR_CONTAINER_GET(UShortContainer);
VAR_CONTAINER_GET(ShortContainer);
VAR_CONTAINER_GET(UCharContainer);
VAR_CONTAINER_GET(CharContainer);
VAR_CONTAINER_GET(BoolContainer);
VAR_CONTAINER_GET(StringContainer);

VAR_CONTAINER_READ(DoubleContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isReal());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal();
	} else if (_setter) {
		(object->*_setter)(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal());
	}
}

VAR_CONTAINER_WRITE(DoubleContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal((object->*_getter)()));
	}
}




VAR_CONTAINER_READ(FloatContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isReal());

	if (_var) {
		object->*_var = static_cast<float>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal());
	} else if (_setter) {
		(object->*_setter)(static_cast<float>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal()));
	}
}

VAR_CONTAINER_WRITE(FloatContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal((object->*_getter)()));
	}
}




VAR_CONTAINER_READ(UIntContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<unsigned int>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<unsigned int>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(UIntContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}




VAR_CONTAINER_READ(IntContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<int>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<int>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(IntContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}




VAR_CONTAINER_READ(UShortContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<unsigned short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<unsigned short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(UShortContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}




VAR_CONTAINER_READ(ShortContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(ShortContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}




VAR_CONTAINER_READ(UCharContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<unsigned char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<unsigned char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(UCharContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}




VAR_CONTAINER_READ(CharContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(CharContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}




VAR_CONTAINER_READ(BoolContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isBoolean());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isTrue();
	} else if (_setter) {
		(object->*_setter)(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isTrue());
	}
}

VAR_CONTAINER_WRITE(BoolContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createBoolean(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createBoolean((object->*_getter)()));
	}
}




VAR_CONTAINER_READ(StringContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isString());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString();
	} else if (_setter) {
		(object->*_setter)(AString<Allocator>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString()));
	}
}

VAR_CONTAINER_WRITE(StringContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createString((object->*_var).getBuffer()));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createString((object->*_getter)().getBuffer()));
	}
}




ARRAY_CONTAINER_CONSTRUCTOR(ArrayDoubleContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayFloatContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayUIntContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayIntContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayUShortContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayShortContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayUCharContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayCharContainer);
ARRAY_CONTAINER_CONSTRUCTOR(ArrayStringContainer);

ARRAY_CONTAINER_VAL_TYPE(ArrayDoubleContainer, VT_DOUBLE);
ARRAY_CONTAINER_VAL_TYPE(ArrayFloatContainer, VT_FLOAT);
ARRAY_CONTAINER_VAL_TYPE(ArrayUIntContainer, VT_UINT);
ARRAY_CONTAINER_VAL_TYPE(ArrayIntContainer, VT_INT);
ARRAY_CONTAINER_VAL_TYPE(ArrayUShortContainer, VT_USHORT);
ARRAY_CONTAINER_VAL_TYPE(ArrayShortContainer, VT_SHORT);
ARRAY_CONTAINER_VAL_TYPE(ArrayUCharContainer, VT_UCHAR);
ARRAY_CONTAINER_VAL_TYPE(ArrayCharContainer, VT_CHAR);
ARRAY_CONTAINER_VAL_TYPE(ArrayStringContainer, VT_STRING);

ARRAY_CONTAINER_SET(ArrayDoubleContainer);
ARRAY_CONTAINER_SET(ArrayFloatContainer);
ARRAY_CONTAINER_SET(ArrayUIntContainer);
ARRAY_CONTAINER_SET(ArrayIntContainer);
ARRAY_CONTAINER_SET(ArrayUShortContainer);
ARRAY_CONTAINER_SET(ArrayShortContainer);
ARRAY_CONTAINER_SET(ArrayUCharContainer);
ARRAY_CONTAINER_SET(ArrayCharContainer);
ARRAY_CONTAINER_SET(ArrayStringContainer);

ARRAY_CONTAINER_GET(ArrayDoubleContainer);
ARRAY_CONTAINER_GET(ArrayFloatContainer);
ARRAY_CONTAINER_GET(ArrayUIntContainer);
ARRAY_CONTAINER_GET(ArrayIntContainer);
ARRAY_CONTAINER_GET(ArrayUShortContainer);
ARRAY_CONTAINER_GET(ArrayShortContainer);
ARRAY_CONTAINER_GET(ArrayUCharContainer);
ARRAY_CONTAINER_GET(ArrayCharContainer);
ARRAY_CONTAINER_GET(ArrayStringContainer);

ARRAY_CONTAINER_SET_INDEX(ArrayDoubleContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayFloatContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayUIntContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayIntContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayUShortContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayShortContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayUCharContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayCharContainer);
ARRAY_CONTAINER_SET_INDEX(ArrayStringContainer);

ARRAY_CONTAINER_GET_INDEX(ArrayDoubleContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayFloatContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayUIntContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayIntContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayUShortContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayShortContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayUCharContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayCharContainer);
ARRAY_CONTAINER_GET_INDEX(ArrayStringContainer);

ARRAY_CONTAINER_RESIZE(ArrayDoubleContainer);
ARRAY_CONTAINER_RESIZE(ArrayFloatContainer);
ARRAY_CONTAINER_RESIZE(ArrayUIntContainer);
ARRAY_CONTAINER_RESIZE(ArrayIntContainer);
ARRAY_CONTAINER_RESIZE(ArrayUShortContainer);
ARRAY_CONTAINER_RESIZE(ArrayShortContainer);
ARRAY_CONTAINER_RESIZE(ArrayUCharContainer);
ARRAY_CONTAINER_RESIZE(ArrayCharContainer);
ARRAY_CONTAINER_RESIZE(ArrayStringContainer);

ARRAY_CONTAINER_MOVE(ArrayDoubleContainer);
ARRAY_CONTAINER_MOVE(ArrayFloatContainer);
ARRAY_CONTAINER_MOVE(ArrayUIntContainer);
ARRAY_CONTAINER_MOVE(ArrayIntContainer);
ARRAY_CONTAINER_MOVE(ArrayUShortContainer);
ARRAY_CONTAINER_MOVE(ArrayShortContainer);
ARRAY_CONTAINER_MOVE(ArrayUCharContainer);
ARRAY_CONTAINER_MOVE(ArrayCharContainer);
ARRAY_CONTAINER_MOVE(ArrayStringContainer);

ARRAY_CONTAINER_SIZE(ArrayDoubleContainer);
ARRAY_CONTAINER_SIZE(ArrayFloatContainer);
ARRAY_CONTAINER_SIZE(ArrayUIntContainer);
ARRAY_CONTAINER_SIZE(ArrayIntContainer);
ARRAY_CONTAINER_SIZE(ArrayUShortContainer);
ARRAY_CONTAINER_SIZE(ArrayShortContainer);
ARRAY_CONTAINER_SIZE(ArrayUCharContainer);
ARRAY_CONTAINER_SIZE(ArrayCharContainer);
ARRAY_CONTAINER_SIZE(ArrayStringContainer);

ARRAY_CONTAINER_READ(ArrayDoubleContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			(object->*_var)[index] = value.getReal();
			return false;
		});

	} else if (_getter) {
		Array<double, Allocator>& arr = const_cast<Array<double, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			arr[index] = value.getReal();
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayDoubleContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createReal((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<double, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createReal(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayFloatContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			(object->*_var)[index] = static_cast<float>(value.getReal());
			return false;
		});

	} else if (_getter) {
		Array<float, Allocator>& arr = const_cast<Array<float, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			arr[index] = static_cast<float>(value.getReal());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayFloatContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createReal((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<float, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createReal(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayUIntContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			(object->*_var)[index] = static_cast<unsigned int>(value.getInteger());
			return false;
		});

	} else if (_getter) {
		Array<unsigned int, Allocator>& arr = const_cast<Array<unsigned int, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<unsigned int>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayUIntContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<unsigned int, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayIntContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			(object->*_var)[index] = static_cast<int>(value.getInteger());
			return false;
		});

	} else if (_getter) {
		Array<int, Allocator>& arr = const_cast<Array<int, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<int>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayIntContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<int, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayUShortContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			(object->*_var)[index] = static_cast<unsigned short>(value.getInteger());
			return false;
		});

	} else if (_getter) {
		Array<unsigned short, Allocator>& arr = const_cast<Array<unsigned short, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<unsigned short>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayUShortContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<unsigned short, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayShortContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			(object->*_var)[index] = static_cast<short>(value.getInteger());
			return false;
		});

	} else if (_getter) {
		Array<short, Allocator>& arr = const_cast<Array<short, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<short>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayShortContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<short, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayUCharContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			(object->*_var)[index] = static_cast<unsigned char>(value.getInteger());
			return false;
		});

	} else if (_getter) {
		Array<unsigned char, Allocator>& arr = const_cast<Array<unsigned char, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<unsigned char>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayUCharContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<unsigned char, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayCharContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			(object->*_var)[index] = static_cast<char>(value.getInteger());
			return false;
		});

	} else if (_getter) {
		Array<char, Allocator>& arr = const_cast<Array<char, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<char>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayCharContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<char, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createInteger(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_READ(ArrayStringContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isString());
			(object->*_var)[index] = value.getString();
			return false;
		});

	} else if (_setter) {
		Array<AString<Allocator>, Allocator>& arr = const_cast<Array<AString<Allocator>, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isString());
			arr[index] = value.getString();
			return false;
		});
	}
}

ARRAY_CONTAINER_WRITE(ArrayStringContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createString((object->*_var)[i].getBuffer()));
		}

	} else if (_getter) {
		const Array<AString<Allocator>, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createString(value[i].getBuffer()));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}




ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_CONSTRUCTOR(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedDoubleContainer, VT_DOUBLE);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedFloatContainer, VT_FLOAT);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedUIntContainer, VT_UINT);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedIntContainer, VT_INT);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedUShortContainer, VT_USHORT);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedShortContainer, VT_SHORT);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedUCharContainer, VT_UCHAR);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedCharContainer, VT_CHAR);
ARRAY_CONTAINER_FIXED_VAL_TYPE(ArrayFixedStringContainer, VT_STRING);

ARRAY_CONTAINER_FIXED_SET(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_SET(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_GET(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_GET(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_SET_INDEX(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_GET_INDEX(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_MOVE(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedDoubleContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedFloatContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedUIntContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedIntContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedUShortContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedShortContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedUCharContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedCharContainer);
ARRAY_CONTAINER_FIXED_SIZE(ArrayFixedStringContainer);

ARRAY_CONTAINER_FIXED_READ(ArrayFixedDoubleContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			arr[index] = value.getReal();
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedDoubleContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createReal(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedFloatContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			arr[index] = static_cast<VarType>(value.getReal());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedFloatContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createReal(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedUIntContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<VarType>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedUIntContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createInteger(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedIntContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<VarType>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedIntContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createInteger(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedUShortContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<VarType>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedUShortContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createInteger(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedShortContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<VarType>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedShortContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createInteger(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedUCharContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<VarType>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedUCharContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createInteger(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedCharContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isInteger());
			arr[index] = static_cast<VarType>(value.getInteger());
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedCharContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createInteger(arr[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_FIXED_READ(ArrayFixedStringContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	VarType* arr = nullptr;
	assert(array.isArray());

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isString());
			arr[index] = value.getString();
			return false;
		});
	}
}

ARRAY_CONTAINER_FIXED_WRITE(ArrayFixedStringContainer)
{
	JSON array = JSON::createArray();
	const VarType* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<VarType*>((object->*_getter)());
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createString(arr[i].getBuffer()));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}



// Array Fixed Enum
template <class T, class Allocator>
template <size_t array_size, class T2>
ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::ArrayFixedEnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _enum_ref_def(enum_ref_def), _getter(getter), _setter(setter), _var(nullptr)
{
}

template <class T, class Allocator>
template <size_t array_size, class T2>
ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::ArrayFixedEnumContainer(const char* key, T2 (T::*var)[array_size], const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, const Allocator& allocator):
	ValueContainerBase(key, allocator), _enum_ref_def(enum_ref_def), _getter(nullptr), _setter(nullptr), _var(var)
{
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::read(const JSON& json, T* object)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	T2* arr = nullptr;
	assert(array.isArray() && array.size() < array_size);

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<T2*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isString());
			arr[index] = _enum_ref_def.getValue(value.getString());
			return false;
		});
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::write(JSON& json, const T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);
	JSON array = JSON::createArray();
	const T2* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = (object->*_getter)();
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			array.setObject(i, JSON::createString(_enum_ref_def.getName(arr[i])));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::get(void* out, const T* object) const
{
	T2* values = reinterpret_cast<T2*>(out);
	const T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = (object->*_getter)();
	}

	if (array) {
		for (size_t i = 0; i < array_size; ++i) {
			values[i] = array[i];
		}
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::get(void* out, size_t index, const void* object) const
{
	get(out, index, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::get(void* out, size_t index, const T* object) const
{
	assert(index < array_size);
	const T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = (object->*_getter)();
	}

	if (array) {
		*reinterpret_cast<T2*>(out) = array[index];
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::set(const void* value, T* object)
{
	const T2* values = reinterpret_cast<const T2*>(value);
	T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = const_cast<T2*>((object->*_getter)());
	}

	if (array) {
		for (size_t i = 0; i < array_size; ++i) {
			array[i] = values[i];
		}
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::set(const void* value, size_t index, void* object)
{
	set(value, index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::set(const void* value, size_t index, T* object)
{
	assert(index < array_size);
	T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = const_cast<T2*>((object->*_getter)());
	}

	if (array) {
		array[index] = *reinterpret_cast<const T2*>(value);
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
size_t ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::size(const void*) const
{
	return array_size;
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::move(size_t src_index, size_t dest_index, void* object)
{
	move(src_index, dest_index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::move(size_t src_index, size_t dest_index, T* object)
{
	assert(src_index < array_size && dest_index <= array_size);
	T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = const_cast<T2*>((object->*_getter)());
	}

	if (array) {
		T2 temp = Move((object->*_var)[src_index]);
		deconstruct(array + src_index);

		for (size_t i = src_index; i  < array_size - 1; ++i) {
			memcpy(array + i, array + i + 1, sizeof(T2));
		}

		// If the dest index was above the source index, shift it down by one.
		dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; 

		for (size_t i = array_size - 1; i > dest_index; --i) {
			memcpy(array + i, array + i - 1, sizeof(T2));
		}

		moveConstruct(array + dest_index, Move(temp));
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
bool ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::isFixedArray(void) const
{
	return true;
}

template <class T, class Allocator>
template <size_t array_size, class T2>
bool ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::isArray(void) const
{
	return true;
}

template <class T, class Allocator>
template <size_t array_size, class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::ArrayFixedEnumContainer<array_size, T2>::getType(void) const
{
	return VT_ENUM;
}




// Array Fixed Object
template <class T, class Allocator>
template <size_t array_size, class T2>
ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::ArrayFixedObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& obj_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _obj_ref_def(obj_ref_def), _getter(getter), _setter(setter), _var(nullptr)
{
}

template <class T, class Allocator>
template <size_t array_size, class T2>
ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::ArrayFixedObjectContainer(const char* key, T2 (T::*var)[array_size], ReflectionDefinition<T2, Allocator>& obj_ref_def, const Allocator& allocator):
	ValueContainerBase(key, allocator), _obj_ref_def(obj_ref_def), _getter(nullptr), _setter(nullptr), _var(var)
{
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::read(const JSON& json, T* object)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	T2* arr = nullptr;
	assert(array.isArray() && array.size() < array_size);

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = const_cast<T2*>((object->*_getter)());
	}

	if (arr) {
		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isObject());
			_obj_ref_def.read(value, arr + index);
			return false;
		});
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::write(JSON& json, const T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);
	JSON array = JSON::createArray();
	const T2* arr = nullptr;

	if (_var) {
		arr = (object->*_var);
	} else if (_getter) {
		arr = (object->*_getter)();
	}

	if (arr) {
		for (size_t i = 0; i < array_size; ++i) {
			JSON obj = JSON::createObject();
			_obj_ref_def.write(obj, arr + i);
			array.setObject(i, obj);
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::get(void* out, const T* object) const
{
	T2* values = reinterpret_cast<T2*>(out);
	const T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = (object->*_getter)();
	}

	if (array) {
		for (size_t i = 0; i < array_size; ++i) {
			values[i] = array[i];
		}
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::get(void* out, size_t index, const void* object) const
{
	get(out, index, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::get(void* out, size_t index, const T* object) const
{
	assert(index < array_size);
	const T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = (object->*_getter)();
	}

	if (array) {
		*reinterpret_cast<T2*>(out) = array[index];
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::set(const void* value, T* object)
{
	const T2* values = reinterpret_cast<const T2*>(value);
	T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = const_cast<T2*>((object->*_getter)());
	}

	if (array) {
		for (size_t i = 0; i < array_size; ++i) {
			array[i] = values[i];
		}
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::set(const void* value, size_t index, void* object)
{
	set(value, index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::set(const void* value, size_t index, T* object)
{
	assert(index < array_size);
	T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = const_cast<T2*>((object->*_getter)());
	}

	if (array) {
		array[index] = *reinterpret_cast<const T2*>(value);
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
size_t ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::size(const void*) const
{
	return array_size;
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::move(size_t src_index, size_t dest_index, void* object)
{
	move(src_index, dest_index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <size_t array_size, class T2>
void ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::move(size_t src_index, size_t dest_index, T* object)
{
	assert(src_index < array_size && dest_index <= array_size);
	T2* array = nullptr;

	if (_var) {
		array = (object->*_var);
	} else if (_getter) {
		array = const_cast<T2*>((object->*_getter)());
	}

	if (array) {
		T2 temp = Move((object->*_var)[src_index]);
		deconstruct(array + src_index);

		for (size_t i = src_index; i  < array_size - 1; ++i) {
			memcpy(array + i, array + i + 1, sizeof(T2));
		}

		// If the dest index was above the source index, shift it down by one.
		dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; 

		for (size_t i = array_size - 1; i > dest_index; --i) {
			memcpy(array + i, array + i - 1, sizeof(T2));
		}

		moveConstruct(array + dest_index, Move(temp));
	}
}

template <class T, class Allocator>
template <size_t array_size, class T2>
bool ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::isFixedArray(void) const
{
	return true;
}

template <class T, class Allocator>
template <size_t array_size, class T2>
bool ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::isArray(void) const
{
	return true;
}

template <class T, class Allocator>
template <size_t array_size, class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::ArrayFixedObjectContainer<array_size, T2>::getType(void) const
{
	return VT_OBJECT;
}




// Array Object
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::ArrayObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& obj_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _obj_ref_def(obj_ref_def), _getter(getter), _setter(setter), _var(nullptr)
{
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::ArrayObjectContainer(const char* key, Array<T2, Allocator> T::* var, ReflectionDefinition<T2, Allocator>& obj_ref_def, const Allocator& allocator):
	ValueContainerBase(key, allocator), _obj_ref_def(obj_ref_def), _getter(nullptr), _setter(nullptr), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::read(const JSON& json, T* object)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isObject());
			_obj_ref_def.read(value, &(object->*_var)[index]);
			return false;
		});

	} else if (_getter) {
		Array<T2, Allocator>& arr = const_cast<Array<T2, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isObject());
			_obj_ref_def.read(value, &arr[index]);
			return false;
		});
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::write(JSON& json, const T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);
	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			JSON obj = JSON::createObject();
			_obj_ref_def.write(obj, &(object->*_var)[i]);
			array.setObject(i, obj);
		}

	} else if (_getter) {
		const Array<T2, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			JSON obj = JSON::createObject();
			_obj_ref_def.write(obj, const_cast<T2*>(&value[i]));
			array.setObject(i, obj);
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::get(void* out, const T* object) const
{
	if (_var) {
		*reinterpret_cast<Array<T2, Allocator>*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<Array<T2, Allocator>*>(out) = (object->*_getter)();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::get(void* out, size_t index, const void* object) const
{
	get(out, index, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::get(void* out, size_t index, const T* object) const
{
	if (_var) {
		assert(index < (object->*_var).size());
		*reinterpret_cast<T2*>(out) = (object->*_var)[index];
	} else if (_getter) {
		const Array<T2, Allocator>& array = (object->*_getter)();
		assert(index < array.size());
		*reinterpret_cast<T2*>(out) = array[index];
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::set(const void* value, T* object)
{
	if (_var) {
		(object->*_var) = *reinterpret_cast<const Array<T2, Allocator>*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const Array<T2, Allocator>*>(value));
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::set(const void* value, size_t index, void* object)
{
	set(value, index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::set(const void* value, size_t index, T* object)
{
	if (_var) {
		assert(index < (object->*_var).size());
		(object->*_var)[index] = *reinterpret_cast<const T2*>(value);
	} else if (_getter) {
		Array<T2, Allocator>& array = const_cast<Array<T2, Allocator>&>((object->*_getter)());
		assert(index < array.size());
		array[index] = *reinterpret_cast<const T2*>(value);
	}
}

template <class T, class Allocator>
template <class T2>
size_t ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::size(const void* object) const
{
	return size(reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
size_t ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::size(const T* object) const
{
	if (_var) {
		return (object->*_var).size();
	} else if (_getter) {
		return (object->*_getter)().size();
	} else {
		return 0;
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::resize(size_t new_size, void* object)
{
	resize(new_size, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::resize(size_t new_size, T* object)
{
	if (_var) {
		return (object->*_var).resize(new_size);
	} else if (_getter) {
		return const_cast<Array<T2, Allocator>&>((object->*_getter)()).resize(new_size);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::move(size_t src_index, size_t dest_index, void* object)
{
	move(src_index, dest_index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::move(size_t src_index, size_t dest_index, T* object)
{
	if (_var) {
		assert(src_index < (object->*_var).size() && dest_index <= (object->*_var).size());
		T2 temp = Move((object->*_var)[src_index]);
		(object->*_var).erase(src_index);
		dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/
		(object->*_var).moveInsert(Move(temp), dest_index);
	} else if (_getter) {
		Array<T2, Allocator>& array = const_cast<Array<T2, Allocator>&>((object->*_getter)());
		T2 temp = Move((object->*_var)[src_index]);
		array.erase(src_index);
		dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/
		array.moveInsert(Move(temp), dest_index);
	}
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::isArray(void) const
{
	return true;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::ArrayObjectContainer<T2>::getType(void) const
{
	return VT_OBJECT;
}




// Array Enum
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::ArrayEnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _enum_ref_def(enum_ref_def), _getter(getter), _setter(setter), _var(nullptr)
{
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::ArrayEnumContainer(const char* key, Array<T2, Allocator> T::* var, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, const Allocator& allocator):
	ValueContainerBase(key, allocator), _enum_ref_def(enum_ref_def), _getter(nullptr), _setter(nullptr), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::read(const JSON& json, T* object)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isString());
			(object->*_var)[index] = _enum_ref_def.getValue(value.getString());
			return false;
		});

	} else if (_getter) {
		Array<T2, Allocator>& arr = const_cast<Array<T2, Allocator>&>((object->*_getter)());
		arr.resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isString());
			arr[index] = _enum_ref_def.getValue(value.getString());
			return false;
		});
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::write(JSON& json, const T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createString(_enum_ref_def.getName((object->*_var)[i])));
		}

	} else if (_getter) {
		const Array<T2, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createString(_enum_ref_def.getName(value[i])));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::get(void* out, const T* object) const
{
	if (_var) {
		*reinterpret_cast<Array<T2, Allocator>*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<Array<T2, Allocator>*>(out) = (object->*_getter)();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::get(void* out, size_t index, const void* object) const
{
	get(out, index, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::get(void* out, size_t index, const T* object) const
{
	if (_var) {
		assert(index < (object->*_var).size());
		*reinterpret_cast<T2*>(out) = (object->*_var)[index];
	} else if (_getter) {
		const Array<T2, Allocator>& array = (object->*_getter)();
		assert(index < array.size());
		*reinterpret_cast<T2*>(out) = array[index];
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::set(const void* value, T* object)
{
	if (_var) {
		(object->*_var) = *reinterpret_cast<const Array<T2, Allocator>*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const Array<T2, Allocator>*>(value));
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::set(const void* value, size_t index, void* object)
{
	set(value, index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::set(const void* value, size_t index, T* object)
{
	if (_var) {
		assert(index < (object->*_var).size());
		(object->*_var)[index] = *reinterpret_cast<const T2*>(value);
	} else if (_getter) {
		Array<T2, Allocator>& array = const_cast<Array<T2, Allocator>&>((object->*_getter)());
		assert(index < array.size());
		array[index] = *reinterpret_cast<const T2*>(value);
	}
}

template <class T, class Allocator>
template <class T2>
size_t ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::size(const void* object) const
{
	return size(reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
size_t ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::size(const T* object) const
{
	if (_var) {
		return (object->*_var).size();
	} else if (_getter) {
		return (object->*_getter)().size();
	} else {
		return 0;
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::resize(size_t new_size, void* object)
{
	resize(new_size, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::resize(size_t new_size, T* object)
{
	if (_var) {
		return (object->*_var).resize(new_size);
	} else if (_getter) {
		return const_cast<Array<T2, Allocator>&>((object->*_getter)()).resize(new_size);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::move(size_t src_index, size_t dest_index, void* object)
{
	move(src_index, dest_index, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::move(size_t src_index, size_t dest_index, T* object)
{
	if (_var) {
		assert(src_index < (object->*_var).size() && dest_index <= (object->*_var).size());
		T2 temp = Move((object->*_var)[src_index]);
		(object->*_var).erase(src_index);
		dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/
		(object->*_var).moveInsert(Move(temp), dest_index);
	} else if (_getter) {
		Array<T2, Allocator>& array = const_cast<Array<T2, Allocator>&>((object->*_getter)());
		T2 temp = Move((object->*_var)[src_index]);
		array.erase(src_index);
		dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/
		array.moveInsert(Move(temp), dest_index);
	}
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::isArray(void) const
{
	return true;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::ArrayEnumContainer<T2>::getType(void) const
{
	return VT_ENUM;
}



// Object
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::ObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& var_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _getter(getter), _setter(setter), _var_ref_def(var_ref_def), _var(nullptr)
{
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def, const Allocator& allocator) :
	ValueContainerBase(key, allocator), _getter(nullptr), _setter(nullptr), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::read(const JSON& json, T* object)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isObject());

	if (_var) {
		_var_ref_def.read(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())], &(object->*_var));
	} else if (_setter) {
		T2 value;
		_var_ref_def.read(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())], &value);
		(object->*_setter)(value);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::write(JSON& json, const T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		JSON obj = JSON::createObject();
		_var_ref_def.write(obj, &(object->*_var));
		json.setObject(ValueContainerBase::_key.getBuffer(), obj);
	} else if (_getter) {
		T2* value = &const_cast<T2&>((object->*_getter)());

		JSON obj = JSON::createObject();
		_var_ref_def.write(obj, value);
		json.setObject(ValueContainerBase::_key.getBuffer(), obj);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::get(void* out, const T* object) const
{
	if (_var) {
		*reinterpret_cast<T2*>(value) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<T2*>(value) = (object->*_getter)();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::set(const void* value, T* object)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const T2*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const T2*>(value));
	}
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::isArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::getType(void) const
{
	return VT_OBJECT;
}




// Enum
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::EnumContainer<T2>::EnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& var_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _getter(getter), _setter(setter), _var_ref_def(var_ref_def), _var(nullptr)
{
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::EnumContainer<T2>::EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& var_ref_def, const Allocator& allocator):
	ValueContainerBase(key, allocator), _getter(nullptr), _setter(nullptr), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::read(const JSON& json, T* object)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isString());

	if (_var) {
		object->*_var = _var_ref_def.getValue(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString());
	} else if (_setter) {
		T2 value = _var_ref_def.getValue(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString());
		(object->*_setter)(value);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::write(JSON& json, const T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		JSON val = JSON::createString(_var_ref_def.getName(object->*_var));
		json.setObject(ValueContainerBase::_key.getBuffer(), val);
	} else if (_getter) {
		JSON val = JSON::createString(_var_ref_def.getName((object->*_getter)()));
		json.setObject(ValueContainerBase::_key.getBuffer(), val);
	}
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::EnumContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::EnumContainer<T2>::isArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::EnumContainer<T2>::getType(void) const
{
	return VT_ENUM;
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::get(void* out, const T* object) const
{
	if (_var) {
		*reinterpret_cast<T2*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<T2*>(out) = (object->*_getter)();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(const void* value, T* object)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const T2*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const T2*>(value));
	}
}





// Base
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::BaseValueContainer(const char* key, const typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr& value_ptr, const Allocator& allocator):
	ValueContainerBase(key, allocator), _value_ptr(value_ptr)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::read(const JSON& json, T* object)
{
	_value_ptr->read(json, object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::write(JSON& json, const T* object) const
{
	_value_ptr->write(json, object);
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::isArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::getType(void) const
{
	return _value_ptr->getType();
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::get(void* out, const T* object) const
{
	_value_ptr->get(out, static_cast<const T2*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(const void* value, T* object)
{
	_value_ptr->set(value, static_cast<T2*>(object));
}
