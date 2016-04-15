#!/usr/bin/env python

#************************************************************************************
#Copyright (C) 2016 by Nicholas LaCroix

#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.
#************************************************************************************

import argparse

parser = argparse.ArgumentParser(description = "Generates a *Manager.h/.cpp file.")
parser.add_argument("manager_name", type = str)
parser.add_argument("--mit", type = str, required = False, help = "Writes the MIT license to the top of the file using the passed string as the copyright owner.")
parser.add_argument("--copyright", type = argparse.FileType("r"), required = False, help = "Writes the text found in the passed in file as the copyright text.")
args = parser.parse_args()

mit_license = """/************************************************************************************
Copyright (C) 2016 by {0}

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
************************************************************************************/"""

manager_name = args.manager_name
copyright_text = ""

if args.copyright:
	copyright_text = args.copyright.read()
elif args.mit:
	copyright_text = mit_license.format(args.mit)

h_text = """{0}

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class {1}Manager : public IManager
{{
public:
	static const char* GetFriendlyName(void);

	{1}Manager(void);
	~{1}Manager(void);

	const char* getName(void) const;

private:
	GAFF_NO_COPY({1}Manager);
	GAFF_NO_MOVE({1}Manager);

	SHIB_REF_DEF({1}Manager);
	REF_DEF_REQ;
}};

NS_END
"""

cpp_text = """{0}

#include \"Shibboleth_{1}Manager.h"

NS_SHIBBOLETH

REF_IMPL_REQ({1}Manager);
SHIB_REF_IMPL({1}Manager)
.addBaseClassInterfaceOnly<{1}Manager>()
;

const char* {1}Manager::GetFriendlyName(void) const
{{
	return \"{1} Manager\";
}}

{1}Manager::{1}Manager(void)
{{
}}

{1}Manager::~{1}Manager(void)
{{
}}

const char* {1}Manager::getName(void) const
{{
	return GetFriendlyName();
}}

NS_END
"""

header = open("../src/Managers/include/Shibboleth_{0}Manager.h".format(manager_name), "w")
source = open("../src/Managers/Shibboleth_{0}Manager.cpp".format(manager_name), "w")

header.write(h_text.format(copyright_text, manager_name))
source.write(cpp_text.format(copyright_text, manager_name))

# print(h_text.format(copyright_text, manager_name))
# print(cpp_text.format(copyright_text, manager_name))
