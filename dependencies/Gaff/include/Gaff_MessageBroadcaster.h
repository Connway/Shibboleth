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

#include "Gaff_IRefCounted.h"
#include "Gaff_SmartPtr.h"
#include "Gaff_HashMap.h"
#include "Gaff_RefPtr.h"
#include "Gaff_Array.h"
#include "Gaff_Pair.h"

NS_GAFF

typedef RefPtr<IRefCounted> MessageReceipt;

template <class Allocator = DefaultAllocator>
class MessageBroadcaster
{
public:
	MessageBroadcaster(const Allocator& allocator = Allocator());
	~MessageBroadcaster(void);

	template <class Message, class Object>
	MessageReceipt listen(Object* object, void (Object::*function)(const Message&));

	template <class Message>
	MessageReceipt listen(void (*function)(const Message&));

	template <class Message, class FunctorT>
	MessageReceipt listen(const FunctorT& functor);

	template <class Message>
	void broadcastNow(const Message& message);

	template <class Message>
	void broadcast(const Message& message);

	void update(void);

private:
	// Base function class for function binders to inherit from
	class IFunction
	{
	public:
		virtual ~IFunction(void) {}

		virtual void call(void* message) = 0;
	};

	// Binder for member functions
	template <class Message, class Object>
	class MemberFunction : public IFunction
	{
	public:
		typedef void (Object::*FunctionType)(const Message&);

		MemberFunction(Object* object, FunctionType function);
		void call(void* message);

	private:
		FunctionType _function;
		Object* _object;
	};

	// Binder for normal functions
	template <class Message>
	class Function : public IFunction
	{
	public:
		typedef void (*FunctionType)(const Message&);

		Function(FunctionType function);
		void call(void* message);

	private:
		FunctionType _function;
	};

	// Binder for functors
	template <class Message, class FunctorT>
	class Functor : public IFunction
	{
	public:
		Functor(const FunctorT& functor);
		void call(void* message);

	private:
		FunctorT _functor;
	};

	class Remover : public IRefCounted
	{
	public:
		Remover(MessageBroadcaster<Allocator>* broadcaster,
				const AHashString<Allocator>* message_type,
				const IFunction* listener);
		~Remover(void);

	void addRef(void) const;
	void release(void) const;

	unsigned int getRefCount(void) const;

	private:
		MessageBroadcaster<Allocator>* _broadcaster;
		const AHashString<Allocator>* _message_type;
		const IFunction* _listener;

		mutable unsigned int _ref_count;
	};

	// Using a normal SmartPtr should be fine, as we're never exposing these outside the array
	typedef SmartPtr<IFunction, Allocator> IFuncPtr;
	typedef Array<IFuncPtr, Allocator> ListenerList;

	Array< Pair<AHashString<Allocator>, void*> > _message_queue;
	HashMap<AHashString<Allocator>, ListenerList> _listeners;
	Allocator _allocator;

	void removeListener(const AHashString<Allocator>& message_type, const IFunction* listener);

	friend class Remover;
};

#include "Gaff_MessageBroadcaster.inl"

NS_END
