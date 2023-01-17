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

#include "CodeGen_Utils.h"
#include <Gaff_File.h>
#include <argparse.hpp>

std::string GetLicenseText(const argparse::ArgumentParser& program)
{
	std::string out;

	if (program.is_used("--license_file")) {
		const std::string license_file_path = program.get("--license_file");
		Gaff::File license_file(license_file_path.data());

		if (license_file.isOpen()) {
			out.resize(license_file.getFileSize() + 1);
			license_file.readEntireFile(out.data());

			out = "/************************************************************************************\n" +
				out +
				"************************************************************************************/\n\n";

		} else {
			std::cerr << "Failed to open license file '" << license_file_path.data() << "'." << std::endl;
		}
	}

	return out;
}
