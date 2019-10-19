/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include <Gaff_MessagePack.h>
#include <Gaff_String.h>
#include <Gaff_JSON.h>
#include <Gaff_File.h>
#include <cstdio>

static void WriteMPack(Gaff::MessagePackWriter& writer, const Gaff::JSON& json)
{
	if (json.isArray()) {
		writer.startArray(json.size());

		json.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
		{
			WriteMPack(writer, value);
			return false;
		});

		writer.endArray();

	} else if (json.isObject()) {
		writer.startObject(json.size());

		json.forEachInObject([&](const char* name, const Gaff::JSON& value) -> bool
		{
			writer.writeKey(name);
			WriteMPack(writer, value);
			return false;
		});

		writer.endObject();

	} else if (json.isNull()) {
		writer.writeNull();
	} else if (json.isBool()) {
		writer.write(json.getBool());
	//} else if (json.isInt8()) {
	//	writer.write(json.getInt8());
	//} else if (json.isInt16()) {
	//	writer.write(json.getInt16());
	} else if (json.isInt32()) {
		writer.write(json.getInt32());
	} else if (json.isInt64()) {
		writer.write(json.getInt64());
	//} else if (json.isUInt8()) {
	//	writer.write(json.getUInt8());
	//} else if (json.isUInt16()) {
	//	writer.write(json.getUInt16());
	} else if (json.isUInt32()) {
		writer.write(json.getUInt32());
	} else if (json.isUInt64()) {
		writer.write(json.getUInt64());
	} else if (json.isFloat()) {
		writer.write(json.getFloat());
	} else if (json.isDouble()) {
		writer.write(json.getDouble());
	} else if (json.isString()) {
		writer.writeUTF8(json.getString());
	}
}

int main(int argc, char** argv)
{
	if (argc <= 1) {
		printf("No arguments given.");
		return 0;
	}

	for (int32_t i = 1; i < argc; ++i) {
		Gaff::JSON json;

		if (!json.parseFile(argv[i])) {
			printf("Failed to parse '%s' with error '%s'", argv[i], json.getErrorText());
			continue;
		}

		const Gaff::U8String<> mpack_file = Gaff::U8String<>(argv[i]) + ".bin";
		Gaff::MessagePackWriter mpack;
		
		if (!mpack.init(mpack_file.data())) {
			printf("Failed to initialize mpack file '%s'.", mpack_file.data());
			continue;
		}

		WriteMPack(mpack, json);

		mpack.finish();
	}

	return 0;
}
