/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "CodeGen_Utils.h"
#include "CodeGen_IncludeArgParse.h"
#include <Gaff_File.h>

void WriteLicense(Gaff::File& gen_file, const argparse::ArgumentParser& program)
{
	if (program.is_used("--license_file")) {
		const std::string license_file_path = program.get("--license_file");
		Gaff::File license_file(license_file_path.data());

		if (license_file.isOpen()) {
			char8_t buffer[2048] = { 0 };

			gen_file.writeString(u8"/************************************************************************************\n");

			while (license_file.readString(buffer, ARRAY_SIZE(buffer))) {
				gen_file.writeString(buffer);
			}

			gen_file.writeString(u8"************************************************************************************/\n\n");

		} else {
			std::cerr << "Failed to open license file '" << license_file_path.data() << "'." << std::endl;
		}
	}
}
