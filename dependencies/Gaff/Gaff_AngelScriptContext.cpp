/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_AngelScriptContext.h"
#include "Gaff_AngelScriptFunction.h"

NS_GAFF

AngelScriptContext::AngelScriptContext(const AngelScriptContext& rhs):
	_context(rhs._context)
{
}

AngelScriptContext::AngelScriptContext(asIScriptContext* context)
{
	_context.set(context);
}

AngelScriptContext::~AngelScriptContext(void)
{
}

const AngelScriptContext& AngelScriptContext::operator=(const AngelScriptContext& rhs)
{
	_context = rhs._context;
	return *this;
}

const AngelScriptContext& AngelScriptContext::operator=(asIScriptContext* rhs)
{
	_context = rhs;
	return *this;
}

bool AngelScriptContext::operator==(const AngelScriptContext& rhs) const
{
	return _context == rhs._context;
}

bool AngelScriptContext::operator==(const asIScriptContext* rhs) const
{
	return _context == rhs;
}

bool AngelScriptContext::operator!=(const AngelScriptContext& rhs) const
{
	return _context != rhs._context;
}

bool AngelScriptContext::operator!=(const asIScriptContext* rhs) const
{
	return _context != rhs;
}

bool AngelScriptContext::isValid(void) const
{
	return _context != nullptr;
}

bool AngelScriptContext::prepare(AngelScriptFunction& function)
{
	return _context->Prepare(function._function.get()) >= 0;
}

bool AngelScriptContext::unprepare(void)
{
	return _context->Unprepare() >= 0;
}

int AngelScriptContext::execute(void)
{
	return _context->Execute();
}

bool AngelScriptContext::abort(void)
{
	return _context->Abort() >= 0;
}

bool AngelScriptContext::suspend(void)
{
	return _context->Suspend() >= 0;
}

asEContextState AngelScriptContext::getState(void) const
{
	return _context->GetState();
}

bool AngelScriptContext::setArgByte(unsigned int index, unsigned char value)
{
	return _context->SetArgByte(index, value) >= 0;
}

bool AngelScriptContext::setArgWord(unsigned int index, unsigned short value)
{
	return _context->SetArgWord(index, value) >= 0;
}

bool AngelScriptContext::setArgDWord(unsigned int index, asDWORD value)
{
	return _context->SetArgDWord(index, value) >= 0;
}

bool AngelScriptContext::setArgQWord(unsigned int index, asQWORD value)
{
	return _context->SetArgQWord(index, value) >= 0;
}

bool AngelScriptContext::setArgFloat(unsigned int index, float value)
{
	return _context->SetArgFloat(index, value) >= 0;
}

bool AngelScriptContext::setArgDouble(unsigned int index, double value)
{
	return _context->SetArgDouble(index, value) >= 0;
}

bool AngelScriptContext::setArgAddress(unsigned int index, void* addr)
{
	assert(addr);
	return _context->SetArgAddress(index, addr) >= 0;
}

bool AngelScriptContext::setArgObject(unsigned int index, void* obj)
{
	assert(obj);
	return _context->SetArgObject(index, obj) >= 0;
}

void* AngelScriptContext::getAddressOfArg(unsigned int index)
{
	return _context->GetAddressOfArg(index);
}

unsigned char AngelScriptContext::getReturnByte(void)
{
	return _context->GetReturnByte();
}

unsigned short AngelScriptContext::getReturnWord(void)
{
	return _context->GetReturnWord();
}

asDWORD AngelScriptContext::getReturnDWord(void)
{
	return _context->GetReturnDWord();
}

asQWORD AngelScriptContext::getReturnQWord(void)
{
	return _context->GetReturnQWord();
}

float AngelScriptContext::getReturnFloat(void)
{
	return _context->GetReturnFloat();
}

double AngelScriptContext::getReturnDouble(void)
{
	return _context->GetReturnDouble();
}

void* AngelScriptContext::getReturnAddress(void)
{
	return _context->GetReturnAddress();
}

void* AngelScriptContext::getReturnObject(void)
{
	return _context->GetReturnObject();
}

int AngelScriptContext::getExceptionLineNumber(int* column, const char** section_name)
{
	return _context->GetExceptionLineNumber(column, section_name);
}

AngelScriptFunction AngelScriptContext::getExceptionFunction(void)
{
	return AngelScriptFunction(_context->GetExceptionFunction());
}

const char* AngelScriptContext::getExceptionString(void)
{
	return _context->GetExceptionString();
}

bool AngelScriptContext::setExceptionCallback(void (*callback)(asIScriptContext*, void*), asECallConvTypes call_type)
{
	assert(callback);
	return setExceptionCallbackHelper(asFUNCTION(callback), nullptr, call_type);
}

void AngelScriptContext::clearExceptionCallback(void)
{
	_context->ClearExceptionCallback();
}

bool AngelScriptContext::setLineCallback(void (*callback)(asIScriptContext*, void*), asECallConvTypes call_type)
{
	assert(callback);
	return _context->SetLineCallback(asFUNCTION(callback), nullptr, call_type) >= 0;
}

void AngelScriptContext::clearLineCallback(void)
{
	_context->ClearLineCallback();
}

bool AngelScriptContext::setExceptionCallbackHelper(asSFuncPtr callback, void* obj, asECallConvTypes call_type)
{
	return _context->SetExceptionCallback(callback, obj, call_type) >= 0;
}

bool AngelScriptContext::setLineCallbackHelper(asSFuncPtr callback, void* obj, asECallConvTypes call_type)
{
	return _context->SetLineCallback(callback, obj, call_type) >= 0;
}

NS_END
