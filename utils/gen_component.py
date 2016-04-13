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

parser = argparse.ArgumentParser(description = "Generates a *Component.h/.cpp file.")
parser.add_argument("component_name", type = str)
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

component_name = args.component_name
copyright_text = ""

if args.copyright:
	copyright_text = args.copyright.read()
elif args.mit:
	copyright_text = mit_license.format(args.mit)

h_text = """{0}

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_Component.h>

NS_SHIBBOLETH

class {1}Component : public Component
{{
public:
	static const char* GetComponentName(void);

	{1}Component(void);
	~{1}Component(void);

	const Gaff::JSON& getSchema(void) const;

	bool load(const Gaff::JSON& json);
	bool save(Gaff::JSON& json);

private:
	GAFF_NO_COPY({1}Component);
	GAFF_NO_MOVE({1}Component);

	SHIB_REF_DEF({1}Component);
	REF_DEF_REQ;
}};

NS_END
"""

cpp_text = """{0}

#include \"Shibboleth_{1}Component.h"

NS_SHIBBOLETH

COMP_REF_DEF_SAVE({1}Component, gRefDef);
REF_IMPL_REQ({1}Component);

SHIB_REF_IMPL({1}Component)
.addBaseClassInterfaceOnly<{1}Component>()
;

const char* {1}Component::GetComponentName(void)
{{
	return \"{1} Component\";
}}

{1}Component::{1}Component(void)
{{
}}

{1}Component::~{1}Component(void)
{{
}}

const Gaff::JSON& {1}Component::getSchema(void) const
{{
	static const Gaff::JSON& schema = GetApp().getManagerT<SchemaManager>().getSchema(\"{1}Component.schema\");
	return schema;
}}

bool {1}Component::load(const Gaff::JSON& json)
{{
	gRefDef.read(json, this);
	return true;
}}

NS_END
"""

header = open("../src/Components/include/Shibboleth_{0}Component.h".format(component_name), "w")
source = open("../src/Components/Shibboleth_{0}Component.cpp".format(component_name), "w")

header.write(h_text.format(copyright_text, component_name))
source.write(cpp_text.format(copyright_text, component_name))

# print(h_text.format(copyright_text, component_name))
# print(cpp_text.format(copyright_text, component_name))
