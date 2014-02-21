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

#pragma once

#include "Gaff_RefPtr.h"
#include "Gaff_IncludeAngelScript.h"
#include "Gaff_IncludeAssert.h"

NS_GAFF

class AngelScriptFunction;

class AngelScriptContext
{
public:
	template <class T>
	INLINE T* getReturnObject(void)
	{
		return (T*)getReturnObject();
	}

	template <class T>
	INLINE bool setExceptionCallback(T* object, void (T::*callback)(asIScriptContext*))
	{
		assert(object && callback);
		return setExceptionCallbackHelper(
			asSMethodPtr<callback>::Convert(callback),
			object,
			asCALL_THISCALL
		);
	}

	template <class T>
	INLINE bool setLineCallback(T* object, void (T::*callback)(asIScriptContext*))
	{
		assert(object && callback);
		return setLineCallbackHelper(
			asSMethodPtr<callback>::Convert(callback),
			object,
			asCALL_THISCALL
		);
	}

	AngelScriptContext(const AngelScriptContext& rhs);
	explicit AngelScriptContext(asIScriptContext* context);
	~AngelScriptContext(void);

	INLINE const AngelScriptContext& operator=(const AngelScriptContext& rhs);
	INLINE const AngelScriptContext& operator=(asIScriptContext* rhs);

	INLINE bool operator==(const AngelScriptContext& rhs) const;
	INLINE bool operator==(const asIScriptContext* rhs) const;
	INLINE bool operator!=(const AngelScriptContext& rhs) const;
	INLINE bool operator!=(const asIScriptContext* rhs) const;

	INLINE bool isValid(void) const;

	INLINE bool prepare(AngelScriptFunction& function);
	INLINE bool unprepare(void);
	INLINE int execute(void);
	INLINE bool abort(void);
	INLINE bool suspend(void);

	INLINE asEContextState getState(void) const;

	INLINE bool setArgByte(unsigned int index, unsigned char value);
	INLINE bool setArgWord(unsigned int index, unsigned short value);
	INLINE bool setArgDWord(unsigned int index, asDWORD value);
	INLINE bool setArgQWord(unsigned int index, asQWORD value);
	INLINE bool setArgFloat(unsigned int index, float value);
	INLINE bool setArgDouble(unsigned int index, double value);
	INLINE bool setArgAddress(unsigned int index, void* addr);
	INLINE bool setArgObject(unsigned int index, void* obj);
	INLINE void* getAddressOfArg(unsigned int index);

	INLINE unsigned char getReturnByte(void);
	INLINE unsigned short getReturnWord(void);
	INLINE asDWORD getReturnDWord(void);
	INLINE asQWORD getReturnQWord(void);
	INLINE float getReturnFloat(void);
	INLINE double getReturnDouble(void);
	INLINE void* getReturnAddress(void);
	INLINE void* getReturnObject(void);

	INLINE int getExceptionLineNumber(int* column = nullptr, const char** section_name = nullptr);
	INLINE AngelScriptFunction getExceptionFunction(void);
	INLINE const char* getExceptionString(void);
	INLINE bool setExceptionCallback(void (*callback)(asIScriptContext*, void*), asECallConvTypes call_type = asCALL_CDECL);
	INLINE void clearExceptionCallback(void);

	INLINE bool setLineCallback(void (*callback)(asIScriptContext*, void*), asECallConvTypes call_type = asCALL_CDECL);
	INLINE void clearLineCallback(void);

private:
	COMRefPtr<asIScriptContext> _context;

	INLINE bool setExceptionCallbackHelper(asSFuncPtr callback, void* obj, asECallConvTypes call_type);
	INLINE bool setLineCallbackHelper(asSFuncPtr callback, void* obj, asECallConvTypes call_type);
};

NS_END
